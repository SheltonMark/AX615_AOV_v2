#include <iostream>
#include <memory>
#include <map>

#include "../../app/alarm/alarm_service_stub.hpp"
#include "../../app/cloud/cloud_service_stub.hpp"
#include "../../app/config/config_service_stub.hpp"
#include "../../app/core/aov_app_runtime.hpp"
#include "../../app/core/aov_orchestrator_stub.hpp"
#include "../../app/packet/packet_service_stub.hpp"
#include "../../app/storage/dhfs/dhfs_writer_stub.hpp"
#include "../../app/storage/storage_service_stub.hpp"
#include "../../libmedia/Include/audio/i_audio_stream_service.hpp"
#include "../../libmedia/Include/detect/i_detect_service.hpp"
#include "../../libmedia/Include/isp/i_isp_service.hpp"
#include "../../libmedia/Include/osd/i_osd_service.hpp"
#include "../../libmedia/Include/runtime/i_media_runtime.hpp"
#include "../../libmedia/Include/video/i_video_stream_service.hpp"
#include "../../libsys/Src/device/device_control_service_stub.hpp"
#include "../../libsys/Src/storage/storage_service_stub.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

class FakeVideoStreamService final : public aov::media::IVideoStreamService {
public:
    aov::media::MediaStatusCode ApplyVideoEncodeConfig(
        const aov::media::VideoEncodeApplyConfig&) override {
        return aov::media::MediaStatusCode::Ok;
    }
    aov::media::MediaStatusCode ApplyVideoEncodeGroupConfig(
        const aov::media::VideoEncodeGroupConfig&) override {
        return aov::media::MediaStatusCode::Ok;
    }
    aov::media::MediaStatusCode RegisterStreamCallback(
        int chn_id,
        aov::media::StreamCallback cb) override {
        callbacks[chn_id] = std::move(cb);
        return aov::media::MediaStatusCode::Ok;
    }
    aov::media::MediaStatusCode StartCapture(int chn_id) override {
        capturing[chn_id] = true;
        return aov::media::MediaStatusCode::Ok;
    }
    void StopCapture(int chn_id) override {
        capturing[chn_id] = false;
    }
    aov::media::MediaStatusCode RequestKeyFrame(int) override {
        return aov::media::MediaStatusCode::Ok;
    }
    aov::media::MediaStatusCode SetLowPowerFrameRate(int, int) override {
        return aov::media::MediaStatusCode::Ok;
    }
    aov::media::MediaStatusCode SetNormalFrameRate(int, int) override {
        return aov::media::MediaStatusCode::Ok;
    }
    bool IsCapturing(int chn_id) const override {
        const auto it = capturing.find(chn_id);
        return it != capturing.end() && it->second;
    }
    void Dispatch(const aov::media::StreamFrame& frame) {
        const auto it = callbacks.find(frame.chn_id);
        if (it != callbacks.end()) {
            it->second(frame.chn_id, frame);
        }
    }

    std::map<int, aov::media::StreamCallback> callbacks;
    std::map<int, bool> capturing;
};

class FakeMediaRuntime final : public aov::media::IMediaRuntime {
public:
    aov::media::MediaStatusCode Init(const aov::media::MediaRuntimeInitConfig&) override {
        return aov::media::MediaStatusCode::Ok;
    }
    aov::media::MediaStatusCode Init(const aov::media::SensorConfig&,
                                     const aov::media::CmPoolConfig&) override {
        return aov::media::MediaStatusCode::Ok;
    }
    aov::media::MediaStatusCode Start() override { return aov::media::MediaStatusCode::Ok; }
    void Stop() override {}
    aov::media::MediaStatusCode ApplyVideoConfig(
        const aov::media::VideoEncodeGroupConfig&) override { return aov::media::MediaStatusCode::Ok; }
    aov::media::MediaStatusCode ApplyAudioConfig(
        const aov::media::AudioEncodeApplyConfig&) override { return aov::media::MediaStatusCode::Ok; }
    aov::media::MediaStatusCode ApplyImageConfig(
        const aov::media::IspImageConfig&) override { return aov::media::MediaStatusCode::Ok; }
    aov::media::MediaStatusCode ApplyDetectConfig(
        const aov::media::DetectApplyConfig&) override { return aov::media::MediaStatusCode::Ok; }
    aov::media::MediaStatusCode ApplyOsdConfig(
        const aov::media::OsdApplyConfig&) override { return aov::media::MediaStatusCode::Ok; }
    aov::media::MediaStatusCode PrepareForAovSleep(
        const aov::media::AovSleepMediaConfig&) override { return aov::media::MediaStatusCode::Ok; }
    aov::media::MediaStatusCode RestoreFromAovSleep(
        const aov::media::WakeupMediaProfile&) override { return aov::media::MediaStatusCode::Ok; }
    aov::media::MediaStatusCode EnterLowPowerMode(
        const aov::media::LowPowerRecordProfile&) override { return aov::media::MediaStatusCode::Ok; }
    aov::media::MediaStatusCode ResumeNormalMode() override { return aov::media::MediaStatusCode::Ok; }
    aov::media::MediaStatusCode CaptureSnapshot(const aov::media::SnapshotRequest&,
                                                aov::media::SnapshotResult&) override {
        return aov::media::MediaStatusCode::Ok;
    }
    aov::media::IIspService* GetIspService() override { return nullptr; }
    aov::media::IVideoStreamService* GetVideoStreamService() override { return &video; }
    aov::media::IAudioStreamService* GetAudioStreamService() override { return nullptr; }
    aov::media::IDetectService* GetDetectService() override { return nullptr; }
    aov::media::IOsdService* GetOsdService() override { return nullptr; }
    bool IsRunning() const override { return true; }
    aov::media::MediaRuntimeStatus GetRuntimeStatus() const override { return {}; }

    FakeVideoStreamService video;
};

} // namespace

int main() {
    using namespace aov::app;

    ConfigServiceStub config;
    AovOrchestratorStub orchestrator;
    CloudServiceStub cloud;
    AlarmServiceStub alarm;
    packet::PacketServiceStub packet;
    storage::StorageServiceStub storage(
        std::make_unique<storage::dhfs::DhfsWriterStub>());
    FakeMediaRuntime media;

    core::AovAppRuntime runtime(config,
                                orchestrator,
                                cloud,
                                alarm,
                                packet,
                                storage,
                                media);

    if (runtime.GetStatus().initialized || runtime.GetStatus().started) {
        return Fail("runtime must start as stopped");
    }

    if (runtime.Init() != AovStatusCode::Ok ||
        !runtime.GetStatus().initialized) {
        return Fail("runtime init must load config and initialize modules");
    }

    if (runtime.Start() != AovStatusCode::Ok ||
        !runtime.GetStatus().started ||
        !cloud.GetRuntimeStatus().connected ||
        orchestrator.GetRuntimeWorkState() == RuntimeWorkState::LowBatteryProtection) {
        return Fail("runtime start must start core and cloud");
    }

    if (runtime.Start() != AovStatusCode::Ok) {
        return Fail("runtime start must be idempotent");
    }

    if (media.video.callbacks.count(0) == 0 ||
        media.video.callbacks.count(1) == 0) {
        return Fail("runtime init must register libmedia stream callbacks for packet routing");
    }

    if (!storage.StartRecord().ok()) {
        return Fail("storage must start recording before media frame routing check");
    }
    const std::uint8_t payload[] = {0, 0, 0, 1, 0x65, 0x88};
    aov::media::StreamFrame frame;
    frame.chn_id = 0;
    frame.codec = aov::media::VideoCodec::H264;
    frame.data = payload;
    frame.size = sizeof(payload);
    frame.pts_ms = 33;
    frame.sequence = 7;
    frame.is_keyframe = true;
    media.video.Dispatch(frame);
    if (packet.GetStats().published_frames != 1 ||
        storage.GetStats().received_frames != 1 ||
        storage.GetStats().written_bytes != sizeof(payload)) {
        return Fail("runtime must route libmedia encoded frame into packet and storage");
    }
    if (!storage.FlushAndClose().ok()) {
        return Fail("storage flush after media frame routing check failed");
    }

    aov::app::config::DeviceConfig alarm_config = config.GetDesiredConfig();
    alarm_config.alarm.human.linkage.trigger_record = true;
    alarm_config.alarm.human.linkage.push_message = true;
    if (!config.UpdateDesiredConfig(alarm_config).ok() ||
        !config.MarkApplied().ok()) {
        return Fail("alarm runtime test config update failed");
    }

    DetectResultSummary detected_person;
    detected_person.has_person = true;
    detected_person.object_count = 1;
    detected_person.timestamp_ms = 200 * 1000;
    if (runtime.OnDetectResult(detected_person) != AovStatusCode::Ok ||
        !storage.GetStats().recording ||
        !cloud.GetRuntimeStatus().cloud_storage_running) {
        return Fail("runtime must trigger local record and cloud storage for alarm detect result");
    }

    DetectResultSummary no_target;
    no_target.timestamp_ms = 220 * 1000;
    if (runtime.OnDetectResult(no_target) != AovStatusCode::Ok) {
        return Fail("runtime must accept no-target detect result");
    }

    aov::app::config::DeviceConfig pending_config = config.GetDesiredConfig();
    pending_config.work_mode.record_interval_sec = 5;
    if (!config.UpdateDesiredConfig(pending_config).ok() ||
        runtime.ApplyPendingConfig() != AovStatusCode::Ok ||
        config.GetActiveConfig().work_mode.record_interval_sec != 5 ||
        !config.GetPendingConfigChange().changed_fields.empty()) {
        return Fail("runtime must apply, persist and report pending config");
    }

    const auto submit_result = runtime.SubmitCloudAction(
        cloud::CloudActionRequest {"Reboot", "delay=3", "req-reboot", 3000});
    if (!submit_result.ok() ||
        runtime.GetPendingCoreCommandCount() != 1) {
        return Fail("runtime must submit cloud action into core command queue");
    }

    core::CoreCommand command;
    if (!runtime.PopCoreCommand(command).ok() ||
        command.type != core::CoreCommandType::Reboot ||
        command.delay_sec != 3 ||
        command.request_id != "req-reboot") {
        return Fail("runtime must pop cloud action command from core queue");
    }

    const auto alarm_submit_result = runtime.SubmitCloudAction(
        cloud::CloudActionRequest {"OneKeyAlarm", "duration=10", "req-alarm", 3100});
    if (!alarm_submit_result.ok() ||
        runtime.GetPendingCoreCommandCount() != 1) {
        return Fail("runtime must enqueue one-key alarm action");
    }
    aov::sys::StorageServiceStub sys_storage;
    aov::sys::DeviceControlServiceStub device;
    if (!runtime.DrainCoreCommands(4, &sys_storage, &device).ok() ||
        runtime.GetPendingCoreCommandCount() != 0 ||
        !storage.GetStats().recording ||
        !device.IsFillLightOn() ||
        !device.IsSoundAlarmOn()) {
        return Fail("runtime must drain queued cloud actions through core command executor");
    }

    if (runtime.Stop() != AovStatusCode::Ok ||
        runtime.GetStatus().started ||
        cloud.GetRuntimeStatus().connected ||
        storage.GetStats().recording) {
        return Fail("runtime stop must stop services and clear running state");
    }

    if (runtime.Stop() != AovStatusCode::Ok) {
        return Fail("runtime stop must be idempotent");
    }

    std::cout << "aov app runtime contract passed\n";
    return 0;
}

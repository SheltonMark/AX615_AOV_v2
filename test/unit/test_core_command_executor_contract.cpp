#include <iostream>
#include <memory>

#include "../../app/config/config_service_stub.hpp"
#include "../../app/core/core_command_executor.hpp"
#include "../../app/storage/dhfs/dhfs_writer_stub.hpp"
#include "../../app/storage/storage_service_stub.hpp"
#include "../../libsys/Src/device/device_control_service_stub.hpp"
#include "../../libsys/Src/storage/storage_service_stub.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

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
    aov::media::MediaStatusCode CaptureSnapshot(const aov::media::SnapshotRequest& request,
                                                aov::media::SnapshotResult&) override {
        ++snapshot_count;
        last_snapshot_channel = request.chn_id;
        return aov::media::MediaStatusCode::Ok;
    }
    aov::media::IIspService* GetIspService() override { return nullptr; }
    aov::media::IVideoStreamService* GetVideoStreamService() override { return nullptr; }
    aov::media::IAudioStreamService* GetAudioStreamService() override { return nullptr; }
    aov::media::IDetectService* GetDetectService() override { return nullptr; }
    aov::media::IOsdService* GetOsdService() override { return nullptr; }
    bool IsRunning() const override { return true; }
    aov::media::MediaRuntimeStatus GetRuntimeStatus() const override { return {}; }

    int snapshot_count {0};
    int last_snapshot_channel {-1};
};

} // namespace

int main() {
    using namespace aov::app::core;

    FakeMediaRuntime media;
    aov::app::config::ConfigServiceStub config;
    aov::app::storage::StorageServiceStub storage(
        std::make_unique<aov::app::storage::dhfs::DhfsWriterStub>());
    aov::sys::DeviceControlServiceStub device;
    aov::sys::StorageServiceStub sys_storage;
    if (!storage.Init(aov::app::storage::StorageConfig {}).ok()) {
        return Fail("storage init failed");
    }

    CoreCommandExecutor executor(media, config, storage, sys_storage, device);

    CoreCommand snapshot;
    snapshot.type = CoreCommandType::PreviewImageCapture;
    snapshot.stream_id = "main";
    if (!executor.Execute(snapshot).ok() ||
        media.snapshot_count != 1 ||
        media.last_snapshot_channel != 0) {
        return Fail("executor must capture preview snapshot through media runtime");
    }

    CoreCommand alarm;
    alarm.type = CoreCommandType::OneKeyAlarm;
    alarm.duration_sec = 10;
    if (!executor.Execute(alarm).ok() ||
        !storage.GetStats().recording ||
        !device.IsFillLightOn() ||
        !device.IsSoundAlarmOn()) {
        return Fail("executor must start local record, fill light and sound alarm for one-key alarm");
    }

    CoreCommand reset;
    reset.type = CoreCommandType::SetDefaultDevConfigs;
    if (!executor.Execute(reset).ok() ||
        !config.GetPendingConfigChange().changed_fields.empty()) {
        return Fail("executor must reset desired config to defaults");
    }

    CoreCommand format;
    format.type = CoreCommandType::FormatStorageMedium;
    format.storage_medium = "sdcard";
    if (!executor.Execute(format).ok() ||
        sys_storage.GetCapacity().free_mb != sys_storage.GetCapacity().total_mb) {
        return Fail("executor must format SD card through libsys storage service");
    }

    CoreCommand reboot;
    reboot.type = CoreCommandType::Reboot;
    reboot.delay_sec = 3;
    if (!executor.Execute(reboot).ok() ||
        !device.HasRebootRequested() ||
        device.GetRebootDelaySec() != 3) {
        return Fail("executor must reboot through libsys device service when injected");
    }

    std::cout << "core command executor contract passed\n";
    return 0;
}

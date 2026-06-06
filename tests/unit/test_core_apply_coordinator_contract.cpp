#include <iostream>

#include "../../app/config/device_config.hpp"
#include "../../app/core/core_apply_coordinator.hpp"

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
    aov::media::MediaStatusCode Start() override {
        return aov::media::MediaStatusCode::Ok;
    }
    void Stop() override {}

    aov::media::MediaStatusCode ApplyVideoConfig(
        const aov::media::VideoEncodeGroupConfig& config) override {
        ++video_apply_count;
        last_video = config;
        return video_result;
    }
    aov::media::MediaStatusCode ApplyAudioConfig(
        const aov::media::AudioEncodeApplyConfig& config) override {
        ++audio_apply_count;
        last_audio = config;
        return aov::media::MediaStatusCode::Ok;
    }
    aov::media::MediaStatusCode ApplyImageConfig(
        const aov::media::IspImageConfig& config) override {
        ++image_apply_count;
        last_image = config;
        return aov::media::MediaStatusCode::Ok;
    }
    aov::media::MediaStatusCode ApplyDetectConfig(
        const aov::media::DetectApplyConfig& config) override {
        ++detect_apply_count;
        last_detect = config;
        return aov::media::MediaStatusCode::Ok;
    }
    aov::media::MediaStatusCode ApplyOsdConfig(
        const aov::media::OsdApplyConfig& config) override {
        ++osd_apply_count;
        last_osd = config;
        return aov::media::MediaStatusCode::Ok;
    }
    aov::media::MediaStatusCode PrepareForAovSleep(
        const aov::media::AovSleepMediaConfig&) override {
        return aov::media::MediaStatusCode::Ok;
    }
    aov::media::MediaStatusCode RestoreFromAovSleep(
        const aov::media::WakeupMediaProfile&) override {
        return aov::media::MediaStatusCode::Ok;
    }
    aov::media::MediaStatusCode EnterLowPowerMode(
        const aov::media::LowPowerRecordProfile& profile) override {
        ++low_power_apply_count;
        last_low_power = profile;
        return aov::media::MediaStatusCode::Ok;
    }
    aov::media::MediaStatusCode ResumeNormalMode() override {
        return aov::media::MediaStatusCode::Ok;
    }
    aov::media::MediaStatusCode CaptureSnapshot(const aov::media::SnapshotRequest&,
                                                aov::media::SnapshotResult&) override {
        return aov::media::MediaStatusCode::Ok;
    }

    aov::media::IIspService* GetIspService() override { return nullptr; }
    aov::media::IVideoStreamService* GetVideoStreamService() override { return nullptr; }
    aov::media::IAudioStreamService* GetAudioStreamService() override { return nullptr; }
    aov::media::IDetectService* GetDetectService() override { return nullptr; }
    aov::media::IOsdService* GetOsdService() override { return nullptr; }
    bool IsRunning() const override { return true; }
    aov::media::MediaRuntimeStatus GetRuntimeStatus() const override { return {}; }

    int video_apply_count {0};
    int audio_apply_count {0};
    int image_apply_count {0};
    int detect_apply_count {0};
    int osd_apply_count {0};
    int low_power_apply_count {0};
    aov::media::MediaStatusCode video_result {aov::media::MediaStatusCode::Ok};
    aov::media::VideoEncodeGroupConfig last_video;
    aov::media::AudioEncodeApplyConfig last_audio;
    aov::media::IspImageConfig last_image;
    aov::media::DetectApplyConfig last_detect;
    aov::media::OsdApplyConfig last_osd;
    aov::media::LowPowerRecordProfile last_low_power;
};

class FakeStorageService final : public aov::app::storage::IStorageService {
public:
    aov::app::storage::StorageResult Init(
        const aov::app::storage::StorageConfig& config) override {
        ++init_count;
        last_config = config;
        return init_result;
    }
    void Deinit() override {}
    aov::app::storage::StorageResult StartRecord() override {
        return aov::app::storage::StorageResult::Ok();
    }
    aov::app::storage::StorageResult StopRecord() override {
        return aov::app::storage::StorageResult::Ok();
    }
    aov::app::storage::StorageResult OnPacketFrame(
        const aov::app::packet::PacketFrame&) override {
        return aov::app::storage::StorageResult::Ok();
    }
    aov::app::storage::StorageResult FlushAndClose() override {
        return aov::app::storage::StorageResult::Ok();
    }
    aov::app::storage::StorageStats GetStats() const override {
        return {};
    }

    int init_count {0};
    aov::app::storage::StorageResult init_result {aov::app::storage::StorageResult::Ok()};
    aov::app::storage::StorageConfig last_config;
};

} // namespace

int main() {
    using namespace aov::app::config;
    using namespace aov::app::core;

    DeviceConfig config = MakeDefaultDeviceConfig();
    config.media.main_stream.codec = VideoEncodeType::H265;
    config.media.main_stream.width = 2560;
    config.media.main_stream.height = 1440;
    config.media.main_stream.fps_aov = 1;
    config.media.main_stream.fps_normal = 15;
    config.media.main_stream.bitrate_kbps = 1024;
    config.media.sub_stream = MakeDefaultSubStreamConfig();
    config.media.audio.codec = AudioEncodeType::G711A;
    config.media.audio.sample_rate_khz = 8;
    config.work_mode.record_interval_sec = 3;
    config.alarm.human.enabled = true;
    config.alarm.vehicle.enabled = true;
    config.image.brightness = 61;
    config.image.contrast = 62;
    config.image.saturation = 63;
    config.image.sharpness = 64;
    config.image.mirror = true;
    config.osd.show_time = true;
    config.osd.show_name = true;
    config.osd.channel_name = "AOV-01";
    config.record.sd.loop_record = false;

    FakeMediaRuntime media;
    FakeStorageService storage;
    CoreApplyCoordinator coordinator(media, storage);

    CoreApplyResult result = coordinator.ApplyDeviceConfig(config);
    if (!result.ok()) {
        return Fail("apply device config must succeed when all modules succeed");
    }
    if (media.video_apply_count != 1 ||
        media.audio_apply_count != 1 ||
        media.image_apply_count != 1 ||
        media.detect_apply_count != 1 ||
        media.osd_apply_count != 1 ||
        media.low_power_apply_count != 1 ||
        storage.init_count != 1) {
        return Fail("coordinator must call every stage-4a apply interface exactly once");
    }

    if (media.last_video.main_stream.codec != aov::media::VideoCodec::H265 ||
        media.last_video.main_stream.resolution.width != 2560 ||
        media.last_video.main_stream.resolution.height != 1440 ||
        media.last_video.main_stream.fps_aov != 1 ||
        media.last_video.sub_stream.profile != aov::media::StreamProfile::Sub ||
        media.last_video.sub_stream.bitrate_kbps != 256) {
        return Fail("video config must map app config to libmedia DTO");
    }

    if (media.last_audio.codec != aov::media::AudioCodec::G711A ||
        media.last_audio.sample_rate_hz != 8000 ||
        media.last_audio.input_volume != 100) {
        return Fail("audio config must map app config to libmedia DTO");
    }

    if (media.last_image.brightness != 61 ||
        media.last_image.contrast != 62 ||
        media.last_image.saturation != 63 ||
        media.last_image.sharpen.strength != 64 ||
        !media.last_image.mirror) {
        return Fail("image config must map app config to libmedia DTO");
    }

    if (!media.last_detect.enabled ||
        media.last_detect.rules.size() < 2 ||
        media.last_osd.items.empty() ||
        media.last_low_power.target_frame_count != 1 ||
        media.last_low_power.record_fps != 1 ||
        media.last_low_power.normal_fps != 15 ||
        storage.last_config.loop_record) {
        return Fail("detect/osd/low-power/storage config must map expected fields");
    }

    media.video_result = aov::media::MediaStatusCode::InvalidArgument;
    result = coordinator.ApplyDeviceConfig(config);
    if (result.ok() ||
        result.failed_module != "libmedia.video") {
        return Fail("coordinator must return explicit failed module when apply fails");
    }

    std::cout << "core apply coordinator contract passed\n";
    return 0;
}

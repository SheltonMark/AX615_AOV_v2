#include "app/core/core_apply_coordinator.hpp"

#include <utility>

namespace aov::app::core {

namespace {

CoreApplyResult Ok() {
    return {};
}

CoreApplyResult MediaError(std::string module, aov::media::MediaStatusCode code) {
    CoreApplyResult result;
    result.code = CoreApplyStatusCode::MediaApplyFailed;
    result.failed_module = std::move(module);
    result.message = "libmedia apply failed: " + std::to_string(static_cast<int>(code));
    return result;
}

CoreApplyResult StorageError(std::string module,
                             const aov::app::storage::StorageResult& storage_result) {
    CoreApplyResult result;
    result.code = CoreApplyStatusCode::StorageApplyFailed;
    result.failed_module = std::move(module);
    result.message = storage_result.message;
    return result;
}

aov::media::VideoCodec MapVideoCodec(const aov::app::config::VideoEncodeType codec) {
    switch (codec) {
        case aov::app::config::VideoEncodeType::H264:
            return aov::media::VideoCodec::H264;
        case aov::app::config::VideoEncodeType::H265:
            return aov::media::VideoCodec::H265;
    }
    return aov::media::VideoCodec::H265;
}

aov::media::RcMode MapRcMode(const aov::app::config::BitrateControlMode mode) {
    switch (mode) {
        case aov::app::config::BitrateControlMode::CBR:
            return aov::media::RcMode::CBR;
        case aov::app::config::BitrateControlMode::VBR:
            return aov::media::RcMode::VBR;
    }
    return aov::media::RcMode::VBR;
}

aov::media::AudioCodec MapAudioCodec(const aov::app::config::AudioEncodeType codec) {
    switch (codec) {
        case aov::app::config::AudioEncodeType::G711A:
            return aov::media::AudioCodec::G711A;
        case aov::app::config::AudioEncodeType::G711U:
            return aov::media::AudioCodec::G711U;
        case aov::app::config::AudioEncodeType::AAC:
            return aov::media::AudioCodec::AAC;
    }
    return aov::media::AudioCodec::G711A;
}

aov::media::StreamProfile MapStreamProfile(const aov::app::config::StreamProfile profile) {
    switch (profile) {
        case aov::app::config::StreamProfile::Main:
            return aov::media::StreamProfile::Main;
        case aov::app::config::StreamProfile::Sub:
            return aov::media::StreamProfile::Sub;
    }
    return aov::media::StreamProfile::Unknown;
}

aov::media::VideoEncodeApplyConfig MapVideoStream(
    const aov::app::config::VideoStreamConfig& config,
    const int fallback_chn_id) {
    aov::media::VideoEncodeApplyConfig media_config;
    media_config.profile = MapStreamProfile(config.stream);
    media_config.chn_id = fallback_chn_id;
    media_config.codec = MapVideoCodec(config.codec);
    media_config.resolution = {config.width, config.height};
    media_config.fps_normal = config.fps_normal;
    media_config.fps_aov = config.fps_aov;
    media_config.rc_mode = MapRcMode(config.bitrate_mode);
    media_config.bitrate_kbps = config.bitrate_kbps;
    media_config.gop = config.gop;
    media_config.enabled = true;
    return media_config;
}

aov::media::VideoEncodeGroupConfig MapVideoGroup(
    const aov::app::config::MediaEncodeConfig& config) {
    aov::media::VideoEncodeGroupConfig media_config;
    media_config.main_stream = MapVideoStream(config.main_stream, 0);
    media_config.sub_stream = MapVideoStream(config.sub_stream, 1);
    return media_config;
}

aov::media::AudioEncodeApplyConfig MapAudio(
    const aov::app::config::AudioConfig& config) {
    aov::media::AudioEncodeApplyConfig media_config;
    media_config.enabled = config.enabled;
    media_config.codec = MapAudioCodec(config.codec);
    media_config.sample_rate_hz = config.sample_rate_khz * 1000;
    media_config.bit_depth = config.bit_depth;
    media_config.channels = 1;
    media_config.input_volume = config.input_volume;
    media_config.output_volume = config.output_volume;
    return media_config;
}

aov::media::DetectTargetType MapDetectTarget(
    const aov::app::config::DetectTargetType type) {
    switch (type) {
        case aov::app::config::DetectTargetType::Human:
            return aov::media::DetectTargetType::Human;
        case aov::app::config::DetectTargetType::Vehicle:
            return aov::media::DetectTargetType::Vehicle;
        case aov::app::config::DetectTargetType::NonMotorVehicle:
            return aov::media::DetectTargetType::NonMotorVehicle;
        case aov::app::config::DetectTargetType::Motion:
            return aov::media::DetectTargetType::Motion;
        case aov::app::config::DetectTargetType::Occlusion:
            return aov::media::DetectTargetType::Occlusion;
    }
    return aov::media::DetectTargetType::Human;
}

aov::media::DetectRuleApplyConfig MapDetectRule(
    const aov::app::config::DetectRuleConfig& config) {
    aov::media::DetectRuleApplyConfig media_config;
    media_config.type = MapDetectTarget(config.type);
    media_config.enabled = config.enabled;
    media_config.pipe_id = 0;
    media_config.ivps_grp_id = 0;
    media_config.ivps_chn_id = 0;
    media_config.region.full_screen = config.region.full_screen;
    media_config.region.motion_cells = config.region.points_or_cells;
    media_config.sensitivity = config.sensitivity;
    media_config.static_filter = config.static_filter;
    return media_config;
}

aov::media::DetectApplyConfig MapDetect(const aov::app::config::AlarmConfig& config) {
    aov::media::DetectApplyConfig media_config;
    media_config.pipe_id = 0;
    media_config.ivps_grp_id = 0;
    media_config.ivps_chn_id = 0;
    media_config.rules.push_back(MapDetectRule(config.human));
    media_config.rules.push_back(MapDetectRule(config.vehicle));
    media_config.rules.push_back(MapDetectRule(config.non_motor_vehicle));
    media_config.rules.push_back(MapDetectRule(config.motion));
    media_config.rules.push_back(MapDetectRule(config.occlusion));
    for (const auto& rule : media_config.rules) {
        media_config.enabled = media_config.enabled || rule.enabled;
    }
    return media_config;
}

aov::media::IspImageConfig MapImage(const aov::app::config::ImageParamConfig& config) {
    aov::media::IspImageConfig media_config;
    media_config.pipe_id = 0;
    media_config.brightness = config.brightness;
    media_config.contrast = config.contrast;
    media_config.saturation = config.saturation;
    media_config.sharpen.strength = config.sharpness;
    media_config.mirror = config.mirror;
    media_config.scene.anti_flicker =
        config.anti_flicker ? aov::media::AntiFlickerMode::Hz50 :
                              aov::media::AntiFlickerMode::Off;
    return media_config;
}

void AddOsdItem(aov::media::OsdApplyConfig& osd,
                const aov::media::OsdItemType type,
                const bool enabled,
                const std::string& text) {
    aov::media::OsdItemConfig item;
    item.type = type;
    item.enabled = enabled;
    item.text = text;
    osd.items.push_back(item);
}

aov::media::OsdApplyConfig MapOsd(const aov::app::config::OsdConfig& config) {
    aov::media::OsdApplyConfig media_config;
    media_config.enabled = config.show_name ||
                           config.show_time ||
                           config.show_week ||
                           config.show_tenda_logo;
    AddOsdItem(media_config, aov::media::OsdItemType::ChannelName,
               config.show_name, config.channel_name);
    AddOsdItem(media_config, aov::media::OsdItemType::Time,
               config.show_time, config.time_format);
    AddOsdItem(media_config, aov::media::OsdItemType::Week,
               config.show_week, {});
    AddOsdItem(media_config, aov::media::OsdItemType::Logo,
               config.show_tenda_logo, "tenda");
    return media_config;
}

aov::media::LowPowerRecordProfile MapLowPower(
    const aov::app::config::DeviceConfig& config) {
    aov::media::LowPowerRecordProfile profile;
    profile.target_frame_count = 1;
    profile.record_fps = config.media.main_stream.fps_aov;
    profile.normal_fps = config.media.main_stream.fps_normal;
    profile.gop = config.media.main_stream.gop;
    return profile;
}

aov::app::storage::StorageConfig MapStorage(
    const aov::app::config::RecordConfig& config) {
    aov::app::storage::StorageConfig storage_config;
    storage_config.loop_record = config.sd.loop_record;
    return storage_config;
}

} // namespace

CoreApplyCoordinator::CoreApplyCoordinator(
    aov::media::IMediaRuntime& media_runtime,
    aov::app::storage::IStorageService& storage_service)
    : media_runtime_(media_runtime),
      storage_service_(storage_service) {}

CoreApplyResult CoreApplyCoordinator::ApplyDeviceConfig(
    const aov::app::config::DeviceConfig& config) {
    CoreApplyResult result = ApplyMediaConfig(config);
    if (!result.ok()) {
        return result;
    }

    return ApplyStorageConfig(config.record);
}

CoreApplyResult CoreApplyCoordinator::ApplyMediaConfig(
    const aov::app::config::DeviceConfig& config) {
    auto status = media_runtime_.ApplyVideoConfig(MapVideoGroup(config.media));
    if (status != aov::media::MediaStatusCode::Ok) {
        return MediaError("libmedia.video", status);
    }

    status = media_runtime_.ApplyAudioConfig(MapAudio(config.media.audio));
    if (status != aov::media::MediaStatusCode::Ok) {
        return MediaError("libmedia.audio", status);
    }

    status = media_runtime_.ApplyImageConfig(MapImage(config.image));
    if (status != aov::media::MediaStatusCode::Ok) {
        return MediaError("libmedia.image", status);
    }

    status = media_runtime_.ApplyDetectConfig(MapDetect(config.alarm));
    if (status != aov::media::MediaStatusCode::Ok) {
        return MediaError("libmedia.detect", status);
    }

    status = media_runtime_.ApplyOsdConfig(MapOsd(config.osd));
    if (status != aov::media::MediaStatusCode::Ok) {
        return MediaError("libmedia.osd", status);
    }

    status = media_runtime_.EnterLowPowerMode(MapLowPower(config));
    if (status != aov::media::MediaStatusCode::Ok) {
        return MediaError("libmedia.low_power", status);
    }

    return Ok();
}

CoreApplyResult CoreApplyCoordinator::ApplyStorageConfig(
    const aov::app::config::RecordConfig& config) {
    const auto result = storage_service_.Init(MapStorage(config));
    if (!result.ok()) {
        return StorageError("app.storage", result);
    }

    return Ok();
}

} // namespace aov::app::core

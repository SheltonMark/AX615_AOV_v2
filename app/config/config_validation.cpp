#include "config_validation.hpp"

namespace aov::app::config {

namespace {

bool IsOneOfAovIntervals(int value) {
    return value == 1 || value == 3 || value == 5;
}

bool IsOneOfAovFps(int value) {
    return value == 1 || value == 2 || value == 3;
}

bool IsInRange(int value, int min_value, int max_value) {
    return value >= min_value && value <= max_value;
}

ConfigResult ValidateVideoStream(const VideoStreamConfig& stream, const char* prefix) {
    if (!IsOneOfAovFps(stream.fps_aov)) {
        return ConfigResult::Error(ConfigStatusCode::InvalidArgument,
                                   std::string(prefix) + ".fps_aov",
                                   "AOV fps must be 1, 2, or 3");
    }
    if (!IsInRange(stream.fps_normal, 1, 15)) {
        return ConfigResult::Error(ConfigStatusCode::InvalidArgument,
                                   std::string(prefix) + ".fps_normal",
                                   "normal fps must be in range 1-15");
    }
    if (stream.width <= 0 || stream.height <= 0) {
        return ConfigResult::Error(ConfigStatusCode::InvalidArgument,
                                   std::string(prefix) + ".resolution",
                                   "resolution must be positive");
    }
    if (stream.bitrate_kbps <= 0) {
        return ConfigResult::Error(ConfigStatusCode::InvalidArgument,
                                   std::string(prefix) + ".bitrate_kbps",
                                   "bitrate must be positive");
    }
    return ConfigResult::Ok();
}

ConfigResult ValidateDetectRule(const DetectRuleConfig& rule, const char* prefix) {
    if (!IsInRange(rule.sensitivity, 1, 100)) {
        return ConfigResult::Error(ConfigStatusCode::InvalidArgument,
                                   std::string(prefix) + ".sensitivity",
                                   "sensitivity must be in range 1-100");
    }
    if (rule.debounce_sec < 0) {
        return ConfigResult::Error(ConfigStatusCode::InvalidArgument,
                                   std::string(prefix) + ".debounce_sec",
                                   "debounce must not be negative");
    }
    return ConfigResult::Ok();
}

} // namespace

ConfigResult ValidateDeviceConfig(const DeviceConfig& config) {
    if (!IsOneOfAovIntervals(config.work_mode.record_interval_sec)) {
        return ConfigResult::Error(ConfigStatusCode::InvalidArgument,
                                   "work_mode.record_interval_sec",
                                   "AOV record interval must be 1, 3, or 5 seconds");
    }
    if (!IsInRange(config.work_mode.low_battery_protect_threshold_pct, 5, 30)) {
        return ConfigResult::Error(ConfigStatusCode::InvalidArgument,
                                   "work_mode.low_battery_protect_threshold_pct",
                                   "low battery threshold must be in range 5-30");
    }
    if (config.work_mode.smart_enter_power_saving_pct >=
        config.work_mode.smart_resume_no_sleep_pct) {
        return ConfigResult::Error(ConfigStatusCode::InvalidArgument,
                                   "work_mode.smart_battery_thresholds",
                                   "smart enter threshold must be lower than resume threshold");
    }

    auto result = ValidateVideoStream(config.media.main_stream, "media.main_stream");
    if (!result.ok()) {
        return result;
    }
    result = ValidateVideoStream(config.media.sub_stream, "media.sub_stream");
    if (!result.ok()) {
        return result;
    }
    if (!IsInRange(config.media.audio.input_volume, 0, 100) ||
        !IsInRange(config.media.audio.output_volume, 0, 100)) {
        return ConfigResult::Error(ConfigStatusCode::InvalidArgument,
                                   "media.audio.volume",
                                   "audio volume must be in range 0-100");
    }

    result = ValidateDetectRule(config.alarm.human, "alarm.human");
    if (!result.ok()) {
        return result;
    }
    result = ValidateDetectRule(config.alarm.vehicle, "alarm.vehicle");
    if (!result.ok()) {
        return result;
    }
    result = ValidateDetectRule(config.alarm.non_motor_vehicle, "alarm.non_motor_vehicle");
    if (!result.ok()) {
        return result;
    }
    result = ValidateDetectRule(config.alarm.motion, "alarm.motion");
    if (!result.ok()) {
        return result;
    }
    result = ValidateDetectRule(config.alarm.occlusion, "alarm.occlusion");
    if (!result.ok()) {
        return result;
    }

    if (config.record.sd.min_capacity_gb <= 0 ||
        config.record.sd.max_capacity_gb < config.record.sd.min_capacity_gb) {
        return ConfigResult::Error(ConfigStatusCode::InvalidArgument,
                                   "record.sd.capacity",
                                   "invalid SD capacity range");
    }
    if (!(config.record.schedule.alarm_post_record_sec == 3 ||
          config.record.schedule.alarm_post_record_sec == 5 ||
          config.record.schedule.alarm_post_record_sec == 10)) {
        return ConfigResult::Error(ConfigStatusCode::InvalidArgument,
                                   "record.schedule.alarm_post_record_sec",
                                   "alarm post record seconds must be 3, 5, or 10");
    }

    return ConfigResult::Ok();
}

} // namespace aov::app::config

#include "app/cloud/cloud_property_mapper.hpp"

#include <cstdlib>
#include <map>

namespace aov::app::cloud {

namespace {

CloudPropertyMapResult Error(const CloudPropertyUpdate& update, const char* message) {
    return CloudPropertyMapResult::Error(CloudConfigStatusCode::InvalidArgument,
                                         update.property_id,
                                         message);
}

bool ParseInt(const std::string& value, int& out) {
    char* end = nullptr;
    const long parsed = std::strtol(value.c_str(), &end, 10);
    if (end == value.c_str() || *end != '\0') {
        return false;
    }
    out = static_cast<int>(parsed);
    return true;
}

bool ParseBool(const std::string& value, bool& out) {
    if (value == "true" || value == "1" || value == "on") {
        out = true;
        return true;
    }
    if (value == "false" || value == "0" || value == "off") {
        out = false;
        return true;
    }
    return false;
}

bool ParseResolution(const std::string& value, int& width, int& height) {
    const std::size_t pos = value.find('x');
    if (pos == std::string::npos) {
        return false;
    }

    int parsed_width = 0;
    int parsed_height = 0;
    if (!ParseInt(value.substr(0, pos), parsed_width) ||
        !ParseInt(value.substr(pos + 1), parsed_height) ||
        parsed_width <= 0 || parsed_height <= 0) {
        return false;
    }

    width = parsed_width;
    height = parsed_height;
    return true;
}

bool IsSupportedAovInterval(const int value) {
    return value == 1 || value == 3 || value == 5;
}

bool IsInRange(const int value, const int min_value, const int max_value) {
    return value >= min_value && value <= max_value;
}

CloudPropertyMapResult FieldError(const CloudPropertyUpdate& update,
                                  const char* suffix,
                                  const char* message) {
    return CloudPropertyMapResult::Error(
        CloudConfigStatusCode::InvalidArgument,
        update.property_id + std::string(".") + suffix,
        message);
}

bool ParseKeyValuePairs(const std::string& value, std::map<std::string, std::string>& out) {
    std::size_t start = 0;
    while (start <= value.size()) {
        const std::size_t end = value.find(';', start);
        const std::string item = value.substr(
            start,
            end == std::string::npos ? std::string::npos : end - start);
        if (!item.empty()) {
            const std::size_t eq = item.find('=');
            if (eq == std::string::npos || eq == 0 || eq + 1 >= item.size()) {
                return false;
            }
            out[item.substr(0, eq)] = item.substr(eq + 1);
        }
        if (end == std::string::npos) {
            break;
        }
        start = end + 1;
    }
    return true;
}

bool ReadBool(const std::map<std::string, std::string>& values,
              const char* key,
              bool& out) {
    const auto it = values.find(key);
    if (it == values.end()) {
        return true;
    }
    return ParseBool(it->second, out);
}

bool ReadInt(const std::map<std::string, std::string>& values,
             const char* key,
             int& out) {
    const auto it = values.find(key);
    if (it == values.end()) {
        return true;
    }
    return ParseInt(it->second, out);
}

CloudPropertyMapResult MapWorkMode(const CloudPropertyUpdate& update) {
    MappedCloudProperty property;
    property.id = MappedCloudPropertyId::AovWorkMode;
    property.raw_value = update.value;
    if (update.value == "PowerSaving") {
        property.work_mode = aov::app::config::ConfiguredWorkMode::PowerSaving;
    } else if (update.value == "SmartNoSleep") {
        property.work_mode = aov::app::config::ConfiguredWorkMode::SmartNoSleep;
    } else if (update.value == "UltraLongStandby") {
        property.work_mode = aov::app::config::ConfiguredWorkMode::UltraLongStandby;
    } else {
        return Error(update, "unsupported AOV work mode");
    }
    return CloudPropertyMapResult::Ok(property);
}

CloudPropertyMapResult MapIntProperty(const CloudPropertyUpdate& update,
                                      const MappedCloudPropertyId id) {
    int value = 0;
    if (!ParseInt(update.value, value)) {
        return Error(update, "value must be integer");
    }

    MappedCloudProperty property;
    property.id = id;
    property.raw_value = update.value;
    property.int_value = value;
    return CloudPropertyMapResult::Ok(property);
}

CloudPropertyMapResult MapVideoEncoding(const CloudPropertyUpdate& update) {
    MappedCloudProperty property;
    property.id = MappedCloudPropertyId::VideoEncoding;
    property.raw_value = update.value;
    if (update.value == "H264") {
        property.video_encode = aov::app::config::VideoEncodeType::H264;
    } else if (update.value == "H265") {
        property.video_encode = aov::app::config::VideoEncodeType::H265;
    } else {
        return Error(update, "unsupported video encoding");
    }
    return CloudPropertyMapResult::Ok(property);
}

CloudPropertyMapResult MapVideoResolution(const CloudPropertyUpdate& update) {
    MappedCloudProperty property;
    property.id = MappedCloudPropertyId::VideoResolution;
    property.raw_value = update.value;
    if (!ParseResolution(update.value, property.width, property.height)) {
        return Error(update, "resolution must be WIDTHxHEIGHT");
    }
    return CloudPropertyMapResult::Ok(property);
}

CloudPropertyMapResult MapBoolProperty(const CloudPropertyUpdate& update,
                                       const MappedCloudPropertyId id) {
    bool value = false;
    if (!ParseBool(update.value, value)) {
        return Error(update, "value must be boolean");
    }

    MappedCloudProperty property;
    property.id = id;
    property.raw_value = update.value;
    property.bool_value = value;
    return CloudPropertyMapResult::Ok(property);
}

CloudPropertyMapResult MapStorageRecordMode(const CloudPropertyUpdate& update) {
    MappedCloudProperty property;
    property.id = MappedCloudPropertyId::StorageRecordMode;
    property.raw_value = update.value;
    if (update.value == "Aov") {
        property.record_type = aov::app::config::RecordType::Aov;
    } else if (update.value == "Normal") {
        property.record_type = aov::app::config::RecordType::Normal;
    } else if (update.value == "Alarm") {
        property.record_type = aov::app::config::RecordType::Alarm;
    } else {
        return Error(update, "unsupported storage record mode");
    }
    return CloudPropertyMapResult::Ok(property);
}

CloudPropertyMapResult MapDetectRule(const CloudPropertyUpdate& update,
                                     const MappedCloudPropertyId id,
                                     const aov::app::config::DetectTargetType type) {
    std::map<std::string, std::string> values;
    if (!ParseKeyValuePairs(update.value, values)) {
        return Error(update, "detect config must be key=value pairs");
    }

    MappedCloudProperty property;
    property.id = id;
    property.raw_value = update.value;
    property.detect_rule.type = type;
    property.detect_rule.enabled = true;
    property.detect_rule.sensitivity = 60;
    property.detect_rule.debounce_sec = 10;
    property.detect_rule.linkage.trigger_record = true;
    property.detect_rule.linkage.push_message = true;

    if (!ReadBool(values, "enabled", property.detect_rule.enabled)) {
        return FieldError(update, "enabled", "enabled must be boolean");
    }
    if (!ReadInt(values, "sensitivity", property.detect_rule.sensitivity)) {
        return FieldError(update, "sensitivity", "sensitivity must be integer");
    }
    if (!IsInRange(property.detect_rule.sensitivity, 1, 100)) {
        return FieldError(update, "sensitivity", "sensitivity must be in range 1-100");
    }
    if (!ReadInt(values, "debounce", property.detect_rule.debounce_sec)) {
        return FieldError(update, "debounce", "debounce must be integer");
    }
    if (property.detect_rule.debounce_sec < 0) {
        return FieldError(update, "debounce", "debounce must not be negative");
    }
    if (!ReadBool(values, "trigger_record", property.detect_rule.linkage.trigger_record)) {
        return FieldError(update, "trigger_record", "trigger_record must be boolean");
    }
    if (!ReadBool(values, "push_message", property.detect_rule.linkage.push_message)) {
        return FieldError(update, "push_message", "push_message must be boolean");
    }

    return CloudPropertyMapResult::Ok(property);
}

CloudPropertyMapResult MapSoundLightAlarmConfig(const CloudPropertyUpdate& update) {
    std::map<std::string, std::string> values;
    if (!ParseKeyValuePairs(update.value, values)) {
        return Error(update, "sound light config must be key=value pairs");
    }

    MappedCloudProperty property;
    property.id = MappedCloudPropertyId::SoundLightAlarmConfig;
    property.raw_value = update.value;
    property.white_light_alarm = false;
    property.sound_alarm = false;
    property.white_light_duration_sec = 10;
    property.sound_repeat_count = 1;
    property.alarm_snapshot_enabled = true;
    property.target_box_overlay = false;

    if (!ReadBool(values, "white_light_alarm", property.white_light_alarm)) {
        return FieldError(update, "white_light_alarm", "white_light_alarm must be boolean");
    }
    if (!ReadBool(values, "sound_alarm", property.sound_alarm)) {
        return FieldError(update, "sound_alarm", "sound_alarm must be boolean");
    }
    if (!ReadInt(values, "duration", property.white_light_duration_sec)) {
        return FieldError(update, "duration", "duration must be integer");
    }
    if (!IsInRange(property.white_light_duration_sec, 1, 300)) {
        return FieldError(update, "duration", "duration must be in range 1-300");
    }
    if (!ReadInt(values, "sound_repeat", property.sound_repeat_count)) {
        return FieldError(update, "sound_repeat", "sound_repeat must be integer");
    }
    if (!IsInRange(property.sound_repeat_count, 1, 10)) {
        return FieldError(update, "sound_repeat", "sound_repeat must be in range 1-10");
    }
    if (!ReadBool(values, "snapshot", property.alarm_snapshot_enabled)) {
        return FieldError(update, "snapshot", "snapshot must be boolean");
    }
    if (!ReadBool(values, "target_box", property.target_box_overlay)) {
        return FieldError(update, "target_box", "target_box must be boolean");
    }

    return CloudPropertyMapResult::Ok(property);
}

} // namespace

CloudPropertyMapResult ApplyCloudProperty(const CloudPropertyUpdate& update) {
    if (update.property_id == "AovWorkMode") {
        return MapWorkMode(update);
    }
    if (update.property_id == "AovRecordInterval") {
        auto result = MapIntProperty(update, MappedCloudPropertyId::AovRecordInterval);
        if (result.ok() && !IsSupportedAovInterval(result.value.int_value)) {
            return Error(update, "AOV record interval must be 1, 3, or 5");
        }
        return result;
    }
    if (update.property_id == "LowBatteryProtectThreshold") {
        return MapIntProperty(update, MappedCloudPropertyId::LowBatteryProtectThreshold);
    }
    if (update.property_id == "VideoEncoding") {
        return MapVideoEncoding(update);
    }
    if (update.property_id == "VideoResolution") {
        return MapVideoResolution(update);
    }
    if (update.property_id == "VideoFPS") {
        return MapIntProperty(update, MappedCloudPropertyId::VideoFPS);
    }
    if (update.property_id == "VideoBitrate") {
        return MapIntProperty(update, MappedCloudPropertyId::VideoBitrate);
    }
    if (update.property_id == "CloudStorageSwitch") {
        return MapBoolProperty(update, MappedCloudPropertyId::CloudStorageSwitch);
    }
    if (update.property_id == "StorageRecordMode") {
        return MapStorageRecordMode(update);
    }
    if (update.property_id == "HumanDetectConfig") {
        return MapDetectRule(update,
                             MappedCloudPropertyId::HumanDetectConfig,
                             aov::app::config::DetectTargetType::Human);
    }
    if (update.property_id == "ImageDetectConfig") {
        return MapDetectRule(update,
                             MappedCloudPropertyId::ImageDetectConfig,
                             aov::app::config::DetectTargetType::Motion);
    }
    if (update.property_id == "SoundLightAlarmConfig") {
        return MapSoundLightAlarmConfig(update);
    }

    return CloudPropertyMapResult::Error(CloudConfigStatusCode::UnsupportedProperty,
                                         update.property_id,
                                         "unsupported cloud property");
}

CloudPropertyApplyResult ApplyCloudPropertyToConfig(
    const MappedCloudProperty& property,
    aov::app::config::DeviceConfig& config) {
    switch (property.id) {
        case MappedCloudPropertyId::AovWorkMode:
            config.work_mode.mode = property.work_mode;
            break;
        case MappedCloudPropertyId::AovRecordInterval:
            config.work_mode.record_interval_sec = property.int_value;
            break;
        case MappedCloudPropertyId::LowBatteryProtectThreshold:
            config.work_mode.low_battery_protect_threshold_pct = property.int_value;
            break;
        case MappedCloudPropertyId::VideoEncoding:
            config.media.main_stream.codec = property.video_encode;
            break;
        case MappedCloudPropertyId::VideoResolution:
            config.media.main_stream.width = property.width;
            config.media.main_stream.height = property.height;
            break;
        case MappedCloudPropertyId::VideoFPS:
            config.media.main_stream.fps_normal = property.int_value;
            break;
        case MappedCloudPropertyId::VideoBitrate:
            config.media.main_stream.bitrate_kbps = property.int_value;
            break;
        case MappedCloudPropertyId::CloudStorageSwitch:
            config.record.cloud_storage.enabled = property.bool_value;
            break;
        case MappedCloudPropertyId::StorageRecordMode:
            config.record.schedule.default_record_type = property.record_type;
            break;
        case MappedCloudPropertyId::HumanDetectConfig:
            config.alarm.human = property.detect_rule;
            break;
        case MappedCloudPropertyId::ImageDetectConfig:
            config.alarm.motion = property.detect_rule;
            break;
        case MappedCloudPropertyId::SoundLightAlarmConfig:
            config.alarm.human.linkage.white_light_alarm = property.white_light_alarm;
            config.alarm.human.linkage.sound_alarm = property.sound_alarm;
            config.alarm.vehicle.linkage.white_light_alarm = property.white_light_alarm;
            config.alarm.vehicle.linkage.sound_alarm = property.sound_alarm;
            config.alarm.motion.linkage.white_light_alarm = property.white_light_alarm;
            config.alarm.motion.linkage.sound_alarm = property.sound_alarm;
            config.alarm.white_light_duration_sec = property.white_light_duration_sec;
            config.alarm.sound_repeat_count = property.sound_repeat_count;
            config.alarm.alarm_snapshot_enabled = property.alarm_snapshot_enabled;
            config.alarm.target_box_overlay = property.target_box_overlay;
            break;
        default:
            return CloudPropertyApplyResult::Error(CloudConfigStatusCode::UnsupportedProperty,
                                                   "MappedCloudPropertyId",
                                                   "unsupported mapped property");
    }

    return CloudPropertyApplyResult::Ok(true);
}

} // namespace aov::app::cloud

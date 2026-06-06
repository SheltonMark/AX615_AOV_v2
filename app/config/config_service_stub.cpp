#include "config_service_stub.hpp"

#include <fstream>
#include <map>
#include <sstream>
#include <utility>

namespace aov::app::config {

namespace {

std::string BoolToString(const bool value) {
    return value ? "true" : "false";
}

bool StringToBool(const std::string& value, bool& out) {
    if (value == "true" || value == "1") {
        out = true;
        return true;
    }
    if (value == "false" || value == "0") {
        out = false;
        return true;
    }
    return false;
}

std::string WorkModeToString(const ConfiguredWorkMode mode) {
    switch (mode) {
        case ConfiguredWorkMode::PowerSaving:
            return "PowerSaving";
        case ConfiguredWorkMode::SmartNoSleep:
            return "SmartNoSleep";
        case ConfiguredWorkMode::UltraLongStandby:
            return "UltraLongStandby";
    }
    return "PowerSaving";
}

bool StringToWorkMode(const std::string& value, ConfiguredWorkMode& out) {
    if (value == "PowerSaving") {
        out = ConfiguredWorkMode::PowerSaving;
        return true;
    }
    if (value == "SmartNoSleep") {
        out = ConfiguredWorkMode::SmartNoSleep;
        return true;
    }
    if (value == "UltraLongStandby") {
        out = ConfiguredWorkMode::UltraLongStandby;
        return true;
    }
    return false;
}

std::string VideoCodecToString(const VideoEncodeType codec) {
    return codec == VideoEncodeType::H264 ? "H264" : "H265";
}

bool StringToVideoCodec(const std::string& value, VideoEncodeType& out) {
    if (value == "H264") {
        out = VideoEncodeType::H264;
        return true;
    }
    if (value == "H265") {
        out = VideoEncodeType::H265;
        return true;
    }
    return false;
}

std::map<std::string, std::string> LoadKeyValues(std::istream& input) {
    std::map<std::string, std::string> values;
    std::string line;
    while (std::getline(input, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        const std::size_t pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }
        values[line.substr(0, pos)] = line.substr(pos + 1);
    }
    return values;
}

bool ReadInt(const std::map<std::string, std::string>& values,
             const char* key,
             int& out) {
    const auto it = values.find(key);
    if (it == values.end()) {
        return true;
    }
    std::istringstream stream(it->second);
    int value = 0;
    if (!(stream >> value)) {
        return false;
    }
    out = value;
    return true;
}

bool ReadBool(const std::map<std::string, std::string>& values,
              const char* key,
              bool& out) {
    const auto it = values.find(key);
    if (it == values.end()) {
        return true;
    }
    return StringToBool(it->second, out);
}

} // namespace

ConfigServiceStub::ConfigServiceStub() {
    LoadFactoryConfig();
}

ConfigServiceStub::ConfigServiceStub(std::string persist_path)
    : persist_path_(std::move(persist_path)) {
    LoadFactoryConfig();
}

ConfigResult ConfigServiceStub::LoadFactoryConfig() {
    desired_ = MakeDefaultDeviceConfig();
    active_ = desired_;
    pending_ = {};
    return ConfigResult::Ok();
}

ConfigResult ConfigServiceStub::LoadPersistedConfig() {
    if (persist_path_.empty()) {
        return LoadFactoryConfig();
    }

    std::ifstream input(persist_path_);
    if (!input) {
        return LoadFactoryConfig();
    }

    DeviceConfig loaded = MakeDefaultDeviceConfig();
    const auto values = LoadKeyValues(input);

    auto work_mode = loaded.work_mode.mode;
    auto codec = loaded.media.main_stream.codec;
    if ((values.count("work_mode.mode") &&
         !StringToWorkMode(values.at("work_mode.mode"), work_mode)) ||
        (values.count("media.main.codec") &&
         !StringToVideoCodec(values.at("media.main.codec"), codec))) {
        return ConfigResult::Error(ConfigStatusCode::PersistFailed,
                                   "config.persist",
                                   "invalid persisted enum value");
    }
    loaded.work_mode.mode = work_mode;
    loaded.media.main_stream.codec = codec;

    if (!ReadInt(values, "work_mode.record_interval_sec", loaded.work_mode.record_interval_sec) ||
        !ReadInt(values, "work_mode.low_battery_threshold", loaded.work_mode.low_battery_protect_threshold_pct) ||
        !ReadInt(values, "media.main.width", loaded.media.main_stream.width) ||
        !ReadInt(values, "media.main.height", loaded.media.main_stream.height) ||
        !ReadInt(values, "media.main.fps_normal", loaded.media.main_stream.fps_normal) ||
        !ReadInt(values, "media.main.fps_aov", loaded.media.main_stream.fps_aov) ||
        !ReadInt(values, "media.main.bitrate_kbps", loaded.media.main_stream.bitrate_kbps) ||
        !ReadBool(values, "record.cloud_storage.enabled", loaded.record.cloud_storage.enabled) ||
        !ReadBool(values, "alarm.human.enabled", loaded.alarm.human.enabled) ||
        !ReadInt(values, "alarm.human.sensitivity", loaded.alarm.human.sensitivity) ||
        !ReadInt(values, "alarm.human.debounce_sec", loaded.alarm.human.debounce_sec) ||
        !ReadBool(values, "alarm.human.trigger_record", loaded.alarm.human.linkage.trigger_record) ||
        !ReadBool(values, "alarm.human.push_message", loaded.alarm.human.linkage.push_message) ||
        !ReadBool(values, "alarm.sound_light.white_light", loaded.alarm.human.linkage.white_light_alarm) ||
        !ReadBool(values, "alarm.sound_light.sound", loaded.alarm.human.linkage.sound_alarm) ||
        !ReadInt(values, "alarm.sound_light.duration_sec", loaded.alarm.white_light_duration_sec) ||
        !ReadInt(values, "alarm.sound_light.repeat_count", loaded.alarm.sound_repeat_count)) {
        return ConfigResult::Error(ConfigStatusCode::PersistFailed,
                                   "config.persist",
                                   "invalid persisted numeric or boolean value");
    }

    const ConfigResult validation = ValidateDeviceConfig(loaded);
    if (!validation.ok()) {
        return validation;
    }

    desired_ = loaded;
    active_ = loaded;
    pending_ = {};
    return ConfigResult::Ok();
}

const DeviceConfig& ConfigServiceStub::GetDesiredConfig() const {
    return desired_;
}

const DeviceConfig& ConfigServiceStub::GetActiveConfig() const {
    return active_;
}

PendingConfigChange ConfigServiceStub::GetPendingConfigChange() const {
    return pending_;
}

aov::app::WorkModePolicy ConfigServiceStub::GetDesiredWorkModePolicy() const {
    return BuildWorkModePolicy(desired_);
}

aov::app::WorkModePolicy ConfigServiceStub::GetActiveWorkModePolicy() const {
    return BuildWorkModePolicy(active_);
}

ConfigResult ConfigServiceStub::UpdateDesiredConfig(const DeviceConfig& desired) {
    const ConfigResult validation = ValidateDeviceConfig(desired);
    if (!validation.ok()) {
        return validation;
    }

    desired_ = desired;
    RebuildPendingChange();
    return ConfigResult::Ok();
}

ConfigResult ConfigServiceStub::MarkApplied() {
    active_ = desired_;
    pending_ = {};
    return ConfigResult::Ok();
}

ConfigResult ConfigServiceStub::Persist() {
    if (persist_path_.empty()) {
        return ConfigResult::Ok();
    }

    std::ofstream output(persist_path_, std::ios::trunc);
    if (!output) {
        return ConfigResult::Error(ConfigStatusCode::PersistFailed,
                                   "config.persist",
                                   "open persist file failed");
    }

    output << "work_mode.mode=" << WorkModeToString(active_.work_mode.mode) << '\n';
    output << "work_mode.record_interval_sec=" << active_.work_mode.record_interval_sec << '\n';
    output << "work_mode.low_battery_threshold="
           << active_.work_mode.low_battery_protect_threshold_pct << '\n';
    output << "media.main.codec=" << VideoCodecToString(active_.media.main_stream.codec) << '\n';
    output << "media.main.width=" << active_.media.main_stream.width << '\n';
    output << "media.main.height=" << active_.media.main_stream.height << '\n';
    output << "media.main.fps_normal=" << active_.media.main_stream.fps_normal << '\n';
    output << "media.main.fps_aov=" << active_.media.main_stream.fps_aov << '\n';
    output << "media.main.bitrate_kbps=" << active_.media.main_stream.bitrate_kbps << '\n';
    output << "record.cloud_storage.enabled="
           << BoolToString(active_.record.cloud_storage.enabled) << '\n';
    output << "alarm.human.enabled=" << BoolToString(active_.alarm.human.enabled) << '\n';
    output << "alarm.human.sensitivity=" << active_.alarm.human.sensitivity << '\n';
    output << "alarm.human.debounce_sec=" << active_.alarm.human.debounce_sec << '\n';
    output << "alarm.human.trigger_record="
           << BoolToString(active_.alarm.human.linkage.trigger_record) << '\n';
    output << "alarm.human.push_message="
           << BoolToString(active_.alarm.human.linkage.push_message) << '\n';
    output << "alarm.sound_light.white_light="
           << BoolToString(active_.alarm.human.linkage.white_light_alarm) << '\n';
    output << "alarm.sound_light.sound="
           << BoolToString(active_.alarm.human.linkage.sound_alarm) << '\n';
    output << "alarm.sound_light.duration_sec="
           << active_.alarm.white_light_duration_sec << '\n';
    output << "alarm.sound_light.repeat_count="
           << active_.alarm.sound_repeat_count << '\n';

    if (!output) {
        return ConfigResult::Error(ConfigStatusCode::PersistFailed,
                                   "config.persist",
                                   "write persist file failed");
    }
    return ConfigResult::Ok();
}

void ConfigServiceStub::RebuildPendingChange() {
    pending_ = BuildPendingConfigChange(active_, desired_);
}

} // namespace aov::app::config

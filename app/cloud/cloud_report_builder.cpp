#include "app/cloud/cloud_report_builder.hpp"

#include <string>

namespace aov::app::cloud {

namespace {

std::string BoolToString(const bool value) {
    return value ? "true" : "false";
}

std::string WorkModeToString(const aov::app::config::ConfiguredWorkMode mode) {
    switch (mode) {
        case aov::app::config::ConfiguredWorkMode::PowerSaving:
            return "PowerSaving";
        case aov::app::config::ConfiguredWorkMode::SmartNoSleep:
            return "SmartNoSleep";
        case aov::app::config::ConfiguredWorkMode::UltraLongStandby:
            return "UltraLongStandby";
        default:
            return "PowerSaving";
    }
}

std::string VideoEncodeToString(const aov::app::config::VideoEncodeType type) {
    switch (type) {
        case aov::app::config::VideoEncodeType::H264:
            return "H264";
        case aov::app::config::VideoEncodeType::H265:
            return "H265";
        default:
            return "H265";
    }
}

std::string RecordTypeToString(const aov::app::config::RecordType type) {
    switch (type) {
        case aov::app::config::RecordType::Aov:
            return "Aov";
        case aov::app::config::RecordType::Normal:
            return "Normal";
        case aov::app::config::RecordType::Alarm:
            return "Alarm";
        default:
            return "Normal";
    }
}

std::string SdCardStatusToString(const aov::app::config::SdCardStatus status) {
    switch (status) {
        case aov::app::config::SdCardStatus::Normal:
            return "Normal";
        case aov::app::config::SdCardStatus::NotDetected:
            return "NotDetected";
        case aov::app::config::SdCardStatus::NotFormatted:
            return "NotFormatted";
        case aov::app::config::SdCardStatus::Formatting:
            return "Formatting";
        case aov::app::config::SdCardStatus::Error:
            return "Error";
        default:
            return "Error";
    }
}

std::string RuntimeStateToString(const aov::app::RuntimeWorkState state) {
    switch (state) {
        case aov::app::RuntimeWorkState::AwakeNormal:
            return "AwakeNormal";
        case aov::app::RuntimeWorkState::SleepPeriodicDetect:
            return "SleepPeriodicDetect";
        case aov::app::RuntimeWorkState::SleepAppWakeOnly:
            return "SleepAppWakeOnly";
        case aov::app::RuntimeWorkState::LowBatteryProtection:
            return "LowBatteryProtection";
        default:
            return "SleepPeriodicDetect";
    }
}

void Add(CloudPropertyReport& report, std::string key, std::string value) {
    report.properties.push_back(CloudProperty {std::move(key), std::move(value)});
}

} // namespace

CloudPropertyReport BuildCloudPropertyReport(
    const aov::app::config::DeviceConfig& active_config,
    const aov::app::RuntimeWorkState runtime_state,
    const aov::app::BatteryStatus& battery,
    const aov::app::config::StorageRuntimeStatus& storage) {
    CloudPropertyReport report;

    Add(report, "AovWorkMode", WorkModeToString(active_config.work_mode.mode));
    Add(report, "AovRecordInterval",
        std::to_string(active_config.work_mode.record_interval_sec));
    Add(report, "LowBatteryProtectThreshold",
        std::to_string(active_config.work_mode.low_battery_protect_threshold_pct));

    Add(report, "VideoEncoding",
        VideoEncodeToString(active_config.media.main_stream.codec));
    Add(report, "VideoResolution",
        std::to_string(active_config.media.main_stream.width) + "x" +
            std::to_string(active_config.media.main_stream.height));
    Add(report, "VideoFPS",
        std::to_string(active_config.media.main_stream.fps_normal));
    Add(report, "VideoBitrate",
        std::to_string(active_config.media.main_stream.bitrate_kbps));

    Add(report, "CloudStorageSwitch",
        BoolToString(active_config.record.cloud_storage.enabled));
    Add(report, "StorageRecordMode",
        RecordTypeToString(active_config.record.schedule.default_record_type));
    Add(report, "StorageStatus", SdCardStatusToString(storage.sd_status));
    Add(report, "StorageFormatProgress", std::to_string(storage.format_progress));

    Add(report, "BatteryPercent", std::to_string(battery.percent));
    Add(report, "ChargingState", BoolToString(battery.charging));
    Add(report, "AovRuntimeState", RuntimeStateToString(runtime_state));

    return report;
}

} // namespace aov::app::cloud

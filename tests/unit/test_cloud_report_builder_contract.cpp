#include <iostream>

#include "../../app/cloud/cloud_report_builder.hpp"
#include "../../app/config/device_config.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

bool HasProperty(const aov::app::cloud::CloudPropertyReport& report,
                 const std::string& key,
                 const std::string& value) {
    for (const auto& property : report.properties) {
        if (property.key == key && property.value == value) {
            return true;
        }
    }
    return false;
}

} // namespace

int main() {
    using namespace aov::app;
    using namespace aov::app::cloud;
    using namespace aov::app::config;

    DeviceConfig config = MakeDefaultDeviceConfig();
    config.work_mode.mode = aov::app::config::ConfiguredWorkMode::SmartNoSleep;
    config.work_mode.record_interval_sec = 3;
    config.work_mode.low_battery_protect_threshold_pct = 15;
    config.media.main_stream.codec = aov::app::config::VideoEncodeType::H264;
    config.media.main_stream.width = 1920;
    config.media.main_stream.height = 1080;
    config.media.main_stream.fps_normal = 12;
    config.media.main_stream.bitrate_kbps = 1536;
    config.record.cloud_storage.enabled = true;
    config.record.schedule.default_record_type = aov::app::config::RecordType::Alarm;

    BatteryStatus battery;
    battery.percent = 66;
    battery.charging = true;
    battery.low_battery_protect = false;

    StorageRuntimeStatus storage;
    storage.sd_status = aov::app::config::SdCardStatus::Normal;
    storage.total_bytes = 1024;
    storage.free_bytes = 512;
    storage.format_progress = 100;

    const CloudPropertyReport report = BuildCloudPropertyReport(
        config,
        RuntimeWorkState::AwakeNormal,
        battery,
        storage);

    if (!HasProperty(report, "AovWorkMode", "SmartNoSleep") ||
        !HasProperty(report, "AovRecordInterval", "3") ||
        !HasProperty(report, "LowBatteryProtectThreshold", "15")) {
        return Fail("report must include active work mode properties");
    }
    if (!HasProperty(report, "VideoEncoding", "H264") ||
        !HasProperty(report, "VideoResolution", "1920x1080") ||
        !HasProperty(report, "VideoFPS", "12") ||
        !HasProperty(report, "VideoBitrate", "1536")) {
        return Fail("report must include active video properties");
    }
    if (!HasProperty(report, "CloudStorageSwitch", "true") ||
        !HasProperty(report, "StorageRecordMode", "Alarm") ||
        !HasProperty(report, "StorageStatus", "Normal")) {
        return Fail("report must include storage properties");
    }
    if (!HasProperty(report, "BatteryPercent", "66") ||
        !HasProperty(report, "ChargingState", "true") ||
        !HasProperty(report, "AovRuntimeState", "AwakeNormal")) {
        return Fail("report must include runtime properties");
    }

    std::cout << "cloud report builder contract passed\n";
    return 0;
}

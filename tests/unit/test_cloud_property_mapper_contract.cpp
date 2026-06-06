#include <iostream>

#include "../../app/cloud/cloud_property_mapper.hpp"
#include "../../app/config/device_config.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

} // namespace

int main() {
    using namespace aov::app::cloud;
    using namespace aov::app::config;

    DeviceConfig config = MakeDefaultDeviceConfig();

    auto result = ApplyCloudProperty(
        CloudPropertyUpdate {"AovWorkMode", "UltraLongStandby"});
    if (!result.ok()) {
        return Fail("AovWorkMode update must parse");
    }
    if (!ApplyCloudPropertyToConfig(result.value, config).ok() ||
        config.work_mode.mode != ConfiguredWorkMode::UltraLongStandby) {
        return Fail("AovWorkMode must update work mode");
    }

    result = ApplyCloudProperty(CloudPropertyUpdate {"AovRecordInterval", "5"});
    if (!result.ok() ||
        !ApplyCloudPropertyToConfig(result.value, config).ok() ||
        config.work_mode.record_interval_sec != 5) {
        return Fail("AovRecordInterval must update AOV interval");
    }

    result = ApplyCloudProperty(CloudPropertyUpdate {"LowBatteryProtectThreshold", "20"});
    if (!result.ok() ||
        !ApplyCloudPropertyToConfig(result.value, config).ok() ||
        config.work_mode.low_battery_protect_threshold_pct != 20) {
        return Fail("LowBatteryProtectThreshold must update threshold");
    }

    result = ApplyCloudProperty(CloudPropertyUpdate {"VideoEncoding", "H264"});
    if (!result.ok() ||
        !ApplyCloudPropertyToConfig(result.value, config).ok() ||
        config.media.main_stream.codec != VideoEncodeType::H264) {
        return Fail("VideoEncoding must update main stream codec");
    }

    result = ApplyCloudProperty(CloudPropertyUpdate {"VideoResolution", "1920x1080"});
    if (!result.ok() ||
        !ApplyCloudPropertyToConfig(result.value, config).ok() ||
        config.media.main_stream.width != 1920 ||
        config.media.main_stream.height != 1080) {
        return Fail("VideoResolution must update main stream resolution");
    }

    result = ApplyCloudProperty(CloudPropertyUpdate {"VideoFPS", "12"});
    if (!result.ok() ||
        !ApplyCloudPropertyToConfig(result.value, config).ok() ||
        config.media.main_stream.fps_normal != 12) {
        return Fail("VideoFPS must update normal fps");
    }

    result = ApplyCloudProperty(CloudPropertyUpdate {"VideoBitrate", "1536"});
    if (!result.ok() ||
        !ApplyCloudPropertyToConfig(result.value, config).ok() ||
        config.media.main_stream.bitrate_kbps != 1536) {
        return Fail("VideoBitrate must update main stream bitrate");
    }

    result = ApplyCloudProperty(CloudPropertyUpdate {"CloudStorageSwitch", "true"});
    if (!result.ok() ||
        !ApplyCloudPropertyToConfig(result.value, config).ok() ||
        !config.record.cloud_storage.enabled) {
        return Fail("CloudStorageSwitch must update cloud storage switch");
    }

    result = ApplyCloudProperty(CloudPropertyUpdate {"StorageRecordMode", "Alarm"});
    if (!result.ok() ||
        !ApplyCloudPropertyToConfig(result.value, config).ok() ||
        config.record.schedule.default_record_type != RecordType::Alarm) {
        return Fail("StorageRecordMode must update storage record mode");
    }

    result = ApplyCloudProperty(CloudPropertyUpdate {"AovRecordInterval", "2"});
    if (result.ok() || result.error.field != "AovRecordInterval") {
        return Fail("invalid AOV interval must be rejected at cloud mapper");
    }

    result = ApplyCloudProperty(CloudPropertyUpdate {"UnknownProperty", "1"});
    if (result.ok() || result.error.field != "UnknownProperty") {
        return Fail("unknown cloud property must be rejected");
    }

    std::cout << "cloud property mapper contract passed\n";
    return 0;
}

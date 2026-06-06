#pragma once

#include "app/cloud/cloud_types.hpp"
#include "app/config/device_config.hpp"

namespace aov::app::cloud {

enum class MappedCloudPropertyId {
    AovWorkMode,
    AovRecordInterval,
    LowBatteryProtectThreshold,
    VideoEncoding,
    VideoResolution,
    VideoFPS,
    VideoBitrate,
    CloudStorageSwitch,
    StorageRecordMode,
    HumanDetectConfig,
    ImageDetectConfig,
    SoundLightAlarmConfig,
};

struct MappedCloudProperty {
    MappedCloudPropertyId id {MappedCloudPropertyId::AovWorkMode};
    std::string raw_value;
    int int_value {0};
    bool bool_value {false};
    int width {0};
    int height {0};
    aov::app::config::ConfiguredWorkMode work_mode {
        aov::app::config::ConfiguredWorkMode::PowerSaving};
    aov::app::config::VideoEncodeType video_encode {
        aov::app::config::VideoEncodeType::H265};
    aov::app::config::RecordType record_type {
        aov::app::config::RecordType::Normal};
    aov::app::config::DetectRuleConfig detect_rule {};
    bool white_light_alarm {false};
    bool sound_alarm {false};
    int white_light_duration_sec {0};
    int sound_repeat_count {0};
    bool alarm_snapshot_enabled {false};
    bool target_box_overlay {false};
};

using CloudPropertyMapResult = CloudConfigResult<MappedCloudProperty>;
using CloudPropertyApplyResult = CloudConfigResult<bool>;

CloudPropertyMapResult ApplyCloudProperty(const CloudPropertyUpdate& update);
CloudPropertyApplyResult ApplyCloudPropertyToConfig(const MappedCloudProperty& property,
                                                    aov::app::config::DeviceConfig& config);

} // namespace aov::app::cloud

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
        CloudPropertyUpdate {
            "HumanDetectConfig",
            "enabled=false;sensitivity=25;debounce=6;trigger_record=false;push_message=true"});
    if (!result.ok() || !ApplyCloudPropertyToConfig(result.value, config).ok()) {
        return Fail("HumanDetectConfig must parse and apply");
    }
    if (config.alarm.human.enabled ||
        config.alarm.human.sensitivity != 25 ||
        config.alarm.human.debounce_sec != 6 ||
        config.alarm.human.linkage.trigger_record ||
        !config.alarm.human.linkage.push_message) {
        return Fail("HumanDetectConfig must update human detect rule");
    }

    result = ApplyCloudProperty(
        CloudPropertyUpdate {
            "ImageDetectConfig",
            "enabled=true;sensitivity=70;debounce=9;trigger_record=true;push_message=false"});
    if (!result.ok() || !ApplyCloudPropertyToConfig(result.value, config).ok()) {
        return Fail("ImageDetectConfig must parse and apply");
    }
    if (!config.alarm.motion.enabled ||
        config.alarm.motion.sensitivity != 70 ||
        config.alarm.motion.debounce_sec != 9 ||
        !config.alarm.motion.linkage.trigger_record ||
        config.alarm.motion.linkage.push_message) {
        return Fail("ImageDetectConfig must update motion detect rule");
    }

    result = ApplyCloudProperty(
        CloudPropertyUpdate {
            "SoundLightAlarmConfig",
            "white_light_alarm=true;sound_alarm=true;duration=15;sound_repeat=3;snapshot=false;target_box=true"});
    if (!result.ok() || !ApplyCloudPropertyToConfig(result.value, config).ok()) {
        return Fail("SoundLightAlarmConfig must parse and apply");
    }
    if (!config.alarm.human.linkage.white_light_alarm ||
        !config.alarm.human.linkage.sound_alarm ||
        config.alarm.white_light_duration_sec != 15 ||
        config.alarm.sound_repeat_count != 3 ||
        config.alarm.alarm_snapshot_enabled ||
        !config.alarm.target_box_overlay) {
        return Fail("SoundLightAlarmConfig must update alarm linkage and global alarm options");
    }

    result = ApplyCloudProperty(
        CloudPropertyUpdate {"HumanDetectConfig", "enabled=true;sensitivity=101"});
    if (result.ok() || result.error.field != "HumanDetectConfig.sensitivity") {
        return Fail("invalid HumanDetectConfig sensitivity must be rejected");
    }

    result = ApplyCloudProperty(
        CloudPropertyUpdate {"SoundLightAlarmConfig", "sound_repeat=0"});
    if (result.ok() || result.error.field != "SoundLightAlarmConfig.sound_repeat") {
        return Fail("invalid SoundLightAlarmConfig sound_repeat must be rejected");
    }

    std::cout << "cloud alarm property mapper contract passed\n";
    return 0;
}

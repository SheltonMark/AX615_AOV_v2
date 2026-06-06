#include "device_config.hpp"

namespace aov::app::config {

DeviceConfig MakeDefaultDeviceConfig() {
    DeviceConfig config;
    config.media.sub_stream = MakeDefaultSubStreamConfig();

    config.alarm.human.type = DetectTargetType::Human;
    config.alarm.vehicle.type = DetectTargetType::Vehicle;
    config.alarm.non_motor_vehicle.type = DetectTargetType::NonMotorVehicle;
    config.alarm.non_motor_vehicle.enabled = false;
    config.alarm.motion.type = DetectTargetType::Motion;
    config.alarm.motion.enabled = false;
    config.alarm.occlusion.type = DetectTargetType::Occlusion;
    config.alarm.occlusion.enabled = false;

    return config;
}

} // namespace aov::app::config

#pragma once

#include "alarm_config.hpp"
#include "config_types.hpp"
#include "device_feature_config.hpp"
#include "media_config.hpp"
#include "record_config.hpp"
#include "work_mode_config.hpp"

namespace aov::app::config {

struct DeviceConfig {
    ConfigVersion version;
    WorkModeConfig work_mode;
    MediaEncodeConfig media;
    AlarmConfig alarm;
    RecordConfig record;
    ImageParamConfig image;
    OsdConfig osd;
    FillLightConfig fill_light;
    NetworkConfig network;
    TimeConfig time;
    PtzConfig ptz;
    DeviceMaintenanceConfig maintenance;
};

struct DesiredConfig {
    DeviceConfig config;
};

struct ActiveConfig {
    DeviceConfig config;
};

DeviceConfig MakeDefaultDeviceConfig();

} // namespace aov::app::config

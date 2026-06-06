#pragma once

#include <string>

namespace aov::app::config {

struct BatteryStatusConfig {
    int percent {100};
    bool charging {false};
    bool low_battery_protect {false};
};

struct NetworkConfig {
    bool cellular_enabled {true};
    std::string region_profile;
};

struct TimeConfig {
    std::string timezone;
    bool ntp_enabled {true};
    bool timing_reboot_enabled {false};
};

struct PtzConfig {
    int speed {50};
    bool self_check_enabled {true};
    bool return_to_guard_position {false};
};

struct DeviceMaintenanceConfig {
    bool status_light_enabled {true};
    bool log_upload_enabled {false};
    bool log_redirect_enabled {false};
};

} // namespace aov::app::config

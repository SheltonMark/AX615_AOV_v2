#pragma once

namespace aov::app::config {

enum class ConfiguredWorkMode {
    PowerSaving,
    SmartNoSleep,
    UltraLongStandby,
};

enum class RuntimeProtectionState {
    None,
    LowBatteryProtection,
};

struct WorkModeConfig {
    ConfiguredWorkMode mode {ConfiguredWorkMode::PowerSaving};
    int record_interval_sec {1};
    int smart_enter_power_saving_pct {70};
    int smart_resume_no_sleep_pct {80};
    int low_battery_protect_threshold_pct {10};
    int low_battery_exit_hysteresis_pct {10};
};

} // namespace aov::app::config

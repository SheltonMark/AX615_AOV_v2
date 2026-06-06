#include "config_core_mapper.hpp"

namespace aov::app::config {

namespace {

aov::app::ConfiguredWorkMode MapWorkMode(const ConfiguredWorkMode mode) {
    switch (mode) {
        case ConfiguredWorkMode::PowerSaving:
            return aov::app::ConfiguredWorkMode::PowerSaving;
        case ConfiguredWorkMode::SmartNoSleep:
            return aov::app::ConfiguredWorkMode::SmartNoSleep;
        case ConfiguredWorkMode::UltraLongStandby:
            return aov::app::ConfiguredWorkMode::UltraLongStandby;
        default:
            return aov::app::ConfiguredWorkMode::PowerSaving;
    }
}

} // namespace

aov::app::WorkModePolicy BuildWorkModePolicy(const DeviceConfig& config) {
    aov::app::WorkModePolicy policy;
    policy.configured_mode = MapWorkMode(config.work_mode.mode);
    policy.periodic_wakeup_interval_sec = config.work_mode.record_interval_sec;
    policy.normal_record_fps = config.media.main_stream.fps_normal;
    policy.low_battery_protect_threshold_pct =
        config.work_mode.low_battery_protect_threshold_pct;
    policy.smart_to_save_threshold_pct = config.work_mode.smart_enter_power_saving_pct;
    policy.smart_to_awake_threshold_pct = config.work_mode.smart_resume_no_sleep_pct;
    return policy;
}

} // namespace aov::app::config

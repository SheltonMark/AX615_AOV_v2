#include <iostream>

#include "../../app/config/config_core_mapper.hpp"
#include "../../app/config/device_config.hpp"
#include "../../app/core/aov_types.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

} // namespace

int main() {
    using aov::app::WorkModePolicy;
    using aov::app::config::BuildWorkModePolicy;
    using aov::app::config::ConfiguredWorkMode;
    using aov::app::config::DeviceConfig;
    using aov::app::config::MakeDefaultDeviceConfig;

    DeviceConfig config = MakeDefaultDeviceConfig();

    WorkModePolicy policy = BuildWorkModePolicy(config);
    if (policy.configured_mode != aov::app::ConfiguredWorkMode::PowerSaving) {
        return Fail("default config must map to core power saving mode");
    }
    if (policy.periodic_wakeup_interval_sec != 1) {
        return Fail("AOV record interval must map to periodic wakeup interval");
    }
    if (policy.normal_record_fps != 15) {
        return Fail("normal record fps must come from main stream normal fps");
    }
    if (policy.low_battery_protect_threshold_pct != 10) {
        return Fail("low battery threshold must map to core policy");
    }
    if (policy.smart_to_save_threshold_pct != 70 ||
        policy.smart_to_awake_threshold_pct != 80) {
        return Fail("smart no-sleep thresholds must map to core policy");
    }

    config.work_mode.mode = ConfiguredWorkMode::SmartNoSleep;
    config.work_mode.record_interval_sec = 3;
    config.work_mode.smart_enter_power_saving_pct = 65;
    config.work_mode.smart_resume_no_sleep_pct = 85;
    config.media.main_stream.fps_normal = 12;
    policy = BuildWorkModePolicy(config);
    if (policy.configured_mode != aov::app::ConfiguredWorkMode::SmartNoSleep ||
        policy.periodic_wakeup_interval_sec != 3 ||
        policy.normal_record_fps != 12 ||
        policy.smart_to_save_threshold_pct != 65 ||
        policy.smart_to_awake_threshold_pct != 85) {
        return Fail("smart no-sleep config must map all core policy fields");
    }

    config.work_mode.mode = ConfiguredWorkMode::UltraLongStandby;
    policy = BuildWorkModePolicy(config);
    if (policy.configured_mode != aov::app::ConfiguredWorkMode::UltraLongStandby) {
        return Fail("ultra long standby config must map to core app-wake-only mode");
    }

    std::cout << "config core mapping contract passed\n";
    return 0;
}

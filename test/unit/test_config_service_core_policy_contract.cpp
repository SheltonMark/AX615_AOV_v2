#include <iostream>

#include "../../app/config/config_service_stub.hpp"
#include "../../app/core/aov_types.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

} // namespace

int main() {
    using aov::app::WorkModePolicy;
    using aov::app::config::ConfigServiceStub;
    using aov::app::config::ConfiguredWorkMode;
    using aov::app::config::DeviceConfig;

    ConfigServiceStub service;
    if (!service.LoadFactoryConfig().ok()) {
        return Fail("service must load factory config");
    }

    WorkModePolicy active_policy = service.GetActiveWorkModePolicy();
    if (active_policy.configured_mode != aov::app::ConfiguredWorkMode::PowerSaving ||
        active_policy.periodic_wakeup_interval_sec != 1 ||
        active_policy.normal_record_fps != 15) {
        return Fail("active core policy must reflect factory active config");
    }

    DeviceConfig desired = service.GetDesiredConfig();
    desired.work_mode.mode = ConfiguredWorkMode::SmartNoSleep;
    desired.work_mode.record_interval_sec = 5;
    desired.media.main_stream.fps_normal = 10;
    if (!service.UpdateDesiredConfig(desired).ok()) {
        return Fail("service must accept desired config update");
    }

    WorkModePolicy desired_policy = service.GetDesiredWorkModePolicy();
    if (desired_policy.configured_mode != aov::app::ConfiguredWorkMode::SmartNoSleep ||
        desired_policy.periodic_wakeup_interval_sec != 5 ||
        desired_policy.normal_record_fps != 10) {
        return Fail("desired core policy must reflect pending desired config");
    }

    active_policy = service.GetActiveWorkModePolicy();
    if (active_policy.configured_mode != aov::app::ConfiguredWorkMode::PowerSaving ||
        active_policy.periodic_wakeup_interval_sec != 1 ||
        active_policy.normal_record_fps != 15) {
        return Fail("active core policy must not change before MarkApplied");
    }

    if (!service.MarkApplied().ok()) {
        return Fail("service must mark desired config applied");
    }

    active_policy = service.GetActiveWorkModePolicy();
    if (active_policy.configured_mode != aov::app::ConfiguredWorkMode::SmartNoSleep ||
        active_policy.periodic_wakeup_interval_sec != 5 ||
        active_policy.normal_record_fps != 10) {
        return Fail("active core policy must reflect desired config after MarkApplied");
    }

    std::cout << "config service core policy contract passed\n";
    return 0;
}

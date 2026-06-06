#include <iostream>

#include "../../app/config/config_service_stub.hpp"
#include "../../app/core/aov_orchestrator_stub.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

} // namespace

int main() {
    using aov::app::AovOrchestratorStub;
    using aov::app::AovStatusCode;
    using aov::app::ConfiguredWorkMode;
    using aov::app::RuntimeWorkState;
    using aov::app::config::ConfigServiceStub;

    ConfigServiceStub config;
    AovOrchestratorStub core;

    if (core.InitFromConfig(config) != AovStatusCode::Ok) {
        return Fail("core must init from active config policy");
    }
    if (core.GetCurrentPolicy().configured_mode != ConfiguredWorkMode::PowerSaving ||
        core.GetRuntimeWorkState() != RuntimeWorkState::SleepPeriodicDetect) {
        return Fail("core init from config must use factory active policy");
    }

    auto desired = config.GetDesiredConfig();
    desired.work_mode.mode = aov::app::config::ConfiguredWorkMode::UltraLongStandby;
    desired.work_mode.record_interval_sec = 5;
    if (!config.UpdateDesiredConfig(desired).ok()) {
        return Fail("config must accept desired update");
    }

    if (core.ReloadPolicyFromConfig(config) != AovStatusCode::Ok) {
        return Fail("core reload before MarkApplied should keep active policy valid");
    }
    if (core.GetCurrentPolicy().configured_mode != ConfiguredWorkMode::PowerSaving) {
        return Fail("core must not apply desired policy before config MarkApplied");
    }

    if (!config.MarkApplied().ok()) {
        return Fail("config must mark desired config applied");
    }
    if (core.ReloadPolicyFromConfig(config) != AovStatusCode::Ok) {
        return Fail("core must reload active policy after config MarkApplied");
    }
    if (core.GetCurrentPolicy().configured_mode != ConfiguredWorkMode::UltraLongStandby ||
        core.GetCurrentPolicy().periodic_wakeup_interval_sec != 5 ||
        core.GetRuntimeWorkState() != RuntimeWorkState::SleepAppWakeOnly) {
        return Fail("core reload must apply active config policy and runtime state");
    }

    std::cout << "core config policy apply contract passed\n";
    return 0;
}

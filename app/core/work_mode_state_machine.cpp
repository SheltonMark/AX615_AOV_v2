#include "app/core/work_mode_state_machine.hpp"

namespace aov::app::core {

namespace {

constexpr int kLowBatteryExitHysteresisPct = 10;

bool IsSleepState(const aov::app::RuntimeWorkState state) {
    return state == aov::app::RuntimeWorkState::SleepPeriodicDetect ||
           state == aov::app::RuntimeWorkState::SleepAppWakeOnly;
}

} // namespace

aov::app::RuntimeWorkState WorkModeStateMachine::Resolve(
    const WorkModeStateInput& input) const {
    if (ShouldEnterLowBatteryProtection(input) ||
        ShouldStayInLowBatteryProtection(input)) {
        return aov::app::RuntimeWorkState::LowBatteryProtection;
    }

    if (input.preview_active) {
        return aov::app::RuntimeWorkState::AwakeNormal;
    }

    return ResolveConfiguredMode(input);
}

bool WorkModeStateMachine::ShouldEnterLowBatteryProtection(
    const WorkModeStateInput& input) const {
    return input.battery.low_battery_protect ||
           input.battery.percent <= input.policy.low_battery_protect_threshold_pct;
}

bool WorkModeStateMachine::ShouldStayInLowBatteryProtection(
    const WorkModeStateInput& input) const {
    if (input.previous_state != aov::app::RuntimeWorkState::LowBatteryProtection) {
        return false;
    }

    const int exit_threshold =
        input.policy.low_battery_protect_threshold_pct + kLowBatteryExitHysteresisPct;
    return input.battery.percent <= exit_threshold;
}

aov::app::RuntimeWorkState WorkModeStateMachine::ResolveConfiguredMode(
    const WorkModeStateInput& input) const {
    switch (input.policy.configured_mode) {
        case aov::app::ConfiguredWorkMode::PowerSaving:
            return aov::app::RuntimeWorkState::SleepPeriodicDetect;
        case aov::app::ConfiguredWorkMode::UltraLongStandby:
            return aov::app::RuntimeWorkState::SleepAppWakeOnly;
        case aov::app::ConfiguredWorkMode::SmartNoSleep:
            if (input.battery.percent < input.policy.smart_to_save_threshold_pct) {
                return aov::app::RuntimeWorkState::SleepPeriodicDetect;
            }
            if (input.battery.percent > input.policy.smart_to_awake_threshold_pct) {
                return aov::app::RuntimeWorkState::AwakeNormal;
            }
            return IsSleepState(input.previous_state)
                ? aov::app::RuntimeWorkState::SleepPeriodicDetect
                : aov::app::RuntimeWorkState::AwakeNormal;
        default:
            return aov::app::RuntimeWorkState::SleepPeriodicDetect;
    }
}

} // namespace aov::app::core

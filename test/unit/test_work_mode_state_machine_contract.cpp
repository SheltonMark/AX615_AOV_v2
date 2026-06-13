#include <iostream>

#include "../../app/core/work_mode_state_machine.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

aov::app::WorkModePolicy MakePolicy(aov::app::ConfiguredWorkMode mode) {
    aov::app::WorkModePolicy policy;
    policy.configured_mode = mode;
    policy.low_battery_protect_threshold_pct = 10;
    policy.smart_to_save_threshold_pct = 70;
    policy.smart_to_awake_threshold_pct = 80;
    return policy;
}

aov::app::BatteryStatus MakeBattery(int percent) {
    aov::app::BatteryStatus battery;
    battery.percent = percent;
    return battery;
}

} // namespace

int main() {
    using namespace aov::app;
    using namespace aov::app::core;

    WorkModeStateMachine sm;

    auto state = sm.Resolve(
        WorkModeStateInput {
            MakePolicy(ConfiguredWorkMode::PowerSaving),
            MakeBattery(90),
            RuntimeWorkState::AwakeNormal,
            false});
    if (state != RuntimeWorkState::SleepPeriodicDetect) {
        return Fail("power saving mode must resolve to periodic detect sleep");
    }

    state = sm.Resolve(
        WorkModeStateInput {
            MakePolicy(ConfiguredWorkMode::UltraLongStandby),
            MakeBattery(90),
            RuntimeWorkState::AwakeNormal,
            false});
    if (state != RuntimeWorkState::SleepAppWakeOnly) {
        return Fail("ultra long standby must resolve to app-wake-only sleep");
    }

    state = sm.Resolve(
        WorkModeStateInput {
            MakePolicy(ConfiguredWorkMode::SmartNoSleep),
            MakeBattery(90),
            RuntimeWorkState::SleepPeriodicDetect,
            false});
    if (state != RuntimeWorkState::AwakeNormal) {
        return Fail("smart no-sleep above awake threshold must be awake normal");
    }

    state = sm.Resolve(
        WorkModeStateInput {
            MakePolicy(ConfiguredWorkMode::SmartNoSleep),
            MakeBattery(60),
            RuntimeWorkState::AwakeNormal,
            false});
    if (state != RuntimeWorkState::SleepPeriodicDetect) {
        return Fail("smart no-sleep below save threshold must degrade to sleep");
    }

    state = sm.Resolve(
        WorkModeStateInput {
            MakePolicy(ConfiguredWorkMode::SmartNoSleep),
            MakeBattery(75),
            RuntimeWorkState::SleepPeriodicDetect,
            false});
    if (state != RuntimeWorkState::SleepPeriodicDetect) {
        return Fail("smart no-sleep between thresholds must keep previous sleep state");
    }

    state = sm.Resolve(
        WorkModeStateInput {
            MakePolicy(ConfiguredWorkMode::SmartNoSleep),
            MakeBattery(75),
            RuntimeWorkState::AwakeNormal,
            false});
    if (state != RuntimeWorkState::AwakeNormal) {
        return Fail("smart no-sleep between thresholds must keep previous awake state");
    }

    state = sm.Resolve(
        WorkModeStateInput {
            MakePolicy(ConfiguredWorkMode::PowerSaving),
            MakeBattery(5),
            RuntimeWorkState::AwakeNormal,
            false});
    if (state != RuntimeWorkState::LowBatteryProtection) {
        return Fail("low battery must override work mode");
    }

    state = sm.Resolve(
        WorkModeStateInput {
            MakePolicy(ConfiguredWorkMode::PowerSaving),
            MakeBattery(15),
            RuntimeWorkState::LowBatteryProtection,
            false});
    if (state != RuntimeWorkState::LowBatteryProtection) {
        return Fail("low battery protection must remain until threshold plus hysteresis");
    }

    state = sm.Resolve(
        WorkModeStateInput {
            MakePolicy(ConfiguredWorkMode::PowerSaving),
            MakeBattery(21),
            RuntimeWorkState::LowBatteryProtection,
            false});
    if (state != RuntimeWorkState::SleepPeriodicDetect) {
        return Fail("low battery protection must exit above threshold plus hysteresis");
    }

    state = sm.Resolve(
        WorkModeStateInput {
            MakePolicy(ConfiguredWorkMode::PowerSaving),
            MakeBattery(90),
            RuntimeWorkState::SleepPeriodicDetect,
            true});
    if (state != RuntimeWorkState::AwakeNormal) {
        return Fail("preview active must force awake normal");
    }

    std::cout << "work mode state machine contract passed\n";
    return 0;
}

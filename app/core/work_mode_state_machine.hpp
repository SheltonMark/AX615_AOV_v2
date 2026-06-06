#pragma once

#include "app/core/aov_types.hpp"

namespace aov::app::core {

struct WorkModeStateInput {
    aov::app::WorkModePolicy policy;
    aov::app::BatteryStatus battery;
    aov::app::RuntimeWorkState previous_state {
        aov::app::RuntimeWorkState::SleepPeriodicDetect};
    bool preview_active {false};
};

class WorkModeStateMachine {
public:
    aov::app::RuntimeWorkState Resolve(const WorkModeStateInput& input) const;

private:
    bool ShouldEnterLowBatteryProtection(const WorkModeStateInput& input) const;
    bool ShouldStayInLowBatteryProtection(const WorkModeStateInput& input) const;
    aov::app::RuntimeWorkState ResolveConfiguredMode(const WorkModeStateInput& input) const;
};

} // namespace aov::app::core

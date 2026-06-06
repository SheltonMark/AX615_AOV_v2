#pragma once

#include <string>

#include "app/core/core_state.hpp"

namespace aov::app::core {

enum class IdleSleepDecisionCode {
    CanSleep,
    BusinessBusy,
    DrainIncomplete,
    WaitingIdleDebounce,
};

struct IdleSleepDecision {
    IdleSleepDecisionCode code {IdleSleepDecisionCode::CanSleep};
    std::string blocker;

    bool can_sleep() const {
        return code == IdleSleepDecisionCode::CanSleep;
    }
};

class IdleSleepController {
public:
    IdleSleepDecision Evaluate(const BusinessState& business,
                               const ModuleDrainState& drain) const;

private:
    IdleSleepDecision Blocked(IdleSleepDecisionCode code,
                              std::string blocker) const;
};

} // namespace aov::app::core

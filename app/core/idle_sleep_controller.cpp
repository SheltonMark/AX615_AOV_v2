#include "app/core/idle_sleep_controller.hpp"

#include <utility>

namespace aov::app::core {

IdleSleepDecision IdleSleepController::Evaluate(
    const BusinessState& business,
    const ModuleDrainState& drain) const {
    if (business.preview_active) {
        return Blocked(IdleSleepDecisionCode::BusinessBusy, "preview");
    }
    if (business.local_recording) {
        return Blocked(IdleSleepDecisionCode::BusinessBusy, "local_record");
    }
    if (business.cloud_storage_running) {
        return Blocked(IdleSleepDecisionCode::BusinessBusy, "cloud_storage");
    }
    if (business.alarm_processing) {
        return Blocked(IdleSleepDecisionCode::BusinessBusy, "alarm");
    }
    if (business.pending_config_apply) {
        return Blocked(IdleSleepDecisionCode::BusinessBusy, "config_apply");
    }
    if (business.command_executing) {
        return Blocked(IdleSleepDecisionCode::BusinessBusy, "command");
    }

    if (!drain.local_record_closed_flushed_synced) {
        return Blocked(IdleSleepDecisionCode::DrainIncomplete, "local_record");
    }
    if (!drain.cloud_storage_finished) {
        return Blocked(IdleSleepDecisionCode::DrainIncomplete, "cloud_storage");
    }
    if (!drain.config_persisted) {
        return Blocked(IdleSleepDecisionCode::DrainIncomplete, "config");
    }
    if (!drain.idle_debounce_expired) {
        return Blocked(IdleSleepDecisionCode::WaitingIdleDebounce, "idle_debounce");
    }

    return {};
}

IdleSleepDecision IdleSleepController::Blocked(
    const IdleSleepDecisionCode code,
    std::string blocker) const {
    IdleSleepDecision decision;
    decision.code = code;
    decision.blocker = std::move(blocker);
    return decision;
}

} // namespace aov::app::core

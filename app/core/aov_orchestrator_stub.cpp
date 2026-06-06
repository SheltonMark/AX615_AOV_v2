#include "app/core/aov_orchestrator_stub.hpp"

namespace aov::app {

namespace {

bool IsSupportedInterval(const int interval_sec) {
    return interval_sec == 1 || interval_sec == 3 || interval_sec == 5 || interval_sec == 15;
}

} // namespace

AovStatusCode AovOrchestratorStub::Init(const WorkModePolicy& policy) {
    if (!IsValidPolicy(policy)) {
        return AovStatusCode::InvalidArgument;
    }

    context_.policy = policy;
    context_.runtime_state = ResolveRuntimeState();
    initialized_ = true;
    return AovStatusCode::Ok;
}

AovStatusCode AovOrchestratorStub::InitFromConfig(
    const config::ConfigServiceStub& config_service) {
    return Init(config_service.GetActiveWorkModePolicy());
}

AovStatusCode AovOrchestratorStub::ReloadPolicyFromConfig(
    const config::ConfigServiceStub& config_service) {
    const WorkModePolicy policy = config_service.GetActiveWorkModePolicy();
    if (!IsValidPolicy(policy)) {
        return AovStatusCode::InvalidArgument;
    }

    context_.policy = policy;
    context_.runtime_state = ResolveRuntimeState();
    return AovStatusCode::Ok;
}

AovStatusCode AovOrchestratorStub::Start() {
    if (!initialized_) {
        return AovStatusCode::InvalidState;
    }

    started_ = true;
    return AovStatusCode::Ok;
}

AovStatusCode AovOrchestratorStub::Stop() {
    started_ = false;
    context_.preview_active = false;
    context_.recording_active = false;
    context_.waiting_idle_debounce = false;
    drain_state_ = core::ModuleDrainState {};
    business_state_ = core::BusinessState {};
    return AovStatusCode::Ok;
}

AovStatusCode AovOrchestratorStub::OnBatteryStatusChanged(const BatteryStatus& status) {
    if (!initialized_) {
        return AovStatusCode::InvalidState;
    }

    context_.battery = status;
    context_.runtime_state = ResolveRuntimeState();
    return AovStatusCode::Ok;
}

AovStatusCode AovOrchestratorStub::OnWakeupEvent(const WakeupEvent& event) {
    if (!initialized_) {
        return AovStatusCode::InvalidState;
    }

    context_.last_wakeup_ts_ms = event.timestamp_ms;
    context_.waiting_idle_debounce = false;
    drain_state_.idle_debounce_expired = false;
    const core::CoreWakeupPlan plan =
        wakeup_planner_.BuildPlan(event, context_.policy, context_.battery);
    if (plan.decision == AovDecision::RejectAction) {
        context_.runtime_state = RuntimeWorkState::LowBatteryProtection;
        return AovStatusCode::Ok;
    }
    if (plan.restore_media) {
        context_.runtime_state = RuntimeWorkState::AwakeNormal;
    }
    if (plan.start_preview) {
        context_.preview_active = true;
        business_state_.preview_active = true;
    }
    if (plan.start_local_record) {
        context_.recording_active = true;
        business_state_.local_recording = true;
    }
    if (plan.start_cloud_storage) {
        business_state_.cloud_storage_running = true;
    }
    if (plan.report_alarm) {
        business_state_.alarm_processing = true;
    }
    if (plan.apply_pending_config) {
        business_state_.pending_config_apply = true;
    }

    return AovStatusCode::Ok;
}

AovStatusCode AovOrchestratorStub::OnDetectResult(const DetectResultSummary& result) {
    if (!initialized_) {
        return AovStatusCode::InvalidState;
    }

    const bool has_target =
        result.has_person || result.has_pet || result.has_vehicle || result.object_count > 0;

    if (context_.battery.low_battery_protect) {
        context_.runtime_state = RuntimeWorkState::LowBatteryProtection;
        return AovStatusCode::Ok;
    }

    if (has_target) {
        context_.runtime_state = RuntimeWorkState::AwakeNormal;
        context_.recording_active = true;
        business_state_.local_recording = true;
        business_state_.cloud_storage_running = true;
        business_state_.alarm_processing = true;
        context_.waiting_idle_debounce = false;
        drain_state_.idle_debounce_expired = false;
    } else if (!context_.preview_active) {
        context_.waiting_idle_debounce = true;
        context_.runtime_state = RuntimeWorkState::AwakeNormal;
        TryEnterSleepAfterDrain();
    }

    return AovStatusCode::Ok;
}

AovStatusCode AovOrchestratorStub::OnPreviewRequest(const PreviewRequest&) {
    if (!initialized_) {
        return AovStatusCode::InvalidState;
    }

    context_.preview_active = true;
    business_state_.preview_active = true;
    context_.waiting_idle_debounce = false;
    drain_state_.idle_debounce_expired = false;
    context_.runtime_state = RuntimeWorkState::AwakeNormal;
    return AovStatusCode::Ok;
}

AovStatusCode AovOrchestratorStub::OnPreviewSessionClosed(const std::string&) {
    if (!initialized_) {
        return AovStatusCode::InvalidState;
    }

    context_.preview_active = false;
    business_state_.preview_active = false;
    context_.waiting_idle_debounce = true;
    context_.runtime_state = RuntimeWorkState::AwakeNormal;
    TryEnterSleepAfterDrain();
    return AovStatusCode::Ok;
}

AovStatusCode AovOrchestratorStub::OnModuleDrainStateChanged(
    const core::ModuleDrainState& state) {
    if (!initialized_) {
        return AovStatusCode::InvalidState;
    }

    drain_state_ = state;
    business_state_.local_recording = !state.local_record_closed_flushed_synced;
    business_state_.cloud_storage_running = !state.cloud_storage_finished;
    business_state_.pending_config_apply = !state.config_persisted;
    if (state.local_record_closed_flushed_synced && state.cloud_storage_finished) {
        business_state_.alarm_processing = false;
    }
    if (drain_state_.idle_debounce_expired) {
        context_.waiting_idle_debounce = true;
    }
    TryEnterSleepAfterDrain();
    return AovStatusCode::Ok;
}

AovStatusCode AovOrchestratorStub::OnIdleDebounceExpired(std::uint64_t timestamp_ms) {
    if (!initialized_) {
        return AovStatusCode::InvalidState;
    }

    context_.last_idle_debounce_ts_ms = timestamp_ms;
    context_.waiting_idle_debounce = true;
    drain_state_.idle_debounce_expired = true;
    TryEnterSleepAfterDrain();
    return AovStatusCode::Ok;
}

RuntimeWorkState AovOrchestratorStub::GetRuntimeWorkState() const {
    return context_.runtime_state;
}

WorkModePolicy AovOrchestratorStub::GetCurrentPolicy() const {
    return context_.policy;
}

AovRuntimeContext AovOrchestratorStub::GetRuntimeContext() const {
    return context_;
}

bool AovOrchestratorStub::IsValidPolicy(const WorkModePolicy& policy) const {
    if (!IsSupportedInterval(policy.periodic_wakeup_interval_sec)) {
        return false;
    }

    if (policy.normal_record_fps <= 0) {
        return false;
    }

    if (policy.low_battery_protect_threshold_pct < 5 ||
        policy.low_battery_protect_threshold_pct > 30) {
        return false;
    }

    return policy.smart_to_save_threshold_pct < policy.smart_to_awake_threshold_pct;
}

RuntimeWorkState AovOrchestratorStub::ResolveRuntimeState() const {
    return work_mode_sm_.Resolve(
        core::WorkModeStateInput {
            context_.policy,
            context_.battery,
            context_.runtime_state,
            context_.preview_active});
}

void AovOrchestratorStub::TryEnterSleepAfterDrain() {
    if (context_.battery.low_battery_protect) {
        context_.runtime_state = RuntimeWorkState::LowBatteryProtection;
        return;
    }

    if (!context_.waiting_idle_debounce) {
        return;
    }

    const core::IdleSleepDecision decision =
        idle_sleep_controller_.Evaluate(business_state_, drain_state_);
    if (!decision.can_sleep()) {
        context_.runtime_state = RuntimeWorkState::AwakeNormal;
        return;
    }

    context_.recording_active = false;
    business_state_ = core::BusinessState {};
    context_.waiting_idle_debounce = false;
    context_.runtime_state = ResolveRuntimeState();
}

} // namespace aov::app

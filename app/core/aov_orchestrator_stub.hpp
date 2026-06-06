#pragma once

#include "app/config/config_service_stub.hpp"
#include "app/core/core_wakeup_planner.hpp"
#include "app/core/idle_sleep_controller.hpp"
#include "app/core/i_aov_orchestrator.hpp"
#include "app/core/work_mode_state_machine.hpp"

namespace aov::app {

class AovOrchestratorStub final : public IAovOrchestrator {
public:
    AovOrchestratorStub() = default;
    ~AovOrchestratorStub() override = default;

    AovStatusCode Init(const WorkModePolicy& policy) override;
    AovStatusCode InitFromConfig(const config::ConfigServiceStub& config_service);
    AovStatusCode ReloadPolicyFromConfig(const config::ConfigServiceStub& config_service);
    AovStatusCode Start() override;
    AovStatusCode Stop() override;

    AovStatusCode OnBatteryStatusChanged(const BatteryStatus& status) override;
    AovStatusCode OnWakeupEvent(const WakeupEvent& event) override;
    AovStatusCode OnDetectResult(const DetectResultSummary& result) override;
    AovStatusCode OnPreviewRequest(const PreviewRequest& request) override;
    AovStatusCode OnPreviewSessionClosed(const std::string& session_id) override;
    AovStatusCode OnModuleDrainStateChanged(const core::ModuleDrainState& state) override;
    AovStatusCode OnIdleDebounceExpired(std::uint64_t timestamp_ms) override;

    RuntimeWorkState GetRuntimeWorkState() const override;
    WorkModePolicy GetCurrentPolicy() const override;
    AovRuntimeContext GetRuntimeContext() const override;

private:
    bool IsValidPolicy(const WorkModePolicy& policy) const;
    RuntimeWorkState ResolveRuntimeState() const;
    void TryEnterSleepAfterDrain();

private:
    bool initialized_ {false};
    bool started_ {false};
    AovRuntimeContext context_ {};
    core::ModuleDrainState drain_state_ {};
    core::BusinessState business_state_ {};
    core::CoreWakeupPlanner wakeup_planner_ {};
    core::IdleSleepController idle_sleep_controller_ {};
    core::WorkModeStateMachine work_mode_sm_ {};
};

} // namespace aov::app

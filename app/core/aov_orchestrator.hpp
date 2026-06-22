#pragma once

#include <atomic>
#include <thread>

#include "app/config/config_service_stub.hpp"
#include "app/core/core_event_queue.hpp"
#include "app/core/core_timer.hpp"
#include "app/core/core_wakeup_planner.hpp"
#include "app/core/i_aov_orchestrator.hpp"
#include "app/core/idle_sleep_controller.hpp"
#include "app/core/work_mode_state_machine.hpp"

namespace aov::app::core {

class AovOrchestrator final : public IAovOrchestrator {
public:
    AovOrchestrator();
    ~AovOrchestrator() override;

    // IAovOrchestrator 接口
    AovStatusCode Init(const WorkModePolicy& policy) override;
    AovStatusCode Start() override;
    AovStatusCode Stop() override;

    AovStatusCode OnBatteryStatusChanged(const BatteryStatus& status) override;
    AovStatusCode OnWakeupEvent(const WakeupEvent& event) override;
    AovStatusCode OnDetectResult(const DetectResultSummary& result) override;
    AovStatusCode OnPreviewRequest(const PreviewRequest& request) override;
    AovStatusCode OnPreviewSessionClosed(const std::string& session_id) override;
    AovStatusCode OnModuleDrainStateChanged(const ModuleDrainState& state) override;
    AovStatusCode OnIdleDebounceExpired(std::uint64_t timestamp_ms) override;

    RuntimeWorkState GetRuntimeWorkState() const override;
    WorkModePolicy GetCurrentPolicy() const override;
    AovRuntimeContext GetRuntimeContext() const override;

    // 外部线程入口（线程安全）
    bool PostEvent(const CoreEvent& event);

private:
    // 主事件循环（独立线程）
    void Run();

    // 事件分发
    void HandleEvent(const CoreEvent& event);
    void HandleWakeupEvent(const WakeupEvent& event);
    void HandleBatteryChanged(const BatteryStatus& status);
    void HandleDetectResult(const DetectResultSummary& result);
    void HandlePreviewRequest(const PreviewRequest& request);
    void HandlePreviewClosed(const std::string& session_id);
    void HandleModuleDrainStateChanged(const ModuleDrainState& state);
    void HandleIdleDebounceExpired(std::uint64_t timestamp_ms);

    // 定时器处理
    void StartIdleDebounceTimer();
    void CancelIdleDebounceTimer();
    void OnIdleDebounceTimerFired(TimerId id);

    // 状态决策
    RuntimeWorkState ResolveRuntimeState() const;
    void TryEnterSleepAfterDrain();

    // 验证
    bool IsValidPolicy(const WorkModePolicy& policy) const;

private:
    std::atomic<bool> initialized_ {false};
    std::atomic<bool> started_ {false};
    std::atomic<bool> running_ {false};

    std::thread event_thread_;
    CoreEventQueue event_queue_;
    CoreTimerManager timer_manager_;

    // 状态（只在 core 线程访问，无需锁）
    AovRuntimeContext context_ {};
    ModuleDrainState drain_state_ {};
    BusinessState business_state_ {};

    // 决策组件
    WorkModeStateMachine work_mode_sm_ {};
    CoreWakeupPlanner wakeup_planner_ {};
    IdleSleepController idle_sleep_controller_ {};

    // 定时器ID
    TimerId idle_debounce_timer_id_ {0};
};

} // namespace aov::app::core

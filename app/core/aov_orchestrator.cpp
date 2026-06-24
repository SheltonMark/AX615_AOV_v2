#include "app/core/aov_orchestrator.hpp"

#include <cstdio>

namespace aov::app::core {

namespace {

constexpr std::chrono::milliseconds kIdleDebounceTimeoutMs{5000};  // 5秒
constexpr std::chrono::milliseconds kDefaultEventTimeoutMs{100};   // 100ms

bool IsSupportedInterval(const int interval_sec) {
    return interval_sec == 1 || interval_sec == 3 || interval_sec == 5 || interval_sec == 15;
}

} // namespace

AovOrchestrator::AovOrchestrator()
    : event_queue_(128) {
}

AovOrchestrator::~AovOrchestrator() {
    Stop();
}

AovStatusCode AovOrchestrator::Init(const WorkModePolicy& policy) {
    if (!IsValidPolicy(policy)) {
        return AovStatusCode::InvalidArgument;
    }

    context_.policy = policy;
    context_.runtime_state = ResolveRuntimeState();
    initialized_.store(true);
    return AovStatusCode::Ok;
}

AovStatusCode AovOrchestrator::Start() {
    if (!initialized_.load()) {
        return AovStatusCode::InvalidState;
    }

    if (started_.load()) {
        return AovStatusCode::Ok;
    }

    running_.store(true);
    event_thread_ = std::thread([this] { Run(); });
    started_.store(true);

    fprintf(stdout, "[AovOrchestrator] started with event loop\n");
    return AovStatusCode::Ok;
}

AovStatusCode AovOrchestrator::Stop() {
    if (!started_.load()) {
        return AovStatusCode::Ok;
    }

    running_.store(false);
    event_queue_.WakeUp();

    if (event_thread_.joinable()) {
        event_thread_.join();
    }

    timer_manager_.Clear();
    event_queue_.Clear();
    started_.store(false);
    context_.preview_active = false;
    context_.recording_active = false;
    context_.waiting_idle_debounce = false;
    drain_state_ = ModuleDrainState {};
    business_state_ = BusinessState {};

    fprintf(stdout, "[AovOrchestrator] stopped\n");
    return AovStatusCode::Ok;
}

bool AovOrchestrator::PostEvent(const CoreEvent& event) {
    return event_queue_.Push(event);
}

void AovOrchestrator::Run() {
    fprintf(stdout, "[AovOrchestrator] event loop started\n");

    while (running_.load()) {
        // 1. 获取下次定时器触发时间
        auto timeout = timer_manager_.GetNextTimeout();
        if (timeout.count() < 0) {
            timeout = kDefaultEventTimeoutMs;
        } else if (timeout.count() == 0) {
            timeout = std::chrono::milliseconds(1);  // 立即处理定时器
        }

        // 2. 等待事件（阻塞/超时）
        CoreEvent event;
        if (event_queue_.Pop(event, timeout)) {
            // 3. 处理事件
            HandleEvent(event);
        }

        // 4. 处理到期的定时器
        timer_manager_.ProcessExpired();

        // 5. 定期检查是否可以回休眠
        TryEnterSleepAfterDrain();
    }

    fprintf(stdout, "[AovOrchestrator] event loop exited\n");
}

void AovOrchestrator::HandleEvent(const CoreEvent& event) {
    switch (event.type) {
        case CoreEventType::Wakeup:
            HandleWakeupEvent(event.wakeup);
            break;
        case CoreEventType::BatteryChanged:
            HandleBatteryChanged(event.battery);
            break;
        case CoreEventType::DetectResult:
            HandleDetectResult(event.detect);
            break;
        case CoreEventType::ConfigChanged:
            // TODO: 处理配置变更
            fprintf(stdout, "[AovOrchestrator] ConfigChanged event (not implemented)\n");
            break;
        case CoreEventType::CloudCommand:
            // TODO: 处理云端命令
            fprintf(stdout, "[AovOrchestrator] CloudCommand event (not implemented)\n");
            break;
        case CoreEventType::ModuleStateChanged:
            HandleModuleDrainStateChanged(event.module_drain_state);
            break;
        case CoreEventType::IdleDebounceExpired:
            HandleIdleDebounceExpired(event.idle_timeout_timestamp_ms);
            break;
        default:
            break;
    }
}

void AovOrchestrator::HandleWakeupEvent(const WakeupEvent& event) {
    fprintf(stdout, "[AovOrchestrator] HandleWakeupEvent: source=%d\n",
            static_cast<int>(event.source));

    context_.last_wakeup_ts_ms = event.timestamp_ms;
    context_.waiting_idle_debounce = false;
    drain_state_.idle_debounce_expired = false;
    CancelIdleDebounceTimer();

    const CoreWakeupPlan plan =
        wakeup_planner_.BuildPlan(event, context_.policy, context_.battery);

    if (plan.decision == AovDecision::RejectAction) {
        context_.runtime_state = RuntimeWorkState::LowBatteryProtection;
        fprintf(stdout, "[AovOrchestrator] Wakeup rejected: LowBatteryProtection\n");
        return;
    }

    if (plan.restore_media) {
        context_.runtime_state = RuntimeWorkState::AwakeNormal;
        fprintf(stdout, "[AovOrchestrator] Restore media\n");
    }

    if (plan.start_preview) {
        context_.preview_active = true;
        business_state_.preview_active = true;
        fprintf(stdout, "[AovOrchestrator] Start preview\n");
    }

    if (plan.start_local_record) {
        context_.recording_active = true;
        business_state_.local_recording = true;
        fprintf(stdout, "[AovOrchestrator] Start local record\n");
    }

    if (plan.start_cloud_storage) {
        business_state_.cloud_storage_running = true;
        fprintf(stdout, "[AovOrchestrator] Start cloud storage\n");
    }

    if (plan.report_alarm) {
        business_state_.alarm_processing = true;
        fprintf(stdout, "[AovOrchestrator] Report alarm\n");
    }

    if (plan.apply_pending_config) {
        business_state_.pending_config_apply = true;
        fprintf(stdout, "[AovOrchestrator] Apply pending config\n");
    }
}

void AovOrchestrator::HandleBatteryChanged(const BatteryStatus& status) {
    fprintf(stdout, "[AovOrchestrator] HandleBatteryChanged: percent=%d, charging=%d\n",
            status.percent, status.charging);

    context_.battery = status;
    context_.runtime_state = ResolveRuntimeState();
}

void AovOrchestrator::HandleDetectResult(const DetectResultSummary& result) {
    const bool has_target =
        result.has_person || result.has_pet || result.has_vehicle || result.object_count > 0;

    fprintf(stdout, "[AovOrchestrator] HandleDetectResult: has_target=%d\n", has_target);

    if (context_.battery.low_battery_protect) {
        context_.runtime_state = RuntimeWorkState::LowBatteryProtection;
        fprintf(stdout, "[AovOrchestrator] LowBatteryProtection mode\n");
        return;
    }

    if (has_target) {
        // 检测到目标 → 取消防抖定时器 → 恢复常电
        CancelIdleDebounceTimer();
        context_.runtime_state = RuntimeWorkState::AwakeNormal;
        context_.recording_active = true;
        business_state_.local_recording = true;
        business_state_.cloud_storage_running = true;
        business_state_.alarm_processing = true;
        context_.waiting_idle_debounce = false;
        drain_state_.idle_debounce_expired = false;
        fprintf(stdout, "[AovOrchestrator] Target detected, start recording\n");
    } else if (!context_.preview_active) {
        // 无目标且无预览 → 启动空闲防抖定时器
        StartIdleDebounceTimer();
        context_.waiting_idle_debounce = true;
        context_.runtime_state = RuntimeWorkState::AwakeNormal;
        fprintf(stdout, "[AovOrchestrator] No target, start idle debounce timer\n");
    }
}

void AovOrchestrator::HandlePreviewRequest(const PreviewRequest& request) {
    fprintf(stdout, "[AovOrchestrator] HandlePreviewRequest: session_id=%s\n",
            request.session_id.c_str());

    CancelIdleDebounceTimer();
    context_.preview_active = true;
    business_state_.preview_active = true;
    context_.waiting_idle_debounce = false;
    drain_state_.idle_debounce_expired = false;
    context_.runtime_state = RuntimeWorkState::AwakeNormal;
}

void AovOrchestrator::HandlePreviewClosed(const std::string& session_id) {
    fprintf(stdout, "[AovOrchestrator] HandlePreviewClosed: session_id=%s\n",
            session_id.c_str());

    context_.preview_active = false;
    business_state_.preview_active = false;
    StartIdleDebounceTimer();
    context_.waiting_idle_debounce = true;
    context_.runtime_state = RuntimeWorkState::AwakeNormal;
}

void AovOrchestrator::HandleModuleDrainStateChanged(const ModuleDrainState& state) {
    fprintf(stdout, "[AovOrchestrator] HandleModuleDrainStateChanged: "
                    "record=%d, cloud=%d, config=%d, debounce=%d\n",
            state.local_record_closed_flushed_synced,
            state.cloud_storage_finished,
            state.config_persisted,
            state.idle_debounce_expired);

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
}

void AovOrchestrator::HandleIdleDebounceExpired(std::uint64_t timestamp_ms) {
    fprintf(stdout, "[AovOrchestrator] HandleIdleDebounceExpired: timestamp=%llu\n",
            static_cast<unsigned long long>(timestamp_ms));

    context_.last_idle_debounce_ts_ms = timestamp_ms;
    context_.waiting_idle_debounce = true;
    drain_state_.idle_debounce_expired = true;
    TryEnterSleepAfterDrain();
}

void AovOrchestrator::StartIdleDebounceTimer() {
    CancelIdleDebounceTimer();

    idle_debounce_timer_id_ = timer_manager_.AddTimer(
        TimerType::OneShot,
        kIdleDebounceTimeoutMs,
        [this](TimerId id) {
            OnIdleDebounceTimerFired(id);
        });

    fprintf(stdout, "[AovOrchestrator] Idle debounce timer started: %llu ms\n",
            static_cast<unsigned long long>(kIdleDebounceTimeoutMs.count()));
}

void AovOrchestrator::CancelIdleDebounceTimer() {
    if (idle_debounce_timer_id_ != 0) {
        timer_manager_.CancelTimer(idle_debounce_timer_id_);
        idle_debounce_timer_id_ = 0;
        fprintf(stdout, "[AovOrchestrator] Idle debounce timer cancelled\n");
    }
}

void AovOrchestrator::OnIdleDebounceTimerFired(TimerId /*id*/) {
    fprintf(stdout, "[AovOrchestrator] Idle debounce timer fired\n");

    CoreEvent event;
    event.type = CoreEventType::IdleDebounceExpired;
    event.wakeup.timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    PostEvent(event);
}

RuntimeWorkState AovOrchestrator::GetRuntimeWorkState() const {
    return context_.runtime_state;
}

WorkModePolicy AovOrchestrator::GetCurrentPolicy() const {
    return context_.policy;
}

AovRuntimeContext AovOrchestrator::GetRuntimeContext() const {
    return context_;
}

AovStatusCode AovOrchestrator::OnBatteryStatusChanged(const BatteryStatus& status) {
    CoreEvent event;
    event.type = CoreEventType::BatteryChanged;
    event.battery = status;
    return PostEvent(event) ? AovStatusCode::Ok : AovStatusCode::Busy;
}

AovStatusCode AovOrchestrator::OnWakeupEvent(const WakeupEvent& event) {
    CoreEvent core_event;
    core_event.type = CoreEventType::Wakeup;
    core_event.wakeup = event;
    return PostEvent(core_event) ? AovStatusCode::Ok : AovStatusCode::Busy;
}

AovStatusCode AovOrchestrator::OnDetectResult(const DetectResultSummary& result) {
    CoreEvent event;
    event.type = CoreEventType::DetectResult;
    event.detect = result;
    return PostEvent(event) ? AovStatusCode::Ok : AovStatusCode::Busy;
}

AovStatusCode AovOrchestrator::OnPreviewRequest(const PreviewRequest& request) {
    CoreEvent event;
    event.type = CoreEventType::Wakeup;
    event.wakeup.source = WakeupSource::AppRemote;
    event.wakeup.timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    return PostEvent(event) ? AovStatusCode::Ok : AovStatusCode::Busy;
}

AovStatusCode AovOrchestrator::OnPreviewSessionClosed(const std::string& session_id) {
    (void)session_id;
    CoreEvent event;
    event.type = CoreEventType::ModuleStateChanged;
    event.module_drain_state.local_record_closed_flushed_synced = true;
    event.module_drain_state.cloud_storage_finished = true;
    event.module_drain_state.config_persisted = true;
    event.module_drain_state.idle_debounce_expired = false;
    return PostEvent(event) ? AovStatusCode::Ok : AovStatusCode::Busy;
}

AovStatusCode AovOrchestrator::OnModuleDrainStateChanged(const ModuleDrainState& state) {
    CoreEvent event;
    event.type = CoreEventType::ModuleStateChanged;
    event.module_drain_state = state;
    return PostEvent(event) ? AovStatusCode::Ok : AovStatusCode::Busy;
}

AovStatusCode AovOrchestrator::OnIdleDebounceExpired(std::uint64_t timestamp_ms) {
    CoreEvent event;
    event.type = CoreEventType::IdleDebounceExpired;
    event.idle_timeout_timestamp_ms = timestamp_ms;
    return PostEvent(event) ? AovStatusCode::Ok : AovStatusCode::Busy;
}

bool AovOrchestrator::IsValidPolicy(const WorkModePolicy& policy) const {
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

RuntimeWorkState AovOrchestrator::ResolveRuntimeState() const {
    return work_mode_sm_.Resolve(
        WorkModeStateInput {
            context_.policy,
            context_.battery,
            context_.runtime_state,
            context_.preview_active});
}

void AovOrchestrator::TryEnterSleepAfterDrain() {
    if (context_.battery.low_battery_protect) {
        context_.runtime_state = RuntimeWorkState::LowBatteryProtection;
        return;
    }

    if (!context_.waiting_idle_debounce) {
        return;
    }

    const IdleSleepDecision decision =
        idle_sleep_controller_.Evaluate(business_state_, drain_state_);

    if (!decision.can_sleep()) {
        context_.runtime_state = RuntimeWorkState::AwakeNormal;
        return;
    }

    // 满足条件 → 可以回休眠
    fprintf(stdout, "[AovOrchestrator] Ready to enter AOV sleep\n");
    context_.recording_active = false;
    business_state_ = BusinessState {};
    context_.waiting_idle_debounce = false;
    context_.runtime_state = ResolveRuntimeState();

    // TODO: 调用 SystemSleepCoordinator.PrepareAndEnterAovSleep()
}

} // namespace aov::app::core

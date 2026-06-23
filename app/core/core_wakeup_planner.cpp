#include "app/core/core_wakeup_planner.hpp"

namespace aov::app::core {

CoreWakeupPlan CoreWakeupPlanner::BuildPlan(
    const WakeupEvent& event,
    const WorkModePolicy& policy,
    const BatteryStatus& battery) const {
    CoreWakeupPlan plan;
    plan.wakeup_interval_sec = policy.periodic_wakeup_interval_sec;

    if (IsLowBatteryProtected(policy, battery)) {
        plan.decision = AovDecision::RejectAction;
        return plan;
    }

    switch (event.source) {
        case WakeupSource::Timer:
            // 定时器唤醒 → 低功耗模式
            plan.decision = AovDecision::EnterLowPower;
            plan.restore_media = false;           // 不立即恢复常电
            plan.start_detect = true;             // 启动检测
            plan.start_low_power_record = true;   // 启动间隔录像（低功耗1fps）
            plan.target_frame_count = 0;          // 持续检测（由libmedia控制）
            break;
        case WakeupSource::AppRemote:
            // App预览唤醒 → 直接常电
            plan.decision = AovDecision::ResumeNormal;
            plan.restore_media = true;            // 立即恢复常电
            plan.start_preview = true;
            plan.start_cloud_live = true;
            plan.start_event_record = true;       // 启动事件录像（常电15fps）
            plan.apply_pending_config = true;
            break;
        case WakeupSource::Detect:
        case WakeupSource::Manual:
            plan.decision = AovDecision::ResumeNormal;
            plan.restore_media = true;
            plan.start_detect = true;
            plan.start_event_record = true;       // 事件录像
            plan.start_cloud_storage = true;
            plan.report_alarm = true;
            break;
        case WakeupSource::Unknown:
            plan.decision = AovDecision::KeepCurrentState;
            break;
    }

    return plan;
}

bool CoreWakeupPlanner::IsLowBatteryProtected(
    const WorkModePolicy& policy,
    const BatteryStatus& battery) const {
    return battery.low_battery_protect ||
           battery.percent <= policy.low_battery_protect_threshold_pct;
}

} // namespace aov::app::core

#include <iostream>

#include "../../app/core/core_wakeup_planner.hpp"
#include "../../app/core/idle_sleep_controller.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

aov::app::WorkModePolicy MakePolicy() {
    aov::app::WorkModePolicy policy;
    policy.configured_mode = aov::app::ConfiguredWorkMode::PowerSaving;
    policy.periodic_wakeup_interval_sec = 1;
    policy.normal_record_fps = 15;
    policy.low_battery_protect_threshold_pct = 10;
    policy.smart_to_save_threshold_pct = 70;
    policy.smart_to_awake_threshold_pct = 80;
    return policy;
}

} // namespace

int main() {
    using namespace aov::app;
    using namespace aov::app::core;

    CoreWakeupPlanner planner;
    WorkModePolicy policy = MakePolicy();

    WakeupEvent timer;
    timer.source = WakeupSource::Timer;
    CoreWakeupPlan plan = planner.BuildPlan(timer, policy, {});
    if (plan.decision != AovDecision::EnterLowPower ||
        !plan.restore_media ||
        !plan.start_detect ||
        plan.start_local_record ||
        plan.start_cloud_storage ||
        plan.target_frame_count != 1 ||
        plan.wakeup_interval_sec != 1) {
        return Fail("timer wakeup must restore low-power media and detect without event record");
    }

    WakeupEvent app_remote;
    app_remote.source = WakeupSource::AppRemote;
    app_remote.from_4g_modem = true;
    plan = planner.BuildPlan(app_remote, policy, {});
    if (plan.decision != AovDecision::ResumeNormal ||
        !plan.restore_media ||
        !plan.start_preview ||
        !plan.start_cloud_live ||
        plan.start_detect ||
        plan.start_local_record) {
        return Fail("app remote wakeup must resume normal media and start cloud live preview");
    }

    WakeupEvent detect;
    detect.source = WakeupSource::Detect;
    plan = planner.BuildPlan(detect, policy, {});
    if (plan.decision != AovDecision::ResumeNormal ||
        !plan.restore_media ||
        !plan.start_detect ||
        !plan.start_local_record ||
        !plan.start_cloud_storage ||
        !plan.report_alarm) {
        return Fail("detect wakeup must resume normal media and start record/cloud alarm chain");
    }

    BatteryStatus low_battery;
    low_battery.percent = 5;
    plan = planner.BuildPlan(app_remote, policy, low_battery);
    if (plan.decision != AovDecision::RejectAction ||
        plan.restore_media ||
        plan.start_cloud_live) {
        return Fail("low battery protection must reject non-essential wakeup plan");
    }

    IdleSleepController idle;
    BusinessState business;
    ModuleDrainState drain;
    drain.local_record_closed_flushed_synced = true;
    drain.cloud_storage_finished = true;
    drain.config_persisted = true;
    drain.idle_debounce_expired = true;
    IdleSleepDecision decision = idle.Evaluate(business, drain);
    if (decision.code != IdleSleepDecisionCode::CanSleep) {
        return Fail("idle business plus complete drain must allow sleep");
    }

    business.preview_active = true;
    decision = idle.Evaluate(business, drain);
    if (decision.code != IdleSleepDecisionCode::BusinessBusy ||
        decision.blocker != "preview") {
        return Fail("active preview must block sleep with explicit blocker");
    }

    business.preview_active = false;
    drain.cloud_storage_finished = false;
    decision = idle.Evaluate(business, drain);
    if (decision.code != IdleSleepDecisionCode::DrainIncomplete ||
        decision.blocker != "cloud_storage") {
        return Fail("unfinished cloud storage must block sleep with explicit blocker");
    }

    drain.cloud_storage_finished = true;
    drain.idle_debounce_expired = false;
    decision = idle.Evaluate(business, drain);
    if (decision.code != IdleSleepDecisionCode::WaitingIdleDebounce) {
        return Fail("sleep must wait for idle debounce after business and drain complete");
    }

    std::cout << "core wakeup idle plan contract passed\n";
    return 0;
}

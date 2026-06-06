#include <iostream>

#include "../../app/core/aov_orchestrator_stub.hpp"
#include "../../app/core/core_state.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

aov::app::WorkModePolicy MakePowerSavingPolicy() {
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

    AovOrchestratorStub core;
    if (core.Init(MakePowerSavingPolicy()) != AovStatusCode::Ok) {
        return Fail("core init must accept power saving policy");
    }

    WakeupEvent timer;
    timer.source = WakeupSource::Timer;
    timer.timestamp_ms = 1000;
    if (core.OnWakeupEvent(timer) != AovStatusCode::Ok ||
        core.GetRuntimeWorkState() != RuntimeWorkState::AwakeNormal) {
        return Fail("timer wakeup must resume normal runtime before detection");
    }

    DetectResultSummary no_target;
    no_target.timestamp_ms = 1200;
    if (core.OnDetectResult(no_target) != AovStatusCode::Ok ||
        core.GetRuntimeWorkState() != RuntimeWorkState::AwakeNormal) {
        return Fail("no-target wakeup must wait for drain and idle debounce before sleep");
    }

    ModuleDrainState drain;
    drain.local_record_closed_flushed_synced = true;
    drain.cloud_storage_finished = true;
    drain.config_persisted = true;
    drain.idle_debounce_expired = false;
    if (core.OnModuleDrainStateChanged(drain) != AovStatusCode::Ok ||
        core.GetRuntimeWorkState() != RuntimeWorkState::AwakeNormal) {
        return Fail("drain without idle debounce must keep device awake");
    }

    if (core.OnIdleDebounceExpired(8000) != AovStatusCode::Ok ||
        core.GetRuntimeWorkState() != RuntimeWorkState::SleepPeriodicDetect) {
        return Fail("all drain conditions plus idle debounce must allow AOV sleep");
    }

    WakeupEvent detect_wakeup;
    detect_wakeup.source = WakeupSource::Detect;
    detect_wakeup.timestamp_ms = 10000;
    if (core.OnWakeupEvent(detect_wakeup) != AovStatusCode::Ok) {
        return Fail("detect wakeup must be accepted");
    }

    DetectResultSummary target;
    target.has_person = true;
    target.object_count = 1;
    target.timestamp_ms = 10100;
    if (core.OnDetectResult(target) != AovStatusCode::Ok ||
        core.GetRuntimeWorkState() != RuntimeWorkState::AwakeNormal ||
        !core.GetRuntimeContext().recording_active) {
        return Fail("target detection must enter normal runtime and mark recording active");
    }

    drain.local_record_closed_flushed_synced = true;
    drain.cloud_storage_finished = false;
    drain.config_persisted = true;
    drain.idle_debounce_expired = true;
    if (core.OnModuleDrainStateChanged(drain) != AovStatusCode::Ok ||
        core.GetRuntimeWorkState() != RuntimeWorkState::AwakeNormal) {
        return Fail("unfinished cloud storage must block AOV sleep");
    }

    drain.cloud_storage_finished = true;
    if (core.OnModuleDrainStateChanged(drain) != AovStatusCode::Ok ||
        core.GetRuntimeWorkState() != RuntimeWorkState::SleepPeriodicDetect ||
        core.GetRuntimeContext().recording_active) {
        return Fail("cloud success or explicit failure plus local fsync must allow sleep");
    }

    PreviewRequest preview;
    preview.session_id = "preview-1";
    if (core.OnPreviewRequest(preview) != AovStatusCode::Ok ||
        core.GetRuntimeWorkState() != RuntimeWorkState::AwakeNormal) {
        return Fail("preview request must keep device awake");
    }

    if (core.OnPreviewSessionClosed(preview.session_id) != AovStatusCode::Ok ||
        core.GetRuntimeWorkState() != RuntimeWorkState::AwakeNormal) {
        return Fail("preview close must wait for idle debounce instead of sleeping immediately");
    }

    drain.local_record_closed_flushed_synced = true;
    drain.cloud_storage_finished = true;
    drain.config_persisted = true;
    drain.idle_debounce_expired = false;
    if (core.OnModuleDrainStateChanged(drain) != AovStatusCode::Ok ||
        core.OnIdleDebounceExpired(20000) != AovStatusCode::Ok ||
        core.GetRuntimeWorkState() != RuntimeWorkState::SleepPeriodicDetect) {
        return Fail("preview close can sleep only after idle debounce expires");
    }

    std::cout << "core sleep drain contract passed\n";
    return 0;
}

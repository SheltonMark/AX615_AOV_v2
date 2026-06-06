#pragma once

#include <cstdint>
#include <string>

#include "common/status_code.hpp"

namespace aov::app {

enum class ConfiguredWorkMode {
    PowerSaving,
    SmartNoSleep,
    UltraLongStandby,
};

enum class RuntimeWorkState {
    AwakeNormal,
    SleepPeriodicDetect,
    SleepAppWakeOnly,
    LowBatteryProtection,
};

enum class WakeupSource {
    Timer,
    Detect,
    AppRemote,
    Manual,
    Unknown,
};

enum class AovDecision {
    ResumeNormal,
    EnterLowPower,
    KeepCurrentState,
    RejectAction,
};

using AovStatusCode = aov::common::StatusCode;

struct WorkModePolicy {
    ConfiguredWorkMode configured_mode {ConfiguredWorkMode::PowerSaving};
    int periodic_wakeup_interval_sec {1};
    int normal_record_fps {15};
    int low_battery_protect_threshold_pct {10};
    int smart_to_save_threshold_pct {70};
    int smart_to_awake_threshold_pct {80};
};

struct BatteryStatus {
    int percent {100};
    bool charging {false};
    bool low_battery_protect {false};
};

struct WakeupEvent {
    WakeupSource source {WakeupSource::Unknown};
    std::uint64_t timestamp_ms {0};
    bool from_4g_modem {false};
};

struct DetectResultSummary {
    bool has_person {false};
    bool has_pet {false};
    bool has_vehicle {false};
    int object_count {0};
    std::uint64_t frame_id {0};
    std::uint64_t timestamp_ms {0};
};

struct PreviewRequest {
    std::string session_id;
    std::string user_id;
    bool force_wakeup {false};
    int expected_timeout_sec {0};
};

struct AovRuntimeContext {
    WorkModePolicy policy;
    BatteryStatus battery;
    RuntimeWorkState runtime_state {RuntimeWorkState::SleepPeriodicDetect};
    bool preview_active {false};
    bool recording_active {false};
    bool waiting_idle_debounce {false};
    std::uint64_t last_wakeup_ts_ms {0};
    std::uint64_t last_idle_debounce_ts_ms {0};
};

} // namespace aov::app

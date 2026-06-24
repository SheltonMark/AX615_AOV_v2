#pragma once

#include "app/config/config_types.hpp"
#include "app/core/aov_types.hpp"
#include "app/core/core_commands.hpp"
#include "app/core/core_state.hpp"

namespace aov::app::core {

enum class CoreEventType {
    Wakeup,
    BatteryChanged,
    DetectResult,
    ConfigChanged,
    CloudCommand,
    ModuleStateChanged,
    IdleDebounceExpired,
};

struct ConfigChangedEvent {
    aov::app::config::PendingConfigChange pending;
    bool triggered_by_cloud {false};
    bool triggered_by_app {false};
};

struct ModuleStateReport {
    bool media_running {false};
    bool storage_recording {false};
    bool cloud_storage_running {false};
    bool sd_ready {false};
    aov::app::BatteryStatus battery;
};

struct CoreEvent {
    CoreEventType type {CoreEventType::Wakeup};
    aov::app::WakeupEvent wakeup;
    aov::app::BatteryStatus battery;
    aov::app::DetectResultSummary detect;
    ConfigChangedEvent config_changed;
    CoreCommand command;
    ModuleDrainState module_drain_state;  // 改为 ModuleDrainState
    std::uint64_t idle_timeout_timestamp_ms {0};  // 用于 IdleDebounceExpired 事件
};

} // namespace aov::app::core

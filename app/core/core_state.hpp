#pragma once

namespace aov::app::core {

enum class ProtectionState {
    None,
    LowBatteryProtection,
};

struct BusinessState {
    bool preview_active {false};
    bool local_recording {false};
    bool cloud_storage_running {false};
    bool alarm_processing {false};
    bool pending_config_apply {false};
    bool command_executing {false};
};

struct ModuleDrainState {
    bool local_record_closed_flushed_synced {false};
    bool cloud_storage_finished {false};
    bool config_persisted {false};
    bool idle_debounce_expired {false};
};

inline bool IsBusinessIdle(const BusinessState& state) {
    return !state.preview_active &&
           !state.local_recording &&
           !state.cloud_storage_running &&
           !state.alarm_processing &&
           !state.pending_config_apply &&
           !state.command_executing;
}

inline bool CanEnterAovSleep(const ModuleDrainState& state) {
    return state.local_record_closed_flushed_synced &&
           state.cloud_storage_finished &&
           state.config_persisted &&
           state.idle_debounce_expired;
}

} // namespace aov::app::core

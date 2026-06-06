#pragma once

#include "app/core/core_state.hpp"

namespace aov::app::core {

inline ModuleDrainState MergeDrainStates(const ModuleDrainState& left,
                                         const ModuleDrainState& right) {
    ModuleDrainState merged;
    merged.local_record_closed_flushed_synced =
        left.local_record_closed_flushed_synced ||
        right.local_record_closed_flushed_synced;
    merged.cloud_storage_finished =
        left.cloud_storage_finished || right.cloud_storage_finished;
    merged.config_persisted = left.config_persisted || right.config_persisted;
    merged.idle_debounce_expired =
        left.idle_debounce_expired || right.idle_debounce_expired;
    return merged;
}

} // namespace aov::app::core

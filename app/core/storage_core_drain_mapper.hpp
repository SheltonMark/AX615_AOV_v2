#pragma once

#include "app/core/core_state.hpp"
#include "app/storage/storage_types.hpp"

namespace aov::app::core {

ModuleDrainState BuildLocalRecordDrainState(
    const aov::app::storage::StorageStats& storage_stats);

} // namespace aov::app::core

#include "app/core/storage_core_drain_mapper.hpp"

namespace aov::app::core {

ModuleDrainState BuildLocalRecordDrainState(
    const aov::app::storage::StorageStats& storage_stats) {
    ModuleDrainState drain;
    drain.local_record_closed_flushed_synced =
        !storage_stats.recording && storage_stats.flushed;
    return drain;
}

} // namespace aov::app::core

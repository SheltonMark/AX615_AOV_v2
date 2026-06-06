#include <iostream>

#include "../../app/core/storage_core_drain_mapper.hpp"
#include "../../app/storage/storage_types.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

} // namespace

int main() {
    using namespace aov::app::core;
    using namespace aov::app::storage;

    StorageStats stats;
    stats.recording = true;
    stats.flushed = false;

    auto drain = BuildLocalRecordDrainState(stats);
    if (drain.local_record_closed_flushed_synced) {
        return Fail("recording storage must not be treated as locally drained");
    }

    stats.recording = false;
    stats.flushed = false;
    drain = BuildLocalRecordDrainState(stats);
    if (drain.local_record_closed_flushed_synced) {
        return Fail("closed but unflushed storage must not be treated as locally drained");
    }

    stats.recording = false;
    stats.flushed = true;
    drain = BuildLocalRecordDrainState(stats);
    if (!drain.local_record_closed_flushed_synced) {
        return Fail("closed and flushed storage must be treated as locally drained");
    }

    drain.cloud_storage_finished = true;
    drain.config_persisted = true;
    drain.idle_debounce_expired = false;
    if (CanEnterAovSleep(drain)) {
        return Fail("storage drain alone must not bypass idle debounce");
    }

    drain.idle_debounce_expired = true;
    if (!CanEnterAovSleep(drain)) {
        return Fail("all drain conditions must allow AOV sleep");
    }

    std::cout << "storage core drain mapper contract passed\n";
    return 0;
}

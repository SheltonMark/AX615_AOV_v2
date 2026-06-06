#include <iostream>

#include "../../app/core/core_drain_merge.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

} // namespace

int main() {
    using namespace aov::app::core;

    ModuleDrainState local;
    local.local_record_closed_flushed_synced = true;

    ModuleDrainState cloud;
    cloud.cloud_storage_finished = false;

    ModuleDrainState merged = MergeDrainStates(local, cloud);
    merged.config_persisted = true;
    merged.idle_debounce_expired = true;
    if (CanEnterAovSleep(merged)) {
        return Fail("unfinished cloud drain must block merged sleep decision");
    }

    cloud.cloud_storage_finished = true;
    merged = MergeDrainStates(local, cloud);
    merged.config_persisted = true;
    merged.idle_debounce_expired = false;
    if (CanEnterAovSleep(merged)) {
        return Fail("idle debounce must still block merged sleep decision");
    }

    merged.idle_debounce_expired = true;
    if (!CanEnterAovSleep(merged)) {
        return Fail("all merged drain conditions must allow sleep");
    }

    std::cout << "core drain merge contract passed\n";
    return 0;
}

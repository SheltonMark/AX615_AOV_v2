#include <iostream>

#include "../../app/core/core_events.hpp"
#include "../../app/core/core_state.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

} // namespace

int main() {
    using namespace aov::app;
    using namespace aov::app::core;

    BusinessState business;
    if (!IsBusinessIdle(business)) {
        return Fail("default business state must be idle");
    }

    business.local_recording = true;
    if (IsBusinessIdle(business)) {
        return Fail("local recording must make business non-idle");
    }
    business.local_recording = false;
    business.cloud_storage_running = true;
    if (IsBusinessIdle(business)) {
        return Fail("cloud storage must make business non-idle");
    }
    business.cloud_storage_running = false;
    business.pending_config_apply = true;
    if (IsBusinessIdle(business)) {
        return Fail("pending config apply must make business non-idle");
    }

    ModuleDrainState drain;
    drain.local_record_closed_flushed_synced = true;
    drain.cloud_storage_finished = true;
    drain.config_persisted = true;
    drain.idle_debounce_expired = true;
    if (!CanEnterAovSleep(drain)) {
        return Fail("all drain conditions satisfied must allow sleep");
    }
    drain.cloud_storage_finished = false;
    if (CanEnterAovSleep(drain)) {
        return Fail("unfinished cloud storage must block sleep");
    }

    ConfigChangedEvent event;
    event.pending.changed_fields.push_back("work_mode");
    event.pending.requires_wakeup_apply = true;
    event.triggered_by_cloud = true;
    if (event.pending.changed_fields.size() != 1 ||
        !event.pending.requires_wakeup_apply ||
        !event.triggered_by_cloud) {
        return Fail("config changed event must carry pending config metadata");
    }

    ModuleStateReport report;
    report.media_running = true;
    report.storage_recording = false;
    report.cloud_storage_running = true;
    report.sd_ready = true;
    report.battery.percent = 80;
    if (!report.media_running ||
        !report.cloud_storage_running ||
        !report.sd_ready ||
        report.battery.percent != 80) {
        return Fail("module state report must carry runtime module states");
    }

    CoreEvent core_event;
    core_event.type = CoreEventType::ConfigChanged;
    core_event.config_changed = event;
    if (core_event.type != CoreEventType::ConfigChanged ||
        !core_event.config_changed.triggered_by_cloud) {
        return Fail("core event must carry config changed event");
    }

    std::cout << "core state events contract passed\n";
    return 0;
}

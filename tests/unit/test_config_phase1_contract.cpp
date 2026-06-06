#include <iostream>

#include "../../app/config/config_diff.hpp"
#include "../../app/config/config_service_stub.hpp"
#include "../../app/config/config_validation.hpp"
#include "../../app/config/device_config.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

} // namespace

int main() {
    using namespace aov::app::config;

    const DeviceConfig defaults = MakeDefaultDeviceConfig();
    if (defaults.work_mode.mode != ConfiguredWorkMode::PowerSaving) {
        return Fail("default work mode must be power saving");
    }
    if (defaults.work_mode.record_interval_sec != 1) {
        return Fail("default AOV record interval must be 1 second");
    }
    if (defaults.work_mode.low_battery_protect_threshold_pct != 10) {
        return Fail("default low battery threshold must be 10 percent");
    }
    if (defaults.media.main_stream.codec != VideoEncodeType::H265 ||
        defaults.media.main_stream.width != 2560 ||
        defaults.media.main_stream.height != 1440) {
        return Fail("default main stream must be H265 2560x1440");
    }
    if (defaults.record.sd.record_enabled != true ||
        defaults.record.sd.loop_record != true) {
        return Fail("default SD recording and loop recording must be enabled");
    }

    auto validation = ValidateDeviceConfig(defaults);
    if (!validation.ok()) {
        return Fail("default device config must validate");
    }

    DeviceConfig invalid = defaults;
    invalid.work_mode.record_interval_sec = 2;
    validation = ValidateDeviceConfig(invalid);
    if (validation.ok() || validation.field != "work_mode.record_interval_sec") {
        return Fail("invalid AOV record interval must be rejected");
    }

    invalid = defaults;
    invalid.work_mode.low_battery_protect_threshold_pct = 31;
    validation = ValidateDeviceConfig(invalid);
    if (validation.ok() || validation.field != "work_mode.low_battery_protect_threshold_pct") {
        return Fail("invalid low battery threshold must be rejected");
    }

    invalid = defaults;
    invalid.media.main_stream.fps_aov = 4;
    validation = ValidateDeviceConfig(invalid);
    if (validation.ok() || validation.field != "media.main_stream.fps_aov") {
        return Fail("invalid AOV fps must be rejected");
    }

    DeviceConfig desired = defaults;
    desired.work_mode.mode = ConfiguredWorkMode::UltraLongStandby;
    desired.media.main_stream.width = 1920;
    desired.media.main_stream.height = 1080;

    const PendingConfigChange pending = BuildPendingConfigChange(defaults, desired);
    if (pending.changed_fields.size() != 2) {
        return Fail("config diff must report two changed fields");
    }
    if (!pending.requires_media_restart) {
        return Fail("media stream resolution change must require media restart");
    }
    if (!pending.requires_wakeup_apply) {
        return Fail("work mode change must require wakeup apply");
    }

    ConfigServiceStub service;
    if (!service.LoadFactoryConfig().ok()) {
        return Fail("config service must load factory config");
    }
    if (!service.UpdateDesiredConfig(desired).ok()) {
        return Fail("config service must accept valid desired config");
    }
    const PendingConfigChange service_pending = service.GetPendingConfigChange();
    if (service_pending.changed_fields.empty()) {
        return Fail("config service must expose pending change after desired update");
    }
    if (!service.MarkApplied().ok()) {
        return Fail("config service must mark pending config applied");
    }
    if (!service.GetPendingConfigChange().changed_fields.empty()) {
        return Fail("pending change must be clear after mark applied");
    }
    if (service.GetActiveConfig().work_mode.mode != ConfiguredWorkMode::UltraLongStandby) {
        return Fail("active config must update after mark applied");
    }

    std::cout << "config phase1 contract passed\n";
    return 0;
}

#include <iostream>

#include "../../app/core/system_sleep_coordinator.hpp"
#include "../../libsys/Src/power/power_service_stub.hpp"
#include "../../libsys/Src/storage/storage_service_stub.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

} // namespace

int main() {
    using namespace aov::app;
    using namespace aov::app::core;
    using namespace aov::sys;

    PowerServiceStub power;
    StorageServiceStub storage;
    SystemSleepCoordinator coordinator(power, storage);

    WorkModePolicy policy;
    policy.configured_mode = ConfiguredWorkMode::PowerSaving;
    policy.periodic_wakeup_interval_sec = 3;

    SystemSleepResult result = coordinator.PrepareAndEnterAovSleep(policy);
    if (!result.ok()) {
        return Fail("power saving sleep preparation must succeed");
    }
    if (!storage.IsMounted() ||
        storage.GetSdStatus() != SdCardStatus::Mounted ||
        power.GetWakeupIntervalSec() != 3 ||
        !power.HasEnteredSleep()) {
        return Fail("sleep coordinator must mount sd, schedule wakeup, and enter sleep");
    }

    policy.configured_mode = ConfiguredWorkMode::UltraLongStandby;
    result = coordinator.PrepareAndEnterAovSleep(policy);
    if (!result.ok() ||
        power.GetWakeupIntervalSec() != 0) {
        return Fail("ultra long standby must disable periodic wakeup timer before sleep");
    }

    storage.SetSdStatus(SdCardStatus::Error);
    policy.configured_mode = ConfiguredWorkMode::PowerSaving;
    result = coordinator.PrepareAndEnterAovSleep(policy);
    if (result.ok() ||
        result.failed_module != "libsys.sd") {
        return Fail("sd error must block entering AOV sleep with explicit module");
    }

    storage.SetSdStatus(SdCardStatus::NotMounted);
    power.SetEnterSleepResult(SysStatusCode::InternalError);
    result = coordinator.PrepareAndEnterAovSleep(policy);
    if (result.ok() ||
        result.failed_module != "libsys.power") {
        return Fail("power sleep failure must return explicit module");
    }

    std::cout << "core system sleep coordinator contract passed\n";
    return 0;
}

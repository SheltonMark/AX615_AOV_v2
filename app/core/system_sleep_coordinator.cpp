#include "app/core/system_sleep_coordinator.hpp"

#include <utility>

namespace aov::app::core {

namespace {

SystemSleepResult Ok() {
    return {};
}

SystemSleepResult Error(SystemSleepStatusCode code,
                        std::string module,
                        std::string message) {
    SystemSleepResult result;
    result.code = code;
    result.failed_module = std::move(module);
    result.message = std::move(message);
    return result;
}

} // namespace

SystemSleepCoordinator::SystemSleepCoordinator(aov::sys::IPowerService& power_service,
                                               aov::sys::IStorageService& storage_service)
    : power_service_(power_service),
      storage_service_(storage_service) {}

SystemSleepResult SystemSleepCoordinator::PrepareAndEnterAovSleep(
    const WorkModePolicy& policy) {
    SystemSleepResult result = EnsureSdReady();
    if (!result.ok()) {
        return result;
    }

    result = ConfigureWakeupPolicy(policy);
    if (!result.ok()) {
        return result;
    }

    return EnterSleep();
}

SystemSleepResult SystemSleepCoordinator::EnsureSdReady() {
    const aov::sys::SdCardStatus status = storage_service_.GetSdStatus();
    if (status == aov::sys::SdCardStatus::Mounted) {
        return Ok();
    }

    if (status == aov::sys::SdCardStatus::Error ||
        status == aov::sys::SdCardStatus::NotDetected ||
        status == aov::sys::SdCardStatus::Formatting) {
        return Error(SystemSleepStatusCode::SdNotReady,
                     "libsys.sd",
                     "sd card is not ready for AOV sleep");
    }

    const aov::sys::SysStatusCode mount_status = storage_service_.MountSdCard();
    if (mount_status != aov::sys::SysStatusCode::Ok) {
        return Error(SystemSleepStatusCode::SdNotReady,
                     "libsys.sd",
                     "failed to mount sd card");
    }

    return Ok();
}

SystemSleepResult SystemSleepCoordinator::ConfigureWakeupPolicy(
    const WorkModePolicy& policy) {
    aov::sys::SysStatusCode status = aov::sys::SysStatusCode::Ok;
    if (policy.configured_mode == ConfiguredWorkMode::UltraLongStandby) {
        status = power_service_.DisableWakeupTimer();
    } else {
        status = power_service_.ConfigureWakeupTimer(policy.periodic_wakeup_interval_sec);
    }

    if (status != aov::sys::SysStatusCode::Ok) {
        return Error(SystemSleepStatusCode::PowerFailed,
                     "libsys.power",
                     "failed to configure wakeup policy");
    }

    return Ok();
}

SystemSleepResult SystemSleepCoordinator::EnterSleep() {
    const aov::sys::SysStatusCode status = power_service_.EnterSleep();
    if (status != aov::sys::SysStatusCode::Ok) {
        return Error(SystemSleepStatusCode::PowerFailed,
                     "libsys.power",
                     "failed to enter AOV sleep");
    }

    return Ok();
}

} // namespace aov::app::core

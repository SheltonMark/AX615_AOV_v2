#pragma once

#include <string>

#include "app/core/aov_types.hpp"
#include "power/i_power_service.hpp"
#include "storage/i_storage_service.hpp"

namespace aov::app::core {

enum class SystemSleepStatusCode {
    Ok,
    InvalidArgument,
    SdNotReady,
    PowerFailed,
};

struct SystemSleepResult {
    SystemSleepStatusCode code {SystemSleepStatusCode::Ok};
    std::string failed_module;
    std::string message;

    bool ok() const {
        return code == SystemSleepStatusCode::Ok;
    }
};

class SystemSleepCoordinator {
public:
    SystemSleepCoordinator(aov::sys::IPowerService& power_service,
                           aov::sys::IStorageService& storage_service);

    SystemSleepResult PrepareAndEnterAovSleep(const WorkModePolicy& policy);

private:
    SystemSleepResult EnsureSdReady();
    SystemSleepResult ConfigureWakeupPolicy(const WorkModePolicy& policy);
    SystemSleepResult EnterSleep();

private:
    aov::sys::IPowerService& power_service_;
    aov::sys::IStorageService& storage_service_;
};

} // namespace aov::app::core

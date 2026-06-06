#pragma once

#include <string>

#include "app/cloud/i_cloud_service.hpp"
#include "app/config/device_config.hpp"
#include "app/core/core_apply_coordinator.hpp"
#include "app/core/system_sleep_coordinator.hpp"
#include "app/storage/i_storage_service.hpp"
#include "libmedia/Include/runtime/i_media_runtime.hpp"
#include "power/i_power_service.hpp"
#include "storage/i_storage_service.hpp"

namespace aov::app::core {

enum class CoreRuntimeStatusCode {
    Ok,
    ApplyFailed,
    DrainIncomplete,
    SleepFailed,
};

struct CoreRuntimeResult {
    CoreRuntimeStatusCode code {CoreRuntimeStatusCode::Ok};
    std::string failed_module;
    std::string message;

    bool ok() const {
        return code == CoreRuntimeStatusCode::Ok;
    }
};

class CoreRuntimeFacade {
public:
    CoreRuntimeFacade(aov::media::IMediaRuntime& media_runtime,
                      aov::app::storage::IStorageService& app_storage,
                      aov::app::ICloudService& cloud_service,
                      aov::sys::IPowerService& power_service,
                      aov::sys::IStorageService& sys_storage);

    CoreRuntimeResult ApplyConfig(const aov::app::config::DeviceConfig& config);
    CoreRuntimeResult TryEnterAovSleep(const aov::app::config::WorkModeConfig& work_mode,
                                       bool idle_debounce_expired,
                                       bool config_persisted);

private:
    aov::media::IMediaRuntime& media_runtime_;
    aov::app::storage::IStorageService& app_storage_;
    aov::app::ICloudService& cloud_service_;
    aov::sys::IPowerService& power_service_;
    aov::sys::IStorageService& sys_storage_;
};

} // namespace aov::app::core

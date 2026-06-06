#pragma once

#include <string>

#include "app/config/device_config.hpp"
#include "app/storage/i_storage_service.hpp"
#include "libmedia/Include/runtime/i_media_runtime.hpp"

namespace aov::app::core {

enum class CoreApplyStatusCode {
    Ok,
    InvalidArgument,
    MediaApplyFailed,
    StorageApplyFailed,
};

struct CoreApplyResult {
    CoreApplyStatusCode code {CoreApplyStatusCode::Ok};
    std::string failed_module;
    std::string message;

    bool ok() const {
        return code == CoreApplyStatusCode::Ok;
    }
};

class CoreApplyCoordinator {
public:
    CoreApplyCoordinator(aov::media::IMediaRuntime& media_runtime,
                         aov::app::storage::IStorageService& storage_service);

    CoreApplyResult ApplyDeviceConfig(const aov::app::config::DeviceConfig& config);

private:
    CoreApplyResult ApplyMediaConfig(const aov::app::config::DeviceConfig& config);
    CoreApplyResult ApplyStorageConfig(const aov::app::config::RecordConfig& config);

private:
    aov::media::IMediaRuntime& media_runtime_;
    aov::app::storage::IStorageService& storage_service_;
};

} // namespace aov::app::core

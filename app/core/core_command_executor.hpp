#pragma once

#include <string>
#include <utility>

#include "app/config/config_service_stub.hpp"
#include "app/core/core_commands.hpp"
#include "app/storage/i_storage_service.hpp"
#include "libmedia/Include/runtime/i_media_runtime.hpp"
#include "device/i_device_control_service.hpp"
#include "storage/i_storage_service.hpp"

namespace aov::app::core {

enum class CoreCommandExecStatusCode {
    Ok,
    Unsupported,
    Failed,
};

struct CoreCommandExecResult {
    CoreCommandExecStatusCode code {CoreCommandExecStatusCode::Ok};
    std::string failed_module;
    std::string message;

    static CoreCommandExecResult Ok() {
        return {};
    }

    static CoreCommandExecResult Error(CoreCommandExecStatusCode status,
                                       std::string module,
                                       std::string reason) {
        CoreCommandExecResult result;
        result.code = status;
        result.failed_module = std::move(module);
        result.message = std::move(reason);
        return result;
    }

    bool ok() const {
        return code == CoreCommandExecStatusCode::Ok;
    }
};

class CoreCommandExecutor final {
public:
    CoreCommandExecutor(aov::media::IMediaRuntime& media_runtime,
                        aov::app::config::ConfigServiceStub& config_service,
                        aov::app::storage::IStorageService& storage_service);
    CoreCommandExecutor(aov::media::IMediaRuntime& media_runtime,
                        aov::app::config::ConfigServiceStub& config_service,
                        aov::app::storage::IStorageService& storage_service,
                        aov::sys::IStorageService& sys_storage_service);
    CoreCommandExecutor(aov::media::IMediaRuntime& media_runtime,
                        aov::app::config::ConfigServiceStub& config_service,
                        aov::app::storage::IStorageService& storage_service,
                        aov::sys::IStorageService& sys_storage_service,
                        aov::sys::IDeviceControlService& device_control_service);

    CoreCommandExecResult Execute(const CoreCommand& command);

private:
    CoreCommandExecResult ExecuteFormatStorageMedium(const CoreCommand& command);
    CoreCommandExecResult ExecuteOneKeyAlarm(const CoreCommand& command);
    CoreCommandExecResult ExecutePreviewImageCapture(const CoreCommand& command);
    CoreCommandExecResult ExecuteReboot(const CoreCommand& command);
    CoreCommandExecResult ExecuteSetDefaultDevConfigs(const CoreCommand& command);

    aov::media::IMediaRuntime& media_runtime_;
    aov::app::config::ConfigServiceStub& config_service_;
    aov::app::storage::IStorageService& storage_service_;
    aov::sys::IStorageService* sys_storage_service_ {nullptr};
    aov::sys::IDeviceControlService* device_control_service_ {nullptr};
};

} // namespace aov::app::core

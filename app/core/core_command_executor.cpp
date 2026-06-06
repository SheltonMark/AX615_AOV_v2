#include "app/core/core_command_executor.hpp"

namespace aov::app::core {

namespace {

CoreCommandExecResult FromStorageResult(const aov::app::storage::StorageResult& result,
                                        const char* module) {
    if (result.ok()) {
        return CoreCommandExecResult::Ok();
    }
    return CoreCommandExecResult::Error(CoreCommandExecStatusCode::Failed,
                                        module,
                                        result.message);
}

CoreCommandExecResult FromMediaStatus(const aov::media::MediaStatusCode status,
                                      const char* module,
                                      const char* message) {
    if (status == aov::media::MediaStatusCode::Ok) {
        return CoreCommandExecResult::Ok();
    }
    return CoreCommandExecResult::Error(CoreCommandExecStatusCode::Failed,
                                        module,
                                        message);
}

} // namespace

CoreCommandExecutor::CoreCommandExecutor(
    aov::media::IMediaRuntime& media_runtime,
    aov::app::config::ConfigServiceStub& config_service,
    aov::app::storage::IStorageService& storage_service)
    : media_runtime_(media_runtime),
      config_service_(config_service),
      storage_service_(storage_service) {
}

CoreCommandExecutor::CoreCommandExecutor(
    aov::media::IMediaRuntime& media_runtime,
    aov::app::config::ConfigServiceStub& config_service,
    aov::app::storage::IStorageService& storage_service,
    aov::sys::IStorageService& sys_storage_service)
    : media_runtime_(media_runtime),
      config_service_(config_service),
      storage_service_(storage_service),
      sys_storage_service_(&sys_storage_service) {
}

CoreCommandExecutor::CoreCommandExecutor(
    aov::media::IMediaRuntime& media_runtime,
    aov::app::config::ConfigServiceStub& config_service,
    aov::app::storage::IStorageService& storage_service,
    aov::sys::IStorageService& sys_storage_service,
    aov::sys::IDeviceControlService& device_control_service)
    : media_runtime_(media_runtime),
      config_service_(config_service),
      storage_service_(storage_service),
      sys_storage_service_(&sys_storage_service),
      device_control_service_(&device_control_service) {
}

CoreCommandExecResult CoreCommandExecutor::Execute(const CoreCommand& command) {
    switch (command.type) {
        case CoreCommandType::FormatStorageMedium:
            return ExecuteFormatStorageMedium(command);
        case CoreCommandType::OneKeyAlarm:
            return ExecuteOneKeyAlarm(command);
        case CoreCommandType::PreviewImageCapture:
            return ExecutePreviewImageCapture(command);
        case CoreCommandType::SetDefaultDevConfigs:
            return ExecuteSetDefaultDevConfigs(command);
        case CoreCommandType::Reboot:
            return ExecuteReboot(command);
    }
    return CoreCommandExecResult::Error(CoreCommandExecStatusCode::Unsupported,
                                        "app.core.command",
                                        "unsupported core command");
}

CoreCommandExecResult CoreCommandExecutor::ExecuteFormatStorageMedium(
    const CoreCommand&) {
    const auto close_result = FromStorageResult(storage_service_.FlushAndClose(),
                                                "app.storage.format");
    if (!close_result.ok()) {
        return close_result;
    }
    if (sys_storage_service_ == nullptr) {
        return CoreCommandExecResult::Error(CoreCommandExecStatusCode::Unsupported,
                                            "libsys.storage.format",
                                            "format command needs libsys storage service");
    }

    const auto format_status = sys_storage_service_->FormatSdCard();
    if (format_status != aov::sys::SysStatusCode::Ok) {
        return CoreCommandExecResult::Error(CoreCommandExecStatusCode::Failed,
                                            "libsys.storage.format",
                                            "format SD card failed");
    }
    return CoreCommandExecResult::Ok();
}

CoreCommandExecResult CoreCommandExecutor::ExecuteOneKeyAlarm(
    const CoreCommand&) {
    const auto record_result = FromStorageResult(storage_service_.StartRecord(),
                                                 "app.storage.one_key_alarm");
    if (!record_result.ok()) {
        return record_result;
    }
    if (device_control_service_ != nullptr) {
        const auto light_status = device_control_service_->SetFillLight(true);
        if (light_status != aov::sys::SysStatusCode::Ok) {
            return CoreCommandExecResult::Error(CoreCommandExecStatusCode::Failed,
                                                "libsys.device.fill_light",
                                                "enable fill light failed");
        }
        const auto sound_status = device_control_service_->SetSoundAlarm(true, 1);
        if (sound_status != aov::sys::SysStatusCode::Ok) {
            return CoreCommandExecResult::Error(CoreCommandExecStatusCode::Failed,
                                                "libsys.device.sound_alarm",
                                                "enable sound alarm failed");
        }
    }
    return CoreCommandExecResult::Ok();
}

CoreCommandExecResult CoreCommandExecutor::ExecutePreviewImageCapture(
    const CoreCommand& command) {
    aov::media::SnapshotRequest request;
    request.chn_id = command.stream_id == "sub" ? 1 : 0;
    aov::media::SnapshotResult result;
    return FromMediaStatus(media_runtime_.CaptureSnapshot(request, result),
                           "libmedia.snapshot",
                           "capture snapshot failed");
}

CoreCommandExecResult CoreCommandExecutor::ExecuteReboot(
    const CoreCommand& command) {
    if (device_control_service_ == nullptr) {
        return CoreCommandExecResult::Error(CoreCommandExecStatusCode::Unsupported,
                                            "libsys.device.reboot",
                                            "reboot command needs libsys device service");
    }
    const auto status = device_control_service_->Reboot(command.delay_sec);
    if (status != aov::sys::SysStatusCode::Ok) {
        return CoreCommandExecResult::Error(CoreCommandExecStatusCode::Failed,
                                            "libsys.device.reboot",
                                            "reboot device failed");
    }
    return CoreCommandExecResult::Ok();
}

CoreCommandExecResult CoreCommandExecutor::ExecuteSetDefaultDevConfigs(
    const CoreCommand&) {
    const auto result = config_service_.LoadFactoryConfig();
    if (result.ok()) {
        return CoreCommandExecResult::Ok();
    }
    return CoreCommandExecResult::Error(CoreCommandExecStatusCode::Failed,
                                        "app.config.reset_default",
                                        result.message);
}

} // namespace aov::app::core

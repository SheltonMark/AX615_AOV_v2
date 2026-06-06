#include "app/core/core_runtime_facade.hpp"

#include "app/config/config_core_mapper.hpp"
#include "app/core/cloud_core_drain_mapper.hpp"
#include "app/core/core_drain_merge.hpp"
#include "app/core/storage_core_drain_mapper.hpp"

namespace aov::app::core {

namespace {

CoreRuntimeResult Ok() {
    return {};
}

CoreRuntimeResult Error(CoreRuntimeStatusCode code,
                        std::string module,
                        std::string message) {
    CoreRuntimeResult result;
    result.code = code;
    result.failed_module = std::move(module);
    result.message = std::move(message);
    return result;
}

aov::app::config::DeviceConfig BuildPolicyConfig(
    const aov::app::config::WorkModeConfig& work_mode) {
    aov::app::config::DeviceConfig config = aov::app::config::MakeDefaultDeviceConfig();
    config.work_mode = work_mode;
    return config;
}

} // namespace

CoreRuntimeFacade::CoreRuntimeFacade(aov::media::IMediaRuntime& media_runtime,
                                     aov::app::storage::IStorageService& app_storage,
                                     aov::app::ICloudService& cloud_service,
                                     aov::sys::IPowerService& power_service,
                                     aov::sys::IStorageService& sys_storage)
    : media_runtime_(media_runtime),
      app_storage_(app_storage),
      cloud_service_(cloud_service),
      power_service_(power_service),
      sys_storage_(sys_storage) {}

CoreRuntimeResult CoreRuntimeFacade::ApplyConfig(
    const aov::app::config::DeviceConfig& config) {
    CoreApplyCoordinator coordinator(media_runtime_, app_storage_);
    const CoreApplyResult result = coordinator.ApplyDeviceConfig(config);
    if (!result.ok()) {
        return Error(CoreRuntimeStatusCode::ApplyFailed,
                     result.failed_module,
                     result.message);
    }

    return Ok();
}

CoreRuntimeResult CoreRuntimeFacade::TryEnterAovSleep(
    const aov::app::config::WorkModeConfig& work_mode,
    const bool idle_debounce_expired,
    const bool config_persisted) {
    ModuleDrainState storage_drain = BuildLocalRecordDrainState(app_storage_.GetStats());
    ModuleDrainState cloud_drain = BuildCloudStorageDrainState(cloud_service_.GetRuntimeStatus());
    ModuleDrainState drain = MergeDrainStates(storage_drain, cloud_drain);
    drain.idle_debounce_expired = idle_debounce_expired;
    drain.config_persisted = config_persisted;

    if (!CanEnterAovSleep(drain)) {
        return Error(CoreRuntimeStatusCode::DrainIncomplete,
                     "app.core.drain",
                     "module drain conditions are not complete");
    }

    const aov::app::config::DeviceConfig policy_config = BuildPolicyConfig(work_mode);
    const WorkModePolicy policy = aov::app::config::BuildWorkModePolicy(policy_config);

    SystemSleepCoordinator coordinator(power_service_, sys_storage_);
    const SystemSleepResult sleep_result = coordinator.PrepareAndEnterAovSleep(policy);
    if (!sleep_result.ok()) {
        return Error(CoreRuntimeStatusCode::SleepFailed,
                     sleep_result.failed_module,
                     sleep_result.message);
    }

    return Ok();
}

} // namespace aov::app::core

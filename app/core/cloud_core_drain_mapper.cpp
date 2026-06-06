#include "app/core/cloud_core_drain_mapper.hpp"

namespace aov::app::core {

ModuleDrainState BuildCloudStorageDrainState(
    const aov::app::cloud::CloudRuntimeStatus& cloud_status) {
    ModuleDrainState drain;
    drain.cloud_storage_finished =
        !cloud_status.cloud_storage_running &&
        (cloud_status.cloud_storage_result == aov::app::cloud::CloudStorageResult::Success ||
         cloud_status.cloud_storage_result == aov::app::cloud::CloudStorageResult::ExplicitFailure);
    return drain;
}

} // namespace aov::app::core

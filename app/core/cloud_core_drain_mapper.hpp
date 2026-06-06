#pragma once

#include "app/cloud/cloud_types.hpp"
#include "app/core/core_state.hpp"

namespace aov::app::core {

ModuleDrainState BuildCloudStorageDrainState(
    const aov::app::cloud::CloudRuntimeStatus& cloud_status);

} // namespace aov::app::core

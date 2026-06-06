#pragma once

#include "app/cloud/cloud_types.hpp"
#include "app/core/core_commands.hpp"

namespace aov::app::cloud {

using CloudActionMapResult = CloudConfigResult<aov::app::core::CoreCommand>;

CloudActionMapResult MapCloudAction(const CloudActionRequest& request);

} // namespace aov::app::cloud

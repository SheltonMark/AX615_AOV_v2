#pragma once

#include "device_config.hpp"
#include "app/core/aov_types.hpp"

namespace aov::app::config {

aov::app::WorkModePolicy BuildWorkModePolicy(const DeviceConfig& config);

} // namespace aov::app::config

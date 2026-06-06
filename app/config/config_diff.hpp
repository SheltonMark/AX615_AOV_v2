#pragma once

#include "device_config.hpp"

namespace aov::app::config {

PendingConfigChange BuildPendingConfigChange(const DeviceConfig& active,
                                             const DeviceConfig& desired);

} // namespace aov::app::config

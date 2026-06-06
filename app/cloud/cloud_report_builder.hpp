#pragma once

#include <string>
#include <vector>

#include "app/config/device_config.hpp"
#include "app/core/aov_types.hpp"

namespace aov::app::cloud {

struct CloudProperty {
    std::string key;
    std::string value;
};

struct CloudPropertyReport {
    std::vector<CloudProperty> properties;
};

CloudPropertyReport BuildCloudPropertyReport(
    const aov::app::config::DeviceConfig& active_config,
    aov::app::RuntimeWorkState runtime_state,
    const aov::app::BatteryStatus& battery,
    const aov::app::config::StorageRuntimeStatus& storage);

} // namespace aov::app::cloud

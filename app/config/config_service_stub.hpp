#pragma once

#include "config_core_mapper.hpp"
#include "config_diff.hpp"
#include "config_validation.hpp"
#include "device_config.hpp"

#include <string>

namespace aov::app::config {

class ConfigServiceStub final {
public:
    ConfigServiceStub();
    explicit ConfigServiceStub(std::string persist_path);

    ConfigResult LoadFactoryConfig();
    ConfigResult LoadPersistedConfig();

    const DeviceConfig& GetDesiredConfig() const;
    const DeviceConfig& GetActiveConfig() const;
    PendingConfigChange GetPendingConfigChange() const;
    aov::app::WorkModePolicy GetDesiredWorkModePolicy() const;
    aov::app::WorkModePolicy GetActiveWorkModePolicy() const;

    ConfigResult UpdateDesiredConfig(const DeviceConfig& desired);
    ConfigResult MarkApplied();
    ConfigResult Persist();

private:
    void RebuildPendingChange();

private:
    std::string persist_path_;
    DeviceConfig desired_ {};
    DeviceConfig active_ {};
    PendingConfigChange pending_ {};
};

} // namespace aov::app::config

namespace aov::app {

using ConfigServiceStub = config::ConfigServiceStub;

} // namespace aov::app

#include "config_service_stub.hpp"

namespace aov::app::config {

ConfigServiceStub::ConfigServiceStub() {
    LoadFactoryConfig();
}

ConfigResult ConfigServiceStub::LoadFactoryConfig() {
    desired_ = MakeDefaultDeviceConfig();
    active_ = desired_;
    pending_ = {};
    return ConfigResult::Ok();
}

ConfigResult ConfigServiceStub::LoadPersistedConfig() {
    return LoadFactoryConfig();
}

const DeviceConfig& ConfigServiceStub::GetDesiredConfig() const {
    return desired_;
}

const DeviceConfig& ConfigServiceStub::GetActiveConfig() const {
    return active_;
}

PendingConfigChange ConfigServiceStub::GetPendingConfigChange() const {
    return pending_;
}

aov::app::WorkModePolicy ConfigServiceStub::GetDesiredWorkModePolicy() const {
    return BuildWorkModePolicy(desired_);
}

aov::app::WorkModePolicy ConfigServiceStub::GetActiveWorkModePolicy() const {
    return BuildWorkModePolicy(active_);
}

ConfigResult ConfigServiceStub::UpdateDesiredConfig(const DeviceConfig& desired) {
    const ConfigResult validation = ValidateDeviceConfig(desired);
    if (!validation.ok()) {
        return validation;
    }

    desired_ = desired;
    RebuildPendingChange();
    return ConfigResult::Ok();
}

ConfigResult ConfigServiceStub::MarkApplied() {
    active_ = desired_;
    pending_ = {};
    return ConfigResult::Ok();
}

ConfigResult ConfigServiceStub::Persist() {
    return ConfigResult::Ok();
}

void ConfigServiceStub::RebuildPendingChange() {
    pending_ = BuildPendingConfigChange(active_, desired_);
}

} // namespace aov::app::config

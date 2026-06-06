#include "sensor_registry.hpp"

#include <algorithm>

namespace aov::media::ax615 {

SensorRegistry& SensorRegistry::Instance() {
    static SensorRegistry registry;
    return registry;
}

bool SensorRegistry::Register(const SensorProfile& profile) {
    if (profile.sensor_name.empty() || profile.driver_lib_name.empty() ||
        profile.driver_obj_name.empty()) {
        return false;
    }

    profiles_[profile.sensor_name] = profile;
    return true;
}

const SensorProfile* SensorRegistry::Find(const std::string& name) const {
    auto it = profiles_.find(name);
    if (it == profiles_.end()) {
        return nullptr;
    }
    return &it->second;
}

bool SensorRegistry::Has(const std::string& name) const {
    return Find(name) != nullptr;
}

std::vector<std::string> SensorRegistry::ListSensorNames() const {
    std::vector<std::string> names;
    names.reserve(profiles_.size());
    for (const auto& item : profiles_) {
        names.push_back(item.first);
    }
    std::sort(names.begin(), names.end());
    return names;
}

}  // namespace aov::media::ax615

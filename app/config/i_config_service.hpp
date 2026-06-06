#pragma once

#include "app/core/aov_types.hpp"

namespace aov::app {

class IConfigService {
public:
    virtual ~IConfigService() = default;

    virtual AovStatusCode LoadFactoryConfig() = 0;
    virtual AovStatusCode LoadPersistedConfig() = 0;

    virtual WorkModePolicy GetWorkModePolicy() const = 0;
    virtual AovStatusCode UpdateWorkModePolicy(const WorkModePolicy& policy) = 0;

    virtual AovStatusCode UpdateMediaConfig() = 0;
    virtual AovStatusCode UpdateAlarmConfig() = 0;
    virtual AovStatusCode Persist() = 0;
};

} // namespace aov::app

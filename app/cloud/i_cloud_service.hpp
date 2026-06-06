#pragma once

#include <string>

#include "app/alarm/alarm_types.hpp"
#include "app/cloud/cloud_types.hpp"
#include "app/core/aov_types.hpp"

namespace aov::app {

class ICloudService {
public:
    virtual ~ICloudService() = default;

    virtual AovStatusCode Start() = 0;
    virtual AovStatusCode Stop() = 0;

    virtual AovStatusCode ReportDeviceState() = 0;
    virtual AovStatusCode ReportBatteryStatus(const BatteryStatus& status) = 0;
    virtual AovStatusCode ReportAlarm(const AlarmEvent& event) = 0;
    virtual AovStatusCode ReportPreviewState(const std::string& session_id) = 0;

    virtual AovStatusCode OnCloudPreviewRequest(const PreviewRequest& request) = 0;
    virtual AovStatusCode OnCloudConfigUpdate() = 0;
    virtual AovStatusCode OnModemHeartbeatState() = 0;

    virtual cloud::CloudRuntimeStatus GetRuntimeStatus() const = 0;
};

} // namespace aov::app

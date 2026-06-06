#pragma once

#include <string>

#include "app/cloud/cloud_action_mapper.hpp"
#include "app/cloud/cloud_property_mapper.hpp"
#include "app/cloud/cloud_report_builder.hpp"
#include "app/cloud/i_cloud_service.hpp"
#include "app/config/config_service_stub.hpp"

namespace aov::app {

class CloudServiceStub final : public ICloudService {
public:
    CloudServiceStub() = default;
    ~CloudServiceStub() override = default;

    AovStatusCode Start() override;
    AovStatusCode Stop() override;

    AovStatusCode ReportDeviceState() override;
    AovStatusCode ReportBatteryStatus(const BatteryStatus& status) override;
    AovStatusCode ReportAlarm(const AlarmEvent& event) override;
    AovStatusCode ReportPreviewState(const std::string& session_id) override;

    AovStatusCode OnCloudPreviewRequest(const PreviewRequest& request) override;
    AovStatusCode OnCloudConfigUpdate() override;
    AovStatusCode OnModemHeartbeatState() override;
    cloud::CloudRuntimeStatus GetRuntimeStatus() const override;

    cloud::CloudPropertyApplyResult OnCloudPropertyUpdate(
        const cloud::CloudPropertyUpdate& update,
        config::ConfigServiceStub& config_service);
    cloud::CloudActionMapResult OnCloudActionRequest(
        const cloud::CloudActionRequest& request);
    cloud::CloudPropertyReport BuildPropertyReport(
        const config::DeviceConfig& active_config,
        RuntimeWorkState runtime_state,
        const BatteryStatus& battery,
        const config::StorageRuntimeStatus& storage) const;
    core::CoreCommand GetLastCommand() const;
    AovStatusCode StartCloudStorage(const std::string& storage_id);
    AovStatusCode FinishCloudStorage(cloud::CloudStorageResult result);

private:
    bool started_ {false};
    cloud::CloudRuntimeStatus runtime_status_ {};
    BatteryStatus last_battery_ {};
    AlarmEvent last_alarm_ {};
    std::string last_preview_session_id_;
    PreviewRequest last_preview_request_ {};
    core::CoreCommand last_command_ {};
};

} // namespace aov::app

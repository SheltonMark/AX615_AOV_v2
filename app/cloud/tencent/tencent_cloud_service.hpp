#pragma once

#include <string>

#include "app/cloud/i_cloud_service.hpp"
#include "app/cloud/tencent/tencent_cloud_types.hpp"
#include "app/cloud/tencent/tencent_generated_callback_bridge.hpp"
#include "app/cloud/tencent/tencent_sdk_adapter.hpp"

namespace aov::app::cloud::tencent {

class TencentCloudService final : public ICloudService {
public:
    TencentCloudService() = default;
    ~TencentCloudService() override = default;

    AovStatusCode Configure(const TencentCloudInitConfig& config);

    AovStatusCode Start() override;
    AovStatusCode Stop() override;

    AovStatusCode ReportDeviceState() override;
    AovStatusCode ReportBatteryStatus(const BatteryStatus& status) override;
    AovStatusCode ReportAlarm(const AlarmEvent& event) override;
    AovStatusCode ReportPreviewState(const std::string& session_id) override;

    AovStatusCode OnCloudPreviewRequest(const PreviewRequest& request) override;
    AovStatusCode OnCloudConfigUpdate() override;
    AovStatusCode OnModemHeartbeatState() override;
    CloudRuntimeStatus GetRuntimeStatus() const override;

    AovStatusCode StartCloudStorage(const std::string& storage_id);
    AovStatusCode FinishCloudStorage(CloudStorageResult result);
    aov::app::cloud::CloudConfigResult<aov::app::cloud::CloudPropertyUpdate>
    OnTencentPropertySet(const std::string& property_id,
                         const std::string& value,
                         std::uint64_t cloud_version,
                         std::uint64_t recv_ts_ms);
    aov::app::cloud::CloudConfigResult<aov::app::cloud::CloudActionRequest>
    OnTencentAction(const std::string& action_id,
                    const std::string& params,
                    const std::string& request_id,
                    std::uint64_t recv_ts_ms);
    int DrainTencentGeneratedCallbacks();

private:
    static AovStatusCode ToAovStatus(const TencentCloudResult& result);

    TencentCloudInitConfig config_ {};
    TencentSdkAdapter sdk_ {};
    TencentGeneratedCallbackBridge generated_callback_bridge_ {};
    CloudRuntimeStatus runtime_status_ {};
    PreviewRequest last_preview_request_ {};
    bool configured_ {false};
    bool started_ {false};
};

} // namespace aov::app::cloud::tencent

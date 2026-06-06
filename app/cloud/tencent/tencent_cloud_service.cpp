#include "app/cloud/tencent/tencent_cloud_service.hpp"

#include "app/cloud/tencent/tencent_generated_callback_dispatcher.h"

namespace aov::app::cloud::tencent {
namespace {

void DrainPropertyCallback(const char* property_id,
                           const char* value,
                           const std::uint64_t cloud_version,
                           const std::uint64_t recv_ts_ms,
                           void* user) {
    auto* service = static_cast<TencentCloudService*>(user);
    if (service != nullptr) {
        service->OnTencentPropertySet(property_id != nullptr ? property_id : "",
                                      value != nullptr ? value : "",
                                      cloud_version,
                                      recv_ts_ms);
    }
}

void DrainActionCallback(const char* action_id,
                         const char* params,
                         const char* request_id,
                         const std::uint64_t recv_ts_ms,
                         void* user) {
    auto* service = static_cast<TencentCloudService*>(user);
    if (service != nullptr) {
        service->OnTencentAction(action_id != nullptr ? action_id : "",
                                 params != nullptr ? params : "",
                                 request_id != nullptr ? request_id : "",
                                 recv_ts_ms);
    }
}

} // namespace

AovStatusCode TencentCloudService::Configure(const TencentCloudInitConfig& config) {
    if (config.product_id.empty() ||
        config.device_name.empty() ||
        config.device_secret.empty()) {
        return AovStatusCode::InvalidArgument;
    }

    config_ = config;
    configured_ = true;
    return AovStatusCode::Ok;
}

AovStatusCode TencentCloudService::Start() {
    if (!configured_) {
        return AovStatusCode::InvalidState;
    }
    if (started_) {
        return AovStatusCode::Ok;
    }

    AovStatusCode status = ToAovStatus(sdk_.InitSystem(config_));
    if (status != AovStatusCode::Ok) {
        return status;
    }

    status = ToAovStatus(sdk_.InitDataModel());
    if (status != AovStatusCode::Ok) {
        sdk_.ExitSystem();
        return status;
    }

    status = ToAovStatus(sdk_.InitAvTransmission(config_));
    if (status != AovStatusCode::Ok) {
        sdk_.ExitDataModel();
        sdk_.ExitSystem();
        return status;
    }

    started_ = true;
    runtime_status_.connected = true;
    return AovStatusCode::Ok;
}

AovStatusCode TencentCloudService::Stop() {
    if (started_) {
        sdk_.ExitAvTransmission();
        sdk_.ExitDataModel();
        sdk_.ExitSystem();
    }

    started_ = false;
    runtime_status_.connected = false;
    runtime_status_.preview_streaming = false;
    runtime_status_.cloud_storage_running = false;
    runtime_status_.active_cloud_storage_id.clear();
    return AovStatusCode::Ok;
}

AovStatusCode TencentCloudService::ReportDeviceState() {
    if (!started_) {
        return AovStatusCode::InvalidState;
    }

    return ToAovStatus(sdk_.ReportProperty("DeviceState"));
}

AovStatusCode TencentCloudService::ReportBatteryStatus(const BatteryStatus&) {
    if (!started_) {
        return AovStatusCode::InvalidState;
    }

    return ToAovStatus(sdk_.ReportProperty("BatteryPercent"));
}

AovStatusCode TencentCloudService::ReportAlarm(const AlarmEvent& event) {
    if (!started_) {
        return AovStatusCode::InvalidState;
    }
    if (!event.need_cloud_report) {
        return AovStatusCode::Ok;
    }

    return ToAovStatus(sdk_.ReportEvent("Alarm"));
}

AovStatusCode TencentCloudService::ReportPreviewState(const std::string& session_id) {
    if (!started_) {
        return AovStatusCode::InvalidState;
    }

    runtime_status_.preview_streaming = !session_id.empty();
    return ToAovStatus(sdk_.ReportProperty("PreviewState"));
}

AovStatusCode TencentCloudService::OnCloudPreviewRequest(const PreviewRequest& request) {
    last_preview_request_ = request;
    return AovStatusCode::Ok;
}

AovStatusCode TencentCloudService::OnCloudConfigUpdate() {
    return AovStatusCode::Ok;
}

AovStatusCode TencentCloudService::OnModemHeartbeatState() {
    return AovStatusCode::Ok;
}

CloudRuntimeStatus TencentCloudService::GetRuntimeStatus() const {
    return runtime_status_;
}

AovStatusCode TencentCloudService::StartCloudStorage(const std::string& storage_id) {
    if (!started_) {
        return AovStatusCode::InvalidState;
    }
    if (storage_id.empty()) {
        return AovStatusCode::InvalidArgument;
    }

    runtime_status_.cloud_storage_running = true;
    runtime_status_.cloud_storage_result = CloudStorageResult::None;
    runtime_status_.active_cloud_storage_id = storage_id;
    return AovStatusCode::Ok;
}

AovStatusCode TencentCloudService::FinishCloudStorage(const CloudStorageResult result) {
    if (result == CloudStorageResult::None) {
        return AovStatusCode::InvalidArgument;
    }

    runtime_status_.cloud_storage_running = false;
    runtime_status_.cloud_storage_result = result;
    return AovStatusCode::Ok;
}

aov::app::cloud::CloudConfigResult<aov::app::cloud::CloudPropertyUpdate>
TencentCloudService::OnTencentPropertySet(const std::string& property_id,
                                          const std::string& value,
                                          const std::uint64_t cloud_version,
                                          const std::uint64_t recv_ts_ms) {
    return generated_callback_bridge_.BuildPropertyUpdate(property_id,
                                                          value,
                                                          cloud_version,
                                                          recv_ts_ms);
}

aov::app::cloud::CloudConfigResult<aov::app::cloud::CloudActionRequest>
TencentCloudService::OnTencentAction(const std::string& action_id,
                                     const std::string& params,
                                     const std::string& request_id,
                                     const std::uint64_t recv_ts_ms) {
    return generated_callback_bridge_.BuildActionRequest(action_id,
                                                         params,
                                                         request_id,
                                                         recv_ts_ms);
}

int TencentCloudService::DrainTencentGeneratedCallbacks() {
    return aov_tencent_generated_drain(&DrainPropertyCallback,
                                       &DrainActionCallback,
                                       this);
}

AovStatusCode TencentCloudService::ToAovStatus(const TencentCloudResult& result) {
    if (result.ok()) {
        return AovStatusCode::Ok;
    }
    if (result.code == TencentCloudStatusCode::InvalidArgument) {
        return AovStatusCode::InvalidArgument;
    }
    if (result.code == TencentCloudStatusCode::InvalidState) {
        return AovStatusCode::InvalidState;
    }
    return AovStatusCode::InternalError;
}

} // namespace aov::app::cloud::tencent

#include "app/cloud/cloud_service_stub.hpp"

namespace aov::app {

namespace {

cloud::CloudPropertyApplyResult ToCloudApplyError(
    const cloud::CloudConfigError& error) {
    return cloud::CloudPropertyApplyResult::Error(error.code, error.field, error.message);
}

cloud::CloudPropertyApplyResult ToCloudApplyError(
    const config::ConfigResult& error) {
    return cloud::CloudPropertyApplyResult::Error(
        cloud::CloudConfigStatusCode::InvalidArgument,
        error.field,
        error.message);
}

} // namespace

AovStatusCode CloudServiceStub::Start() {
    started_ = true;
    runtime_status_.connected = true;
    return AovStatusCode::Ok;
}

AovStatusCode CloudServiceStub::Stop() {
    started_ = false;
    runtime_status_.connected = false;
    runtime_status_.preview_streaming = false;
    runtime_status_.cloud_storage_running = false;
    return AovStatusCode::Ok;
}

AovStatusCode CloudServiceStub::ReportDeviceState() {
    return started_ ? AovStatusCode::Ok : AovStatusCode::InvalidState;
}

AovStatusCode CloudServiceStub::ReportBatteryStatus(const BatteryStatus& status) {
    if (!started_) {
        return AovStatusCode::InvalidState;
    }

    last_battery_ = status;
    return AovStatusCode::Ok;
}

AovStatusCode CloudServiceStub::ReportAlarm(const AlarmEvent& event) {
    if (!started_) {
        return AovStatusCode::InvalidState;
    }

    last_alarm_ = event;
    return AovStatusCode::Ok;
}

AovStatusCode CloudServiceStub::ReportPreviewState(const std::string& session_id) {
    if (!started_) {
        return AovStatusCode::InvalidState;
    }

    last_preview_session_id_ = session_id;
    runtime_status_.preview_streaming = !session_id.empty();
    return AovStatusCode::Ok;
}

AovStatusCode CloudServiceStub::OnCloudPreviewRequest(const PreviewRequest& request) {
    last_preview_request_ = request;
    return AovStatusCode::Ok;
}

AovStatusCode CloudServiceStub::OnCloudConfigUpdate() {
    return AovStatusCode::Ok;
}

AovStatusCode CloudServiceStub::OnModemHeartbeatState() {
    return AovStatusCode::Ok;
}

cloud::CloudRuntimeStatus CloudServiceStub::GetRuntimeStatus() const {
    return runtime_status_;
}

cloud::CloudPropertyApplyResult CloudServiceStub::OnCloudPropertyUpdate(
    const cloud::CloudPropertyUpdate& update,
    config::ConfigServiceStub& config_service) {
    auto map_result = cloud::ApplyCloudProperty(update);
    if (!map_result.ok()) {
        return ToCloudApplyError(map_result.error);
    }

    config::DeviceConfig desired = config_service.GetDesiredConfig();
    auto apply_result = cloud::ApplyCloudPropertyToConfig(map_result.value, desired);
    if (!apply_result.ok()) {
        return apply_result;
    }

    const config::ConfigResult config_result = config_service.UpdateDesiredConfig(desired);
    if (!config_result.ok()) {
        return ToCloudApplyError(config_result);
    }

    return cloud::CloudPropertyApplyResult::Ok(true);
}

cloud::CloudActionMapResult CloudServiceStub::OnCloudActionRequest(
    const cloud::CloudActionRequest& request) {
    auto result = cloud::MapCloudAction(request);
    if (result.ok()) {
        last_command_ = result.value;
    }
    return result;
}

cloud::CloudPropertyReport CloudServiceStub::BuildPropertyReport(
    const config::DeviceConfig& active_config,
    const RuntimeWorkState runtime_state,
    const BatteryStatus& battery,
    const config::StorageRuntimeStatus& storage) const {
    return cloud::BuildCloudPropertyReport(active_config, runtime_state, battery, storage);
}

core::CoreCommand CloudServiceStub::GetLastCommand() const {
    return last_command_;
}

AovStatusCode CloudServiceStub::StartCloudStorage(const std::string& storage_id) {
    runtime_status_.cloud_storage_running = true;
    runtime_status_.cloud_storage_result = cloud::CloudStorageResult::None;
    runtime_status_.active_cloud_storage_id = storage_id;
    return AovStatusCode::Ok;
}

AovStatusCode CloudServiceStub::FinishCloudStorage(const cloud::CloudStorageResult result) {
    if (result == cloud::CloudStorageResult::None) {
        return AovStatusCode::InvalidArgument;
    }

    runtime_status_.cloud_storage_running = false;
    runtime_status_.cloud_storage_result = result;
    return AovStatusCode::Ok;
}

} // namespace aov::app

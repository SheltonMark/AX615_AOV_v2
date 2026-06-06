#include <iostream>

#include "../../app/cloud/tencent/tencent_cloud_service.hpp"
#include "../../app/cloud/tencent/tencent_generated_callback_dispatcher.h"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

} // namespace

int main() {
    using namespace aov::app;
    using namespace aov::app::cloud;
    using namespace aov::app::cloud::tencent;

    TencentCloudService cloud;
    if (cloud.ReportDeviceState() != AovStatusCode::InvalidState) {
        return Fail("TencentCloudService must reject report before start");
    }

    TencentCloudInitConfig config;
    config.product_id = "pid";
    config.device_name = "dev";
    config.device_secret = "secret";
    if (cloud.Configure(config) != AovStatusCode::Ok) {
        return Fail("TencentCloudService configure must accept valid config");
    }

    if (cloud.Start() != AovStatusCode::Ok ||
        !cloud.GetRuntimeStatus().connected) {
        return Fail("TencentCloudService start must initialize sdk and mark connected");
    }

    BatteryStatus battery;
    battery.percent = 88;
    if (cloud.ReportBatteryStatus(battery) != AovStatusCode::Ok) {
        return Fail("TencentCloudService must report battery after start");
    }

    AlarmEvent alarm;
    alarm.type = AlarmType::Person;
    alarm.need_cloud_report = true;
    if (cloud.ReportAlarm(alarm) != AovStatusCode::Ok) {
        return Fail("TencentCloudService must report alarm event after start");
    }

    if (cloud.ReportPreviewState("preview-1") != AovStatusCode::Ok ||
        !cloud.GetRuntimeStatus().preview_streaming) {
        return Fail("TencentCloudService preview report must update streaming state");
    }
    if (cloud.ReportPreviewState("") != AovStatusCode::Ok ||
        cloud.GetRuntimeStatus().preview_streaming) {
        return Fail("TencentCloudService empty preview session must clear streaming state");
    }

    if (cloud.StartCloudStorage("alarm-1") != AovStatusCode::Ok ||
        !cloud.GetRuntimeStatus().cloud_storage_running ||
        cloud.GetRuntimeStatus().active_cloud_storage_id != "alarm-1") {
        return Fail("TencentCloudService cloud storage start must update runtime status");
    }
    if (cloud.FinishCloudStorage(CloudStorageResult::ExplicitFailure) != AovStatusCode::Ok ||
        cloud.GetRuntimeStatus().cloud_storage_running ||
        cloud.GetRuntimeStatus().cloud_storage_result != CloudStorageResult::ExplicitFailure) {
        return Fail("TencentCloudService cloud storage finish must update drain status");
    }

    if (cloud.Stop() != AovStatusCode::Ok ||
        cloud.GetRuntimeStatus().connected) {
        return Fail("TencentCloudService stop must exit sdk and clear connected state");
    }

    const auto property = cloud.OnTencentPropertySet("VideoFPS", "15", 3, 4000);
    if (!property.ok() ||
        property.value.property_id != "VideoFPS" ||
        property.value.value != "15") {
        return Fail("TencentCloudService must bridge tencent property set");
    }
    if (cloud.OnTencentPropertySet("UnknownTencentProperty", "1", 4, 4100).ok()) {
        return Fail("TencentCloudService must reject unsupported generated property callback");
    }

    const auto action = cloud.OnTencentAction("OneKeyAlarm", "{}", "req-1", 5000);
    if (!action.ok() ||
        action.value.action_id != "OneKeyAlarm" ||
        action.value.request_id != "req-1") {
        return Fail("TencentCloudService must bridge tencent action request");
    }
    if (cloud.OnTencentAction("UnknownTencentAction", "{}", "req-2", 5100).ok()) {
        return Fail("TencentCloudService must reject unsupported generated action callback");
    }

    aov_tencent_generated_clear();
    aov_tencent_generated_push_property("VideoBitrate", "2048", 5, 6000);
    aov_tencent_generated_push_action("FormatStorageMedium", "medium=sdcard", "req-format", 6100);
    if (cloud.DrainTencentGeneratedCallbacks() != 2) {
        return Fail("TencentCloudService must drain generated callback dispatcher events");
    }
    if (cloud.DrainTencentGeneratedCallbacks() != 0) {
        return Fail("TencentCloudService generated callback dispatcher must be empty after drain");
    }

    std::cout << "tencent cloud service contract passed\n";
    return 0;
}

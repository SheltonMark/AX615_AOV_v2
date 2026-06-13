#include <iostream>

#include "../../app/cloud/cloud_action_mapper.hpp"
#include "../../app/cloud/cloud_property_mapper.hpp"
#include "../../app/cloud/cloud_report_builder.hpp"
#include "../../app/cloud/cloud_service_stub.hpp"
#include "../../app/config/config_service_stub.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

bool HasProperty(const aov::app::cloud::CloudPropertyReport& report,
                 const std::string& key,
                 const std::string& value) {
    for (const auto& property : report.properties) {
        if (property.key == key && property.value == value) {
            return true;
        }
    }
    return false;
}

} // namespace

int main() {
    using namespace aov::app;
    using namespace aov::app::cloud;

    ConfigServiceStub config;
    CloudServiceStub cloud;

    auto property_result = cloud.OnCloudPropertyUpdate(
        CloudPropertyUpdate {"AovWorkMode", "UltraLongStandby"},
        config);
    if (!property_result.ok() ||
        config.GetDesiredConfig().work_mode.mode !=
            aov::app::config::ConfiguredWorkMode::UltraLongStandby) {
        return Fail("cloud property update must update config desired config");
    }
    if (config.GetPendingConfigChange().changed_fields.empty()) {
        return Fail("cloud property update must leave pending config change");
    }

    auto action_result = cloud.OnCloudActionRequest(
        CloudActionRequest {"Reboot", "delay=3", "req-reboot"});
    if (!action_result.ok() ||
        cloud.GetLastCommand().type != aov::app::core::CoreCommandType::Reboot ||
        cloud.GetLastCommand().delay_sec != 3 ||
        cloud.GetLastCommand().request_id != "req-reboot") {
        return Fail("cloud action request must map and store last core command");
    }

    BatteryStatus battery;
    battery.percent = 88;
    battery.charging = false;

    aov::app::config::StorageRuntimeStatus storage;
    storage.sd_status = aov::app::config::SdCardStatus::NotDetected;

    const CloudPropertyReport report = cloud.BuildPropertyReport(
        config.GetDesiredConfig(),
        RuntimeWorkState::SleepAppWakeOnly,
        battery,
        storage);
    if (!HasProperty(report, "AovWorkMode", "UltraLongStandby") ||
        !HasProperty(report, "BatteryPercent", "88") ||
        !HasProperty(report, "StorageStatus", "NotDetected")) {
        return Fail("cloud service must build report from config and runtime state");
    }

    property_result = cloud.OnCloudPropertyUpdate(
        CloudPropertyUpdate {"AovRecordInterval", "2"},
        config);
    if (property_result.ok() || property_result.error.field != "AovRecordInterval") {
        return Fail("cloud service must expose property update errors");
    }

    std::cout << "cloud service integration contract passed\n";
    return 0;
}

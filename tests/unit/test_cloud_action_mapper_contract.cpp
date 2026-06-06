#include <iostream>

#include "../../app/cloud/cloud_action_mapper.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

} // namespace

int main() {
    using namespace aov::app::cloud;
    using namespace aov::app::core;

    auto result = MapCloudAction(
        CloudActionRequest {"FormatStorageMedium", "medium=sdcard", "req-format-1"});
    if (!result.ok() ||
        result.value.type != CoreCommandType::FormatStorageMedium ||
        result.value.request_id != "req-format-1" ||
        result.value.storage_medium != "sdcard") {
        return Fail("FormatStorageMedium action must map to core storage format command");
    }

    result = MapCloudAction(
        CloudActionRequest {"OneKeyAlarm", "duration=10", "req-alarm-1"});
    if (!result.ok() ||
        result.value.type != CoreCommandType::OneKeyAlarm ||
        result.value.duration_sec != 10) {
        return Fail("OneKeyAlarm action must map to core one-key alarm command");
    }

    result = MapCloudAction(
        CloudActionRequest {"PreviewImageCapture", "stream=main", "req-snapshot-1"});
    if (!result.ok() ||
        result.value.type != CoreCommandType::PreviewImageCapture ||
        result.value.stream_id != "main") {
        return Fail("PreviewImageCapture action must map to snapshot command");
    }

    result = MapCloudAction(
        CloudActionRequest {"Reboot", "delay=3", "req-reboot-1"});
    if (!result.ok() ||
        result.value.type != CoreCommandType::Reboot ||
        result.value.delay_sec != 3) {
        return Fail("Reboot action must map to reboot command");
    }

    result = MapCloudAction(
        CloudActionRequest {"SetDefaultDevConfigs", "", "req-default-1"});
    if (!result.ok() ||
        result.value.type != CoreCommandType::SetDefaultDevConfigs) {
        return Fail("SetDefaultDevConfigs action must map to reset config command");
    }

    result = MapCloudAction(
        CloudActionRequest {"OneKeyAlarm", "duration=0", "req-bad-alarm"});
    if (result.ok() || result.error.field != "OneKeyAlarm.duration") {
        return Fail("invalid OneKeyAlarm duration must be rejected");
    }

    result = MapCloudAction(
        CloudActionRequest {"UnsupportedAction", "", "req-unknown"});
    if (result.ok() || result.error.field != "UnsupportedAction") {
        return Fail("unknown action must be rejected");
    }

    std::cout << "cloud action mapper contract passed\n";
    return 0;
}

#include <iostream>

#include "../../app/cloud/tencent/tencent_generated_callback_bridge.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

} // namespace

int main() {
    using namespace aov::app::cloud;
    using namespace aov::app::cloud::tencent;

    TencentGeneratedCallbackBridge bridge;

    const auto fps = bridge.BuildPropertyUpdate("VideoFPS", "15", 7, 1000);
    if (!fps.ok() ||
        fps.value.property_id != "VideoFPS" ||
        fps.value.value != "15" ||
        fps.value.op != CloudPropertyOp::Set ||
        fps.value.cloud_version != 7 ||
        fps.value.recv_ts_ms != 1000) {
        return Fail("generated callback bridge must build VideoFPS property update");
    }

    const auto storage = bridge.BuildPropertyUpdate("CloudStorageSwitch", "true", 8, 1100);
    if (!storage.ok() ||
        storage.value.property_id != "CloudStorageSwitch" ||
        storage.value.value != "true") {
        return Fail("generated callback bridge must build CloudStorageSwitch property update");
    }

    const auto unsupported = bridge.BuildPropertyUpdate("UnsupportedProperty", "1", 9, 1200);
    if (unsupported.ok() ||
        unsupported.error.code != CloudConfigStatusCode::UnsupportedProperty) {
        return Fail("generated callback bridge must reject unsupported property");
    }

    const auto reboot = bridge.BuildActionRequest("Reboot", "{}", "req-1", 2000);
    if (!reboot.ok() ||
        reboot.value.action_id != "Reboot" ||
        reboot.value.params != "{}" ||
        reboot.value.request_id != "req-1" ||
        reboot.value.recv_ts_ms != 2000) {
        return Fail("generated callback bridge must build Reboot action request");
    }

    const auto unsupported_action = bridge.BuildActionRequest("UnsupportedAction", "{}", "req-2", 2100);
    if (unsupported_action.ok() ||
        unsupported_action.error.code != CloudConfigStatusCode::UnsupportedProperty) {
        return Fail("generated callback bridge must reject unsupported action");
    }

    std::cout << "tencent generated callback bridge contract passed\n";
    return 0;
}

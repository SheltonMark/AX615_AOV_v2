#include <iostream>

#include "../../app/cloud/tencent/tencent_product_model_bridge.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

} // namespace

int main() {
    using namespace aov::app::cloud;
    using namespace aov::app::cloud::tencent;

    TencentProductModelBridge bridge;

    const auto property = bridge.BuildPropertyUpdate("VideoFPS", "15", 22, 1000);
    if (!property.ok() ||
        property.value.property_id != "VideoFPS" ||
        property.value.value != "15" ||
        property.value.op != CloudPropertyOp::Set ||
        property.value.cloud_version != 22 ||
        property.value.recv_ts_ms != 1000) {
        return Fail("product model bridge must build property update");
    }

    const auto action = bridge.BuildActionRequest("OneKeyAlarm",
                                                  "{\"Operate\":true}",
                                                  "req-1",
                                                  2000);
    if (!action.ok() ||
        action.value.action_id != "OneKeyAlarm" ||
        action.value.params != "{\"Operate\":true}" ||
        action.value.request_id != "req-1" ||
        action.value.recv_ts_ms != 2000) {
        return Fail("product model bridge must build action request");
    }

    if (bridge.BuildPropertyUpdate("", "1", 0, 0).ok()) {
        return Fail("product model bridge must reject empty property id");
    }
    if (bridge.BuildActionRequest("", "{}", "req", 0).ok()) {
        return Fail("product model bridge must reject empty action id");
    }

    std::cout << "tencent product model bridge contract passed\n";
    return 0;
}

#include <iostream>

#include "../../app/cloud/tencent/tencent_sdk_adapter.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

} // namespace

int main() {
    using namespace aov::app::cloud::tencent;

    TencentSdkAdapter sdk;
    TencentCloudInitConfig config;
    config.product_id = "pid";
    config.device_name = "dev";
    config.device_secret = "secret";
    config.max_channel_num = 2;

    if (sdk.InitAvTransmission(config).code != TencentCloudStatusCode::InvalidState) {
        return Fail("av transmission init must require system init first");
    }

    if (!sdk.InitSystem(config).ok() || !sdk.IsSystemReady()) {
        return Fail("system init failed");
    }
    if (!sdk.InitDataModel().ok() || !sdk.IsDataModelReady()) {
        return Fail("data model init failed");
    }
    if (!sdk.InitAvTransmission(config).ok() || !sdk.IsAvTransmissionReady()) {
        return Fail("av transmission init failed");
    }
    if (!sdk.ExitAvTransmission().ok() || sdk.IsAvTransmissionReady()) {
        return Fail("av transmission exit failed");
    }
    if (!sdk.ExitDataModel().ok() || sdk.IsDataModelReady()) {
        return Fail("data model exit failed");
    }
    if (!sdk.ExitSystem().ok() || sdk.IsSystemReady()) {
        return Fail("system exit failed");
    }

    std::cout << "tencent sdk adapter contract passed\n";
    return 0;
}

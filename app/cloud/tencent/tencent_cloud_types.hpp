#pragma once

#include <cstdint>
#include <string>

namespace aov::app::cloud::tencent {

enum class TencentCloudStatusCode {
    Ok,
    InvalidArgument,
    InvalidState,
    SdkUnavailable,
    SdkError,
};

struct TencentCloudResult {
    TencentCloudStatusCode code {TencentCloudStatusCode::Ok};
    int sdk_error {0};
    std::string message;

    bool ok() const {
        return code == TencentCloudStatusCode::Ok;
    }

    static TencentCloudResult Ok() {
        return {};
    }

    static TencentCloudResult Error(TencentCloudStatusCode status,
                                    int sdk_code,
                                    std::string reason) {
        TencentCloudResult result;
        result.code = status;
        result.sdk_error = sdk_code;
        result.message = std::move(reason);
        return result;
    }
};

struct TencentCloudInitConfig {
    std::string sys_cache_path {"/tmp/tencent_cloud"};
    std::string sys_store_path {"/data/tencent_cloud"};
    std::string dev_info_path {"/data/tencent_cloud/device_info.json"};
    std::string product_id;
    std::string device_name;
    std::string device_secret;
    std::uint32_t command_timeout_ms {5000};
    std::uint32_t keep_alive_ms {240000};
    std::uint32_t mqtt_ping_interval_ms {60000};
    std::uint32_t max_channel_num {0};
};

} // namespace aov::app::cloud::tencent

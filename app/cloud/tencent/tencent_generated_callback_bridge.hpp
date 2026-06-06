#pragma once

#include <cstdint>
#include <string>

#include "app/cloud/cloud_types.hpp"
#include "app/cloud/tencent/tencent_product_model_bridge.hpp"

namespace aov::app::cloud::tencent {

class TencentGeneratedCallbackBridge final {
public:
    aov::app::cloud::CloudConfigResult<aov::app::cloud::CloudPropertyUpdate>
    BuildPropertyUpdate(const std::string& property_id,
                        const std::string& value,
                        std::uint64_t cloud_version,
                        std::uint64_t recv_ts_ms) const;

    aov::app::cloud::CloudConfigResult<aov::app::cloud::CloudActionRequest>
    BuildActionRequest(const std::string& action_id,
                       const std::string& params,
                       const std::string& request_id,
                       std::uint64_t recv_ts_ms) const;

private:
    TencentProductModelBridge product_model_bridge_ {};
};

} // namespace aov::app::cloud::tencent

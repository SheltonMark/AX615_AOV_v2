#include "app/cloud/tencent/tencent_product_model_bridge.hpp"

namespace aov::app::cloud::tencent {

aov::app::cloud::CloudConfigResult<aov::app::cloud::CloudPropertyUpdate>
TencentProductModelBridge::BuildPropertyUpdate(const std::string& property_id,
                                               const std::string& value,
                                               const std::uint64_t cloud_version,
                                               const std::uint64_t recv_ts_ms) const {
    if (property_id.empty()) {
        return aov::app::cloud::CloudConfigResult<aov::app::cloud::CloudPropertyUpdate>::Error(
            aov::app::cloud::CloudConfigStatusCode::InvalidArgument,
            "property_id",
            "property id is empty");
    }

    aov::app::cloud::CloudPropertyUpdate update;
    update.property_id = property_id;
    update.value = value;
    update.op = aov::app::cloud::CloudPropertyOp::Set;
    update.cloud_version = cloud_version;
    update.recv_ts_ms = recv_ts_ms;
    return aov::app::cloud::CloudConfigResult<aov::app::cloud::CloudPropertyUpdate>::Ok(update);
}

aov::app::cloud::CloudConfigResult<aov::app::cloud::CloudActionRequest>
TencentProductModelBridge::BuildActionRequest(const std::string& action_id,
                                              const std::string& params,
                                              const std::string& request_id,
                                              const std::uint64_t recv_ts_ms) const {
    if (action_id.empty()) {
        return aov::app::cloud::CloudConfigResult<aov::app::cloud::CloudActionRequest>::Error(
            aov::app::cloud::CloudConfigStatusCode::InvalidArgument,
            "action_id",
            "action id is empty");
    }

    aov::app::cloud::CloudActionRequest request;
    request.action_id = action_id;
    request.params = params;
    request.request_id = request_id;
    request.recv_ts_ms = recv_ts_ms;
    return aov::app::cloud::CloudConfigResult<aov::app::cloud::CloudActionRequest>::Ok(request);
}

} // namespace aov::app::cloud::tencent

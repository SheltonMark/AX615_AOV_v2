#include "app/cloud/cloud_core_command_bridge.hpp"

namespace aov::app::cloud {

CloudCoreCommandBridge::CloudCoreCommandBridge(
    aov::app::core::CoreCommandQueue& queue)
    : queue_(queue) {
}

CloudActionMapResult CloudCoreCommandBridge::SubmitCloudAction(
    const CloudActionRequest& request) {
    auto mapped = MapCloudAction(request);
    if (!mapped.ok()) {
        return mapped;
    }

    const auto push_result = queue_.Push(mapped.value);
    if (!push_result.ok()) {
        return CloudActionMapResult::Error(CloudConfigStatusCode::InvalidArgument,
                                           request.action_id,
                                           push_result.message);
    }

    return mapped;
}

} // namespace aov::app::cloud

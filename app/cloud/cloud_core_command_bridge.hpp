#pragma once

#include "app/cloud/cloud_action_mapper.hpp"
#include "app/core/core_command_queue.hpp"

namespace aov::app::cloud {

class CloudCoreCommandBridge final {
public:
    explicit CloudCoreCommandBridge(aov::app::core::CoreCommandQueue& queue);

    CloudActionMapResult SubmitCloudAction(const CloudActionRequest& request);

private:
    aov::app::core::CoreCommandQueue& queue_;
};

} // namespace aov::app::cloud

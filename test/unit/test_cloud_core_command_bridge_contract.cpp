#include <iostream>

#include "../../app/cloud/cloud_core_command_bridge.hpp"
#include "../../app/core/core_command_queue.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

} // namespace

int main() {
    using namespace aov::app::cloud;
    using namespace aov::app::core;

    CoreCommandQueue queue;
    CloudCoreCommandBridge bridge(queue);

    const CloudActionRequest reboot {"Reboot", "delay=5", "req-reboot", 1000};
    const auto result = bridge.SubmitCloudAction(reboot);
    if (!result.ok() ||
        queue.PendingCount() != 1) {
        return Fail("cloud core command bridge must submit mapped command to core queue");
    }

    CoreCommand command;
    if (!queue.Pop(command).ok() ||
        command.type != CoreCommandType::Reboot ||
        command.delay_sec != 5 ||
        command.request_id != "req-reboot" ||
        command.recv_ts_ms != 1000) {
        return Fail("cloud core command bridge must preserve mapped command fields");
    }

    const auto bad = bridge.SubmitCloudAction(
        CloudActionRequest {"UnsupportedAction", "", "req-bad", 2000});
    if (bad.ok() ||
        queue.PendingCount() != 0) {
        return Fail("cloud core command bridge must reject unsupported action without enqueue");
    }

    std::cout << "cloud core command bridge contract passed\n";
    return 0;
}

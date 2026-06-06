#include <iostream>

#include "../../app/core/core_command_queue.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

} // namespace

int main() {
    using namespace aov::app::core;

    CoreCommandQueue queue;
    if (queue.PendingCount() != 0) {
        return Fail("new core command queue must be empty");
    }

    CoreCommand reboot;
    reboot.type = CoreCommandType::Reboot;
    reboot.request_id = "req-reboot";
    reboot.delay_sec = 3;
    if (!queue.Push(reboot).ok() ||
        queue.PendingCount() != 1 ||
        !queue.HasPending()) {
        return Fail("core command queue must accept first command");
    }

    CoreCommand snapshot;
    snapshot.type = CoreCommandType::PreviewImageCapture;
    snapshot.request_id = "req-snapshot";
    snapshot.stream_id = "main";
    if (!queue.Push(snapshot).ok() ||
        queue.PendingCount() != 2) {
        return Fail("core command queue must accept second command");
    }

    CoreCommand popped;
    if (!queue.Pop(popped).ok() ||
        popped.type != CoreCommandType::Reboot ||
        popped.request_id != "req-reboot" ||
        popped.delay_sec != 3 ||
        queue.PendingCount() != 1) {
        return Fail("core command queue must pop commands in FIFO order");
    }

    if (!queue.Pop(popped).ok() ||
        popped.type != CoreCommandType::PreviewImageCapture ||
        popped.stream_id != "main" ||
        queue.HasPending()) {
        return Fail("core command queue must pop second command and become empty");
    }

    if (queue.Pop(popped).ok()) {
        return Fail("core command queue must reject pop on empty queue");
    }

    std::cout << "core command queue contract passed\n";
    return 0;
}

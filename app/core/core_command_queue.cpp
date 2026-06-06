#include "app/core/core_command_queue.hpp"

#include <utility>

namespace aov::app::core {

CoreCommandQueue::CoreCommandQueue(const std::size_t max_pending)
    : max_pending_(max_pending == 0 ? 1 : max_pending) {
}

CoreCommandQueueResult CoreCommandQueue::Push(const CoreCommand& command) {
    if (pending_.size() >= max_pending_) {
        return CoreCommandQueueResult::Error(CoreCommandQueueStatusCode::Full,
                                             "core command queue is full");
    }

    pending_.push_back(command);
    return CoreCommandQueueResult::Ok();
}

CoreCommandQueueResult CoreCommandQueue::Pop(CoreCommand& out) {
    if (pending_.empty()) {
        return CoreCommandQueueResult::Error(CoreCommandQueueStatusCode::Empty,
                                             "core command queue is empty");
    }

    out = pending_.front();
    pending_.pop_front();
    return CoreCommandQueueResult::Ok();
}

bool CoreCommandQueue::HasPending() const {
    return !pending_.empty();
}

std::size_t CoreCommandQueue::PendingCount() const {
    return pending_.size();
}

void CoreCommandQueue::Clear() {
    pending_.clear();
}

} // namespace aov::app::core

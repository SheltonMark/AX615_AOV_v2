#include "app/core/core_event_queue.hpp"

namespace aov::app::core {

CoreEventQueue::CoreEventQueue(std::size_t max_pending)
    : max_pending_(max_pending) {
}

bool CoreEventQueue::Push(const CoreEvent& event) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (queue_.size() >= max_pending_) {
        return false;  // 队列满，拒绝入队
    }

    queue_.push_back(event);
    cv_.notify_one();
    return true;
}

bool CoreEventQueue::Pop(CoreEvent& out, std::chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (timeout.count() == 0) {
        // 永久等待
        cv_.wait(lock, [this] { return !queue_.empty() || wakeup_flag_; });
    } else {
        // 超时等待
        if (!cv_.wait_for(lock, timeout, [this] { return !queue_.empty() || wakeup_flag_; })) {
            return false;  // 超时
        }
    }

    if (wakeup_flag_) {
        wakeup_flag_ = false;
        return false;  // 被唤醒但无事件
    }

    if (queue_.empty()) {
        return false;
    }

    out = queue_.front();
    queue_.pop_front();
    return true;
}

void CoreEventQueue::WakeUp() {
    std::lock_guard<std::mutex> lock(mutex_);
    wakeup_flag_ = true;
    cv_.notify_all();
}

std::size_t CoreEventQueue::PendingCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
}

bool CoreEventQueue::HasPending() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return !queue_.empty();
}

void CoreEventQueue::Clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.clear();
}

} // namespace aov::app::core

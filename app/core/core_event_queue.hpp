#pragma once

#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <mutex>

#include "app/core/core_events.hpp"

namespace aov::app::core {

class CoreEventQueue {
public:
    explicit CoreEventQueue(std::size_t max_pending = 128);

    // 入队（其他线程调用，线程安全）
    bool Push(const CoreEvent& event);

    // 出队（core 线程调用，阻塞等待）
    // timeout: 最大等待时间，0 表示永久等待
    // 返回: true = 成功获取事件，false = 超时
    bool Pop(CoreEvent& out, std::chrono::milliseconds timeout);

    // 唤醒等待线程（用于优雅退出）
    void WakeUp();

    // 统计
    std::size_t PendingCount() const;
    bool HasPending() const;
    void Clear();

private:
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::deque<CoreEvent> queue_;
    std::size_t max_pending_;
    bool wakeup_flag_ {false};
};

} // namespace aov::app::core

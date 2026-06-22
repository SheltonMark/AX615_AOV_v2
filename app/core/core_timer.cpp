#include "app/core/core_timer.hpp"

#include <algorithm>

namespace aov::app::core {

TimerId CoreTimerManager::AddTimer(TimerType type,
                                   std::chrono::milliseconds interval,
                                   TimerCallback callback) {
    const TimerId id = next_id_++;

    TimerInfo info;
    info.id = id;
    info.type = type;
    info.interval = interval;
    info.next_fire = std::chrono::steady_clock::now() + interval;
    info.callback = std::move(callback);
    info.active = true;

    timers_[id] = std::move(info);
    return id;
}

void CoreTimerManager::CancelTimer(TimerId id) {
    timers_.erase(id);
}

std::chrono::milliseconds CoreTimerManager::GetNextTimeout() const {
    if (timers_.empty()) {
        return std::chrono::milliseconds(-1);  // 无定时器
    }

    const auto now = std::chrono::steady_clock::now();
    std::chrono::milliseconds min_timeout = std::chrono::milliseconds::max();

    for (const auto& [id, timer] : timers_) {
        if (!timer.active) {
            continue;
        }

        const auto timeout = std::chrono::duration_cast<std::chrono::milliseconds>(
            timer.next_fire - now);

        if (timeout.count() <= 0) {
            return std::chrono::milliseconds(0);  // 已到期
        }

        min_timeout = std::min(min_timeout, timeout);
    }

    return min_timeout;
}

void CoreTimerManager::ProcessExpired() {
    const auto now = std::chrono::steady_clock::now();

    // 收集到期的定时器（避免在回调中修改 map）
    std::vector<TimerId> expired_ids;
    for (const auto& [id, timer] : timers_) {
        if (timer.active && timer.next_fire <= now) {
            expired_ids.push_back(id);
        }
    }

    // 触发回调
    for (TimerId id : expired_ids) {
        auto it = timers_.find(id);
        if (it == timers_.end()) {
            continue;  // 已被取消
        }

        TimerInfo& timer = it->second;
        if (!timer.active) {
            continue;
        }

        // 调用回调
        if (timer.callback) {
            timer.callback(id);
        }

        // 处理周期定时器
        if (timer.type == TimerType::Periodic) {
            timer.next_fire = now + timer.interval;
        } else {
            // 单次定时器，删除
            timers_.erase(it);
        }
    }
}

void CoreTimerManager::Clear() {
    timers_.clear();
}

} // namespace aov::app::core

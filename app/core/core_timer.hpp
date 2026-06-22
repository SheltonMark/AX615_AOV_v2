#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <map>

namespace aov::app::core {

using TimerId = std::uint64_t;
using TimerCallback = std::function<void(TimerId)>;

enum class TimerType {
    OneShot,   // 单次触发
    Periodic,  // 周期触发
};

struct TimerInfo {
    TimerId id {0};
    TimerType type {TimerType::OneShot};
    std::chrono::milliseconds interval {0};
    std::chrono::steady_clock::time_point next_fire;
    TimerCallback callback;
    bool active {true};
};

class CoreTimerManager {
public:
    CoreTimerManager() = default;

    // 添加定时器
    // 返回: 定时器ID（用于取消）
    TimerId AddTimer(TimerType type,
                     std::chrono::milliseconds interval,
                     TimerCallback callback);

    // 取消定时器
    void CancelTimer(TimerId id);

    // 获取下次触发时间（用于事件循环超时）
    // 返回: 距离下次触发的毫秒数，-1 表示无定时器
    std::chrono::milliseconds GetNextTimeout() const;

    // 处理到期的定时器
    void ProcessExpired();

    // 清空所有定时器
    void Clear();

private:
    TimerId next_id_ {1};
    std::map<TimerId, TimerInfo> timers_;
};

} // namespace aov::app::core

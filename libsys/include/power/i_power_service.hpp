#pragma once

#include <functional>
#include "common/sys_types.hpp"

namespace aov::sys {

// GPIO唤醒回调类型
using GpioWakeupCallback = std::function<void(const WakeupEvent&)>;

class IPowerService {
public:
    virtual ~IPowerService() = default;

    virtual SysStatusCode EnterSleep() = 0;
    virtual SysStatusCode RequestWakeup() = 0;
    virtual SysStatusCode ConfigureWakeupTimer(int interval_sec) = 0;
    virtual SysStatusCode DisableWakeupTimer() = 0;

    virtual WakeupEvent GetLastWakeupEvent() const = 0;
    virtual BatteryStatus GetBatteryStatus() const = 0;

    virtual SysStatusCode RegisterBatteryCallback() = 0;
    virtual SysStatusCode RegisterWakeupCallback() = 0;

    // 新增：GPIO唤醒监听接口
    virtual SysStatusCode RegisterGpioWakeupCallback(GpioWakeupCallback cb) = 0;
    virtual SysStatusCode StartGpioMonitor() = 0;
    virtual void StopGpioMonitor() = 0;
};

}  // namespace aov::sys

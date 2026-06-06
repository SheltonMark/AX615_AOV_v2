#pragma once

#include "common/sys_types.hpp"

namespace aov::sys {

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
};

}  // namespace aov::sys

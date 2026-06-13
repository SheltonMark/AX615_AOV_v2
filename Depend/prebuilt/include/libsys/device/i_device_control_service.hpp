#pragma once

#include "common/sys_types.hpp"

namespace aov::sys {

class IDeviceControlService {
public:
    virtual ~IDeviceControlService() = default;

    virtual SysStatusCode SetWakeGpioLevel(bool high) = 0;
    virtual SysStatusCode SetFillLight(bool on) = 0;
    virtual SysStatusCode SetSoundAlarm(bool on, int repeat_count) = 0;
    virtual SysStatusCode SetPtzEnabled(bool enabled) = 0;
    virtual SysStatusCode MovePtz(int pan, int tilt) = 0;
    virtual SysStatusCode Reboot(int delay_sec) = 0;
};

}  // namespace aov::sys

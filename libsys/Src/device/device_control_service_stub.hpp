#pragma once

#include "device/i_device_control_service.hpp"

namespace aov::sys {

class DeviceControlServiceStub final : public IDeviceControlService {
public:
    DeviceControlServiceStub() = default;
    ~DeviceControlServiceStub() override = default;

    SysStatusCode SetWakeGpioLevel(bool high) override;
    SysStatusCode SetFillLight(bool on) override;
    SysStatusCode SetSoundAlarm(bool on, int repeat_count) override;
    SysStatusCode SetPtzEnabled(bool enabled) override;
    SysStatusCode MovePtz(int pan, int tilt) override;
    SysStatusCode Reboot(int delay_sec) override;

    bool IsFillLightOn() const;
    bool IsWakeGpioHigh() const;
    bool IsSoundAlarmOn() const;
    int GetSoundAlarmRepeatCount() const;
    bool HasRebootRequested() const;
    int GetRebootDelaySec() const;

private:
    bool wake_gpio_high_ {false};
    bool fill_light_on_ {false};
    bool sound_alarm_on_ {false};
    bool ptz_enabled_ {true};
    bool reboot_requested_ {false};
    int sound_alarm_repeat_count_ {0};
    int reboot_delay_sec_ {0};
    int pan_ {0};
    int tilt_ {0};
};

}  // namespace aov::sys

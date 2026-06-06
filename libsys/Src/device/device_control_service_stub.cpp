#include "device_control_service_stub.hpp"

namespace aov::sys {

SysStatusCode DeviceControlServiceStub::SetWakeGpioLevel(const bool high) {
    wake_gpio_high_ = high;
    return SysStatusCode::Ok;
}

SysStatusCode DeviceControlServiceStub::SetFillLight(const bool on) {
    fill_light_on_ = on;
    return SysStatusCode::Ok;
}

SysStatusCode DeviceControlServiceStub::SetSoundAlarm(const bool on,
                                                      const int repeat_count) {
    sound_alarm_on_ = on;
    sound_alarm_repeat_count_ = repeat_count;
    return SysStatusCode::Ok;
}

SysStatusCode DeviceControlServiceStub::SetPtzEnabled(const bool enabled) {
    ptz_enabled_ = enabled;
    return SysStatusCode::Ok;
}

SysStatusCode DeviceControlServiceStub::MovePtz(const int pan, const int tilt) {
    if (!ptz_enabled_) {
        return SysStatusCode::InvalidState;
    }
    pan_ = pan;
    tilt_ = tilt;
    return SysStatusCode::Ok;
}

SysStatusCode DeviceControlServiceStub::Reboot(const int delay_sec) {
    reboot_requested_ = true;
    reboot_delay_sec_ = delay_sec;
    return SysStatusCode::Ok;
}

bool DeviceControlServiceStub::IsFillLightOn() const {
    return fill_light_on_;
}

bool DeviceControlServiceStub::IsWakeGpioHigh() const {
    return wake_gpio_high_;
}

bool DeviceControlServiceStub::IsSoundAlarmOn() const {
    return sound_alarm_on_;
}

int DeviceControlServiceStub::GetSoundAlarmRepeatCount() const {
    return sound_alarm_repeat_count_;
}

bool DeviceControlServiceStub::HasRebootRequested() const {
    return reboot_requested_;
}

int DeviceControlServiceStub::GetRebootDelaySec() const {
    return reboot_delay_sec_;
}

}  // namespace aov::sys

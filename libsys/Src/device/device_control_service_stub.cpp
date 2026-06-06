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

}  // namespace aov::sys

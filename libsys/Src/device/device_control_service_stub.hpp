#pragma once

#include "device/i_device_control_service.hpp"

namespace aov::sys {

class DeviceControlServiceStub final : public IDeviceControlService {
public:
    DeviceControlServiceStub() = default;
    ~DeviceControlServiceStub() override = default;

    SysStatusCode SetWakeGpioLevel(bool high) override;
    SysStatusCode SetFillLight(bool on) override;
    SysStatusCode SetPtzEnabled(bool enabled) override;
    SysStatusCode MovePtz(int pan, int tilt) override;

private:
    bool wake_gpio_high_ {false};
    bool fill_light_on_ {false};
    bool ptz_enabled_ {true};
    int pan_ {0};
    int tilt_ {0};
};

}  // namespace aov::sys

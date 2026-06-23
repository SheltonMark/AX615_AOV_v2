#pragma once

#include "power/i_power_service.hpp"

namespace aov::sys {

class PowerServiceStub final : public IPowerService {
public:
    PowerServiceStub() = default;
    ~PowerServiceStub() override = default;

    SysStatusCode EnterSleep() override;
    SysStatusCode RequestWakeup() override;
    SysStatusCode ConfigureWakeupTimer(int interval_sec) override;
    SysStatusCode DisableWakeupTimer() override;

    WakeupEvent GetLastWakeupEvent() const override;
    BatteryStatus GetBatteryStatus() const override;

    SysStatusCode RegisterBatteryCallback() override;
    SysStatusCode RegisterWakeupCallback() override;

    // 新增：GPIO唤醒监听接口
    SysStatusCode RegisterGpioWakeupCallback(GpioWakeupCallback cb) override;
    SysStatusCode StartGpioMonitor() override;
    void StopGpioMonitor() override;

    int GetWakeupIntervalSec() const;
    bool HasEnteredSleep() const;
    void SetEnterSleepResult(SysStatusCode status);

private:
    int wakeup_interval_sec_ {0};
    bool entered_sleep_ {false};
    SysStatusCode enter_sleep_result_ {SysStatusCode::Ok};
    WakeupEvent last_wakeup_ {};
    BatteryStatus battery_ {};
    GpioWakeupCallback gpio_wakeup_callback_;  // 新增：GPIO回调
};

}  // namespace aov::sys

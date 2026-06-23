#include "power_service_stub.hpp"

namespace aov::sys {

SysStatusCode PowerServiceStub::EnterSleep() {
    if (enter_sleep_result_ == SysStatusCode::Ok) {
        entered_sleep_ = true;
    }
    return enter_sleep_result_;
}

SysStatusCode PowerServiceStub::RequestWakeup() {
    entered_sleep_ = false;
    last_wakeup_.source = WakeupSource::Manual;
    return SysStatusCode::Ok;
}

SysStatusCode PowerServiceStub::ConfigureWakeupTimer(const int interval_sec) {
    if (interval_sec != 1 && interval_sec != 3 && interval_sec != 5 && interval_sec != 15) {
        return SysStatusCode::InvalidArgument;
    }
    wakeup_interval_sec_ = interval_sec;
    return SysStatusCode::Ok;
}

SysStatusCode PowerServiceStub::DisableWakeupTimer() {
    wakeup_interval_sec_ = 0;
    return SysStatusCode::Ok;
}

WakeupEvent PowerServiceStub::GetLastWakeupEvent() const {
    return last_wakeup_;
}

BatteryStatus PowerServiceStub::GetBatteryStatus() const {
    return battery_;
}

SysStatusCode PowerServiceStub::RegisterBatteryCallback() {
    return SysStatusCode::Ok;
}

SysStatusCode PowerServiceStub::RegisterWakeupCallback() {
    return SysStatusCode::Ok;
}

SysStatusCode PowerServiceStub::RegisterGpioWakeupCallback(GpioWakeupCallback cb) {
    gpio_wakeup_callback_ = std::move(cb);
    return SysStatusCode::Ok;
}

SysStatusCode PowerServiceStub::StartGpioMonitor() {
    // Stub实现：空实现，DSP同事后续补充poll/select逻辑
    // 真实实现应该：
    // 1. 打开 GPIO 设备文件（如 /sys/class/gpio/gpio123/value）
    // 2. 启动监听线程，使用 select/poll 阻塞等待 GPIO 电平变化
    // 3. GPIO 拉高时，调用 gpio_wakeup_callback_ 通知应用层
    return SysStatusCode::Ok;
}

void PowerServiceStub::StopGpioMonitor() {
    // Stub实现：空实现
    // 真实实现应该：
    // 1. 停止监听线程
    // 2. 关闭 GPIO 设备文件
}

int PowerServiceStub::GetWakeupIntervalSec() const {
    return wakeup_interval_sec_;
}

bool PowerServiceStub::HasEnteredSleep() const {
    return entered_sleep_;
}

void PowerServiceStub::SetEnterSleepResult(const SysStatusCode status) {
    enter_sleep_result_ = status;
}

}  // namespace aov::sys

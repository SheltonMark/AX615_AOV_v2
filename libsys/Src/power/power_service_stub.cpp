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

#include "libsys/Include/device/i_device_control_service.hpp"
#include "libsys/Include/modem/i_modem_service.hpp"
#include "libsys/Include/network/i_network_service.hpp"
#include "libsys/Include/power/i_power_service.hpp"
#include "libsys/Include/runtime/i_sys_runtime.hpp"
#include "libsys/Include/storage/i_storage_service.hpp"
#include "libsys/Src/runtime/sys_runtime_factory.hpp"

#include <memory>

int main() {
    using namespace aov::sys;

    BatteryStatus battery;
    battery.percent = 80;
    battery.low_battery_protect = false;

    WakeupEvent wakeup;
    wakeup.source = WakeupSource::Timer;
    wakeup.from_4g_modem = false;

    std::unique_ptr<ISysRuntime> runtime = CreateDefaultSysRuntime();
    if (!runtime) {
        return 1;
    }
    if (!runtime->GetPowerService() || !runtime->GetModemService() ||
        !runtime->GetDeviceControlService() || !runtime->GetNetworkService() ||
        !runtime->GetStorageService()) {
        return 2;
    }
    if (runtime->GetPowerService()->ConfigureWakeupTimer(3) != SysStatusCode::Ok) {
        return 3;
    }
    if (runtime->GetModemService()->Start() != SysStatusCode::Ok) {
        return 4;
    }
    if (!runtime->GetModemService()->GetLinkState()) {
        return 5;
    }
    if (runtime->GetDeviceControlService()->SetWakeGpioLevel(true) != SysStatusCode::Ok) {
        return 6;
    }
    if (runtime->GetStorageService()->EnsureDirectory("/tmp/aov") != SysStatusCode::Ok) {
        return 7;
    }
    return 0;
}

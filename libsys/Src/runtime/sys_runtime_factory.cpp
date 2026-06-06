#include "sys_runtime_factory.hpp"

#include "sys_runtime.hpp"
#include "../device/device_control_service_stub.hpp"
#include "../modem/modem_service_stub.hpp"
#include "../network/network_service_stub.hpp"
#include "../power/power_service_stub.hpp"
#include "../storage/storage_service_stub.hpp"

namespace aov::sys {

std::unique_ptr<ISysRuntime> CreateDefaultSysRuntime() {
    return std::make_unique<SysRuntime>(
        std::make_unique<PowerServiceStub>(),
        std::make_unique<ModemServiceStub>(),
        std::make_unique<DeviceControlServiceStub>(),
        std::make_unique<NetworkServiceStub>(),
        std::make_unique<StorageServiceStub>());
}

}  // namespace aov::sys

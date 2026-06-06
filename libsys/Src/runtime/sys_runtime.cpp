#include "sys_runtime.hpp"

#include "device/i_device_control_service.hpp"
#include "modem/i_modem_service.hpp"
#include "network/i_network_service.hpp"
#include "power/i_power_service.hpp"
#include "storage/i_storage_service.hpp"

namespace aov::sys {

SysRuntime::SysRuntime(std::unique_ptr<IPowerService> power,
                       std::unique_ptr<IModemService> modem,
                       std::unique_ptr<IDeviceControlService> device,
                       std::unique_ptr<INetworkService> network,
                       std::unique_ptr<IStorageService> storage)
    : power_(std::move(power)),
      modem_(std::move(modem)),
      device_(std::move(device)),
      network_(std::move(network)),
      storage_(std::move(storage)) {}

IPowerService* SysRuntime::GetPowerService() {
    return power_.get();
}

IModemService* SysRuntime::GetModemService() {
    return modem_.get();
}

IDeviceControlService* SysRuntime::GetDeviceControlService() {
    return device_.get();
}

INetworkService* SysRuntime::GetNetworkService() {
    return network_.get();
}

IStorageService* SysRuntime::GetStorageService() {
    return storage_.get();
}

}  // namespace aov::sys

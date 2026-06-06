#pragma once

#include "runtime/i_sys_runtime.hpp"

#include <memory>

namespace aov::sys {

class IDeviceControlService;
class IModemService;
class INetworkService;
class IPowerService;
class IStorageService;

class SysRuntime final : public ISysRuntime {
public:
    SysRuntime(std::unique_ptr<IPowerService> power,
               std::unique_ptr<IModemService> modem,
               std::unique_ptr<IDeviceControlService> device,
               std::unique_ptr<INetworkService> network,
               std::unique_ptr<IStorageService> storage);
    ~SysRuntime() override = default;

    IPowerService* GetPowerService() override;
    IModemService* GetModemService() override;
    IDeviceControlService* GetDeviceControlService() override;
    INetworkService* GetNetworkService() override;
    IStorageService* GetStorageService() override;

private:
    std::unique_ptr<IPowerService> power_;
    std::unique_ptr<IModemService> modem_;
    std::unique_ptr<IDeviceControlService> device_;
    std::unique_ptr<INetworkService> network_;
    std::unique_ptr<IStorageService> storage_;
};

}  // namespace aov::sys

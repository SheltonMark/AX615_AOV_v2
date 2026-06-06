#pragma once

namespace aov::sys {

class IDeviceControlService;
class IModemService;
class INetworkService;
class IPowerService;
class IStorageService;

class ISysRuntime {
public:
    virtual ~ISysRuntime() = default;

    virtual IPowerService* GetPowerService() = 0;
    virtual IModemService* GetModemService() = 0;
    virtual IDeviceControlService* GetDeviceControlService() = 0;
    virtual INetworkService* GetNetworkService() = 0;
    virtual IStorageService* GetStorageService() = 0;
};

}  // namespace aov::sys

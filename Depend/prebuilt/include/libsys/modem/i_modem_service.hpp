#pragma once

#include "common/sys_types.hpp"

namespace aov::sys {

class IModemService {
public:
    virtual ~IModemService() = default;

    virtual SysStatusCode Start() = 0;
    virtual SysStatusCode Stop() = 0;

    virtual bool GetLinkState() const = 0;
    virtual ModemHeartbeatState GetHeartbeatState() const = 0;

    virtual SysStatusCode RegisterWakeSignalCallback() = 0;
    virtual SysStatusCode TriggerHostWakeByGpio() = 0;
    virtual SysStatusCode ReportHeartbeatToCloud() = 0;
};

}  // namespace aov::sys

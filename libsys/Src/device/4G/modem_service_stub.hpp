#pragma once

#include "modem/i_modem_service.hpp"

namespace aov::sys {

class ModemServiceStub final : public IModemService {
public:
    ModemServiceStub() = default;
    ~ModemServiceStub() override = default;

    SysStatusCode Start() override;
    SysStatusCode Stop() override;

    bool GetLinkState() const override;
    ModemHeartbeatState GetHeartbeatState() const override;

    SysStatusCode RegisterWakeSignalCallback() override;
    SysStatusCode TriggerHostWakeByGpio() override;
    SysStatusCode ReportHeartbeatToCloud() override;

private:
    bool started_ {false};
    bool link_up_ {false};
    ModemHeartbeatState heartbeat_ {};
};

}  // namespace aov::sys

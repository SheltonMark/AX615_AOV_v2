#include "modem_service_stub.hpp"

namespace aov::sys {

SysStatusCode ModemServiceStub::Start() {
    started_ = true;
    link_up_ = true;
    heartbeat_.online = true;
    return SysStatusCode::Ok;
}

SysStatusCode ModemServiceStub::Stop() {
    started_ = false;
    link_up_ = false;
    heartbeat_.online = false;
    return SysStatusCode::Ok;
}

bool ModemServiceStub::GetLinkState() const {
    return link_up_;
}

ModemHeartbeatState ModemServiceStub::GetHeartbeatState() const {
    return heartbeat_;
}

SysStatusCode ModemServiceStub::RegisterWakeSignalCallback() {
    return SysStatusCode::Ok;
}

SysStatusCode ModemServiceStub::TriggerHostWakeByGpio() {
    return started_ ? SysStatusCode::Ok : SysStatusCode::InvalidState;
}

SysStatusCode ModemServiceStub::ReportHeartbeatToCloud() {
    return started_ ? SysStatusCode::Ok : SysStatusCode::InvalidState;
}

}  // namespace aov::sys

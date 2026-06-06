#pragma once

#include "network/i_network_service.hpp"

namespace aov::sys {

class NetworkServiceStub final : public INetworkService {
public:
    NetworkServiceStub() = default;
    ~NetworkServiceStub() override = default;

    NetworkState GetNetworkState() const override;
    SysStatusCode RegisterNetworkStateCallback() override;

private:
    NetworkState state_ {true, "eth0"};
};

}  // namespace aov::sys

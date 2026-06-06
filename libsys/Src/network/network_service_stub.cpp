#include "network_service_stub.hpp"

namespace aov::sys {

NetworkState NetworkServiceStub::GetNetworkState() const {
    return state_;
}

SysStatusCode NetworkServiceStub::RegisterNetworkStateCallback() {
    return SysStatusCode::Ok;
}

}  // namespace aov::sys

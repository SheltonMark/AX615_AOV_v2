#pragma once

#include "common/sys_types.hpp"

namespace aov::sys {

class INetworkService {
public:
    virtual ~INetworkService() = default;

    virtual NetworkState GetNetworkState() const = 0;
    virtual SysStatusCode RegisterNetworkStateCallback() = 0;
};

}  // namespace aov::sys

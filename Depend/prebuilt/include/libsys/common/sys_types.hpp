#pragma once

#include "common/status_code.hpp"

#include <cstdint>
#include <string>

namespace aov::sys {

using SysStatusCode = aov::common::StatusCode;

enum class WakeupSource {
    Unknown,
    Timer,
    AppRemote,
    Detect,
    Manual,
};

struct BatteryStatus {
    int percent {0};
    bool charging {false};
    bool low_battery_protect {false};
};

struct WakeupEvent {
    WakeupSource source {WakeupSource::Unknown};
    std::uint64_t timestamp_ms {0};
    bool from_4g_modem {false};
};

struct ModemHeartbeatState {
    bool online {false};
    int signal_strength {0};
    std::uint64_t timestamp_ms {0};
};

struct NetworkState {
    bool connected {false};
    std::string ifname;
};

}  // namespace aov::sys

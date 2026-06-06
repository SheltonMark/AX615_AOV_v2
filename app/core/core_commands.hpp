#pragma once

#include <cstdint>
#include <string>

namespace aov::app::core {

enum class CoreCommandType {
    FormatStorageMedium,
    OneKeyAlarm,
    PreviewImageCapture,
    Reboot,
    SetDefaultDevConfigs,
};

struct CoreCommand {
    CoreCommandType type {CoreCommandType::SetDefaultDevConfigs};
    std::string request_id;
    std::uint64_t recv_ts_ms {0};
    std::string storage_medium;
    std::string stream_id;
    int duration_sec {0};
    int delay_sec {0};
};

} // namespace aov::app::core

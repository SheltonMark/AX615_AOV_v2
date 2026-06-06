#pragma once

#include <cstdint>

namespace aov::app {

enum class AlarmType {
    Person,
    Pet,
    Vehicle,
    ManualSOS,
    Unknown,
};

struct AlarmEvent {
    AlarmType type {AlarmType::Unknown};
    std::uint64_t timestamp_ms {0};
    bool need_cloud_report {true};
    bool need_record {true};
    bool need_snapshot {false};
    bool need_white_light_alarm {false};
    bool need_sound_alarm {false};
    int white_light_duration_sec {0};
    int sound_repeat_count {0};
};

} // namespace aov::app

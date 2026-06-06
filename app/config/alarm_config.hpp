#pragma once

#include <array>
#include <vector>

namespace aov::app::config {

enum class DetectTargetType {
    Human,
    Vehicle,
    NonMotorVehicle,
    Motion,
    Occlusion,
};

struct DetectRegion {
    bool full_screen {true};
    std::vector<int> points_or_cells;
};

struct TimeRangeMinutes {
    int start_minute {0};
    int end_minute {24 * 60};
};

struct ArmSchedule {
    std::array<std::vector<TimeRangeMinutes>, 7> daily_ranges {};
};

struct AlarmLinkageConfig {
    bool trigger_record {true};
    bool push_message {true};
    bool white_light_alarm {false};
    bool sound_alarm {false};
};

struct DetectRuleConfig {
    DetectTargetType type {DetectTargetType::Human};
    bool enabled {true};
    DetectRegion region;
    int sensitivity {60};
    int debounce_sec {10};
    ArmSchedule schedule;
    AlarmLinkageConfig linkage;
    bool static_filter {false};
};

struct AlarmConfig {
    DetectRuleConfig human;
    DetectRuleConfig vehicle;
    DetectRuleConfig non_motor_vehicle;
    DetectRuleConfig motion;
    DetectRuleConfig occlusion;
    int white_light_duration_sec {10};
    int sound_repeat_count {1};
    bool target_box_overlay {false};
    bool target_zoom {false};
    bool alarm_snapshot_enabled {true};
};

} // namespace aov::app::config

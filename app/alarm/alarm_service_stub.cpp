#include "app/alarm/alarm_service_stub.hpp"

namespace aov::app {

namespace {

constexpr std::uint64_t kMsPerMinute = 60 * 1000;
constexpr std::uint64_t kMinutesPerDay = 24 * 60;

const config::DetectRuleConfig* SelectRule(const AlarmType type,
                                           const config::AlarmConfig& config) {
    switch (type) {
        case AlarmType::Person:
            return &config.human;
        case AlarmType::Vehicle:
            return &config.vehicle;
        default:
            return nullptr;
    }
}

AlarmEvent SuppressedAlarm(std::uint64_t timestamp_ms) {
    AlarmEvent event;
    event.type = AlarmType::Unknown;
    event.timestamp_ms = timestamp_ms;
    event.need_cloud_report = false;
    event.need_record = false;
    return event;
}

std::size_t AlarmTypeIndex(const AlarmType type) {
    switch (type) {
        case AlarmType::Person:
            return 0;
        case AlarmType::Pet:
            return 1;
        case AlarmType::Vehicle:
            return 2;
        case AlarmType::ManualSOS:
            return 3;
        case AlarmType::Unknown:
            return 4;
    }
    return 4;
}

bool IsInsideArmSchedule(const config::ArmSchedule& schedule,
                         const std::uint64_t timestamp_ms) {
    const std::uint64_t total_minutes = timestamp_ms / kMsPerMinute;
    const std::size_t day_index = static_cast<std::size_t>((total_minutes / kMinutesPerDay) % 7);
    const int minute_of_day = static_cast<int>(total_minutes % kMinutesPerDay);
    const auto& ranges = schedule.daily_ranges[day_index];
    if (ranges.empty()) {
        return true;
    }

    for (const auto& range : ranges) {
        if (range.start_minute <= range.end_minute) {
            if (minute_of_day >= range.start_minute &&
                minute_of_day < range.end_minute) {
                return true;
            }
        } else if (minute_of_day >= range.start_minute ||
                   minute_of_day < range.end_minute) {
            return true;
        }
    }
    return false;
}

bool IsDebounced(const std::uint64_t previous_ms,
                 const std::uint64_t current_ms,
                 const int debounce_sec) {
    if (previous_ms == 0 || debounce_sec <= 0) {
        return false;
    }
    if (current_ms < previous_ms) {
        return false;
    }
    return current_ms - previous_ms < static_cast<std::uint64_t>(debounce_sec) * 1000;
}

} // namespace

AlarmEvent AlarmServiceStub::EvaluateDetectResult(const DetectResultSummary& result) {
    AlarmEvent event {};

    if (result.has_person) {
        event.type = AlarmType::Person;
    } else if (result.has_pet) {
        event.type = AlarmType::Pet;
    } else if (result.has_vehicle) {
        event.type = AlarmType::Vehicle;
    } else {
        event.type = AlarmType::Unknown;
        event.need_cloud_report = false;
        event.need_record = false;
    }

    event.timestamp_ms = result.timestamp_ms;
    return event;
}

AlarmEvent AlarmServiceStub::EvaluateDetectResult(
    const DetectResultSummary& result,
    const config::AlarmConfig& config,
    const RuntimeWorkState state,
    const BatteryStatus& status) {
    if (!IsAlarmAllowed(state, status)) {
        return SuppressedAlarm(result.timestamp_ms);
    }

    AlarmEvent event = EvaluateDetectResult(result);
    const config::DetectRuleConfig* rule = SelectRule(event.type, config);
    if (!rule || !rule->enabled) {
        return SuppressedAlarm(result.timestamp_ms);
    }
    if (!IsInsideArmSchedule(rule->schedule, result.timestamp_ms)) {
        return SuppressedAlarm(result.timestamp_ms);
    }
    const std::size_t alarm_index = AlarmTypeIndex(event.type);
    if (IsDebounced(last_alarm_ts_ms_[alarm_index],
                    result.timestamp_ms,
                    rule->debounce_sec)) {
        return SuppressedAlarm(result.timestamp_ms);
    }

    event.need_record = rule->linkage.trigger_record;
    event.need_cloud_report = rule->linkage.push_message;
    event.need_snapshot = config.alarm_snapshot_enabled && rule->linkage.push_message;
    event.need_white_light_alarm = rule->linkage.white_light_alarm;
    event.need_sound_alarm = rule->linkage.sound_alarm;
    event.white_light_duration_sec =
        event.need_white_light_alarm ? config.white_light_duration_sec : 0;
    event.sound_repeat_count = event.need_sound_alarm ? config.sound_repeat_count : 0;
    last_alarm_ts_ms_[alarm_index] = result.timestamp_ms;
    return event;
}

bool AlarmServiceStub::IsAlarmAllowed(const RuntimeWorkState state, const BatteryStatus& status) const {
    if (status.low_battery_protect) {
        return false;
    }

    return state != RuntimeWorkState::LowBatteryProtection &&
        state != RuntimeWorkState::SleepAppWakeOnly;
}

} // namespace aov::app

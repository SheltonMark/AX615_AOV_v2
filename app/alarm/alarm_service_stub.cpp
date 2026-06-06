#include "app/alarm/alarm_service_stub.hpp"

namespace aov::app {

namespace {

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

    event.need_record = rule->linkage.trigger_record;
    event.need_cloud_report = rule->linkage.push_message;
    event.need_snapshot = config.alarm_snapshot_enabled && rule->linkage.push_message;
    event.need_white_light_alarm = rule->linkage.white_light_alarm;
    event.need_sound_alarm = rule->linkage.sound_alarm;
    event.white_light_duration_sec =
        event.need_white_light_alarm ? config.white_light_duration_sec : 0;
    event.sound_repeat_count = event.need_sound_alarm ? config.sound_repeat_count : 0;
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

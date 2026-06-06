#include <iostream>

#include "../../app/alarm/alarm_service_stub.hpp"
#include "../../app/config/alarm_config.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

} // namespace

int main() {
    using namespace aov::app;

    AlarmServiceStub alarm;

    DetectResultSummary person;
    person.has_person = true;
    person.object_count = 1;
    person.timestamp_ms = 100;
    AlarmEvent event = alarm.EvaluateDetectResult(person);
    if (event.type != AlarmType::Person ||
        !event.need_cloud_report ||
        !event.need_record ||
        event.timestamp_ms != 100) {
        return Fail("person detect must create recordable cloud-report alarm");
    }

    DetectResultSummary unknown;
    unknown.object_count = 0;
    event = alarm.EvaluateDetectResult(unknown);
    if (event.type != AlarmType::Unknown ||
        event.need_cloud_report ||
        event.need_record) {
        return Fail("no target must not create alarm linkage");
    }

    BatteryStatus low;
    low.low_battery_protect = true;
    if (alarm.IsAlarmAllowed(RuntimeWorkState::AwakeNormal, low)) {
        return Fail("low battery protection must block alarm");
    }

    BatteryStatus normal;
    if (!alarm.IsAlarmAllowed(RuntimeWorkState::SleepPeriodicDetect, normal)) {
        return Fail("periodic detect wake state must allow alarm confirmation");
    }
    if (alarm.IsAlarmAllowed(RuntimeWorkState::SleepAppWakeOnly, normal)) {
        return Fail("ultra long standby sleep must block automatic alarm");
    }

    aov::app::config::AlarmConfig config;
    config.human.enabled = true;
    config.human.linkage.trigger_record = false;
    config.human.linkage.push_message = true;
    config.human.linkage.white_light_alarm = true;
    config.human.linkage.sound_alarm = true;
    config.alarm_snapshot_enabled = true;
    config.white_light_duration_sec = 15;
    config.sound_repeat_count = 3;

    event = alarm.EvaluateDetectResult(person, config, RuntimeWorkState::AwakeNormal, normal);
    if (event.type != AlarmType::Person ||
        event.need_record ||
        !event.need_cloud_report ||
        !event.need_snapshot ||
        !event.need_white_light_alarm ||
        !event.need_sound_alarm ||
        event.white_light_duration_sec != 15 ||
        event.sound_repeat_count != 3) {
        return Fail("alarm config linkage must drive record/cloud/snapshot/sound-light decisions");
    }

    config.human.enabled = false;
    event = alarm.EvaluateDetectResult(person, config, RuntimeWorkState::AwakeNormal, normal);
    if (event.type != AlarmType::Unknown ||
        event.need_cloud_report ||
        event.need_record ||
        event.need_snapshot) {
        return Fail("disabled detect rule must suppress alarm linkage");
    }

    config.human.enabled = true;
    config.human.debounce_sec = 10;
    person.timestamp_ms = 60 * 1000;
    event = alarm.EvaluateDetectResult(person, config, RuntimeWorkState::AwakeNormal, normal);
    if (event.type != AlarmType::Person) {
        return Fail("first alarm in debounce window must be allowed");
    }
    person.timestamp_ms = 65 * 1000;
    event = alarm.EvaluateDetectResult(person, config, RuntimeWorkState::AwakeNormal, normal);
    if (event.type != AlarmType::Unknown ||
        event.need_cloud_report ||
        event.need_record) {
        return Fail("same alarm target within debounce window must be suppressed");
    }
    person.timestamp_ms = 71 * 1000;
    event = alarm.EvaluateDetectResult(person, config, RuntimeWorkState::AwakeNormal, normal);
    if (event.type != AlarmType::Person) {
        return Fail("same alarm target after debounce window must be allowed again");
    }

    config.human.schedule.daily_ranges[0].clear();
    config.human.schedule.daily_ranges[0].push_back({60, 120});
    person.timestamp_ms = 30 * 60 * 1000;
    event = alarm.EvaluateDetectResult(person, config, RuntimeWorkState::AwakeNormal, normal);
    if (event.type != AlarmType::Unknown ||
        event.need_cloud_report ||
        event.need_record) {
        return Fail("alarm outside arm schedule must be suppressed");
    }
    person.timestamp_ms = 90 * 60 * 1000;
    event = alarm.EvaluateDetectResult(person, config, RuntimeWorkState::AwakeNormal, normal);
    if (event.type != AlarmType::Person) {
        return Fail("alarm inside arm schedule must be allowed");
    }

    std::cout << "alarm service contract passed\n";
    return 0;
}

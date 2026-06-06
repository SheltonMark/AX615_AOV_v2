#pragma once

#include "app/alarm/i_alarm_service.hpp"

#include <array>

namespace aov::app {

class AlarmServiceStub final : public IAlarmService {
public:
    AlarmServiceStub() = default;
    ~AlarmServiceStub() override = default;

    AlarmEvent EvaluateDetectResult(const DetectResultSummary& result) override;
    AlarmEvent EvaluateDetectResult(const DetectResultSummary& result,
                                    const config::AlarmConfig& config,
                                    RuntimeWorkState state,
                                    const BatteryStatus& status) override;
    bool IsAlarmAllowed(RuntimeWorkState state, const BatteryStatus& status) const override;

private:
    std::array<std::uint64_t, 5> last_alarm_ts_ms_ {};
};

} // namespace aov::app

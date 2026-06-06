#pragma once

#include "app/alarm/alarm_types.hpp"
#include "app/config/alarm_config.hpp"
#include "app/core/aov_types.hpp"

namespace aov::app {

class IAlarmService {
public:
    virtual ~IAlarmService() = default;

    virtual AlarmEvent EvaluateDetectResult(const DetectResultSummary& result) = 0;
    virtual AlarmEvent EvaluateDetectResult(const DetectResultSummary& result,
                                            const config::AlarmConfig& config,
                                            RuntimeWorkState state,
                                            const BatteryStatus& status) = 0;
    virtual bool IsAlarmAllowed(RuntimeWorkState state, const BatteryStatus& status) const = 0;
};

} // namespace aov::app

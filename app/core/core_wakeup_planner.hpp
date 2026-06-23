#pragma once

#include <cstdint>

#include "app/core/aov_types.hpp"

namespace aov::app::core {

struct CoreWakeupPlan {
    AovDecision decision {AovDecision::KeepCurrentState};
    bool restore_media {false};
    bool start_detect {false};
    bool start_local_record {false};
    bool start_low_power_record {false};  // 新增：启动间隔录像（低功耗）
    bool start_event_record {false};      // 新增：启动事件录像（常电）
    bool start_cloud_storage {false};
    bool start_preview {false};
    bool start_cloud_live {false};
    bool report_alarm {false};
    bool apply_pending_config {false};
    int target_frame_count {0};
    int wakeup_interval_sec {0};
};

class CoreWakeupPlanner {
public:
    CoreWakeupPlan BuildPlan(const WakeupEvent& event,
                             const WorkModePolicy& policy,
                             const BatteryStatus& battery) const;

private:
    bool IsLowBatteryProtected(const WorkModePolicy& policy,
                               const BatteryStatus& battery) const;
};

} // namespace aov::app::core

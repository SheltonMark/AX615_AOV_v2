# Batch1 `app/core` + `app/config` 头文件草案

## 1. 目标

本稿不是实现代码，而是把 `Batch1` 压到可以直接落 `.hpp` 的粒度。

建议未来对应文件：

- `app/core/aov_types.hpp`
- `app/core/i_aov_orchestrator.hpp`
- `app/config/i_config_service.hpp`

---

## 2. `aov_types.hpp`

```cpp
#pragma once

#include <cstdint>
#include <string>

namespace aov::app {

enum class ConfiguredWorkMode {
    PowerSaving,
    SmartNoSleep,
    UltraLongStandby,
};

enum class RuntimeWorkState {
    AwakeNormal,
    SleepPeriodicDetect,
    SleepAppWakeOnly,
    LowBatteryProtection,
};

enum class WakeupSource {
    Timer,
    Detect,
    AppRemote,
    Manual,
    Unknown,
};

enum class AovDecision {
    ResumeNormal,
    EnterLowPower,
    KeepCurrentState,
    RejectAction,
};

enum class AovStatusCode {
    Ok,
    InvalidArgument,
    InvalidState,
    Busy,
    Unsupported,
    InternalError,
};

struct WorkModePolicy {
    ConfiguredWorkMode configured_mode {ConfiguredWorkMode::PowerSaving};
    int periodic_wakeup_interval_sec {1};
    int normal_record_fps {15};
    int low_battery_protect_threshold_pct {10};
    int smart_to_save_threshold_pct {70};
    int smart_to_awake_threshold_pct {80};
};

struct BatteryStatus {
    int percent {100};
    bool charging {false};
    bool low_battery_protect {false};
};

struct WakeupEvent {
    WakeupSource source {WakeupSource::Unknown};
    std::uint64_t timestamp_ms {0};
    bool from_4g_modem {false};
};

struct DetectResultSummary {
    bool has_person {false};
    bool has_pet {false};
    bool has_vehicle {false};
    int object_count {0};
    std::uint64_t frame_id {0};
    std::uint64_t timestamp_ms {0};
};

struct PreviewRequest {
    std::string session_id;
    std::string user_id;
    bool force_wakeup {false};
    int expected_timeout_sec {0};
};

struct AovRuntimeContext {
    WorkModePolicy policy;
    BatteryStatus battery;
    RuntimeWorkState runtime_state {RuntimeWorkState::SleepPeriodicDetect};
    bool preview_active {false};
    bool recording_active {false};
    std::uint64_t last_wakeup_ts_ms {0};
};

} // namespace aov::app
```

---

## 3. `i_aov_orchestrator.hpp`

```cpp
#pragma once

#include "app/core/aov_types.hpp"

namespace aov::app {

class IAovOrchestrator {
public:
    virtual ~IAovOrchestrator() = default;

    virtual AovStatusCode Init(const WorkModePolicy& policy) = 0;
    virtual AovStatusCode Start() = 0;
    virtual AovStatusCode Stop() = 0;

    virtual AovStatusCode OnBatteryStatusChanged(const BatteryStatus& status) = 0;
    virtual AovStatusCode OnWakeupEvent(const WakeupEvent& event) = 0;
    virtual AovStatusCode OnDetectResult(const DetectResultSummary& result) = 0;
    virtual AovStatusCode OnPreviewRequest(const PreviewRequest& request) = 0;
    virtual AovStatusCode OnPreviewSessionClosed(const std::string& session_id) = 0;

    virtual RuntimeWorkState GetRuntimeWorkState() const = 0;
    virtual WorkModePolicy GetCurrentPolicy() const = 0;
    virtual AovRuntimeContext GetRuntimeContext() const = 0;
};

} // namespace aov::app
```

---

## 4. `i_config_service.hpp`

```cpp
#pragma once

#include "app/core/aov_types.hpp"

namespace aov::app {

class IConfigService {
public:
    virtual ~IConfigService() = default;

    virtual AovStatusCode LoadFactoryConfig() = 0;
    virtual AovStatusCode LoadPersistedConfig() = 0;

    virtual WorkModePolicy GetWorkModePolicy() const = 0;
    virtual AovStatusCode UpdateWorkModePolicy(const WorkModePolicy& policy) = 0;

    virtual AovStatusCode UpdateMediaConfig() = 0;
    virtual AovStatusCode UpdateAlarmConfig() = 0;
    virtual AovStatusCode Persist() = 0;
};

} // namespace aov::app
```

---

## 5. 关键说明

- `periodic_wakeup_interval_sec` 是单一真值来源
- `normal_record_fps` 第一阶段固定默认 `15`
- `IAovOrchestrator` 是整个设备端 AOV 决策中心
- `IConfigService` 只负责配置，不负责状态机

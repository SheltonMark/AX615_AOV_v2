# Batch4 `app/cloud` + `app/preview` + `app/record` + `app/alarm` 头文件草案

## 1. 建议未来对应文件

- `app/cloud/i_cloud_service.hpp`
- `app/preview/i_preview_service.hpp`
- `app/record/i_record_policy_service.hpp`
- `app/alarm/i_alarm_service.hpp`
- `app/domain/domain_types.hpp`

---

## 2. `domain_types.hpp`

```cpp
#pragma once

#include <cstdint>
#include <string>

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
};

} // namespace aov::app
```

---

## 3. `i_cloud_service.hpp`

```cpp
#pragma once

#include "app/core/aov_types.hpp"
#include "app/domain/domain_types.hpp"

namespace aov::app {

class ICloudService {
public:
    virtual ~ICloudService() = default;

    virtual AovStatusCode Start() = 0;
    virtual AovStatusCode Stop() = 0;

    virtual AovStatusCode ReportDeviceState() = 0;
    virtual AovStatusCode ReportBatteryStatus(const BatteryStatus& status) = 0;
    virtual AovStatusCode ReportAlarm(const AlarmEvent& event) = 0;
    virtual AovStatusCode ReportPreviewState(const std::string& session_id) = 0;

    virtual AovStatusCode OnCloudPreviewRequest(const PreviewRequest& request) = 0;
    virtual AovStatusCode OnCloudConfigUpdate() = 0;
    virtual AovStatusCode OnModemHeartbeatState() = 0;
};

} // namespace aov::app
```

---

## 4. `i_preview_service.hpp`

```cpp
#pragma once

#include "app/core/aov_types.hpp"

namespace aov::app {

class IPreviewService {
public:
    virtual ~IPreviewService() = default;

    virtual AovStatusCode StartPreview(const PreviewRequest& request) = 0;
    virtual AovStatusCode StopPreview(const std::string& session_id) = 0;
    virtual bool HasActivePreview() const = 0;
    virtual int GetActiveSessionCount() const = 0;
};

} // namespace aov::app
```

---

## 5. `i_record_policy_service.hpp`

```cpp
#pragma once

#include "app/domain/domain_types.hpp"
#include "app/core/aov_types.hpp"

namespace aov::app {

class IRecordPolicyService {
public:
    virtual ~IRecordPolicyService() = default;

    virtual AovStatusCode OnAlarmTriggered(const AlarmEvent& event) = 0;
    virtual AovStatusCode StartEventRecord() = 0;
    virtual AovStatusCode StopEventRecord() = 0;
    virtual AovStatusCode ApplyLowPowerRecordPolicy(int interval_sec) = 0;
    virtual AovStatusCode QueryRecordIndex() = 0;
};

} // namespace aov::app
```

---

## 6. `i_alarm_service.hpp`

```cpp
#pragma once

#include "app/core/aov_types.hpp"
#include "app/domain/domain_types.hpp"

namespace aov::app {

class IAlarmService {
public:
    virtual ~IAlarmService() = default;

    virtual AlarmEvent EvaluateDetectResult(const DetectResultSummary& result) = 0;
    virtual bool IsAlarmAllowed(RuntimeWorkState state, const BatteryStatus& status) const = 0;
};

} // namespace aov::app
```

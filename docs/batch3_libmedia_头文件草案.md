# Batch3 `libmedia` 头文件草案

## 1. 建议未来对应文件

- `libmedia/common/media_types.hpp`
- `libmedia/i_media_runtime.hpp`
- `libmedia/detect/i_detect_service.hpp`
- `libmedia/preview/i_media_preview_service.hpp`
- `libmedia/record/i_media_record_service.hpp`
- `libmedia/osd/i_osd_service.hpp`

---

## 2. `media_types.hpp`

```cpp
#pragma once

#include <cstdint>

namespace aov::media {

enum class MediaStatusCode {
    Ok,
    InvalidArgument,
    InvalidState,
    Busy,
    Unsupported,
    InternalError,
};

struct MediaProfile {
    int record_fps {15};
    int interval_sec {1};
    bool low_power_mode {false};
};

} // namespace aov::media
```

---

## 3. `i_media_runtime.hpp`

```cpp
#pragma once

#include "libmedia/common/media_types.hpp"

namespace aov::media {

class IMediaRuntime {
public:
    virtual ~IMediaRuntime() = default;

    virtual MediaStatusCode Init() = 0;
    virtual MediaStatusCode Start() = 0;
    virtual MediaStatusCode Stop() = 0;

    virtual MediaStatusCode ResumeNormalMode() = 0;
    virtual MediaStatusCode EnterLowPowerMode() = 0;
    virtual MediaStatusCode ApplyLowPowerRecordProfile(int interval_sec) = 0;
    virtual MediaStatusCode ApplyNormalRecordProfile(int fps) = 0;
    virtual MediaStatusCode RequestIdr() = 0;
};

} // namespace aov::media
```

---

## 4. `i_detect_service.hpp`

```cpp
#pragma once

#include "app/core/aov_types.hpp"
#include "libmedia/common/media_types.hpp"

namespace aov::media {

class IDetectService {
public:
    virtual ~IDetectService() = default;

    virtual MediaStatusCode StartDetect() = 0;
    virtual MediaStatusCode StopDetect() = 0;
    virtual aov::app::DetectResultSummary RunOnceDetect() = 0;
    virtual MediaStatusCode RegisterDetectCallback() = 0;
};

} // namespace aov::media
```

---

## 5. `i_media_preview_service.hpp`

```cpp
#pragma once

#include "libmedia/common/media_types.hpp"

namespace aov::media {

class IMediaPreviewService {
public:
    virtual ~IMediaPreviewService() = default;

    virtual MediaStatusCode StartPreview() = 0;
    virtual MediaStatusCode StopPreview() = 0;
    virtual bool IsPreviewRunning() const = 0;
};

} // namespace aov::media
```

---

## 6. `i_media_record_service.hpp`

```cpp
#pragma once

#include "libmedia/common/media_types.hpp"

namespace aov::media {

class IMediaRecordService {
public:
    virtual ~IMediaRecordService() = default;

    virtual MediaStatusCode StartRecord() = 0;
    virtual MediaStatusCode StopRecord() = 0;
    virtual MediaStatusCode SetLowPowerRecordInterval(int interval_sec) = 0;
    virtual MediaStatusCode SetNormalRecordFps(int fps) = 0;
    virtual bool IsRecording() const = 0;
};

} // namespace aov::media
```

---

## 7. `i_osd_service.hpp`

```cpp
#pragma once

#include "app/core/aov_types.hpp"
#include "libmedia/common/media_types.hpp"

namespace aov::media {

class IOsdService {
public:
    virtual ~IOsdService() = default;

    virtual MediaStatusCode UpdateDetectOverlay() = 0;
    virtual MediaStatusCode UpdateModeOverlay(aov::app::RuntimeWorkState state) = 0;
    virtual MediaStatusCode UpdateBatteryOverlay(const aov::app::BatteryStatus& status) = 0;
};

} // namespace aov::media
```

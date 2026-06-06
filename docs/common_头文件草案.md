# Common 头文件草案

## 1. 建议未来对应文件

- `common/status_code.hpp`
- `common/error_info.hpp`
- `common/callback.hpp`
- `common/event_header.hpp`

---

## 2. `status_code.hpp`

```cpp
#pragma once

namespace aov::common {

enum class StatusCode {
    Ok,
    InvalidArgument,
    InvalidState,
    NotFound,
    Busy,
    Timeout,
    Unsupported,
    NoResource,
    AlreadyExists,
    InternalError,
};

} // namespace aov::common
```

---

## 3. `error_info.hpp`

```cpp
#pragma once

#include <string>
#include "common/status_code.hpp"

namespace aov::common {

struct ErrorInfo {
    StatusCode code {StatusCode::Ok};
    int vendor_code {0};
    std::string message;
};

} // namespace aov::common
```

---

## 4. `callback.hpp`

```cpp
#pragma once

#include <functional>

namespace aov::common {

using VoidCallback = std::function<void()>;

template <typename T>
using EventCallback = std::function<void(const T&)>;

} // namespace aov::common
```

---

## 5. `event_header.hpp`

```cpp
#pragma once

#include <cstdint>

namespace aov::common {

struct EventHeader {
    std::uint64_t timestamp_ms {0};
    std::uint32_t sequence {0};
};

} // namespace aov::common
```

---

## 6. 使用建议

- `StatusCode` 作为全局统一返回码
- `ErrorInfo` 在需要携带底层 vendor 错误码时使用
- `EventCallback<T>` 统一回调签名
- `EventHeader` 作为所有跨模块事件的公共头

---

## 7. 与 batch 文档的关系

这些公共头文件应作为：

- `Batch1` 的状态机类型依赖
- `Batch2` 的系统事件依赖
- `Batch3` 的检测/媒体事件依赖
- `Batch4` 的业务事件依赖

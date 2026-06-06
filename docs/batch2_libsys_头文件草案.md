# Batch2 `libsys` 头文件草案

## 1. 建议未来对应文件

- `libsys/common/sys_types.hpp`
- `libsys/power/i_power_service.hpp`
- `libsys/modem/i_modem_service.hpp`
- `libsys/device/i_device_control_service.hpp`
- `libsys/storage/i_storage_service.hpp`
- `libsys/network/i_network_service.hpp`

---

## 2. `sys_types.hpp`

```cpp
#pragma once

#include <cstdint>
#include <string>

namespace aov::sys {

enum class SysStatusCode {
    Ok,
    InvalidArgument,
    InvalidState,
    Busy,
    Unsupported,
    InternalError,
};

struct ModemHeartbeatState {
    bool online {false};
    std::uint64_t timestamp_ms {0};
};

struct NetworkState {
    bool connected {false};
    std::string ifname;
};

} // namespace aov::sys
```

---

## 3. `i_power_service.hpp`

```cpp
#pragma once

#include "app/core/aov_types.hpp"
#include "libsys/common/sys_types.hpp"

namespace aov::sys {

class IPowerService {
public:
    virtual ~IPowerService() = default;

    virtual SysStatusCode EnterSleep() = 0;
    virtual SysStatusCode RequestWakeup() = 0;
    virtual SysStatusCode ConfigureWakeupTimer(int interval_sec) = 0;
    virtual SysStatusCode DisableWakeupTimer() = 0;

    virtual aov::app::WakeupEvent GetLastWakeupEvent() const = 0;
    virtual aov::app::BatteryStatus GetBatteryStatus() const = 0;

    virtual SysStatusCode RegisterBatteryCallback() = 0;
    virtual SysStatusCode RegisterWakeupCallback() = 0;
};

} // namespace aov::sys
```

---

## 4. `i_modem_service.hpp`

```cpp
#pragma once

#include "libsys/common/sys_types.hpp"

namespace aov::sys {

class IModemService {
public:
    virtual ~IModemService() = default;

    virtual SysStatusCode Start() = 0;
    virtual SysStatusCode Stop() = 0;

    virtual bool GetLinkState() const = 0;
    virtual ModemHeartbeatState GetHeartbeatState() const = 0;

    virtual SysStatusCode RegisterWakeSignalCallback() = 0;
    virtual SysStatusCode TriggerHostWakeByGpio() = 0;
    virtual SysStatusCode ReportHeartbeatToCloud() = 0;
};

} // namespace aov::sys
```

---

## 5. `i_device_control_service.hpp`

```cpp
#pragma once

#include "libsys/common/sys_types.hpp"

namespace aov::sys {

class IDeviceControlService {
public:
    virtual ~IDeviceControlService() = default;

    virtual SysStatusCode SetWakeGpioLevel(bool high) = 0;
    virtual SysStatusCode SetFillLight(bool on) = 0;
    virtual SysStatusCode SetPtzEnabled(bool enabled) = 0;
    virtual SysStatusCode MovePtz(int pan, int tilt) = 0;
};

} // namespace aov::sys
```

---

## 6. `i_storage_service.hpp`

```cpp
#pragma once

#include "libsys/common/sys_types.hpp"

namespace aov::sys {

class IStorageService {
public:
    virtual ~IStorageService() = default;

    virtual SysStatusCode MountSdCard() = 0;
    virtual std::string GetRecordRootPath() const = 0;
    virtual SysStatusCode EnsureDirectory(const std::string& path) = 0;
    virtual int GetFreeSpaceMb() const = 0;
};

} // namespace aov::sys
```

---

## 7. `i_network_service.hpp`

```cpp
#pragma once

#include "libsys/common/sys_types.hpp"

namespace aov::sys {

class INetworkService {
public:
    virtual ~INetworkService() = default;

    virtual NetworkState GetNetworkState() const = 0;
    virtual SysStatusCode RegisterNetworkStateCallback() = 0;
};

} // namespace aov::sys
```

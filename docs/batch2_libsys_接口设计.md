# Batch2 `libsys` 接口设计

## 1. 范围

本批覆盖：

- `libsys/power`
- `libsys/modem`
- `libsys/device`
- `libsys/storage`
- `libsys/network`

目标是把设备的系统输入面和硬件执行面定死，为 `app/core` 提供稳定下游依赖。

---

## 2. 本批目标

本批必须解决：

- 定时唤醒能力如何抽象
- 电池状态如何抽象
- 4G 模组唤醒链路如何抽象
- GPIO / 云台 / 补光灯如何抽象
- 存储挂载和路径根如何抽象

本批不解决：

- 云协议业务语义
- AX615 媒体链细节

---

## 3. `libsys/power`

## 3.1 职责

- 睡眠与唤醒控制
- 定时器配置
- 电池状态采集
- 唤醒原因上报

## 3.2 接口：`IPowerService`

```text
EnterSleep(...)
RequestWakeup(...)
ConfigureWakeupTimer(int interval_sec)
DisableWakeupTimer()
GetLastWakeupEvent() -> WakeupEvent
GetBatteryStatus() -> BatteryStatus
RegisterBatteryCallback(...)
RegisterWakeupCallback(...)
```

## 3.3 关键约束

- `ConfigureWakeupTimer(interval_sec)` 必须支持 `1 / 3 / 5 / 15`
- 不承载工作模式决策
- 不承载预览/报警语义

---

## 4. `libsys/modem`

## 4.1 职责

- 4G 模组链路状态
- 模组在线状态采集
- 心跳承接
- 接收远程唤醒指令
- 通过 GPIO 拉脚唤醒主控

## 4.2 接口：`IModemService`

```text
Start()
Stop()
GetLinkState() -> ...
GetHeartbeatState() -> ...
RegisterWakeSignalCallback(...)
TriggerHostWakeByGpio()
ReportHeartbeatToCloud(...)
```

## 4.3 关键约束

- 不解释云业务协议
- 只负责模组行为与硬件动作
- 不直接控制媒体链恢复顺序

---

## 5. `libsys/device`

## 5.1 职责

- GPIO
- PWM
- I2C
- 云台
- 补光灯

## 5.2 接口：`IDeviceControlService`

```text
SetWakeGpioLevel(...)
SetFillLight(bool on)
SetPtzEnabled(bool enabled)
MovePtz(...)
```

## 5.3 关键约束

- `LowBatteryProtection` 下必须支持：
  - `SetFillLight(false)`
  - `SetPtzEnabled(false)`

---

## 6. `libsys/storage`

## 6.1 职责

- 挂载 SD 卡
- 统一录像根路径
- 目录与空间能力

## 6.2 接口：`IStorageService`

```text
MountSdCard()
GetRecordRootPath() -> std::string
EnsureDirectory(const std::string& path)
GetFreeSpaceMb() -> int
```

## 6.3 关键约束

- 必须屏蔽 `/tfcard` 和 `/mnt/sdcard` 差异
- 不承载录像业务策略

---

## 7. `libsys/network`

## 7.1 职责

- 提供基础联网状态
- 为 `libsys/modem`、`app/cloud` 提供支撑信息

## 7.2 建议接口

```text
GetNetworkState() -> ...
RegisterNetworkStateCallback(...)
```

---

## 8. 事件输出给上层的形式

建议 `libsys` 对上游统一输出：

- `WakeupEvent`
- `BatteryStatus`
- `NetworkState`
- `ModemHeartbeatState`

这些都是“事实输入”，不携带业务决策。

---

## 9. 与 QSDemo 的映射点

重点参考：

- 睡眠/唤醒定时器
- 唤醒原因链路
- 低功耗相关硬件控制

不直接搬运：

- Demo 中混合业务判断的系统代码

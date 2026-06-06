# code_v2 模块接口清单

## 1. 文档目的

这份文档用于在正式编码前冻结 `code_v2` 的主要接口边界、核心数据模型和工作模式相关接口要求。

接口设计目标：

- 先稳定业务边界，再接入 AX615/QSDemo 真实实现
- 把“工作模式 / 电池 / 4G 唤醒 / 预览 / 检测 / 录像”放在正确层级
- 让后续更换芯片 SDK、4G 模组实现、云协议实现时，业务层改动最小

---

## 2. 工作模式冻结

## 2.1 配置工作模式

建议在 `app/config` 和 `app/core` 中冻结以下配置工作模式枚举：

```text
ConfiguredWorkMode
- PowerSaving          // 省电模式
- SmartNoSleep         // 智能不休眠模式
- UltraLongStandby     // 超长待机模式
```

说明：
- `极低电量保护` 不建议作为用户主动配置模式
- `极低电量保护` 应作为系统根据电池阈值进入的运行时保护态

## 2.2 运行时有效工作状态

建议业务层再维护一组“运行时有效状态”，不要把“配置模式”和“实际运行状态”混成一个字段：

```text
RuntimeWorkState
- AwakeNormal              // 常电正常工作
- SleepPeriodicDetect      // 定时唤醒检测 + 低功耗间隔录像
- SleepAppWakeOnly         // 仅支持 App 唤醒查看
- LowBatteryProtection     // 极低电量保护
```

说明：
- `SmartNoSleep` 在电量充足时，对应 `AwakeNormal`
- `SmartNoSleep` 在电量低于 70% 时，降级到 `SleepPeriodicDetect`
- `UltraLongStandby` 对应 `SleepAppWakeOnly`
- 电量低于极低电量阈值时，不论配置模式是什么，都进入 `LowBatteryProtection`

## 2.3 电池阈值规则

根据当前需求，冻结以下规则：

- `smart_to_save_threshold_pct = 70`
- `smart_to_awake_threshold_pct = 80`
- `low_battery_protect_threshold_pct = 5~30`，默认 `10`
- 退出极低电量保护条件：`battery_pct > low_battery_protect_threshold_pct + 10`

## 2.4 各工作模式能力约束

### 省电模式
- 支持定时唤醒检测
- 唤醒间隔支持：`1 / 3 / 5 / 15` 秒
- **录像间隔 = 定时唤醒间隔 = 低功耗取帧间隔**
- 例如配置 `1` 秒时，无事件状态下按 `1秒1帧` 规格取帧并存储
- 例如配置 `3` 秒时，无事件状态下按 `3秒1帧` 规格取帧并存储
- 检测到事件后进入常电恢复链路，并切到 `15fps` 正常录像规格
- 无事件时回休眠，等待下次唤醒检测

### 智能不休眠模式
- 默认按常电模式工作
- 电量 `< 70%` 自动切换到省电模式行为
- 电量 `> 80%` 恢复常电模式行为
- 后续仍可能进入极低电量保护

### 超长待机模式
- 不自动定时唤醒
- 仅支持 App 远程唤醒预览
- 后续仍可能进入极低电量保护

### 极低电量保护
- 不自动定时唤醒
- 仅支持 App 唤醒查看
- 不支持云台操作
- 不支持一键报警
- 关闭补光灯
- 退出条件：电量高于保护阈值 `+10%`

### 电池管理
- 充电状态需要采集并上报云端
- 电池电量需要采集并上报云端
- 电池百分比建议用 `1~100` 整数表示

---

## 3. 核心数据模型

## 3.1 `BatteryStatus`

```text
struct BatteryStatus {
    int percent;               // 1~100
    bool charging;             // true=充电中
    bool low_battery_protect;  // 是否处于极低电量保护
};
```

## 3.2 `WorkModePolicy`

```text
struct WorkModePolicy {
    ConfiguredWorkMode configured_mode;
    int periodic_wakeup_interval_sec;      // 1/3/5/15；同时作为低功耗录像间隔
    int normal_record_fps;                 // 默认 15
    int low_battery_protect_threshold_pct; // 5~30, default 10
    int smart_to_save_threshold_pct;       // default 70
    int smart_to_awake_threshold_pct;      // default 80
};
```

说明：
- `periodic_wakeup_interval_sec` 是单一真值来源
- 不再额外拆一个“低功耗录像间隔配置项”，避免两个配置不一致

## 3.3 `WakeupEvent`

```text
enum WakeupSource {
    Timer,
    Detect,
    AppRemote,
    Manual,
    Unknown
};

struct WakeupEvent {
    WakeupSource source;
    uint64_t timestamp_ms;
    bool from_4g_modem;
};
```

## 3.4 `DetectResultSummary`

```text
struct DetectResultSummary {
    bool has_person;
    bool has_pet;
    bool has_vehicle;
    int object_count;
    uint64_t frame_id;
    uint64_t timestamp_ms;
};
```

## 3.5 `PreviewRequest`

```text
struct PreviewRequest {
    std::string session_id;
    std::string user_id;
    bool force_wakeup;
    int expected_timeout_sec;
};
```

## 3.6 `AlarmEvent`

```text
enum AlarmType {
    Person,
    Pet,
    Vehicle,
    ManualSOS,
    Unknown
};

struct AlarmEvent {
    AlarmType type;
    uint64_t timestamp_ms;
    bool need_cloud_report;
    bool need_record;
};
```

---

## 4. app 层接口

## 4.1 `app/core` -> `IAovOrchestrator`

**职责**
- 管理 AOV 主状态机
- 接收电池、唤醒、检测、预览请求等事件
- 统一决定是否进入常电 / 休眠 / 预览 / 报警录像

**建议接口**

```text
Init(const WorkModePolicy& policy)
Start()
Stop()
OnBatteryStatusChanged(const BatteryStatus& status)
OnWakeupEvent(const WakeupEvent& event)
OnDetectResult(const DetectResultSummary& result)
OnPreviewRequest(const PreviewRequest& request)
OnPreviewSessionClosed(const std::string& session_id)
GetRuntimeWorkState() -> RuntimeWorkState
GetCurrentPolicy() -> WorkModePolicy
```

**关键约束**
- `IAovOrchestrator` 必须知道当前 `periodic_wakeup_interval_sec`
- `IAovOrchestrator` 必须负责把：
  - 低功耗间隔录像规格
  - 常电 `15fps` 录像规格
  分发给媒体层

## 4.2 `app/config` -> `IConfigService`

**职责**
- 加载默认配置
- 覆盖本地持久化配置
- 校验并下发配置

**建议接口**

```text
LoadFactoryConfig()
LoadPersistedConfig()
GetWorkModePolicy() -> WorkModePolicy
UpdateWorkModePolicy(const WorkModePolicy& policy)
UpdateMediaConfig(...)
UpdateAlarmConfig(...)
Persist()
```

**必须覆盖的工作模式字段**
- `configured_mode`
- `periodic_wakeup_interval_sec`
- `normal_record_fps`
- `low_battery_protect_threshold_pct`
- `smart_to_save_threshold_pct`
- `smart_to_awake_threshold_pct`

## 4.3 `app/cloud` -> `ICloudService`

**职责**
- 处理设备业务协议
- 同步设备状态
- 接收云端命令
- 和 `libsys/modem` 协作完成远程唤醒链路

**建议接口**

```text
Start()
Stop()
ReportDeviceState(...)
ReportBatteryStatus(const BatteryStatus& status)
ReportAlarm(const AlarmEvent& event)
ReportPreviewState(const std::string& session_id, ...)
OnCloudPreviewRequest(const PreviewRequest& request)
OnCloudConfigUpdate(...)
OnModemHeartbeatState(...)
```

**注意**
- 这里不负责 GPIO 拉脚
- 这里不负责 4G 驱动细节

## 4.4 `app/preview` -> `IPreviewService`

**职责**
- 管理预览会话
- 调用 `libmedia/preview` 启停预览

**建议接口**

```text
StartPreview(const PreviewRequest& request)
StopPreview(const std::string& session_id)
HasActivePreview() -> bool
GetActiveSessionCount() -> int
```

## 4.5 `app/record` -> `IRecordPolicyService`

**职责**
- 决定何时开始/停止录像
- 管理事件录像片段

**建议接口**

```text
OnAlarmTriggered(const AlarmEvent& event)
StartEventRecord(...)
StopEventRecord(...)
ApplyLowPowerRecordPolicy(int interval_sec)
QueryRecordIndex(...)
```

## 4.6 `app/alarm` -> `IAlarmService`

**职责**
- 根据检测结果和工作模式决定是否触发报警

**建议接口**

```text
EvaluateDetectResult(const DetectResultSummary& result) -> AlarmEvent
IsAlarmAllowed(RuntimeWorkState state, const BatteryStatus& status) -> bool
```

**关键约束**
- `LowBatteryProtection` 下，不允许一键报警
- `UltraLongStandby` 下，不自动走检测唤醒报警链路

---

## 5. libsys 层接口

## 5.1 `libsys/power` -> `IPowerService`

**职责**
- 管理系统睡眠与唤醒
- 管理定时唤醒配置
- 管理电池/工作模式相关状态输入

**建议接口**

```text
EnterSleep(...)
RequestWakeup(...)
ConfigureWakeupTimer(int interval_sec)
DisableWakeupTimer()
GetLastWakeupEvent() -> WakeupEvent
GetBatteryStatus() -> BatteryStatus
```

## 5.2 `libsys/modem` -> `IModemService`

**职责**
- 4G 模组状态管理
- 接收模组唤醒事件
- GPIO 拉脚唤醒主控
- 心跳状态采集

**建议接口**

```text
Start()
Stop()
GetLinkState() -> ...
GetHeartbeatState() -> ...
RegisterWakeSignalCallback(...)
TriggerHostWakeByGpio()
ReportHeartbeatToCloud(...)
```

**关键约束**
- 不解释云端业务协议语义
- 不直接控制预览/录像媒体链

## 5.3 `libsys/device` -> `IDeviceControlService`

**职责**
- GPIO / PWM / I2C / 云台 / 补光灯

**建议接口**

```text
SetWakeGpioLevel(...)
SetFillLight(bool on)
SetPtzEnabled(bool enabled)
MovePtz(...)
```

**关键约束**
- `LowBatteryProtection` 下必须支持：
  - `SetFillLight(false)`
  - `SetPtzEnabled(false)`

## 5.4 `libsys/storage` -> `IStorageService`

**职责**
- 管理存储挂载与目录能力

**建议接口**

```text
MountSdCard()
GetRecordRootPath() -> std::string
EnsureDirectory(const std::string& path)
GetFreeSpaceMb() -> int
```

---

## 6. libmedia 层接口

## 6.1 `libmedia` -> `IMediaRuntime`

**职责**
- 媒体链总入口
- 统一恢复/降级媒体状态

**建议接口**

```text
Init(...)
Start()
Stop()
ResumeNormalMode()
EnterLowPowerMode()
ApplyLowPowerRecordProfile(int interval_sec)
ApplyNormalRecordProfile(int fps)
RequestIdr()
```

## 6.2 `libmedia/detect` -> `IDetectService`

**职责**
- 提供轻量检测能力
- 输出检测摘要

**建议接口**

```text
StartDetect()
StopDetect()
RunOnceDetect() -> DetectResultSummary
RegisterDetectCallback(...)
```

## 6.3 `libmedia/preview` -> `IMediaPreviewService`

**职责**
- 启停预览媒体链
- 输出 RTSP/预览流

**建议接口**

```text
StartPreview(...)
StopPreview(...)
IsPreviewRunning() -> bool
```

## 6.4 `libmedia/record` -> `IMediaRecordService`

**职责**
- 控制录像媒体输出
- 区分低功耗间隔录像与事件常电录像
- 提供文件写入前的编码数据出口

**建议接口**

```text
StartRecord(...)
StopRecord(...)
SetLowPowerRecordInterval(int interval_sec)
SetNormalRecordFps(int fps)
IsRecording() -> bool
```

## 6.5 `libmedia/osd` -> `IOsdService`

**职责**
- 叠加检测框、状态文字、电量/模式信息

**建议接口**

```text
UpdateDetectOverlay(...)
UpdateModeOverlay(RuntimeWorkState state)
UpdateBatteryOverlay(const BatteryStatus& status)
```

---

## 7. 接口设计上的硬约束

- `app/*` 不直接包含 AX SDK 头文件
- `libsys/modem` 不直接解释云协议 JSON/私有协议
- `libmedia` 不直接决定业务状态切换
- `app/core` 必须同时感知：
  - 配置工作模式
  - 运行时有效状态
  - 电池状态
  - 唤醒来源
- `LowBatteryProtection` 相关限制，必须通过接口层可表达，不能只靠注释约定
- 低功耗录像规格必须由单一配置项 `periodic_wakeup_interval_sec` 驱动，不能出现第二个并行配置项

---

## 8. 第一阶段建议先落哪些接口

建议首批只冻结以下 8 个接口：

- `IAovOrchestrator`
- `IConfigService`
- `ICloudService`
- `IPowerService`
- `IModemService`
- `IDeviceControlService`
- `IMediaRuntime`
- `IDetectService`

这 8 个接口足够支撑：
- 工作模式切换
- 定时唤醒检测
- App 远程唤醒
- 4G 心跳状态反馈
- 极低电量保护约束
- 省电模式下“间隔录制 / 事件 15fps 录制”双规格切换

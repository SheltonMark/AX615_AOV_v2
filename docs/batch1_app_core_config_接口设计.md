# Batch1 `app/core` + `app/config` 接口设计

## 1. 范围

本批只覆盖：

- `app/core`
- `app/config`

目标是先把设备端最核心的“状态机 + 配置主线”定死，后续 `libsys`、`libmedia`、`app/cloud` 都以这份文档为上游约束。

---

## 2. 本批目标

本批必须解决：

- 工作模式如何表达
- 运行时有效状态如何表达
- 电池状态如何影响运行时状态
- 定时唤醒 / App 唤醒 / 手动唤醒如何进入状态机
- 省电模式下“录像间隔 = 定时唤醒间隔 = 低功耗取帧间隔”的规则在哪里生效
- 常电 `15fps` 与低功耗间隔录像如何切换决策

本批不解决：

- AX615 SDK 具体调用
- 4G 模组底层实现
- 预览协议和录像文件格式细节

---

## 3. 核心枚举

## 3.1 `ConfiguredWorkMode`

```text
PowerSaving
SmartNoSleep
UltraLongStandby
```

## 3.2 `RuntimeWorkState`

```text
AwakeNormal
SleepPeriodicDetect
SleepAppWakeOnly
LowBatteryProtection
```

## 3.3 `WakeupSource`

```text
Timer
Detect
AppRemote
Manual
Unknown
```

## 3.4 `AovDecision`

```text
ResumeNormal
EnterLowPower
KeepCurrentState
RejectAction
```

用于 `app/core` 内部在处理事件后输出动作决策。

---

## 4. 核心数据结构

## 4.1 `WorkModePolicy`

```text
struct WorkModePolicy {
    ConfiguredWorkMode configured_mode;
    int periodic_wakeup_interval_sec;      // 1/3/5/15
    int normal_record_fps;                 // default 15
    int low_battery_protect_threshold_pct; // 5~30, default 10
    int smart_to_save_threshold_pct;       // default 70
    int smart_to_awake_threshold_pct;      // default 80
};
```

约束：
- `periodic_wakeup_interval_sec` 为低功耗间隔录像和定时唤醒间隔的单一真值来源
- `normal_record_fps` 第一阶段默认固定为 `15`

## 4.2 `BatteryStatus`

```text
struct BatteryStatus {
    int percent;
    bool charging;
    bool low_battery_protect;
};
```

## 4.3 `WakeupEvent`

```text
struct WakeupEvent {
    WakeupSource source;
    uint64_t timestamp_ms;
    bool from_4g_modem;
};
```

## 4.4 `DetectResultSummary`

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

## 4.5 `AovRuntimeContext`

```text
struct AovRuntimeContext {
    WorkModePolicy policy;
    BatteryStatus battery;
    RuntimeWorkState runtime_state;
    bool preview_active;
    bool recording_active;
    uint64_t last_wakeup_ts_ms;
};
```

---

## 5. `app/config` 设计

## 5.1 职责

- 装载工厂默认配置
- 装载持久化配置
- 校验工作模式相关配置合法性
- 对外提供统一配置读取接口
- 对配置变更做持久化

## 5.2 对外接口：`IConfigService`

```text
LoadFactoryConfig()
LoadPersistedConfig()
GetWorkModePolicy() -> WorkModePolicy
UpdateWorkModePolicy(const WorkModePolicy& policy)
UpdateMediaConfig(...)
UpdateAlarmConfig(...)
Persist()
```

## 5.3 配置校验规则

必须校验：

- `configured_mode` 必须属于枚举集
- `periodic_wakeup_interval_sec` 只能取 `1 / 3 / 5 / 15`
- `normal_record_fps` 第一阶段固定允许 `15`
- `low_battery_protect_threshold_pct` 必须在 `5~30`
- `smart_to_save_threshold_pct < smart_to_awake_threshold_pct`

## 5.4 典型输出

`app/config` 至少要向 `app/core` 提供：

- 工作模式配置
- 唤醒间隔配置
- 常电录像帧率配置
- 电量阈值配置

---

## 6. `app/core` 设计

## 6.1 职责

- 管理 AOV 主状态机
- 消费配置输入、电池输入、唤醒输入、检测输入、预览输入
- 决定是否进入：
  - 常电工作
  - 省电检测休眠
  - 仅 App 唤醒
  - 极低电量保护
- 向下游分发媒体策略和系统策略

## 6.2 对外接口：`IAovOrchestrator`

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
GetRuntimeContext() -> AovRuntimeContext
```

## 6.3 内部必需能力

虽然本批不定义实现类，但 `app/core` 必须具备以下内部子职责：

- `ModeResolver`
  - 根据配置模式 + 电池状态，决议运行时状态
- `WakeupDecisionResolver`
  - 根据唤醒事件决定后续动作
- `DetectDecisionResolver`
  - 根据检测结果决定是否恢复常电/触发报警/保持低功耗
- `MediaPolicyResolver`
  - 输出：低功耗录像规格 / 常电 15fps 规格

## 6.4 状态转换规则

### 规则 A：省电模式
- 初始进入 `SleepPeriodicDetect`
- `Timer` 唤醒后执行一次检测
- 无事件：保持 `SleepPeriodicDetect`
- 有事件：切到 `AwakeNormal`

### 规则 B：智能不休眠模式
- 电量 `>= 70%`：`AwakeNormal`
- 电量 `< 70%`：降级到 `SleepPeriodicDetect`
- 电量 `> 80%`：恢复到 `AwakeNormal`

### 规则 C：超长待机模式
- 固定为 `SleepAppWakeOnly`
- 不响应定时唤醒检测链路
- 仅响应 `AppRemote`

### 规则 D：极低电量保护
- 一旦进入，覆盖其它运行时状态
- 禁止自动检测唤醒
- 禁止一键报警
- 仅允许 App 唤醒查看

## 6.5 `app/core` 对下游的决策输出

`app/core` 不直接实现媒体/系统动作，但必须输出明确决策：

```text
ApplyLowPowerProfile(interval_sec)
ApplyNormalProfile(fps=15)
EnableWakeupTimer(interval_sec)
DisableWakeupTimer()
DisablePtzAndFillLight()
AllowPreviewOnly()
RejectAlarmAction()
```

---

## 7. 输入事件表

## 7.1 电池事件

输入：`BatteryStatus`

输出影响：
- 更新 `runtime_state`
- 决定是否进入/退出 `LowBatteryProtection`
- 决定 `SmartNoSleep` 是否降级/恢复

## 7.2 唤醒事件

输入：`WakeupEvent`

输出影响：
- `Timer` -> 触发检测流程
- `AppRemote` -> 触发预览恢复流程
- `Manual` -> 视策略进入常电或保持当前状态

## 7.3 检测事件

输入：`DetectResultSummary`

输出影响：
- 有目标 -> 切到 `AwakeNormal`
- 无目标 -> 保持 `SleepPeriodicDetect`

## 7.4 预览事件

输入：`PreviewRequest` / `PreviewSessionClosed`

输出影响：
- 开启预览时强制保持常电
- 关闭预览后回到工作模式规则控制

---

## 8. 与后续批次的依赖边界

`Batch1` 对后续批次的要求：

- `Batch2` 必须提供：
  - 唤醒输入
  - 电池输入
  - 定时器控制
  - 低电量硬件裁剪能力
- `Batch3` 必须提供：
  - 低功耗录像规格应用
  - 常电 `15fps` 规格应用
  - 单次检测执行
- `Batch4` 必须遵守：
  - 所有业务子域动作都受 `app/core` 状态机约束

---

## 9. QSDemo 参考点

本批重点参考 QSDemo 中：

- AOV 状态切换入口
- 定时唤醒后检测再决策的主流程
- 恢复常电后的媒体切换时机

本批不直接搬运：

- QSDemo 的具体 AX615 调用
- QSDemo 的单文件控制结构

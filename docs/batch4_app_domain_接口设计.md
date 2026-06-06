# Batch4 `app/cloud` + `app/preview` + `app/record` + `app/alarm` 接口设计

## 1. 范围

本批覆盖：

- `app/cloud`
- `app/preview`
- `app/record`
- `app/alarm`

目标是冻结业务子域的输入输出，保证它们都在 `app/core` 状态机约束下运行。

---

## 2. 本批目标

本批必须解决：

- 云端请求如何进入设备业务层
- 预览会话如何管理
- 报警和录像如何联动
- 电池、状态、心跳如何回传云端

本批不解决：

- 媒体底层实现
- 4G 模组底层实现

---

## 3. `app/cloud`

## 3.1 职责

- 接入云业务协议
- 路由预览请求、配置请求
- 上报设备状态、电池状态、报警状态、预览状态
- 与 `libsys/modem` 协同完成远程唤醒业务闭环

## 3.2 接口：`ICloudService`

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

## 3.3 关键约束

- 不直接拉 GPIO
- 不直接控制媒体链
- 不直接解释硬件细节

---

## 4. `app/preview`

## 4.1 职责

- 预览会话管理
- 调用 `libmedia/preview` 启停媒体预览
- 在预览结束后，把控制权交还 `app/core`

## 4.2 接口：`IPreviewService`

```text
StartPreview(const PreviewRequest& request)
StopPreview(const std::string& session_id)
HasActivePreview() -> bool
GetActiveSessionCount() -> int
```

## 4.3 关键约束

- 不自己决定系统休眠策略
- 预览是否保持常电由 `app/core` 决策

---

## 5. `app/record`

## 5.1 职责

- 事件录像策略
- 录像片段管理
- 录像索引与元数据管理

## 5.2 接口：`IRecordPolicyService`

```text
OnAlarmTriggered(const AlarmEvent& event)
StartEventRecord(...)
StopEventRecord(...)
ApplyLowPowerRecordPolicy(int interval_sec)
QueryRecordIndex(...)
```

## 5.3 关键约束

- 录像规格来自 `app/core` / `libmedia`
- 自己不决定底层帧率和编码器行为

---

## 6. `app/alarm`

## 6.1 职责

- 检测结果 -> 报警事件
- 报警事件 -> 上报/录像联动

## 6.2 接口：`IAlarmService`

```text
EvaluateDetectResult(const DetectResultSummary& result) -> AlarmEvent
IsAlarmAllowed(RuntimeWorkState state, const BatteryStatus& status) -> bool
```

## 6.3 关键约束

- `LowBatteryProtection` 下禁止一键报警
- `UltraLongStandby` 下不自动走检测唤醒报警链路
- 报警类型第一阶段至少支持：人 / 宠物 / 车辆

---

## 7. 子域之间的依赖关系

允许：

- `app/cloud` -> `app/core`
- `app/preview` -> `app/core` + `libmedia/preview`
- `app/record` -> `app/core` + `libmedia/record` + `libsys/storage`
- `app/alarm` -> `app/core` + `app/cloud` + `app/record` + `libmedia/detect`

禁止：

- `app/preview` 反向驱动 `app/core` 状态机决策
- `app/cloud` 直接操作 `libmedia` AX 细节
- `app/alarm` 直接操作底层硬件

---

## 8. 与 QSDemo 的映射点

本批主要不是直接从 QSDemo 搬接口，而是参考它已经打通的链路：

- 检测后恢复常电并开始编码/录像
- 预览链路恢复
- 状态变化时的日志与留证节奏

真正的业务子域接口，不建议照搬 QSDemo 命名。

# Batch3 `libmedia` 接口设计

## 1. 范围

本批覆盖：

- `libmedia`
- `libmedia/detect`
- `libmedia/preview`
- `libmedia/record`
- `libmedia/osd`
- `libmedia/adapter/ax615`

目标是冻结媒体链对上提供的能力接口，不讨论具体 AX615 实现细节。

---

## 2. 本批目标

本批必须解决：

- 低功耗间隔录像与常电 `15fps` 录像如何抽象
- 单次检测如何抽象
- 预览媒体链如何抽象
- OSD 如何抽象
- AX615 SDK 接口如何隔离

本批不解决：

- 告警业务策略
- 云协议交互

---

## 3. `libmedia` 总入口

## 3.1 职责

- 初始化媒体链
- 管理低功耗 / 常电媒体规格切换
- 暴露检测、预览、录像、OSD 子能力

## 3.2 接口：`IMediaRuntime`

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

## 3.3 关键约束

- `ApplyLowPowerRecordProfile(interval_sec)` 必须能表达 `1秒1帧`、`3秒1帧` 等规格
- `ApplyNormalRecordProfile(fps)` 第一阶段固定先支持 `15`
- 不承载业务状态机

---

## 4. `libmedia/detect`

## 4.1 职责

- 提供轻量检测能力
- 输出检测摘要

## 4.2 接口：`IDetectService`

```text
StartDetect()
StopDetect()
RunOnceDetect() -> DetectResultSummary
RegisterDetectCallback(...)
```

## 4.3 关键约束

- 对上只输出检测摘要，不输出业务决策
- 第一阶段优先满足：人形 / 宠物 / 车辆摘要结果

---

## 5. `libmedia/preview`

## 5.1 职责

- 启动/停止预览链路
- 输出 RTSP 或预览媒体流

## 5.2 接口：`IMediaPreviewService`

```text
StartPreview(...)
StopPreview(...)
IsPreviewRunning() -> bool
```

## 5.3 关键约束

- 不负责预览会话管理
- 只负责媒体链动作

---

## 6. `libmedia/record`

## 6.1 职责

- 录像媒体输出
- 低功耗间隔录像规格应用
- 常电 `15fps` 录像规格应用

## 6.2 接口：`IMediaRecordService`

```text
StartRecord(...)
StopRecord(...)
SetLowPowerRecordInterval(int interval_sec)
SetNormalRecordFps(int fps)
IsRecording() -> bool
```

## 6.3 关键约束

- `SetLowPowerRecordInterval(int interval_sec)` 必须与 `periodic_wakeup_interval_sec` 对齐
- 第一阶段常电帧率先固定 `15`
- 不负责录像文件命名和索引业务

---

## 7. `libmedia/osd`

## 7.1 职责

- 叠加框
- 模式状态叠加
- 电量状态叠加

## 7.2 接口：`IOsdService`

```text
UpdateDetectOverlay(...)
UpdateModeOverlay(RuntimeWorkState state)
UpdateBatteryOverlay(const BatteryStatus& status)
```

---

## 8. `libmedia/adapter/ax615`

## 8.1 职责

- 封装 AX615 媒体 SDK
- 对上游隐藏 AX 细节
- 吸收 QSDemo 中 `src/com`、`src/osd` 的有效实现经验

## 8.2 原则

- 只做 SDK 映射
- 不承载业务状态机
- 不承载产品默认配置

---

## 9. 与 QSDemo 的映射点

重点参考：

- 媒体恢复顺序
- 低功耗切换顺序
- 检测取帧链路
- OSD 叠加链路
- RTSP/编码输出链路

不直接搬运：

- QSDemo 中和业务策略混在一起的媒体控制流程

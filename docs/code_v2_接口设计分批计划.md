# code_v2 接口设计分批计划

## 1. 文档目的

这份文档用于把 `code_v2` 的接口设计按模块分批推进，避免一次铺太大，导致边界失焦。

原则：

- 先设计状态机和输入事件
- 再设计系统侧输入能力
- 再设计媒体侧能力
- 最后设计业务子域接口

---

## 2. 分批原则

每一批接口设计都要满足：

- 能回答“这个模块输入什么、输出什么”
- 能回答“这个模块依赖谁、不依赖谁”
- 能回答“工作模式规则在哪一层生效”
- 不进入实现细节，不先写 AX615 SDK 映射

---

## 3. 分批顺序

## Batch 1：状态机与配置主线

**目标模块**
- `app/core`
- `app/config`

**本批要解决的问题**
- 工作模式如何表达
- 电池状态如何影响运行时状态
- 定时唤醒 / App 唤醒如何进入状态机
- 省电模式下“录制间隔 = 定时唤醒间隔”的规则在哪里生效

**本批输出物**
- `IAovOrchestrator`
- `IConfigService`
- 核心枚举与数据结构

## Batch 2：系统输入与4G唤醒主线

**目标模块**
- `libsys/power`
- `libsys/modem`
- `libsys/device`
- `libsys/storage`

**本批要解决的问题**
- 定时唤醒事件怎么上送
- App 远程唤醒怎样经由 4G 模组和 GPIO 拉脚完成
- 极低电量保护如何裁剪硬件能力
- 存储根路径如何统一抽象

**本批输出物**
- `IPowerService`
- `IModemService`
- `IDeviceControlService`
- `IStorageService`

## Batch 3：媒体主线

**目标模块**
- `libmedia`
- `libmedia/detect`
- `libmedia/preview`
- `libmedia/record`

**本批要解决的问题**
- 低功耗取帧和常电 `15fps` 如何切换
- 预览媒体链如何恢复
- 检测摘要如何返回业务层
- 录像媒体链如何表达“低功耗间隔录像”和“事件常电录像”

**本批输出物**
- `IMediaRuntime`
- `IDetectService`
- `IMediaPreviewService`
- `IMediaRecordService`

## Batch 4：业务子域主线

**目标模块**
- `app/cloud`
- `app/preview`
- `app/record`
- `app/alarm`

**本批要解决的问题**
- 云端协议事件怎样落到业务域
- 预览会话如何管理
- 报警和录像如何联动
- 设备状态、心跳、电池如何反馈给云端/App

**本批输出物**
- `ICloudService`
- `IPreviewService`
- `IRecordPolicyService`
- `IAlarmService`

---

## 4. 推荐推进顺序

建议下一步严格按以下顺序设计接口：

1. `app/core` + `app/config`
2. `libsys/power` + `libsys/modem`
3. `libmedia` + `libmedia/detect` + `libmedia/record`
4. `app/cloud` + `app/preview` + `app/alarm` + `app/record`

原因：

- 先把“谁决策”定清楚
- 再把“谁提供外部输入”定清楚
- 再把“谁执行媒体能力”定清楚
- 最后再把“谁对接云和 App”定清楚

---

## 5. 下一步建议

下一步不要继续散写所有模块。

建议直接开始 **Batch 1**：

- 先把 `app/core` 和 `app/config` 的接口、状态、事件、配置项完全定死
- 特别确认：
  - 工作模式枚举
  - 运行时状态枚举
  - 电池阈值规则
  - 低功耗录像间隔规则
  - App 唤醒和定时唤醒的事件输入模型

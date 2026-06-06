# app 模块开发计划与状态

本文用于跟踪 `app/` 目录剩余开发事项。后续每完成一个任务，先更新代码，再同步更新本文对应状态。

## 状态约定

| 状态 | 含义 |
| --- | --- |
| DONE | 已完成，并有编译或测试验证 |
| DOING | 当前正在做 |
| TODO | 已确认要做，尚未开始 |
| BLOCKED | 需要外部信息、板端验证或参考代码确认 |
| LATER | 后置优化，不阻塞当前主链 |

## 当前总览

| 模块 | 当前状态 | 说明 |
| --- | --- | --- |
| `app/Build` | DONE | 已支持交叉编译 `Kylin`，支持 `build/clean/distclean`，并接入 Depend 预编译库链接开关 |
| `app/config` | DONE | 配置结构体、默认值、校验、Desired/Active/Pending 框架已完成 |
| `app/cloud` | DOING | 属性/动作/状态映射已完成，TencentCloudService 骨架已完成，cloud packet sink、直播推流适配层、腾讯物模型桥接已接入，generated SDK 回调和云存推流待接 |
| `app/core` | DOING | 状态机、配置应用、drain 判断已有合同测试；已新增 app runtime，后续继续接真实事件 |
| `app/packet` | DONE | 当前已有媒体帧承载、订阅分发、storage/cloud sink 基础链路；后续接真实 Packet 库为优化项 |
| `app/storage` | DOING | Packet 到 storage stub 链路已完成；真实 PS/PES + DHFS 写盘待做 |
| `app/alarm` | DOING | 告警配置联动字段已接入；防抖、布防时间、声光实际执行待补 |
| `app/entry` | DONE | 已从 stub bootstrap 切换为 app runtime bootstrap |
| `app/domain` | LATER | 当前已基本空置，后续确认无引用后删除目录 |

## 构建规则

当前阶段 app 构建统一由根目录 `CMakeLists.txt` 收口：

```text
code_v2/CMakeLists.txt
  -> ax615_aov_device_stubs
  -> Kylin
```

`app/` 子目录下不再保留独立 `CMakeLists.txt`。判断某个 `.cpp` 是否编进 `Kylin`，以根目录 `CMakeLists.txt` 的 `ax615_aov_device_stubs` 源文件列表为准。

## 阶段计划

### 阶段 A：Kylin 主程序骨架

目标：让 `Kylin` 不只是能编译，而是由统一 app runtime 管理各模块生命周期。

| ID | 任务 | 状态 | 验收方式 |
| --- | --- | --- | --- |
| A1 | `app/Build/build_ax615.sh` 支持交叉编译 `Kylin` | DONE | `sh build_ax615.sh` 生成 `app/Build/Kylin` |
| A2 | `app/Build/build_ax615.sh` 支持 `clean/distclean` | DONE | `sh build_ax615.sh clean`、`distclean` 可执行 |
| A3 | CMake 接入 Depend 头文件和预编译库链接开关 | DONE | 交叉编译配置通过，`Kylin` 可链接 |
| A3.1 | 删除 app 子目录下未生效的 `CMakeLists.txt` | DONE | `app/packet`、`app/storage` 不再保留独立 CMake，根 CMake 统一收口 |
| A4 | 新增 `app/core/aov_app_runtime.hpp/.cpp` | DONE | `test_aov_app_runtime_contract` 通过 |
| A5 | `app/entry/main.cpp` 改为启动 `AovAppRuntime` | DONE | host `Kylin_runtime_check.exe` 编译通过，交叉编译由 `app/Build/build_ax615.sh` 验证 |

### 阶段 B：app/core 主控闭环

目标：让 core 汇总 config/cloud/storage/alarm/packet 状态，决定唤醒、业务执行、drain 和回 AOV 休眠。

| ID | 任务 | 状态 | 验收方式 |
| --- | --- | --- | --- |
| B1 | 工作模式状态机、低电保护、定时/App 唤醒策略 | DONE | `test_work_mode_state_machine_contract` 等通过 |
| B2 | 配置应用协调：停止链路、应用配置、重启链路 | DONE | `test_core_apply_coordinator_contract` 通过 |
| B3 | storage/cloud drain 合并判断 | DONE | `test_core_sleep_drain_contract`、`test_core_drain_merge_contract` 通过 |
| B4 | core runtime facade 串接 config/cloud/storage 状态 | DONE | `test_core_runtime_facade_contract` 通过 |
| B5 | app runtime 调用 core，并驱动模块生命周期 | DONE | `test_aov_app_runtime_contract` 通过，`main.cpp` 已接入 runtime |
| B6 | 云端动作命令进入 core 执行队列 | TODO | 云端动作映射后能触发 core command |

### 阶段 C：app/cloud 腾讯云接入

目标：把腾讯云 SDK 接入到当前 `ICloudService`，并让直播、云存、属性、动作都服从 core 低功耗策略。

| ID | 任务 | 状态 | 验收方式 |
| --- | --- | --- | --- |
| C1 | cloud 属性映射到 `DeviceConfig` | DONE | `test_cloud_property_mapper_contract` 通过 |
| C2 | cloud 动作映射到 `CoreCommand` | DONE | `test_cloud_action_mapper_contract` 通过 |
| C3 | cloud 状态上报 builder | DONE | `test_cloud_report_builder_contract` 通过 |
| C4 | Tencent SDK adapter host-safe 骨架 | DONE | host 编译不需要真实 SDK 链接 |
| C5 | TencentCloudService 实现 `ICloudService` | DONE | `test_tencent_cloud_service_contract` 通过 |
| C6 | 腾讯 SDK `iv_sys/iv_dm/iv_avt` 真实初始化参数接入 | DOING | `iv_dm/iv_avt` 生命周期已进 `TencentSdkAdapter`；`iv_sys_init` 产品三元组和 OS 回调参数仍待板端配置 |
| C7 | 腾讯物模型回调接入 property/action mapper | DONE | 已新增 `TencentGeneratedCallbackBridge` 和 C ABI dispatcher；已接入高优先级 `iv_usrcb_*` generated 回调：视频参数、云存开关、本地录像模式、智能检测、声光报警、重启、格式化、抓图、一键报警、恢复默认配置；回调只轻量入队，业务处理由 `TencentCloudService::DrainTencentGeneratedCallbacks()` drain 后继续分发 |
| C8 | 直播推流：Packet -> TencentStreamAdapter -> `iv_av` | DONE | `test_tencent_live_stream_contract`、`test_cloud_packet_sink_contract` 通过；真实 SDK 调用由 `AOV_ENABLE_TENCENT_SDK` 控制 |
| C9 | 云存推流：Packet -> TencentStreamAdapter -> `iv_cs` | TODO | 云存成功或明确失败后通知 core drain 完成 |

### 阶段 D：app/storage 本地存储

目标：让 SD 卡录像走 `Packet -> PS/PES -> DHFS`，并能把写盘状态反馈给 core。

| ID | 任务 | 状态 | 验收方式 |
| --- | --- | --- | --- |
| D1 | Packet 到 storage sink 基础链路 | DONE | `test_packet_storage_pipeline` 通过 |
| D2 | StorageService drain 状态接入 core | DONE | `test_storage_service_drain_integration`、`test_storage_core_drain_mapper_contract` 通过 |
| D3 | `DhfsWriterStub` 统计写入帧数/字节数 | DONE | storage pipeline 测试覆盖 |
| D4 | 真实 PS/PES 封装接口设计和最小实现 | TODO | 输入 PacketFrame 输出可写入 PS/PES 数据 |
| D5 | 真实 DHFS writer 接入 `libDHFS.a` | TODO | 板端可创建/写入/close/flush/fsync 文件 |
| D6 | AOV 间隔帧时间戳策略 | BLOCKED | 需要确认 AOV 间隔录像文件时间轴和播放器兼容策略 |
| D7 | SD 卡格式化/容量/状态与 libsys/SD 联动 | TODO | 云端 `FormatStorageMedium` 能触发并上报结果 |

### 阶段 E：app/packet 媒体帧分发

目标：保持裸码流分发，不加私有帧头；storage 和 cloud 可以并行消费同一帧。

| ID | 任务 | 状态 | 验收方式 |
| --- | --- | --- | --- |
| E1 | 定义 `PacketFrame`、订阅者、统计信息 | DONE | packet/storage 测试覆盖 |
| E2 | 分发到 storage sink | DONE | storage sink 收到帧并更新 drain 状态 |
| E3 | 分发到 cloud sink | DONE | `test_cloud_packet_sink_contract` 通过，runtime 已同时绑定 storage/cloud sink |
| E4 | 接入 libmedia 裸码流回调 | TODO | libmedia 输出编码帧后进入 PacketService |
| E5 | 评估是否替换为旧项目 `libPacket.a` 真实实现 | LATER | 当前 wrapper 满足架构验证，真实库迁移后再替换 |

### 阶段 F：app/alarm 告警策略

目标：把 libmedia 检测结果转换为产品告警事件，并驱动 storage/cloud/声光联动。

| ID | 任务 | 状态 | 验收方式 |
| --- | --- | --- | --- |
| F1 | AlarmEvent 和联动字段定义 | DONE | `test_alarm_service_contract` 通过 |
| F2 | 检测结果 + AlarmConfig 生成告警事件 | DONE | 合同测试覆盖基本联动 |
| F3 | 告警防抖和布防时间 | TODO | 同一目标短时间内不重复触发 |
| F4 | 声光报警执行接口接入 libsys/device | TODO | 一键报警/检测报警可触发补光灯和声音 |
| F5 | 告警触发本地录像和云存 | TODO | 告警后 storage/cloud 均进入 running，并在完成后通知 core |

### 阶段 G：app/config 配置闭环

目标：支持云端配置下发、持久化、待生效、生效确认和状态上报。

| ID | 任务 | 状态 | 验收方式 |
| --- | --- | --- | --- |
| G1 | `DeviceConfig` 聚合结构体 | DONE | `test_config_phase1_contract` 通过 |
| G2 | 配置校验规则 | DONE | 配置合同测试通过 |
| G3 | Desired/Active/Pending 分离 | DONE | `test_config_service_core_policy_contract` 通过 |
| G4 | 云端属性更新 DesiredConfig | DONE | `test_cloud_service_integration_contract` 通过 |
| G5 | 配置持久化真实文件实现 | TODO | 重启后可恢复上次 Desired/Active 配置 |
| G6 | 配置生效结果回报云端 | TODO | app/core MarkApplied 后 cloud report 能体现已生效 |

## 下一步建议

优先继续 `C6/C7`：补腾讯 `iv_sys/iv_dm/iv_avt_init` 初始化参数和物模型回调桥接，让直播开始/停止能由腾讯 SDK 回调驱动。

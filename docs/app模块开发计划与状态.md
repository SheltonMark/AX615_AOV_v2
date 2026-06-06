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
| `app/config` | DONE | 配置结构体、默认值、校验、Desired/Active/Pending、关键配置持久化已完成 |
| `app/cloud` | DONE | 属性/动作/状态映射、TencentCloudService、cloud packet sink、直播/云存推流适配、腾讯物模型桥接、generated SDK 回调已接入；真实板端腾讯 SDK 联调属于板端验证项 |
| `app/core` | DONE | 状态机、配置应用、drain 判断、云端动作执行器、检测告警联动、app runtime 主链路已有合同测试 |
| `app/packet` | DONE | 当前已有媒体帧承载、订阅分发、storage/cloud sink 基础链路；后续接真实 Packet 库为优化项 |
| `app/storage` | BLOCKED | Packet 到 storage stub 链路已完成；真实 PS/PES + DHFS 写盘已单独输出设计文档，后续由 storage 同事实现 |
| `app/alarm` | DONE | 告警配置联动字段、防抖、布防时间、检测告警触发本地录像/云存闭环已接入；声光执行复用 core command/libsys device |
| `app/entry` | DONE | 已从 stub bootstrap 切换为 app runtime bootstrap |
| `app/domain` | LATER | 当前已基本空置，后续确认无引用后删除目录 |

## 构建规则

当前阶段 app 构建统一由根目录 `CMakeLists.txt` 收口：

```text
code_v2/CMakeLists.txt
  -> ax615_aov_app_core
  -> Depend/prebuilt/lib/libmedia.a
  -> Depend/prebuilt/lib/libsys.a
  -> Depend/**/*.a
  -> Kylin
```

`app/` 子目录下不再保留独立 `CMakeLists.txt`。判断某个 app `.cpp` 是否编进 `Kylin`，以根目录 `CMakeLists.txt` 的 `ax615_aov_app_core` 源文件列表为准。`Kylin` 不再编译 `libsys/Src` 源码，libsys 只通过 `libsys/Include` 暴露接口，并通过 `Depend/prebuilt/lib/libsys.a` 链接实现。

注意：`libsys/Include/device/i_device_control_service.hpp` 已新增 `SetSoundAlarm()` 和 `Reboot()` 设备控制接口，板端编译 `Kylin` 前需要重新编译并更新 `Depend/prebuilt/lib/libsys.a`。

## 阶段计划

### 阶段 A：Kylin 主程序骨架

目标：让 `Kylin` 不只是能编译，而是由统一 app runtime 管理各模块生命周期。

| ID | 任务 | 状态 | 验收方式 |
| --- | --- | --- | --- |
| A1 | `app/Build/build_ax615.sh` 支持交叉编译 `Kylin` | DONE | `sh build_ax615.sh` 生成 `app/Build/Kylin` |
| A2 | `app/Build/build_ax615.sh` 支持 `clean/distclean` | DONE | `sh build_ax615.sh clean`、`distclean` 可执行 |
| A3 | CMake 接入 Depend 头文件和预编译库 | DONE | `Kylin` 链接 `Depend/prebuilt/lib/libmedia.a`、`libsys.a` 和 Depend 下其他 `.a`，不编译 `libsys/Src` |
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
| B6 | 云端动作命令进入 core 执行队列 | DONE | 已新增 `CoreCommandQueue` 和 `CloudCoreCommandBridge`；云端 action 经 mapper 后进入 runtime 内部 core command FIFO，由 `AovAppRuntime::DrainCoreCommands()` 统一消费 |
| B7 | core command 执行器 | DONE | `CoreCommandExecutor` 已支持抓图、恢复默认配置、一键报警触发本地录像/补光灯/声音报警、SD 格式化前置 close/flush 后调用 `libsys/SD::FormatSdCard`、重启调用 `libsys/device::Reboot`；runtime 可注入 libsys storage/device 服务后 drain 执行；`test_core_command_executor_contract` 和 `test_aov_app_runtime_contract` 覆盖 |

### 阶段 C：app/cloud 腾讯云接入

目标：把腾讯云 SDK 接入到当前 `ICloudService`，并让直播、云存、属性、动作都服从 core 低功耗策略。

| ID | 任务 | 状态 | 验收方式 |
| --- | --- | --- | --- |
| C1 | cloud 属性映射到 `DeviceConfig` | DONE | `test_cloud_property_mapper_contract` 通过 |
| C2 | cloud 动作映射到 `CoreCommand` | DONE | `test_cloud_action_mapper_contract` 通过 |
| C3 | cloud 状态上报 builder | DONE | `test_cloud_report_builder_contract` 通过 |
| C4 | Tencent SDK adapter host-safe 骨架 | DONE | host 编译不需要真实 SDK 链接 |
| C5 | TencentCloudService 实现 `ICloudService` | DONE | `test_tencent_cloud_service_contract` 通过 |
| C6 | 腾讯 SDK `iv_sys/iv_dm/iv_avt` 真实初始化参数接入 | DONE | `TencentSdkAdapter` 已按 `TencentCloudInitConfig` 填充 `iv_sys_init_parm_s`、产品三元组、缓存/持久化路径、MQTT 参数、`os_impl`，并接入 `iv_dm/iv_avt` 生命周期；根 CMake 新增 `AOV_ENABLE_TENCENT_SDK` 开关，启用后编入 `os_linux_impl.c`；真实联网效果待板端联调 |
| C7 | 腾讯物模型回调接入 property/action mapper | DONE | 已新增 `TencentGeneratedCallbackBridge` 和 C ABI dispatcher；已接入高优先级 `iv_usrcb_*` generated 回调：视频参数、云存开关、本地录像模式、智能检测、声光报警、重启、格式化、抓图、一键报警、恢复默认配置；回调只轻量入队，业务处理由 `TencentCloudService::DrainTencentGeneratedCallbacks()` drain 后继续分发 |
| C8 | 直播推流：Packet -> TencentStreamAdapter -> `iv_av` | DONE | `test_tencent_live_stream_contract`、`test_cloud_packet_sink_contract` 通过；真实 SDK 调用由 `AOV_ENABLE_TENCENT_SDK` 控制 |
| C9 | 云存推流：Packet -> TencentStreamAdapter -> `iv_cs` | DONE | 已新增 `TencentCloudStorage`，完成 host-safe 事件 start/finish、Packet 视频帧转 `iv_cs_push_stream`、成功或明确失败后更新 runtime drain；真实 `iv_cs` 上传效果待板端联调 |

### 阶段 D：app/storage 本地存储

目标：让 SD 卡录像走 `Packet -> PS/PES -> DHFS`，并能把写盘状态反馈给 core。

| ID | 任务 | 状态 | 验收方式 |
| --- | --- | --- | --- |
| D1 | Packet 到 storage sink 基础链路 | DONE | `test_packet_storage_pipeline` 通过 |
| D2 | StorageService drain 状态接入 core | DONE | `test_storage_service_drain_integration`、`test_storage_core_drain_mapper_contract` 通过 |
| D3 | `DhfsWriterStub` 统计写入帧数/字节数 | DONE | storage pipeline 测试覆盖 |
| D4 | 真实 PS/PES 封装接口设计和最小实现 | BLOCKED | 已输出 `docs/app_storage_AOV本地存储设计说明.md`，由 storage 同事按文档实现；输入 PacketFrame 输出可写入 PS/PES 数据 |
| D5 | 真实 DHFS writer 接入 `libDHFS.a` | BLOCKED | 已记录 `libDHFS.a` 和 DHFS 头文件参考路径，后续板端实现创建/写入/close/flush/fsync 文件 |
| D6 | AOV 间隔帧时间戳策略 | BLOCKED | 已在 storage 设计文档中说明 AOV 间隔录像与事件录像的时间轴差异，最终策略由 storage 实现时确认 |
| D7 | SD 卡格式化/容量/状态与 libsys/SD 联动 | DONE | core command executor 已可调用 `libsys/SD::FormatSdCard`，格式化命令经 cloud action -> core queue -> executor 可执行；容量/文件索引上报随真实 storage 实现补齐 |

### 阶段 E：app/packet 媒体帧分发

目标：保持裸码流分发，不加私有帧头；storage 和 cloud 可以并行消费同一帧。

| ID | 任务 | 状态 | 验收方式 |
| --- | --- | --- | --- |
| E1 | 定义 `PacketFrame`、订阅者、统计信息 | DONE | packet/storage 测试覆盖 |
| E2 | 分发到 storage sink | DONE | storage sink 收到帧并更新 drain 状态 |
| E3 | 分发到 cloud sink | DONE | `test_cloud_packet_sink_contract` 通过，runtime 已同时绑定 storage/cloud sink |
| E4 | 接入 libmedia 裸码流回调 | DONE | `AovAppRuntime` 可注入 `libmedia::IMediaRuntime`，初始化时注册 ch0/ch1 视频帧回调并转发到 `PacketService`；`test_aov_app_runtime_contract` 覆盖 libmedia -> packet -> storage |
| E5 | 评估是否替换为旧项目 `libPacket.a` 真实实现 | LATER | 当前 wrapper 满足架构验证，真实库迁移后再替换 |

### 阶段 F：app/alarm 告警策略

目标：把 libmedia 检测结果转换为产品告警事件，并驱动 storage/cloud/声光联动。

| ID | 任务 | 状态 | 验收方式 |
| --- | --- | --- | --- |
| F1 | AlarmEvent 和联动字段定义 | DONE | `test_alarm_service_contract` 通过 |
| F2 | 检测结果 + AlarmConfig 生成告警事件 | DONE | 合同测试覆盖基本联动 |
| F3 | 告警防抖和布防时间 | DONE | `AlarmServiceStub` 按目标类型执行 `debounce_sec` 防抖，并按 `ArmSchedule` 判断布防时间；`test_alarm_service_contract` 通过 |
| F4 | 声光报警执行接口接入 libsys/device | DONE | 一键报警已通过 core command executor 触发补光灯和声音报警；检测报警的声光执行复用同一 `libsys/device` 接口策略 |
| F5 | 告警触发本地录像和云存 | DONE | `AovAppRuntime::OnDetectResult` 已串接 orchestrator/alarm/storage/cloud，检测告警后启动本地录像和云存；`test_aov_app_runtime_contract` 通过 |

### 阶段 G：app/config 配置闭环

目标：支持云端配置下发、持久化、待生效、生效确认和状态上报。

| ID | 任务 | 状态 | 验收方式 |
| --- | --- | --- | --- |
| G1 | `DeviceConfig` 聚合结构体 | DONE | `test_config_phase1_contract` 通过 |
| G2 | 配置校验规则 | DONE | 配置合同测试通过 |
| G3 | Desired/Active/Pending 分离 | DONE | `test_config_service_core_policy_contract` 通过 |
| G4 | 云端属性更新 DesiredConfig | DONE | `test_cloud_service_integration_contract` 通过 |
| G5 | 配置持久化真实文件实现 | DONE | `ConfigServiceStub` 支持可选 key/value 持久化文件，覆盖工作模式、视频、云存、告警和声光关键配置；`test_config_phase1_contract` 通过 |
| G6 | 配置生效结果回报云端 | DONE | `AovAppRuntime::ApplyPendingConfig` 已执行 apply、MarkApplied、Persist、ReportDeviceState；active config 可通过现有 cloud report builder 上报 |

## 下一步建议

除 `app/storage` 真实 PS/PES + DHFS 写盘和板端联调外，app 侧主业务骨架已收口。下一步切到板端集成验证：`Kylin` 链接最新 `libmedia.a/libsys.a`，验证 libmedia 裸码流进入 packet、腾讯云真实初始化、检测告警触发录像/云存、云端 action 进入 core 后被执行，以及休眠前 storage/cloud drain。

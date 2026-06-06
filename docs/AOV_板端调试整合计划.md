# AOV 板端调试整合计划

本文用于把 `libmedia`、`libsys`、`app/packet`、`app/cloud`、`app/core`、`app/storage` 的板端调试顺序串起来。目标不是一次性全量联调，而是每一步都有明确输入、输出和验收标准，避免问题混在一起。

## 0. 当前边界

已完成或基本完成：

- `libmedia.a`：媒体库主链、sensor profile、AOV 休眠/唤醒、编码参数重配、裸码流回调接口。
- `libsys.a`：电源、SD、4G、设备控制等 app 侧依赖接口。
- `app/packet`：接收 libmedia 裸码流，分发到 cloud/storage sink。
- `app/cloud`：腾讯云 SDK 初始化、物模型属性/动作、直播、云存推流骨架。
- `app/core`：工作模式、配置应用、云端 action、检测告警、sleep drain 主控。
- `Kylin`：app 侧最终可执行程序。

未完成或单独移交：

- `app/storage` 真实 PS/PES + DHFS 写盘。
- AOV 间隔录像的文件时间轴、索引、回放检索。
- 板端真实联网、真实出流、真实休眠功耗验证。

## 1. 总体调试顺序

推荐顺序：

```text
阶段 1：基础产物确认
  -> libmedia.a / libsys.a / Kylin 能稳定编译

阶段 2：libmedia 单独出流
  -> 04d10 sensor 初始化
  -> 常电模式编码出裸码流
  -> AOV 唤醒后能恢复媒体链路

阶段 3：libmedia -> packet
  -> libmedia StreamFrame 回调
  -> app/packet PublishFrame
  -> storage/cloud sink 能收到同一帧

阶段 4：packet -> cloud
  -> 腾讯云 SDK 初始化
  -> 直播推流
  -> 云存事件 start/push/finish

阶段 5：detect/alarm/core 闭环
  -> 检测结果进入 app/core
  -> alarm 判断联动
  -> 本地录像 start
  -> 云存 start
  -> 告警上报

阶段 6：AOV 休眠与唤醒
  -> 定时唤醒
  -> App 主动唤醒
  -> 事件唤醒
  -> storage/cloud drain 完成后回 AOV 休眠

阶段 7：storage 真实写盘
  -> 由 storage 同事接入 PS/PES + DHFS
```

不要一开始就验证“检测 + 云 + 存储 + 休眠”全链路。任何失败都很难定位。必须先让每个边界都有可观察日志。

## 2. 阶段 1：基础产物确认

### 目标

确认编译产物完整，避免后面调试时混入链接或依赖问题。

### 输入

- `Depend/prebuilt/lib/libmedia.a`
- `Depend/prebuilt/lib/libsys.a`
- `Depend/ax_sdk/arm_uclibc/include`
- `Depend/ax_sdk/arm_uclibc/lib`
- `app/cloud/TencentCloud_SDK/include`
- `app/cloud/TencentCloud_SDK/lib`

### 命令

普通 app 编译：

```sh
cd ~/AOV_AX615/code_v2/app/Build
sh build_ax615.sh distclean
sh build_ax615.sh
```

打开真实腾讯云 SDK：

```sh
cd ~/AOV_AX615/code_v2/app/Build
sh build_ax615.sh distclean
sh build_ax615.sh tencent
```

### 成功标准

- 生成 `app/Build/Kylin`
- 编译过程不再编译 `libsys/Src/*`
- `Kylin` 链接 `Depend/prebuilt/lib/libmedia.a` 和 `Depend/prebuilt/lib/libsys.a`

### 失败排查

- 找不到 `AX_*`：检查 `Depend/ax_sdk/arm_uclibc/include` 和 `lib` 是否完整。
- 找不到 `libmedia.a/libsys.a`：检查是否放在 `Depend/prebuilt/lib/`。
- 腾讯 SDK 符号找不到：检查 `app/cloud/TencentCloud_SDK/lib/*.a` 是否存在，且 `sh build_ax615.sh tencent` 是否打开。

## 3. 阶段 2：libmedia 单独出流

### 目标

先确认媒体库本身能在板端工作，不经过 app/packet 和云端。

### 关键链路

```text
libmedia Init
  -> AX_SYS / AX_POOL
  -> VIN sensor 04d10
  -> ISP
  -> IVPS
  -> VENC/JENC
  -> StreamFrame callback
```

### 建议观察日志

每个模块至少打印一次成功日志：

- sensor profile：sensor name、pipe id、lane、resolution、fps。
- VIN：dev/pipe 创建成功。
- ISP：init/run 成功。
- IVPS：group/channel 启动成功。
- VENC：channel 启动成功。
- stream callback：channel、codec、size、pts、sequence、keyframe。

### 成功标准

- 常电模式下 ch0 或 ch1 能持续收到 `StreamFrame`。
- `frame.size > 0`
- `frame.data != nullptr`
- H264/H265 帧开头符合裸码流特征。
- `pts_ms` 单调递增。

### 失败排查

- 没有帧：先查 VIN/ISP/IVPS/VENC 哪一级没有启动成功。
- 有帧但 size 为 0：查 VENC 获取码流接口。
- 只有第一帧：查 VENC 线程/回调是否退出。
- PTS 不动：查时间戳来源。

## 4. 阶段 3：libmedia -> packet

### 目标

确认 app 能接住 libmedia 的裸码流，并且 packet 能并行分发。

### 关键链路

```text
libmedia::IVideoStreamService::RegisterStreamCallback
  -> AovAppRuntime::BindMediaToPacket
  -> MediaPacketRouter::OnMediaFrame
  -> PacketService::PublishFrame
  -> StoragePacketSink
  -> CloudPacketSink
```

### 需要加的日志点

`AovAppRuntime::BindMediaToPacket`：

```text
[core] bind libmedia stream callback ch=0 ok
[core] bind libmedia stream callback ch=1 ok
```

`MediaPacketRouter::OnMediaFrame`：

```text
[packet] media frame ch=0 codec=h264 size=xxx pts=xxx seq=xxx key=1
```

`PacketService::PublishFrame`：

```text
[packet] publish frame seq=xxx consumers=2 dropped=0
```

`StoragePacketSink`：

```text
[storage] recv packet frame seq=xxx size=xxx
```

`CloudPacketSink`：

```text
[cloud] recv packet frame seq=xxx size=xxx
```

### 成功标准

- libmedia 每来一帧，packet 统计 `published_frames` 增加。
- storage sink 和 cloud sink 都能收到同一帧。
- packet 不添加私有帧头。
- storage 当前可以只走 stub 统计，不要求真实写盘。

### 失败排查

- libmedia 有帧，packet 没帧：查 `AovAppRuntime` 是否注入真实 `IMediaRuntime`，而不是无 media runtime 的构造函数。
- packet 有帧，cloud sink 没帧：查 sink 是否订阅。
- packet 有帧，storage sink 没帧：查 `StorageService::StartRecord()` 是否已调用。

## 5. 阶段 4：packet -> cloud

### 目标

确认裸码流能从 packet 进入腾讯云直播/云存。

### 直播链路

```text
PacketFrame
  -> CloudPacketSink
  -> TencentLiveStream
  -> TencentStreamAdapter
  -> iv_av / iv_avt
  -> 腾讯云
  -> 手机 App 预览
```

### 云存链路

```text
Alarm/Event start
  -> TencentCloudStorage::StartEvent
  -> PacketFrame
  -> TencentStreamAdapter
  -> TencentCloudStorage::SendVideoPack
  -> iv_cs_push_stream
  -> TencentCloudStorage::FinishEvent
  -> app/core drain
```

### 成功标准

- 腾讯 SDK `iv_sys_init` 成功。
- `iv_dm` 生命周期启动成功。
- 直播时 packet 帧持续进入 `iv_av`。
- 云存事件开始后，packet 帧进入 `iv_cs_push_stream`。
- 云存成功或明确失败后，cloud drain 变为完成。

### 失败排查

- SDK init 失败：查三元组、路径、网络、4G。
- 手机 App 没预览：查 `iv_av` 通道、codec、I 帧、时间戳。
- 云存不结束：查 `FinishCloudStorage()` 是否被调用，或 SDK 回调是否回来了。

## 6. 阶段 5：detect/alarm/core 闭环

### 目标

确认检测结果能驱动告警、录像、云存和上报。

### 关键链路

```text
libmedia detect result
  -> AovAppRuntime::OnDetectResult
  -> AovOrchestratorStub::OnDetectResult
  -> AlarmServiceStub::EvaluateDetectResult
  -> StorageService::StartRecord
  -> CloudService::ReportAlarm
  -> CloudService::StartCloudStorage
```

### 成功标准

- 检测到目标后，`AlarmEvent.type != Unknown`。
- 防抖时间内重复目标不会重复触发。
- 非布防时间不触发。
- 低电保护状态不触发。
- 触发后本地录像状态为 recording。
- 触发后云存状态为 running。

### 失败排查

- 检测有结果但不告警：查 `AlarmConfig`、布防时间、防抖、低电保护。
- 告警有了但不录像：查 `linkage.trigger_record`。
- 告警有了但不云存：查 `linkage.push_message` 和云存开关。

## 7. 阶段 6：AOV 休眠与唤醒

### 目标

确认 core 能决定什么时候回 AOV 休眠，并且不会在云存/写盘未结束时休眠。

### 唤醒来源

```text
定时唤醒
App 主动唤醒
事件触发唤醒
4G/云端控制唤醒
```

### 回休眠条件

```text
业务结束
  -> idle debounce（空闲防抖 3-10 秒）
  -> storage drain 完成
  -> cloud drain 完成
  -> libmedia PrepareForAovSleep
  -> libsys power 进入 AOV 低功耗
```

### 成功标准

- App 退出预览后，不立刻休眠，而是进入 idle debounce。
- debounce 时间内有新业务，则取消本次休眠。
- 本地录像未 close/flush/fsync 完成，不休眠。
- 云存未成功或明确失败，不休眠。
- drain 都完成后，才调用 libmedia 进入 AOV sleep。

### 失败排查

- 太早休眠：查 storage/cloud drain 是否接入 `SystemSleepCoordinator`。
- 不休眠：查哪个 drain 一直 busy。
- 唤醒后媒体链不恢复：查 `RestoreFromAovSleep()`。

## 8. 阶段 7：storage 真实写盘

本阶段由 storage 同事实现，当前 app 只提供边界。

### 输入

```text
PacketFrame {
  codec
  data
  size
  pts_ms
  sequence
  is_keyframe
}
```

### 目标链路

```text
PacketFrame
  -> PS/PES muxer
  -> DHFS writer
  -> SD card file
  -> close + flush + fsync
  -> storage drain done
```

### 必须注意

- libmedia 不加私有帧头。
- packet 不剥私有帧头。
- storage 自己做 PS/PES 封装。
- AOV 间隔帧和事件录像时间戳不连续，要单独设计文件时间轴。
- SD 不要频繁 mount/unmount，避免功耗上升。

参考文档：

- `docs/app_storage_AOV本地存储设计说明.md`

## 9. 最小整机验收路径

第一轮不要验所有需求，只验最小闭环：

```text
1. Kylin 启动
2. 腾讯云 SDK init 成功
3. libmedia 04d10 出 H264/H265 裸码流
4. packet 收到帧
5. 云端直播能看到画面
6. 模拟检测结果触发告警
7. 云存 start
8. 云存收到若干帧
9. 云存 finish
10. core 判断 drain 完成
11. idle debounce 后回 AOV 休眠
12. 再次唤醒能恢复出流
```

这 12 步通过后，再进入配置项、低电保护、声光报警、格式化 SD、回放、异常恢复等扩展验证。

## 10. 推荐日志前缀

为了板端排查方便，建议统一日志前缀：

```text
[media]  libmedia 初始化、出流、检测、休眠/恢复
[packet] packet 接收和分发
[cloud]  腾讯云 init、直播、云存、物模型
[config] 配置下发、校验、生效、持久化
[core]   状态机、命令、drain、休眠决策
[alarm]  检测告警、防抖、布防、联动
[storage] 本地录像、写盘、flush、fsync
[sys]    SD、4G、电源、设备控制
```

## 11. 每次调试记录模板

```text
日期：
固件版本 / commit：
libmedia.a 版本：
libsys.a 版本：
Kylin 编译命令：
是否打开 Tencent SDK：
测试目标：
测试步骤：
成功现象：
失败现象：
关键日志：
下一步：
```

调试时不要只记录“失败了”。必须记录失败发生在哪个边界：media、packet、cloud、core、storage、sys。

## 12. 建议增加的运行参数

当前 `Kylin` 还偏骨架，后续板端调试时建议补最小运行参数。参数不要做复杂，先满足定位问题。

```text
Kylin --mode smoke
Kylin --mode media
Kylin --mode packet
Kylin --mode cloud
Kylin --mode alarm
Kylin --mode sleep
Kylin --log-level debug
Kylin --config /mnt/app/aov_config.kv
```

建议语义：

| 参数 | 用途 |
| --- | --- |
| `--mode smoke` | 只启动 app/core/config/cloud stub，确认进程能起来 |
| `--mode media` | 启动 libmedia，验证 sensor 和 VENC 出流 |
| `--mode packet` | 启动 libmedia + packet，验证裸码流分发 |
| `--mode cloud` | 启动 packet + Tencent SDK，验证直播和云存 |
| `--mode alarm` | 注入或等待检测结果，验证告警联动 |
| `--mode sleep` | 验证 idle debounce、drain、AOV 休眠和恢复 |
| `--log-level debug` | 打印每个边界的关键日志 |
| `--config` | 指定配置持久化文件路径 |

第一版如果来不及实现参数，也可以用编译宏或配置文件控制，但最终建议落到命令行参数，板端调试效率会高很多。

## 13. 分阶段 checklist

### 13.1 编译 checklist

```text
[ ] libmedia.a 已重新编译
[ ] libsys.a 已重新编译
[ ] Depend/prebuilt/lib/libmedia.a 已更新
[ ] Depend/prebuilt/lib/libsys.a 已更新
[ ] TencentCloud_SDK/lib 下真实库存在
[ ] sh build_ax615.sh 能编译 Kylin
[ ] sh build_ax615.sh tencent 能编译 Kylin
[ ] Kylin 已拷贝到板端
[ ] Kylin 依赖的动态库路径已配置
```

### 13.2 libmedia 出流 checklist

```text
[ ] sensor profile 选择为 os04d10
[ ] AX_SYS_Init 成功
[ ] AX_POOL_Init 成功
[ ] VIN dev 创建成功
[ ] VIN pipe 创建成功
[ ] ISP 初始化成功
[ ] IVPS group/channel 启动成功
[ ] VENC channel 启动成功
[ ] ch0 StreamFrame callback 有帧
[ ] ch1 StreamFrame callback 有帧，若当前只用单码流可暂不要求
[ ] I 帧能周期性出现
[ ] PTS 单调递增
```

### 13.3 packet 分发 checklist

```text
[ ] AovAppRuntime 注入真实 IMediaRuntime
[ ] BindMediaToPacket 注册 ch0/ch1 callback 成功
[ ] PacketService Init 成功
[ ] StoragePacketSink Subscribe 成功
[ ] CloudPacketSink Subscribe 成功
[ ] libmedia 每来一帧，PacketService published_frames 增加
[ ] storage sink received_frames 增加
[ ] cloud sink received_frames 增加
[ ] dropped_frames 为 0 或可解释
```

### 13.4 cloud checklist

```text
[ ] 4G 模组在线
[ ] 设备能解析 DNS
[ ] 设备时间正确，至少不能明显错乱
[ ] Tencent product_id/device_name/device_secret 正确
[ ] sys_cache_path 可写
[ ] sys_store_path 可写且掉电保留
[ ] dev_info_path 可写
[ ] iv_sys_init 成功
[ ] iv_dm 初始化成功
[ ] iv_av/iv_avt 初始化成功
[ ] 物模型属性上报成功
[ ] 物模型 action 回调能进入队列
[ ] 直播开始后 iv_av 推流有帧
[ ] 手机 App 能看到画面
[ ] 云存事件 start 成功
[ ] 云存 push stream 有帧
[ ] 云存 finish 后 drain 完成
```

### 13.5 core/sleep checklist

```text
[ ] 工作模式配置加载成功
[ ] 定时唤醒策略计算正确
[ ] App 唤醒能切到常电业务状态
[ ] 检测唤醒能切到常电业务状态
[ ] idle debounce 计时正确
[ ] debounce 期间有新业务会取消休眠
[ ] cloud drain busy 时不休眠
[ ] storage drain busy 时不休眠
[ ] drain 完成后调用 libmedia PrepareForAovSleep
[ ] libsys power 进入 AOV 低功耗
[ ] 再次唤醒后调用 libmedia RestoreFromAovSleep
```

## 14. 问题定位矩阵

| 现象 | 优先看哪里 | 常见原因 |
| --- | --- | --- |
| `Kylin` 启动失败 | app/entry、动态库、配置路径 | 依赖库缺失、权限不足、配置文件路径不可写 |
| Tencent SDK init 失败 | app/cloud/tencent、4G、三元组 | 产品三元组错误、网络未就绪、缓存路径不可写 |
| libmedia 没有帧 | libmedia VIN/ISP/IVPS/VENC | sensor profile 错、MIPI 不通、VENC 未启动 |
| libmedia 有帧但 packet 没帧 | app/core BindMediaToPacket | runtime 没注入真实 IMediaRuntime、callback 注册失败 |
| packet 有帧但 cloud 没帧 | app/packet、CloudPacketSink | cloud sink 未订阅、cloud 未 start、通道过滤错误 |
| cloud 有帧但手机无画面 | TencentStreamAdapter、iv_av | codec 不匹配、没有 I 帧、时间戳异常、通道号错误 |
| 检测结果有但不告警 | app/alarm、app/config | 告警关闭、防抖中、非布防时间、低电保护 |
| 告警有但不云存 | app/cloud、AlarmLinkageConfig | push_message 关闭、云存开关关闭、SDK 未初始化 |
| 告警有但不录像 | app/storage、AlarmLinkageConfig | trigger_record 关闭、storage 未 start、SD 不在线 |
| 业务完成后不休眠 | app/core drain | cloud/storage drain 一直 busy、idle debounce 未到 |
| 休眠太早 | app/core sleep coordinator | 未等待 cloud/storage 完成、失败状态未明确回报 |
| 唤醒后不出流 | libmedia RestoreFromAovSleep | sensor 未恢复、VENC 未重启、callback 丢失 |

## 15. 建议日志格式

统一日志格式，方便 grep：

```text
[module][level][timestamp_ms][tag] key=value key=value message
```

示例：

```text
[media][I][123456][venc] ch=0 codec=h265 size=1432 pts=90033 seq=18 key=0
[packet][I][123457][publish] seq=18 consumers=2 dropped=0
[cloud][I][123458][live] ch=0 seq=18 bytes=1432 sdk_ret=0
[core][I][123900][sleep] idle=1 storage=done cloud=busy action=wait
```

建议每个关键状态变化只打一条日志，不要每个循环都刷屏。码流帧日志可以每 N 帧打印一次，例如每 30 帧打印一次。

## 16. 板端常用命令模板

### 16.1 查看进程

```sh
ps | grep Kylin
```

### 16.2 运行 Kylin

```sh
cd /mnt/app
chmod +x Kylin
./Kylin
```

如果后续支持参数：

```sh
./Kylin --mode packet --log-level debug --config /mnt/app/aov_config.kv
```

### 16.3 查看网络

```sh
ifconfig
route -n
cat /etc/resolv.conf
ping -c 3 8.8.8.8
```

### 16.4 查看 SD 卡

```sh
mount
df -h
ls -lh /mnt/sdcard
```

### 16.5 查看日志

如果用 stdout：

```sh
./Kylin 2>&1 | tee /tmp/kylin.log
```

如果系统有 logcat/syslog，则按板端实际日志系统调整。

## 17. 最小日志插桩清单

如果现在代码日志不够，优先补这些点：

| 文件/模块 | 插桩点 | 必须打印 |
| --- | --- | --- |
| `app/entry/main.cpp` | 进程启动和退出 | build mode、是否启用 Tencent SDK |
| `app/core/aov_app_runtime.cpp` | Init/Start/Stop | 每个模块 init/start 结果 |
| `app/core/aov_app_runtime.cpp` | BindMediaToPacket | chn_id、注册结果 |
| `app/packet/media_packet_router.cpp` | OnMediaFrame | chn_id、codec、size、pts、seq、keyframe |
| `app/packet/packet_service_stub.cpp` | PublishFrame | consumer 数、drop 数、队列状态 |
| `app/cloud/cloud_packet_sink.cpp` | OnFrame | live/cloud-storage 是否启用 |
| `app/cloud/tencent/tencent_sdk_adapter.cpp` | InitSystem | 三元组是否为空、路径、SDK 返回值 |
| `app/cloud/tencent/tencent_stream_adapter.cpp` | SendVideo | seq、pts、bytes、SDK 返回值 |
| `app/cloud/tencent/tencent_cloud_storage.cpp` | Start/Send/Finish | event_id、channel、result |
| `app/core/system_sleep_coordinator.cpp` | TrySleep | idle、storage drain、cloud drain、最终动作 |
| `app/alarm/alarm_service_stub.cpp` | EvaluateDetectResult | type、debounce、schedule、linkage |

## 18. 调试分工建议

建议三个人并行，但边界要清楚：

| 人员 | 负责范围 | 输出 |
| --- | --- | --- |
| 媒体负责人 | libmedia 出流、检测、休眠恢复 | 证明 `StreamFrame` 和 detect result 稳定 |
| 云端负责人 | Tencent SDK、直播、云存、物模型 | 证明 packet 帧能上云，action 能下发 |
| 存储负责人 | PS/PES、DHFS、索引、回放 | 证明 PacketFrame 能落盘和检索 |

core/packet 作为集成边界，由主线负责人维护。任何模块发现问题，先给出“输入是否收到、输出是否发出”的证据。

## 19. 不要同时做的事情

以下事情不要混在同一轮调试：

- 不要一边调 sensor，一边调腾讯云。
- 不要在 libmedia 没稳定出流时调 packet。
- 不要在 packet 没确认分发时调直播。
- 不要在云存 drain 没跑通时调休眠功耗。
- 不要在 storage 真实写盘没完成时验证回放。
- 不要在配置闭环没稳定时频繁改编码参数。

每轮只验证一个边界。边界通过后，再进入下一个边界。

## 20. 第一周推荐推进节奏

### Day 1：编译和启动

目标：

```text
Kylin 能在板端启动，普通模式和 tencent 模式都能跑到 main loop。
```

验收：

```text
[entry] start
[config] load ok
[core] init ok
[packet] init ok
[cloud] init ok
[entry] running
```

### Day 2：libmedia 出流

目标：

```text
04d10 常电模式能稳定出 H264/H265 裸码流。
```

验收：

```text
[media] stream ch=0 size>0 pts increasing
```

### Day 3：packet 分发

目标：

```text
libmedia 裸码流进入 packet，storage/cloud sink 都能收到。
```

验收：

```text
[packet] published_frames increasing
[storage] received_frames increasing
[cloud] received_frames increasing
```

### Day 4：腾讯直播

目标：

```text
手机 App 能看到实时画面。
```

验收：

```text
[cloud] iv_sys_init ok
[cloud] live send frame ok
手机 App 有画面
```

### Day 5：云存和告警

目标：

```text
模拟或真实检测结果触发告警，云存能 start/push/finish。
```

验收：

```text
[alarm] event type=person
[cloud] cloud storage start ok
[cloud] cloud storage push frame ok
[cloud] cloud storage finish result=success 或明确失败
```

### Day 6：休眠恢复

目标：

```text
业务完成后等待 idle debounce，drain 完成后回 AOV 休眠；再次唤醒能恢复。
```

验收：

```text
[core] sleep wait cloud=done storage=done
[media] prepare sleep ok
[sys] enter aov sleep ok
[media] restore wakeup ok
```

### Day 7：整理问题

目标：

```text
把所有失败点归类到 media/cloud/core/storage/sys，不留“玄学问题”。
```

输出：

```text
问题列表
复现步骤
关键日志
责任模块
下一步修复人
```

## 21. 配置项调试顺序

配置项不要一开始全测。先测会影响主链路的配置，再测体验类配置。

### 21.1 第一批：工作模式配置

目标：

```text
云端或本地配置能改变 core 的工作策略，但不会直接绕过 app/core 调 libmedia。
```

链路：

```text
cloud property/action
  -> app/config DesiredConfig
  -> app/core ApplyPendingConfig
  -> libmedia/libsys
  -> MarkApplied
  -> Persist
  -> cloud report
```

优先验证：

- 工作模式。
- 省电模式。
- 定时唤醒间隔。
- App 主动唤醒。
- 低电保护阈值。

成功标准：

- Desired 和 Active 分离。
- 配置未 apply 前不影响当前媒体链路。
- apply 成功后 active config 更新。
- apply 失败时 active config 不乱跳。
- 重启后关键配置能从持久化文件恢复。

### 21.2 第二批：视频编码配置

优先验证：

- 分辨率。
- H264/H265。
- fps。
- bitrate。
- I 帧间隔。

建议顺序：

```text
先只改 fps
  -> 再改 bitrate
  -> 再改编码格式
  -> 最后改分辨率
```

原因：

- fps/bitrate 通常可以轻量应用。
- 编码格式和分辨率更可能需要停链路、应用配置、重启链路。

成功标准：

- App 一次只改一个参数时，不频繁重复 stop/start。
- 多个参数连续下发时，core 可以按 pending config 合并后应用。
- 重配后 packet 仍能收到帧。
- 云端直播能重新恢复。

### 21.3 第三批：图像/ISP 配置

优先验证：

- 日夜模式。
- AE。
- AWB。
- 慢快门。
- LDC。
- 锐化。
- WBC。

成功标准：

- 配置进入 `libmedia::ApplyImageConfig()`。
- libmedia 的 ISP service 能映射到 AX_ISP_IQ 接口。
- 失败时返回明确错误，不影响已有出流。

### 21.4 第四批：智能检测配置

优先验证：

- 人形检测开关。
- 灵敏度。
- 检测区域。
- 布防时间。
- 告警防抖。
- 告警联动：录像、推送、声光。

成功标准：

- 关闭检测后，不触发告警。
- 非布防时间不触发告警。
- 防抖时间内不重复触发。
- 联动字段能分别控制本地录像、云存/推送、声光。

### 21.5 第五批：云端动作

优先验证：

- 抓图。
- 一键告警。
- 重启。
- 恢复默认配置。
- 格式化 SD 卡。

链路：

```text
Tencent generated callback
  -> TencentGeneratedCallbackBridge
  -> TencentCloudService::DrainTencentGeneratedCallbacks
  -> CloudActionMapper
  -> AovAppRuntime::SubmitCloudAction
  -> CoreCommandQueue
  -> AovAppRuntime::DrainCoreCommands
  -> CoreCommandExecutor
```

成功标准：

- 回调线程只入队，不做重活。
- action 能带 request_id。
- core 执行后能返回成功或失败。
- 失败要能定位到模块，例如 `libmedia.snapshot`、`libsys.storage.format`。

## 22. sleep drain 调试细则

休眠问题最容易混乱，必须把状态拆开看。

### 22.1 drain 状态定义

```text
Idle     当前没有任务
Busy     正在处理，不能休眠
Done     本轮任务已完成
Failed   本轮任务明确失败，但也允许结束本轮 drain
Unknown  状态未知，不允许休眠
```

### 22.2 storage drain

当前真实 storage 未完成，stub 阶段只验证接口：

```text
StartRecord
  -> recording=true
FlushAndClose
  -> recording=false
  -> drain=Done
```

真实 storage 接入后必须验证：

```text
stop record request
  -> close file
  -> flush buffer
  -> fsync
  -> update index
  -> drain=Done 或 Failed
```

### 22.3 cloud drain

云存必须满足：

```text
StartCloudStorage
  -> cloud drain Busy
FinishCloudStorage(Success)
  -> cloud drain Done
FinishCloudStorage(Failed)
  -> cloud drain Failed
```

注意：Failed 也算“本轮完成”，可以回休眠，但必须上报或记录错误。

### 22.4 idle debounce

建议策略：

```text
业务结束
  -> 开始 3-10 秒 idle debounce
  -> debounce 期间有 App 预览/云存/录像/检测事件，则取消
  -> debounce 到期后检查 drain
  -> drain 完成才休眠
```

不要在 App 退出瞬间立即休眠，否则频繁打开/退出 App 会导致反复上下电，功耗和稳定性都不好。

## 23. 板端验收记录表

| 编号 | 验收项 | 结果 | 证据 |
| --- | --- | --- | --- |
| V1 | `Kylin` 普通模式启动 |  | 启动日志 |
| V2 | `Kylin` Tencent 模式启动 |  | `iv_sys_init` 日志 |
| V3 | 04d10 出裸码流 |  | ch0/ch1 frame 日志 |
| V4 | packet 收到 libmedia 帧 |  | `published_frames` |
| V5 | cloud sink 收到 packet 帧 |  | cloud packet 日志 |
| V6 | 手机 App 直播有画面 |  | App 截图/日志 |
| V7 | 云端属性下发到 DesiredConfig |  | config 日志 |
| V8 | ApplyPendingConfig 生效 |  | active config 日志 |
| V9 | 检测触发告警 |  | alarm event 日志 |
| V10 | 告警触发本地录像 start |  | storage recording 日志 |
| V11 | 告警触发云存 start |  | cloud storage event id |
| V12 | 云存收到视频帧 |  | `iv_cs_push_stream` 日志 |
| V13 | 云存 finish 后 drain 完成 |  | cloud drain 日志 |
| V14 | idle debounce 生效 |  | core sleep 日志 |
| V15 | drain 完成后进入 AOV 休眠 |  | media/sys sleep 日志 |
| V16 | 再次唤醒后恢复出流 |  | restore + frame 日志 |
| V17 | 低电保护不触发告警 |  | core/alarm 日志 |
| V18 | 格式化 SD action 能到 libsys |  | command executor 日志 |
| V19 | 抓图 action 能到 libmedia |  | snapshot 日志 |
| V20 | 重启 action 能到 libsys |  | reboot 日志 |

## 24. 当前代码还需要补的调试辅助

这些不是业务大功能，但建议尽快补，否则板端定位会慢。

| 优先级 | 项目 | 说明 |
| --- | --- | --- |
| P0 | app 统一日志宏 | 至少支持模块前缀和等级 |
| P0 | `Kylin` 运行模式参数 | `smoke/media/packet/cloud/alarm/sleep` |
| P0 | app/core 状态 dump | 打印 work state、drain、pending command 数 |
| P0 | packet 统计 dump | published/dropped/consumer count |
| P1 | cloud 统计 dump | connected/live/cloud_storage/sdk_error |
| P1 | config dump | desired/active/pending 摘要 |
| P1 | libmedia frame 统计 | 每路 fps、bytes、keyframe |
| P2 | 一键模拟检测结果 | 没有真实 AI 时也能测 alarm/core/cloud |
| P2 | 一键模拟云端 action | 没有真实云时也能测 command executor |

## 25. 推荐先补的两个调试入口

### 25.1 模拟检测

用途：不依赖真实 AI，先验证 alarm/core/cloud/storage。

建议命令：

```sh
./Kylin --mode alarm --simulate-detect person
```

预期链路：

```text
simulate detect person
  -> AovAppRuntime::OnDetectResult
  -> AlarmService
  -> StorageService::StartRecord
  -> CloudService::ReportAlarm
  -> CloudService::StartCloudStorage
```

### 25.2 模拟云端动作

用途：不依赖腾讯物模型真实回调，先验证 command queue 和 executor。

建议命令：

```sh
./Kylin --mode smoke --simulate-action reboot
./Kylin --mode smoke --simulate-action snapshot
./Kylin --mode smoke --simulate-action one_key_alarm
```

预期链路：

```text
simulate action
  -> CloudActionMapper
  -> CoreCommandQueue
  -> DrainCoreCommands
  -> CoreCommandExecutor
```

这两个入口能极大降低板端联调难度。真实云和真实 AI 都不稳定时，仍然可以先验证 app 主控逻辑。

# 腾讯云 SDK 接入方案与实施计划

本文用于指导 AOV 项目接入腾讯云 SDK。当前目标不是直接搬运旧项目 Cloud 模块，而是在现有 `app/core`、`app/config`、`app/packet`、`app/storage` 架构上，建立清晰的腾讯云适配层，后续再逐步接入直播、云存、属性上报和动作下发。

参考输入：

- 当前工程：`Z:\AOV_AX615\code_v2`
- 当前 SDK：`app/cloud/TencentCloud_SDK`
- 当前配置交互文档：`docs/AOV_配置与模块交互设计.md`
- 旧项目参考代码：`Z:\cp3prov1.2\Apps\Kylin\comps\Cloud`

## 1. 当前结论

当前 `app/cloud` 已有属性映射、动作映射、上报构造和 `ICloudService` stub，但业务代码尚未真实调用腾讯云 SDK。

腾讯云 SDK 已放入：

```text
app/cloud/TencentCloud_SDK/
  include/
  lib/
  tools/
```

已有头文件包括：

```text
iv_sys.h
iv_dm.h
iv_av.h
iv_cs.h
iv_rd.h
iv_cm.h
iv_err.h
explorer_inc/qcloud_iot_export.h
explorer_inc/qcloud_iot_import.h
```

已有库包括：

```text
libiot_sdk.a
libiot_video.a
libiot_platform.a
libmbedtls.a
libmbedcrypto.a
libmbedx509.a
libevent_core.a
libevent_extra.a
libevent_pthreads.a
libflv.a
libmpeg.a
libz.a
```

设计原则：

- `app/core` 只依赖 `ICloudService`，不直接包含腾讯云 SDK 头文件。
- `app/packet` 只负责裸码流分发，不关心腾讯云 SDK。
- `app/cloud/tencent` 作为唯一真实 SDK 适配层，负责 `PacketFrame` 到 `iv_cm_*` 的转换和 SDK 调用。
- 云存未成功或未明确失败前，`app/core` 不能进入 AOV 休眠。
- Host 测试继续走 stub，不链接腾讯云 SDK；真实 SDK 只在 AX615 交叉编译目标启用。

## 2. 目标目录结构

建议在 `app/cloud` 下新增 `tencent` 子目录：

```text
app/cloud/
  i_cloud_service.hpp
  cloud_types.hpp
  cloud_property_mapper.hpp/.cpp
  cloud_action_mapper.hpp/.cpp
  cloud_report_builder.hpp/.cpp
  cloud_service_stub.hpp/.cpp

  tencent/
    tencent_cloud_service.hpp/.cpp
    tencent_sdk_adapter.hpp/.cpp
    tencent_cloud_types.hpp
    tencent_product_model.hpp/.cpp
    tencent_stream_adapter.hpp/.cpp
    tencent_live_stream.hpp/.cpp
    tencent_cloud_storage.hpp/.cpp
```

职责划分：

| 文件 | 职责 |
| --- | --- |
| `tencent_cloud_service` | 实现 `ICloudService`，对 `app/core` 提供统一云服务入口。 |
| `tencent_sdk_adapter` | 只在这里包含 `iv_sys.h`、`iv_dm.h`、`iv_av.h`、`iv_cs.h` 等 SDK 头文件。 |
| `tencent_cloud_types` | 定义腾讯云适配层内部配置、状态和错误码映射。 |
| `tencent_product_model` | 对接物模型属性、动作、事件回调，把腾讯回调转换为当前 `CloudPropertyUpdate` / `CloudActionRequest`。 |
| `tencent_stream_adapter` | 把 `app/packet::PacketFrame` 转换为 `iv_cm_venc_pack_s` / `iv_cm_aenc_pack_s`。 |
| `tencent_live_stream` | 处理直播开始/停止回调、直播推流和拥塞状态查询。 |
| `tencent_cloud_storage` | 处理云存初始化、事件开始/结束、云存推流、云存完成状态。 |

## 3. 模块关系

整体关系：

```text
libmedia
  -> 输出裸编码帧

app/packet
  -> 接收裸帧
  -> 分发给 Storage / Cloud

app/storage
  -> PS/PES 封装
  -> DHFS 写 SD 卡

app/cloud/tencent
  -> 腾讯 SDK 初始化
  -> 物模型属性/动作/事件
  -> 直播推流
  -> 云存推流

app/core
  -> 汇总 storage/cloud/config 状态
  -> 判断能否回 AOV 休眠
```

云端链路只处理裸码流，不做 PS/PES：

```text
PacketFrame
  -> tencent_stream_adapter
  -> iv_cm_venc_pack_s / iv_cm_aenc_pack_s
  -> iv_avt_send_stream 或 iv_cs_push_stream
```

## 4. SDK 接口映射

### 4.1 系统初始化

SDK 头文件：`iv_sys.h`

关键接口：

```c
int iv_sys_init(const iv_sys_init_parm_s *pstInitParm);
int iv_sys_exit(void);
int iv_sys_get_time(uint64_t *milliseconds);
int iv_sys_set_time(uint64_t milliseconds);
int iv_sys_dyn_reg_device(iv_sys_device_info *psys_devInfo, char *dev_psk, int dev_psk_len);
```

旧项目参考：

```text
Apps/Kylin/comps/Cloud/cloud_main.cpp
  CCloudMain::OnCloudInit()
    iv_sys_init(&stSysInitParameters)
  CCloudMain::OnCloudDestroy()
    iv_sys_exit()
```

新项目落地：

```text
TencentSdkAdapter::InitSystem()
TencentSdkAdapter::ExitSystem()
```

注意点：

- `sys_store_path` 必须可读写并能掉电保存。
- `sys_cache_path` 应放临时读写路径。
- `os_impl_cb` 不能为空，参考 `TencentCloud_SDK/os_linux_impl.c`。
- 三元组、设备密钥、ProductKey 不应写死在代码里，应从安全存储或配置层读取。

### 4.2 物模型

SDK 头文件：`iv_dm.h`

关键接口：

```c
int iv_dm_init(const iv_dm_init_parm_s *pstInitParm);
int iv_dm_exit(void);
int iv_dm_property_report(const char *key, ivm_property_report_cb cb, void *param);
int iv_dm_property_sync(int timeout_ms);
int iv_dm_init_sync_status(void);
int iv_dm_event_report(const char *event_name);
int iv_dm_camera_event_report(const char *evt_msg);
```

旧项目参考：

```text
cloud_main.cpp
  iv_dm_init(&stInitParm)

cloud_IOT.h/.cpp
  iv_dm_property_report(...)
  OnIOT_Property_CloudStorageSwitch(...)
  OnIOT_Action_CloudStorageStaus(...)
  OnIOT_Action_Reboot(...)
  OnIOT__Action_FormatStorageMedium(...)

iv_usrex.c/.h
iv_usrex_func.c
  腾讯物模型生成代码和回调入口
```

新项目落地：

```text
TencentProductModel::Init()
TencentProductModel::ReportProperty(...)
TencentProductModel::ReportEvent(...)
TencentProductModel::OnPropertySet(...)
TencentProductModel::OnAction(...)
```

物模型接入原则：

- 旧项目的 `cloud_IOT.cpp` 只能作为字段和回调参考，不建议整体迁移。
- 腾讯回调统一转换成当前项目已有结构：
  - `CloudPropertyUpdate`
  - `CloudActionRequest`
  - `CloudEventReport`
- 属性映射继续复用当前 `cloud_property_mapper`。
- 动作映射继续复用当前 `cloud_action_mapper`。
- 状态上报继续复用当前 `cloud_report_builder`。

### 4.3 直播和回放传输

SDK 头文件：`iv_av.h`

关键接口：

```c
int iv_avt_init(const iv_avt_init_parm_s *pstInitParm);
int iv_avt_exit(void);
int iv_avt_send_stream(uint32_t visitor,
                       uint32_t channel,
                       iv_avt_video_res_type_e video_res_type,
                       iv_avt_stream_type_e stream_type,
                       void *p_stream);
size_t iv_avt_get_send_stream_buf(...);
int iv_avt_get_send_stream_status(...);
int iv_avt_send_finish_stream(...);
```

旧项目参考：

```text
cloud_LV.cpp
  iv_avt_init(&stAvtInitParameters)
  iv_avt_start_real_play_cb = start_push_streaming_cb
  iv_avt_stop_real_play_cb = stop_push_streaming_cb

cloud_live.cpp
  iv_avt_send_stream(..., IV_AVT_STREAM_TYPE_VIDEO, ...)
  iv_avt_send_stream(..., IV_AVT_STREAM_TYPE_AUDIO, ...)
```

新项目落地：

```text
TencentLiveStream::Init()
TencentLiveStream::OnStartRealPlay(...)
TencentLiveStream::OnStopRealPlay(...)
TencentLiveStream::OnPacketFrame(...)
```

直播链路：

```text
手机 App 请求直播
  -> 腾讯 SDK 回调 start_real_play
  -> TencentLiveStream 标记会话 active
  -> app/cloud 更新 preview_streaming=true
  -> app/core 保持常电，不进入 AOV 休眠
  -> app/packet 分发 PacketFrame
  -> TencentStreamAdapter 转 iv_cm_venc_pack_s
  -> iv_avt_send_stream
  -> 手机 App 预览
```

停止直播：

```text
手机 App 停止直播
  -> 腾讯 SDK 回调 stop_real_play
  -> TencentLiveStream 清理 visitor/session
  -> app/cloud 更新 preview_streaming=false
  -> app/core 开始 idle debounce
  -> storage/cloud/config drain 完成后允许回 AOV 休眠
```

### 4.4 云存

SDK 头文件：`iv_cs.h`

关键接口：

```c
int iv_cs_init(iv_cs_init_parm_s *pstInitParm);
int iv_cs_exit(void);
int iv_cs_push_stream(iv_cs_chn_e channel,
                      iv_cm_stream_type_e eCsStreamType,
                      void *pstCsStream);
int iv_cs_event_start(iv_cs_chn_e channel, int32_t event_id, const char *info);
int iv_cs_event_stop(iv_cs_chn_e channel, int32_t event_id, const char *info);
int iv_cs_event_start_ext(...);
int iv_cs_event_stop_ext(...);
int iv_cs_event_directly_report(...);
int iv_cs_get_balance_info(iv_cs_chn_e channel,
                           iv_cs_balance_info_s *pstBalanceInfo,
                           uint32_t timeout_s);
int iv_cs_set_trans_time(uint32_t upload_timeout_ms, uint32_t reply_timeout_ms);
```

旧项目参考：

```text
cloud_storage.cpp
  iv_cs_init(&stCsInitParm)
  iv_cs_event_start(...)
  iv_cs_event_stop(...)
  iv_cs_push_stream(..., IV_CM_STREAM_TYPE_VIDEO, ...)
  iv_cs_push_stream(..., IV_CM_STREAM_TYPE_AUDIO, ...)
  iv_cs_get_balance_info(...)
```

新项目落地：

```text
TencentCloudStorage::Init()
TencentCloudStorage::StartEvent(...)
TencentCloudStorage::StopEvent(...)
TencentCloudStorage::OnPacketFrame(...)
TencentCloudStorage::GetRuntimeStatus()
```

云存链路：

```text
检测到目标 / 告警触发
  -> app/core 决定触发云存
  -> app/cloud/tencent 调 iv_cs_event_start
  -> CloudRuntimeStatus.cloud_storage_running = true
  -> app/packet 分发 PacketFrame
  -> TencentStreamAdapter 转 iv_cm_venc_pack_s / iv_cm_aenc_pack_s
  -> iv_cs_push_stream
  -> 事件结束后 iv_cs_event_stop
  -> 成功或明确失败
  -> CloudRuntimeStatus.cloud_storage_running = false
  -> CloudRuntimeStatus.cloud_storage_result = Success / ExplicitFailure
  -> app/core drain 条件满足后才允许休眠
```

失败处理原则：

- `iv_cs_event_start` 失败且确认无套餐、鉴权失败、参数非法等不可继续场景，记为明确失败。
- `iv_cs_push_stream` 短暂失败可以有限重试或缓存，但必须有最大缓存和超时策略。
- 超过最大云存等待时间后，应转为明确失败，避免设备一直常电无法休眠。
- 明确失败也算云存完成，可以让 `app/core` 后续进入 AOV 休眠。

### 4.5 本地云端录像 RD

SDK 头文件：`iv_rd.h`

关键接口：

```c
int iv_rd_init(iv_rd_init_parm_s *pstInitParm);
int iv_rd_exit(void);
int iv_rd_record_start(char *ps8RecordType, iv_cm_av_data_info_s *format);
int iv_rd_record_stop(void);
int iv_rd_push_stream(iv_cm_stream_type_e eStreamType, void *pStream);
```

当前建议：

- 首轮不接入 `iv_rd_*`。
- 本地 SD 录像仍走 `app/storage -> PS/PES -> DHFS`。
- 回放、下载、云端录像索引等能力后置，避免影响 AOV 低功耗主链闭环。

### 4.6 媒体结构

SDK 头文件：`iv_cm.h`

关键结构：

```c
iv_cm_av_data_info_s
iv_cm_venc_pack_s
iv_cm_aenc_pack_s
iv_cm_venc_stream_s
iv_cm_aenc_stream_s
iv_cm_venc_type_e
iv_cm_aenc_type_e
iv_cm_stream_type_e
```

当前 `PacketFrame` 主要字段：

```cpp
struct PacketFrame {
    int chn_id;
    aov::media::VideoCodec codec;
    uint64_t pts_ms;
    uint64_t sequence;
    bool is_keyframe;
    std::shared_ptr<const std::vector<uint8_t>> payload;
};
```

视频转换规则：

```text
PacketFrame::data()
  -> iv_cm_venc_pack_s::pu8Addr

PacketFrame::size()
  -> iv_cm_venc_pack_s::u32Len

PacketFrame::pts_ms
  -> iv_cm_venc_pack_s::u64PTS

PacketFrame::sequence
  -> iv_cm_venc_pack_s::u32Seq

PacketFrame::is_keyframe
  -> iv_cm_venc_pack_s::eFrameType

PacketFrame::codec
  -> IV_CM_VENC_TYPE_H264 / IV_CM_VENC_TYPE_H265
```

注意：

- 当前 `PacketFrame` 偏视频，后续如音频也走 packet，需要补充音频类型、采样率、bit depth、音频 codec 等字段，或新增 `PacketMediaType`。
- 腾讯直播和云存需要 `iv_cm_av_data_info_s` 描述编码格式，应从 `DeviceConfig::media` 或 libmedia 当前 active profile 映射生成。
- AOV 间隔帧场景下，PTS 是否使用真实时间间隔还是连续播放时间轴，需要云存和回放方案单独确认。

## 5. 与现有 core/config/packet 的交互

### 5.1 app/core

`app/core` 当前通过 `ICloudService` 获取云端状态：

```cpp
virtual cloud::CloudRuntimeStatus GetRuntimeStatus() const = 0;
```

云存 drain 判断：

```text
cloud_storage_finished =
  !cloud_storage_running &&
  (cloud_storage_result == Success ||
   cloud_storage_result == ExplicitFailure)
```

因此腾讯云真实实现必须维护：

```text
CloudRuntimeStatus.connected
CloudRuntimeStatus.preview_streaming
CloudRuntimeStatus.cloud_storage_running
CloudRuntimeStatus.cloud_storage_result
CloudRuntimeStatus.active_cloud_storage_id
```

### 5.2 app/config

配置下发链路：

```text
腾讯物模型属性回调
  -> TencentProductModel
  -> CloudPropertyUpdate
  -> cloud_property_mapper
  -> app/config DesiredConfig
  -> app/core 决定何时生效
```

设计约束：

- 休眠期间允许写 DesiredConfig，但普通配置不单独唤醒生效。
- 唤醒后由 `app/core` 拉取 pending config 并决定是否重启媒体链。
- 云端显示的配置状态要区分“已接收”和“已生效”。

### 5.3 app/packet

Packet 对云端是裸帧分发器：

```text
libmedia StreamFrame
  -> app/packet PublishFrame
  -> PacketConsumerId::Cloud
  -> app/cloud/tencent OnPacketFrame
```

云端不需要 PS/PES，也不需要私有帧头。

### 5.4 app/storage

本地 SD 和云端存储并行：

```text
Packet
  -> Storage sink
     -> PS/PES
     -> DHFS
     -> SD

  -> Cloud sink
     -> iv_cm_*
     -> Tencent SDK
     -> Cloud
```

本地 SD 的 `close + flush + fsync` 和云存的 `Success / ExplicitFailure` 都是回 AOV 休眠前的必要条件。

## 6. 状态机和低功耗要求

腾讯云接入必须服从 AOV 低功耗策略：

```text
直播中：
  preview_streaming = true
  不允许休眠

云存中：
  cloud_storage_running = true
  不允许休眠

云存成功：
  cloud_storage_running = false
  cloud_storage_result = Success
  可参与 drain

云存明确失败：
  cloud_storage_running = false
  cloud_storage_result = ExplicitFailure
  可参与 drain

云存未知状态：
  cloud_storage_running = true 或 result = None
  不允许休眠
```

回 AOV 休眠前必须满足：

```text
本地录像 close + flush + fsync 完成
云存成功或明确失败
配置持久化完成
idle debounce（抖动）到期
没有直播/回放/动作命令等活跃业务
```

## 7. 实施计划

### 阶段 1：SDK 编译和链接 smoke

目标：确认当前 SDK 头文件和库能在 AX615 交叉编译环境下被真实链接。

任务：

a. CMake 开关

```text
新增 BUILD_TENCENT_CLOUD_SDK 或 AOV_ENABLE_TENCENT_CLOUD
Host 默认 OFF
AX615 交叉编译可 ON
```

b. include/lib 路径

```text
include:
  app/cloud/TencentCloud_SDK/include
  app/cloud/TencentCloud_SDK/include/explorer_inc

lib:
  app/cloud/TencentCloud_SDK/lib
```

c. 最小 smoke 文件

```text
只 include iv_sys.h / iv_dm.h
只引用 iv_sys_init / iv_dm_init 符号
不启动真实联网
```

d. 验收

```text
AX615 交叉编译链接通过
Host 单元测试不受影响
```

### 阶段 2：系统初始化和物模型最小闭环

目标：实现 `iv_sys_init -> iv_dm_init` 的真实启动和退出。

任务：

a. 新增 `tencent_sdk_adapter`

```text
InitSystem()
InitDeviceModel()
Start()
Stop()
```

b. 读取设备三元组

```text
ProductKey
DeviceName
DeviceSecret
```

先预留接口，真实来源可后接 `app/config` 或安全存储。

c. 接入 `os_impl_cb`

参考：

```text
app/cloud/TencentCloud_SDK/os_linux_impl.c
```

d. 验收

```text
设备端能完成腾讯 SDK 初始化
云端能看到设备上线/离线
Stop 时能按 iv_dm_exit -> iv_sys_exit 释放
```

### 阶段 3：物模型属性、动作、事件接入

目标：把腾讯云回调接到当前 `app/config` 和 `app/core`。

任务：

a. 接入 `iv_usrex`

```text
使用 app/cloud/TencentCloud_SDK/tools/iv_usrex.c
使用 app/cloud/TencentCloud_SDK/tools/iv_usrex_func.c
使用 app/cloud/TencentCloud_SDK/tools/iv_usrex.h
```

b. 属性下发转换

```text
腾讯回调
  -> TencentProductModel::OnPropertySet
  -> CloudPropertyUpdate
  -> cloud_property_mapper
  -> app/config
```

优先字段：

```text
AovWorkMode
AovRecordInterval
LowBatteryProtectThreshold
VideoEncoding
VideoResolution
VideoFPS
VideoBitrate
AudioEncoding
DeviceInputVolume
DeviceOutputVolume
CloudStorageSwitch
StorageRecordMode
HumanDetectConfig
ImageDetectConfig
SoundLightAlarmConfig
```

c. 动作下发转换

优先动作：

```text
FormatStorageMedium
OneKeyAlarm
PreviewImageCapture
Reboot
SetDefaultDevConfigs
CloudStorageStaus
```

d. 状态上报

```text
CloudReportBuilder
  -> TencentProductModel::ReportProperty
  -> iv_dm_property_report
```

e. 验收

```text
云端属性下发能写入 DesiredConfig
云端动作能生成 core command
设备状态能通过 iv_dm_property_report 上报
```

### 阶段 4：直播推流

目标：手机 App 拉直播时，设备能从 Packet 获取裸帧并推到腾讯云。

任务：

a. 新增 `tencent_live_stream`

```text
Init iv_avt_init
处理 start_real_play 回调
处理 stop_real_play 回调
维护 visitor/session
```

b. 接入 Packet

```text
start_real_play:
  BindCloudSink 或启动 cloud consumer

stop_real_play:
  清理 session
  必要时退订 cloud consumer
```

c. 新增 `tencent_stream_adapter`

```text
PacketFrame -> iv_cm_venc_pack_s
PacketFrame -> iv_cm_aenc_pack_s
DeviceConfig::media -> iv_cm_av_data_info_s
```

d. 推流

```text
视频 -> iv_avt_send_stream(..., IV_AVT_STREAM_TYPE_VIDEO, ...)
音频 -> iv_avt_send_stream(..., IV_AVT_STREAM_TYPE_AUDIO, ...)
```

e. 验收

```text
App 请求直播后 preview_streaming=true
能收到视频帧
停止直播后 preview_streaming=false
停止后 app/core 可进入 idle debounce
```

### 阶段 5：云存推流

目标：告警触发后，设备能按腾讯云存接口推送事件流，并把完成状态反馈给 core。

任务：

a. 新增 `tencent_cloud_storage`

```text
Init iv_cs_init
StartEvent -> iv_cs_event_start / iv_cs_event_start_ext
StopEvent -> iv_cs_event_stop / iv_cs_event_stop_ext
OnPacketFrame -> iv_cs_push_stream
```

b. 云存套餐状态

```text
iv_cs_get_balance_info
```

套餐不足要进入明确失败，避免设备持续常电。

c. 云存失败分类

```text
Success
ExplicitFailure
Retrying
Timeout
NoBalance
NetworkError
InvalidState
```

对外仍映射到：

```text
CloudStorageResult::Success
CloudStorageResult::ExplicitFailure
CloudStorageResult::None
```

d. 超时和缓存

```text
短暂 iv_cs_push_stream 失败可有限缓存
缓存必须有最大字节数
云存必须有最大等待时间
超过上限后转 ExplicitFailure
```

e. 验收

```text
云存开始后 cloud_storage_running=true
云存未完成时 app/core TryEnterAovSleep 返回 DrainIncomplete
云存成功后 cloud_storage_result=Success
云存明确失败后 cloud_storage_result=ExplicitFailure
成功或明确失败后 app/core 可继续回休眠判断
```

### 阶段 6：低功耗联调

目标：验证腾讯云业务不会破坏 AOV 休眠策略。

场景：

```text
手机 App 打开直播 -> 退出 App -> idle debounce 到期 -> 回 AOV 休眠
检测到目标 -> 本地 SD 写盘 + 云存并行 -> 两者完成 -> 回 AOV 休眠
云存失败 -> 明确失败 -> 回 AOV 休眠
4G 断网 -> 云存超时 -> 明确失败 -> 回 AOV 休眠
云端配置下发 -> 只写 DesiredConfig -> 下次唤醒生效
```

验收：

```text
不会因为云存未知状态长期常电
不会在云存未完成时提前休眠
不会在直播中进入休眠
不会因为配置下发频繁唤醒
```

### 阶段 7：后置能力

首轮不做或后置：

```text
iv_rd_* 本地云端录像
云端回放
文件下载
语音对讲
OTA
日志上传
复杂 P2P 拥塞控制
```

这些能力在 AOV 主链稳定后，再分别设计。

## 8. 测试计划

Host 测试：

```text
继续使用 CloudServiceStub
测试 cloud_property_mapper
测试 cloud_action_mapper
测试 cloud_report_builder
测试 cloud_core_drain_mapper
测试 PacketFrame -> 腾讯流结构的纯转换逻辑
```

交叉编译测试：

```text
腾讯 SDK include/link smoke
iv_sys_init / iv_dm_init 符号链接
iv_avt_init 符号链接
iv_cs_init / iv_cs_push_stream 符号链接
```

板端测试：

```text
设备上线
属性同步
属性下发
状态上报
直播拉流
云存事件开始/结束
云存成功/失败 drain
AOV 休眠恢复
```

## 9. 风险和待确认问题

1. 产品三元组来源需要确认：安全分区、配置文件、出厂烧录还是动态注册。
2. 当前 `PacketFrame` 偏视频，音频进入云端前需要补充媒体类型和音频参数。
3. 云存 AOV 间隔帧的 PTS 策略需要单独确认，尤其影响云端播放体验。
4. 云存失败重试和最大等待时间需要产品和功耗目标共同确认。
5. 腾讯产品物模型是否已包含 AOV 新字段，例如 `AovWorkMode`、`AovRecordInterval`、`LowBatteryProtectThreshold`，需要和云端产品模型确认。
6. `iv_usrex` 生成代码是否要基于新产品模型重新生成，不能直接默认旧 `cp3prov1.2` 的字段完全适配。
7. SDK 静态库链接顺序需要在交叉编译阶段实测确认。

## 10. 推荐执行顺序

```text
1. CMake + SDK link smoke
2. iv_sys + iv_dm 最小上线
3. 物模型属性/动作桥接
4. 直播 iv_avt 推流
5. 云存 iv_cs 推流
6. core drain + AOV 休眠联调
7. 回放/语音/OTA 等后置能力
```

推荐先完成阶段 1 和阶段 2，确认 SDK 能上线，再进入直播和云存。直播和云存都依赖 Packet 裸帧分发，因此不要绕过 `app/packet` 直接从 libmedia 回调里推云端。

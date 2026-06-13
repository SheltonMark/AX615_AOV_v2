# app/packet 产品化改造设计

本文定义 `app/packet` 的产品化目标、阶段状态和剩余实施计划。结论先行：AOV 项目不迁移旧 Kylin 的 Web/NVR/RTSP/GB28181 分支，也不迁移 DHAV/VALZ 私有帧头解析；但需要吸收旧 Packet 作为媒体内存池和多消费者分发中心的核心能力。

## 1. 改造目标

`app/packet` 是 AOV 的媒体帧总线，职责是：

```text
libmedia 裸码流
  -> app/packet
      -> app/cloud live
      -> app/cloud storage
      -> app/storage local record
      -> snapshot/debug/audio 预留
```

必须保证：

- packet 不加私有帧头。
- packet 不剥私有帧头。
- cloud 消费裸码流或云 SDK 封装。
- storage 自己做 PS/PES 封装。
- cloud 和 storage 可以并行消费同一帧。
- 单个消费者卡住不能拖死其他消费者。

## 2. 不迁移范围

旧项目里很多模块依赖 `CPacket`，但 AOV 不需要全部迁移。

不迁移：

- `MOD_ID_NVR_LIVE`
- `MOD_ID_WEB_LIVE`
- `MOD_ID_RTSP_LIVE`
- `MOD_ID_RTMP_LIVE`
- GB28181
- Web/NVR 回放下载
- Console 测试入口
- DHAV/VALZ 私有帧头解析
- `getFrameHeadAndTail()` / `getFrameTime()` / `findIframePtsfrom_*()` 这类旧私有格式解析

保留思想：

- 内存池。
- 引用计数。
- 多消费者队列。
- 背压。
- 丢帧策略。
- 内存恢复通知。
- dump/统计。

## 3. 对外接口目标

### 3.1 当前基础接口

```cpp
class IPacketService {
public:
    virtual PacketResult Init(const PacketConfig& config) = 0;
    virtual void Deinit() = 0;

    virtual PacketResult Subscribe(PacketConsumerId consumer,
                                   PacketFrameCallback callback) = 0;
    virtual void Unsubscribe(PacketConsumerId consumer) = 0;

    virtual PacketResult PublishFrame(const aov::media::StreamFrame& frame) = 0;
    virtual PacketStats GetStats() const = 0;
};
```

### 3.2 产品化扩展接口

```cpp
PacketResult SetConsumerConfig(PacketConsumerId consumer,
                               const PacketConsumerConfig& config);

PacketConsumerStats GetConsumerStats(PacketConsumerId consumer) const;

PacketResult Dispatch(PacketConsumerId consumer, std::uint32_t max_frames);

PacketResult AcquireBuffer(std::uint32_t size, PacketWritableBuffer& out);

PacketResult PublishBuffer(PacketWritableBuffer& buffer,
                           const PacketFrameInfo& info);

void ReleaseBuffer(PacketWritableBuffer& buffer);

bool HasCapacity(std::uint32_t size) const;

std::string DumpStats() const;
```

其中 `AcquireBuffer/PublishBuffer/ReleaseBuffer` 对应旧版：

```cpp
GetPacket()
GetPacketList()
```

这是从“packet 被动接收一帧”升级到“packet 主动管理生产者写入内存”的关键。

## 4. 阶段状态

| 阶段 | 内容 | 状态 | 说明 |
| --- | --- | --- | --- |
| P1 | 接口和类型整理 | DONE | 已扩展 `PacketMediaType`、`PacketConsumerId`、`PacketStats`、`PacketConsumerStats` |
| P2 | 产品化 `PacketService` 第一版 | DONE | 已支持 buffer pool、每消费者队列、独立背压、P 帧丢弃后等 I 帧 |
| P3 | 默认链路切到 `PacketService` | DONE | `main`、`PacketStoragePipeline`、核心测试已从 stub 切换到 `PacketService` |
| P4 | 生产者侧 buffer API | DONE | 已补 `AcquireBuffer/PublishBuffer/ReleaseBuffer`，对齐旧 `GetPacket/GetPacketList` 核心能力 |
| P5 | 容量查询和 dump | DONE | 已补 `HasCapacity()`、`DumpStats()` |
| P6 | Storage / CloudLive / CloudStorage 默认策略细化 | DONE | 已按消费者设置不同默认队列、背压和溢出策略：Storage 偏完整，CloudLive 偏实时，CloudStorage 偏事件完整且有界 |
| P7 | 内存恢复通知 | DONE | 已补 `PacketPressureCallback`，支持 pool exhausted/recovered、consumer overflow/recovered 事件，对齐旧 `AttachNullPacket/on_free_packet` 思想 |
| P8 | CloudLive / CloudStorage 分离绑定 | DONE | 已新增 `BindCloudLiveSink()` / `BindCloudStorageSink()`，`BindCloudSink()` 继续兼容映射到 `CloudLive` |
| P9 | 音频帧字段补齐 | DONE | 已补 `PacketAudioCodec`、sample rate、channels、bit width，支持音频元信息随 `PacketFrame` 分发 |
| P10 | 内建 worker 线程 | LATER | 当前已有队列和 `Dispatch()`，是否内建 worker 视板端性能和调用模型决定 |

## 5. 当前已完成能力

已完成：

- `PacketService` 新实现。
- 固定 buffer pool 雏形。
- `Storage` / `CloudLive` / `CloudStorage` 消费者枚举。
- 每消费者独立队列。
- 每消费者独立队列配置。
- 队列满时背压。
- 丢帧后等待下一个 I 帧恢复。
- 全局统计。
- 单消费者统计。
- 手动 `Dispatch()`。
- `auto_dispatch` 模式。

已验证：

```text
test_packet_service_contract
test_packet_storage_pipeline
test_cloud_packet_sink_contract
test_aov_app_runtime_contract
```

第二阶段已完成：

- `PacketWritableBuffer`
- `PacketFrameInfo`
- `AcquireBuffer()`
- `PublishBuffer()`
- `ReleaseBuffer()`
- `HasCapacity()`
- `DumpStats()`

## 6. 第二阶段实施结果

### P4.1 新增写入型类型

已新增：

```cpp
struct PacketFrameInfo {
    PacketMediaType media_type;
    int chn_id;
    aov::media::VideoCodec codec;
    std::uint64_t pts_ms;
    std::uint64_t sequence;
    bool is_keyframe;
};

struct PacketWritableBuffer {
    std::uint8_t* data;
    std::uint32_t capacity;
    std::uint32_t size;
    // 内部持有 packet buffer 引用
};
```

### P4.2 新增生产者侧接口

```cpp
PacketResult AcquireBuffer(std::uint32_t size, PacketWritableBuffer& out);
PacketResult PublishBuffer(PacketWritableBuffer& buffer,
                           const PacketFrameInfo& info);
void ReleaseBuffer(PacketWritableBuffer& buffer);
```

语义：

- `AcquireBuffer()` 从 packet pool 取 buffer。
- 生产者写入 `buffer.data`。
- 生产者设置 `buffer.size`。
- `PublishBuffer()` 转成 `PacketFrame` 并投递给消费者。
- 如果生产者放弃写入，调用 `ReleaseBuffer()`。

### P5.1 新增容量查询

```cpp
bool HasCapacity(std::uint32_t size) const;
```

用途：

- libmedia 回调或上层调度可以提前判断 packet 是否有空间。
- 避免明知 pool 满还继续做大帧拷贝。

### P5.2 新增 dump

```cpp
std::string DumpStats() const;
```

输出建议：

```text
packet published=100 delivered=180 dropped=3 buffered=2 consumers=2
consumer=Storage queued=0 delivered=100 dropped=0 waiting_keyframe=0
consumer=CloudLive queued=2 delivered=80 dropped=3 waiting_keyframe=1
```

## 7. 验收标准

P4/P5 完成后必须满足：

- 旧 `PublishFrame(StreamFrame)` 仍可用。
- 新 `AcquireBuffer + PublishBuffer` 可发布帧。
- `AcquireBuffer` 获取后不发布，`ReleaseBuffer` 能归还 pool。
- pool 满时 `AcquireBuffer` 返回 `Backpressure`。
- `PublishBuffer` 后 storage/cloud 可以收到同一帧。
- `HasCapacity` 能反映 pool 可用状态。
- `DumpStats` 能输出全局和消费者关键统计。

## 8. 后续原则

- 不引入旧 `CPacket` 类型到 AOV 对外接口。
- 不暴露 `MOD_ID_*` 旧枚举。
- 不把私有帧头解析放回 packet。
- packet 只做裸码流承载、引用、队列、分发和背压。

## 9. 参考代码策略分析

旧项目参考路径：

```text
Z:\cp3prov1.2\Apps\Kylin\System\Packet.cpp/h
Z:\cp3prov1.2\Apps\Kylin\Devices\DevBufSendManager.cpp/h
Z:\cp3prov1.2\Apps\Kylin\Devices\DevBufManager.cpp/h
```

旧项目核心策略不是“所有消费者共用一套队列”，而是：

```text
统一 packet memory pool
  -> 不同 MOD_ID 独立配置上限
  -> 不同 MOD_ID 独立 delay queue
  -> 不同 MOD_ID 独立发送线程
  -> 哪个 MOD_ID 堵，就清哪个 MOD_ID 的 delay queue
```

关键参考点：

```cpp
m_delayPacket[mod]
m_ilen[mod]
m_iSize[mod]
ThreadProcBufSendAppLive()
ThreadProcBufSendNvrLive()
ThreadProcBufSendRtspLive()
pushNetBuffer(..., MOD_ID mod)
ClearPacket(mod)
AttachNullPacket(...)
```

`pushNetBuffer()` 的思想是：

```text
如果 m_ilen[mod] > m_iSize[mod]
  -> ClearPacket(mod)
  -> 只清当前堵塞模块
  -> 不影响其他模块队列
```

这点应该迁移到 AOV，但 AOV 只保留：

```text
Storage
CloudLive
CloudStorage
```

不迁移：

```text
NVR_LIVE
WEB_LIVE
RTSP_LIVE
RTMP_LIVE
GB28181
```

## 10. 三类消费者默认策略

### 10.1 Storage

定位：

```text
本地 SD 卡录像，优先完整性。
```

默认策略建议：

```cpp
PacketConsumerConfig Storage {
    max_frames = 128 或 256,
    max_bytes = 8 * 1024 * 1024 或 16 * 1024 * 1024,
    drop_non_keyframe_first = false,
    wait_next_keyframe_after_drop = false,
};
```

溢出处理建议：

```text
Storage 队列满
  -> 不静默当作正常丢帧
  -> PacketConsumerStats 标记 dropped/overflow
  -> app/storage 得到写盘链路异常
  -> storage 当前录像片段标记 BrokenSegment 或 Failed
  -> core 根据 storage drain 决定是否等待、失败结束或重新开段
```

原因：

- 本地录像如果静默丢帧，会影响回放索引和文件完整性。
- AOV 事件录像需要解释“成功、失败、损坏片段”。
- storage 是最不应该为了实时性牺牲完整性的消费者。

### 10.2 CloudLive

定位：

```text
手机 App 直播预览，优先实时性。
```

默认策略建议：

```cpp
PacketConsumerConfig CloudLive {
    max_frames = 8,
    max_bytes = 1 * 1024 * 1024,
    drop_non_keyframe_first = true,
    wait_next_keyframe_after_drop = true,
};
```

溢出处理建议：

```text
CloudLive 队列满
  -> 优先丢 P 帧
  -> 标记 waiting_keyframe
  -> 下一个 I 帧到来后恢复
  -> 不影响 Storage / CloudStorage
```

原因：

- 直播预览宁可丢帧，也不能积累越来越高的延迟。
- 丢 P 帧后必须等 I 帧恢复，否则解码器可能花屏。

### 10.3 CloudStorage

定位：

```text
事件云存，完整性优先级高于直播，但不能无限阻塞休眠。
```

默认策略建议：

```cpp
PacketConsumerConfig CloudStorage {
    max_frames = 64,
    max_bytes = 4 * 1024 * 1024,
    drop_non_keyframe_first = true,
    wait_next_keyframe_after_drop = true,
};
```

溢出处理建议：

```text
CloudStorage 队列满
  -> 尽量少丢
  -> 必要时丢 P 帧并等 I 帧恢复
  -> 如果云存片段已不连续，CloudStorage 应明确 failed 或 broken
  -> cloud drain 从 Busy 变为 Failed/Done
  -> core 允许后续回 AOV 休眠
```

原因：

- 云存不能无限 busy，否则设备无法回低功耗。
- 云存失败也算本轮完成，但必须明确失败，不允许状态悬挂。

### 10.4 策略对比

| 消费者 | 目标 | 队列 | 丢帧策略 | 溢出动作 |
| --- | --- | --- | --- | --- |
| `Storage` | 本地录像完整 | 最大 | 默认不主动丢 | 标记 storage overflow，交给 storage/core 处理 |
| `CloudLive` | 实时预览 | 最小 | 丢 P 帧，等 I 帧 | 继续直播，降低延迟 |
| `CloudStorage` | 事件片段完整 | 中等 | 必要时丢 P 帧，等 I 帧 | 明确 failed/broken，结束 cloud drain |

## 11. P6 实施细化

建议新增内部函数：

```cpp
PacketConsumerConfig MakeDefaultConsumerConfig(PacketConsumerId consumer,
                                               const PacketConfig& global);
```

`Subscribe()` 时不再所有消费者使用同一套全局配置，而是：

```text
Storage      -> Storage 默认策略
CloudLive    -> CloudLive 默认策略
CloudStorage -> CloudStorage 默认策略
其他 Debug   -> global 默认策略
```

建议增加配置字段：

```cpp
struct PacketConfig {
    ...
    PacketConsumerConfig storage_default;
    PacketConsumerConfig cloud_live_default;
    PacketConsumerConfig cloud_storage_default;
};
```

或者第一版不暴露到 `PacketConfig`，直接在 `MakeDefaultConsumerConfig()` 写默认值，后续再接配置文件。

验收测试：

```text
1. Storage 默认队列大于 CloudLive。
2. CloudLive 队列满会丢 P 帧并等待 I 帧。
3. Storage 队列满不会被当作普通直播丢帧处理。
4. CloudStorage 溢出后能形成明确 failed/broken 状态输入。
5. CloudLive 溢出不影响 Storage 消费同一帧。
```

## 12. P7 内存恢复通知细化

旧项目对应：

```cpp
AttachNullPacket(...)
DetachNullPacket(...)
on_free_packet()
```

AOV 不建议引入 `CObject/TSignal`，建议改成 C++ callback：

```cpp
using PacketPressureCallback =
    std::function<void(PacketConsumerId consumer, PacketStatusCode status)>;

PacketResult SetPressureCallback(PacketPressureCallback callback);
```

触发时机：

```text
pool exhausted
  -> callback(Unknown, Backpressure)

consumer queue overflow
  -> callback(consumer, Backpressure)

consumer 从 waiting_keyframe 恢复
  -> callback(consumer, Ok)

ReleaseBuffer 或 Dispatch 后 pool 从满到可用
  -> callback(Unknown, Ok)
```

用途：

- libmedia 可根据 backpressure 降低帧率或跳过非关键帧。
- core 可记录 packet 压力。
- cloud/storage 可知道自己是否拖慢链路。

## 13. P8 CloudLive / CloudStorage 分离绑定

当前：

```text
MediaPacketRouter::BindCloudSink()
  -> PacketConsumerId::CloudLive
```

后续建议：

```cpp
PacketResult BindCloudLiveSink(PacketFrameCallback callback);
PacketResult BindCloudStorageSink(PacketFrameCallback callback);
void UnbindCloudLiveSink();
void UnbindCloudStorageSink();
```

保留兼容接口：

```cpp
BindCloudSink() -> BindCloudLiveSink()
```

云端使用方式：

```text
直播打开
  -> BindCloudLiveSink

云存事件开始
  -> BindCloudStorageSink

云存事件结束
  -> UnbindCloudStorageSink
```

这样直播和云存可以使用不同队列、不同背压、不同 drain 语义。

## 14. P9 音频字段细化

当前只预留：

```cpp
PacketMediaType::Audio
```

但真实音频还需要：

```cpp
enum class PacketAudioCodec {
    G711A,
    G711U,
    AAC,
    PCM,
    Unknown,
};

int sample_rate;
int channels;
int bit_width;
```

建议方式：

```cpp
struct PacketFrame {
    PacketMediaType media_type;
    int chn_id;
    VideoCodec video_codec;
    AudioCodec audio_codec;
    ...
};
```

也可以拆分：

```cpp
PacketVideoFrameInfo
PacketAudioFrameInfo
```

短期如果 AOV 只先做视频，可以保持 TODO。

## 15. P10 worker 线程边界

参考代码是每个 `MOD_ID` 一个线程：

```text
ThreadProcBufSendAppLive
ThreadProcBufSendNvrLive
ThreadProcBufSendRtspLive
...
```

AOV 不建议现在把 worker 内建到 packet，原因：

- storage 真实写盘线程模型还没最终实现。
- 腾讯云 SDK 推流线程是否能被 packet 线程直接调用，需要板端验证。
- packet 如果内建线程，会从“媒体总线”变成“线程调度器”，职责变重。

建议边界：

```text
packet:
  pool + queue + backpressure + Dispatch

storage:
  StorageWorker -> Dispatch(Storage)

cloud live:
  CloudLiveWorker -> Dispatch(CloudLive)

cloud storage:
  CloudStorageWorker -> Dispatch(CloudStorage)
```

只有当 storage/cloud 都重复实现类似 worker，且板端证明 packet 内建更合适时，再做：

```cpp
StartConsumerWorker(PacketConsumerId consumer, batch_size, interval_ms);
StopConsumerWorker(PacketConsumerId consumer);
```

默认不启用。

## 16. 需要补充到 app 调试计划的点

板端调试 packet 时必须区分三类消费者：

```text
[packet][storage] queued/dropped/waiting
[packet][cloud_live] queued/dropped/waiting
[packet][cloud_storage] queued/dropped/waiting
```

建议日志：

```text
[packet][I][publish] seq=100 size=1200 key=0 consumers=3
[packet][W][drop] consumer=CloudLive seq=101 reason=backpressure wait_keyframe=1
[packet][E][overflow] consumer=Storage seq=102 action=mark_broken
[packet][I][recover] consumer=CloudLive seq=120 reason=keyframe
```

这能避免调试时只看到 packet dropped，却不知道是直播丢、云存丢，还是本地录像出问题。

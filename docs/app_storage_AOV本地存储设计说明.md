# app/storage AOV 本地存储设计说明

本文用于指导 `app/storage` 后续实现。当前 AOV 项目的存储逻辑不能直接照搬旧项目连续录像方案，需要在参考旧项目 PS/PES 与 DHFS 能力的基础上，重新设计 AOV 录像会话、时间轴、检索和告警关联。

## 参考代码和依赖

旧项目参考路径：

```text
Z:\cp3prov1.2\Apps\Kylin\comps\Storage
  PSAnalysis.cpp/.h          # PS/PES 解析，主要用于回放或 Web 侧解析
  PSEncapsulation.cpp/.h     # ES -> PS/PES 封装
  SDStoragePublic.cpp/.h     # PS/PES 公共结构、编码类型转换

Z:\cp3prov1.2\Depend\Includes\DHFS
  dhfs.h                     # DHFS 文件系统主接口、索引、检索、报警接口
  adapt.h                    # DHFS 标准文件 API 适配层

Z:\cp3prov1.2\Depend\Libs\Platform\AX520CE
  libDHFS.a                  # DHFS 静态库
```

AOV 项目建议放置方式：

```text
Depend/third_party/include/DHFS/
  dhfs.h
  adapt.h

Depend/third_party/lib/
  libDHFS.a
```

注意：不要沿用 `AX520CE` 作为目录名。对 AOV 项目来说，`libDHFS.a` 是第三方文件系统库，不应该绑定旧平台目录命名。

## 当前 app/storage 职责

`app/storage` 只负责本地 SD 卡录像业务链路：

```text
PacketFrame
  -> AOV 录像会话判断
  -> PS/PES 封装
  -> DHFS 写盘
  -> 文件 close + flush + fsync
  -> 录像检索 / 告警索引 / 状态反馈
```

不属于 `app/storage` 的职责：

```text
SD 卡在线检测、挂载、卸载、上电、断电  -> libsys/SD
裸码流生产                               -> libmedia
裸码流分发                               -> app/packet
云存、直播、云端回放                      -> app/cloud
是否回 AOV 休眠                           -> app/core
```

`app/storage` 必须向 `app/core` 反馈 drain 状态：本地录像是否已经 close、flush、fsync 完成。只有本地存储完成后，`app/core` 才能允许系统回 AOV 休眠。

## 与旧项目的关键差异

旧项目更偏连续录像：

```text
编码持续出帧
  -> 连续 PS/PES 封装
  -> 连续写 DHFS 文件
  -> 文件时间轴天然连续
```

AOV 项目是间歇唤醒录像：

```text
定时唤醒：例如 1 秒 1 帧、3 秒 1 帧，可配置
事件触发：检测确认后进入常电录像
App 主动唤醒：手机 App 打开预览/控制期间设备被唤醒
```

因此不能简单按“收到帧就写连续文件”处理。AOV 必须区分录像来源，并明确真实墙上时间与编码 PTS 的关系。

## 推荐模块拆分

建议 `app/storage` 后续拆成以下子模块：

```text
app/storage/
  storage_service              # 对 app/core 暴露的业务服务
  storage_session_manager      # AOV 录像会话管理
  storage_query_service        # 本地录像检索、报警检索
  storage_packet_sink          # PacketFrame 入口
  ps/
    ps_pes_muxer               # PacketFrame -> PS/PES buffer
    ps_pes_types               # PS/PES 结构、常量、类型转换
  dhfs/
    dhfs_writer                # 真实 DHFS 写盘
    dhfs_query                 # DHFS 文件列表/报警列表查询
    dhfs_time                  # DHTIME 和系统时间转换
    dhfs_types                 # DHFS wrapper 类型
```

当前已有 stub：

```text
app/storage/i_storage_service.hpp
app/storage/storage_service_stub.*
app/storage/dhfs/i_dhfs_writer.hpp
app/storage/dhfs/dhfs_writer_stub.*
```

后续真实实现建议新增真实类，不要在 stub 上堆业务：

```text
DhfsWriter        # 真实写盘实现
PsPesMuxer        # 真实 PS/PES 封装
StorageSessionManager
StorageQueryService
```

## 核心数据结构建议

### 录像触发类型

```cpp
enum class RecordTriggerType {
    PeriodicWakeup,   // 定时唤醒，例如 1 秒 1 帧
    AlarmEvent,       // 检测确认后的事件录像
    AppWakeup,        // 手机 App 主动唤醒
    Manual,           // 手动录像/一键报警
};
```

### 录像会话

```cpp
struct RecordSession {
    std::string session_id;
    int channel {0};
    RecordTriggerType trigger_type {RecordTriggerType::PeriodicWakeup};

    uint64_t start_wall_time_ms {0};
    uint64_t end_wall_time_ms {0};
    uint64_t first_frame_pts_ms {0};
    uint64_t last_frame_pts_ms {0};

    uint32_t configured_interval_ms {1000};
    uint32_t frame_count {0};

    std::string alarm_id;
    uint32_t alarm_type {0};

    bool require_read_only {false};
    bool cloud_synced {false};
};
```

解释：

- `wall_time_ms` 用于文件索引、检索、回放时间定位。
- `pts_ms` 只用于 PS/PES 内部媒体时间戳。
- 定时唤醒 1 秒 1 帧时，`configured_interval_ms` 应记录真实采样间隔。
- 事件录像和 App 唤醒录像可以是连续帧，`configured_interval_ms` 可按实际帧率推导。

### 写盘输入

```cpp
struct StorageWriteFrame {
    RecordSession session;
    aov::app::packet::PacketFrame packet_frame;
    uint64_t wall_time_ms {0};
};
```

`PacketFrame` 来自 `app/packet`，不带私有帧头，也不做剥头动作。PS/PES 封装只在本地 SD 存储链路发生。

## PS/PES 封装策略

旧项目 `PSEncapsulation` 可作为封装格式参考。其核心逻辑：

```text
I 帧：
  PS Header
  SYS Header
  SYS MAP Header
  PES Header
  ES Payload

P 帧：
  PS Header
  PES Header
  ES Payload

音频：
  PES Header
  ES Payload
```

关键常量来自 `SDStoragePublic.h`：

```text
PS_HEADER_LEN       = 14
PS_SYS_HEADER_LEN   = 18
PS_SYS_MAP_HEADER_LEN = 24
PES_HEADER_LEN      = 40

PES_HEAD_STREAM_TYPE_VIDEO = 0xe0
PES_HEAD_STREAM_TYPE_AUDIO = 0xc0

PSMUX_ST_VIDEO_H264 = 0x1b
PSMUX_ST_VIDEO_H265 = 0x24
```

迁移建议：

1. 不直接包含旧项目 `SDStoragePublic.h`，因为它依赖旧项目 `ConfigManager_new.h`、`Capture.h`、`Object.h`。
2. 把必要常量、枚举和转换逻辑整理到 AOV 自己的 `ps_pes_types.hpp`。
3. `PsPesMuxer` 输入 `PacketFrame`，输出一组 buffer：

```cpp
struct PsPesPacket {
    std::vector<uint8_t> header;
    std::shared_ptr<const std::vector<uint8_t>> payload;
    uint64_t wall_time_ms {0};
    uint32_t sec_num {0};
    bool change_file {false};
};
```

4. 封装时的 PTS 可以使用“会话内相对时间”：

```text
relative_pts_ms = wall_time_ms - session.start_wall_time_ms
```

这样可以避免 AOV 间歇帧导致 PS/PES 内部时间戳巨大跳变。

## AOV 时间轴策略

需要区分两个时间：

```text
墙上时间 wall time：
  真实年月日时分秒，用于 DHFS 文件 start/end time、录像检索、报警时间。

媒体 PTS：
  PS/PES 内部媒体时间戳，用于播放器解码排序。
```

推荐策略：

1. DHFS 文件索引使用真实墙上时间。
2. PS/PES PTS 使用会话内相对时间。
3. 定时唤醒 1 秒 1 帧，不要伪造成 25fps 连续录像。
4. 检索结果要返回真实时间段，而不是仅返回文件内 PTS。
5. 如果播放器不支持稀疏帧，需要在回放层做“按实际时间显示”或“按文件内 PTS 快速播放”的产品策略选择。

待确认点：

```text
定时唤醒 1 秒 1 帧生成的视频，App 回放时是按真实时间播放，还是快速播放？
例如 60 秒内只有 60 帧：
  方案 A：按真实时间播放 60 秒
  方案 B：按 15/25fps 快速播放几秒
```

这会影响 PS/PES PTS 和 DHFS 写入的 `sec_num` 策略。

## DHFS 关键接口映射

头文件：`Z:\cp3prov1.2\Depend\Includes\DHFS\dhfs.h`

### 初始化和挂载

```c
int  dh_fs_init();
int  dh_fs_mount(uint disk_no, uint part);
int  dh_fs_umount(uint disk_no, uint part);
int  dh_file_sys_init(uint *pwork_num, DHTIME *shutdown_time);
uint dh_get_fs_info(pSYS_USE_INFO pinfo);
uint dh_insert_storage_device();
uint dh_remove_storage_device();
uint dh_free_mem();
```

建议边界：

- `libsys/SD` 负责 SD 在线、挂载、卸载、上电、断电。
- `app/storage/dhfs` 只在 SD 已挂载后初始化 DHFS 业务。
- AOV 低功耗下不要频繁 mount/umount。只有 SD 热插拔、格式化、异常恢复才做挂载状态变化。

### 创建、写入、关闭

```c
uint dh_file_create(
    ushort channel,
    uchar video,
    uchar alarm,
    uchar image,
    DHTIME time,
    uint redu,
    uint driver_type,
    DHFILE *p_handle);

uint dh_file_write(
    DHFILE handle,
    uchar *pbuf,
    uint sec_num,
    uint changeFile,
    DHTIME* time);

uint dh_file_multi_write(
    DHFILE handle,
    FS_IOVEC* iov,
    uint iov_num,
    uint sec_num,
    DHTIME* time);

uint dh_file_close(DHFILE handle, DHTIME* time);
uint dh_file_close_ex(DHFILE handle, DHTIME end_time);
uint dh_file_close_all();
uint dh_file_break(DHFILE handle);
```

建议：

- 单帧写入可以先用 `dh_file_write`。
- 后续优化可以用 `dh_file_multi_write`，把 PS/PES header 和 ES payload 用 iovec 聚合写，减少一次内存拷贝。
- `sec_num` 和 `time` 的含义需要板端验证。AOV 间歇帧不能假设每帧 `sec_num=1` 就一定正确。
- `changeFile` 应由 `StorageSessionManager` 根据分段策略、触发类型变化、I 帧边界决定。

### 文件检索和定位

```c
uint dh_file_list(
    ushort channel,
    DHTIME start_time,
    DHTIME end_time,
    uint *pmax_ret_num,
    pFILE_INFO pfile_info,
    uchar type,
    uchar hint,
    uint driver_type);

uint dh_file_goto(DHFILE handle, ushort channel, DHTIME time, uchar type, uint driver_type);
uint dh_file_moveto_time(DHFILE handle, DHTIME time);
uint dh_get_file_info2(DHFILE handle, DHTIME *pstart_time, DHTIME *pend_time, uint *plen);
uint dh_get_cur_time(DHFILE handle, DHTIME *pstart_time, uint *ms, uint *curr_offset);
```

建议：

- 普通时间轴回放走 `dh_file_list + dh_file_goto/dh_file_moveto_time`。
- `FILE_INFO.start_time/end_time` 应以真实墙上时间为准。
- App 查询某个时间段录像时，storage 返回的是 AOV 录像片段列表，不要假定时间段内每秒都有连续视频。

### 只读文件

```c
uint dh_file_set_read_only(ushort channel, DHTIME start_time, DHTIME end_time, uint *pmax_ret_num, uchar type, uchar hint, uint driver_type);
uint dh_file_set_normal(ushort channel, DHTIME start_time, DHTIME end_time, uint *pmax_ret_num);
uint dh_get_read_only_file_by_time(ushort channel, DHTIME time, pINDEX_FILE_INFO pIndex_file_info);
uint dh_get_read_only_file_list(ushort channel, DHTIME start_time, DHTIME end_time, uint *pmax_ret_num, pINDEX_FILE_INFO pIndex_file_info);
uint dh_goto_read_only_file(DHFILE handle, pINDEX_FILE_INFO pIndex_file_info);
```

建议：

- 事件录像可以考虑设置为只读，避免循环覆盖过早删除。
- 普通定时唤醒录像可作为普通文件，遵循循环覆盖。
- 只读策略需要和产品保存时长、SD 容量策略一起确定。

### 报警索引

```c
uint dh_write_alarm(DHFILE handle, uchar flag, uchar alarm_type, DHTIME alarm_time, uchar *pbuf, uint data_length);
uint dh_write_alarm_gather(DHFILE handle, uchar flag, uchar alarm_type, DHTIME alarm_time, FS_IOVEC* iov, uint iov_num);
uint dh_get_alarm(ushort channel, DHTIME time, uchar alarm_type, pALARM_INDEX_INFO palarm_index_info, uint driver_type);
uint dh_get_alarm_list(ushort channel, uchar alarm_type, DHTIME start_time, DHTIME end_time, uint *pmax_ret_num, pALARM_INDEX_INFO palarm_index_info, uint driver_type);
uint dh_get_alarm_by_id(uint alarm_id, uint driver_no, pALARM_INDEX_INFO palarm_index_info);
uint dh_get_related_alarm(DHFILE handle, uint *pmax_ret_num, pALARM_INDEX_INFO palarm_index_info);
uint dh_goto_alarm_video_file(DHFILE handle, uint second, pALARM_INDEX_INFO palarm_index_info);
```

建议：

- 检测确认后的事件录像应写报警索引。
- `alarm_time` 使用事件确认时间，而不是第一帧 PTS。
- 报警附加数据可以存 detection summary、目标类型、置信度、截图路径、云端 alarm id 等。
- App 事件回放优先走 `dh_get_alarm_list` 查询事件，再用 `dh_goto_alarm_video_file` 定位关联视频。

## 文件类型映射建议

DHFS 中常见文件类型：

```text
F_COMMON  = 普通录像
F_ALERT   = 外部报警
F_DYNAMIC = 动态检测
F_HAND    = 手动录像
F_JPG     = 图片
```

AOV 映射建议：

| AOV 触发来源 | DHFS 文件类型 | 是否报警索引 | 是否只读 |
| --- | --- | --- | --- |
| 定时唤醒 | `F_COMMON` | 否 | 否 |
| 检测事件 | `F_DYNAMIC` 或 `F_ALERT` | 是 | 可选 |
| App 主动唤醒 | `F_HAND` 或 `F_COMMON` | 否 | 否 |
| 一键报警 | `F_ALERT` 或 `F_HAND` | 是 | 可选 |

最终使用 `F_DYNAMIC` 还是 `F_ALERT`，需要结合 App 端筛选条件和旧项目回放兼容性确认。

## StorageService 对 app/core 的接口建议

当前接口：

```cpp
class IStorageService {
public:
    virtual StorageResult Init(const StorageConfig& config) = 0;
    virtual void Deinit() = 0;
    virtual StorageResult StartRecord() = 0;
    virtual StorageResult StopRecord() = 0;
    virtual StorageResult OnPacketFrame(const PacketFrame& frame) = 0;
    virtual StorageResult FlushAndClose() = 0;
    virtual StorageStats GetStats() const = 0;
};
```

后续真实实现建议扩展为：

```cpp
struct StartRecordRequest {
    RecordTriggerType trigger_type;
    uint64_t start_wall_time_ms;
    uint32_t configured_interval_ms;
    std::string alarm_id;
    uint32_t alarm_type;
};

struct StopRecordRequest {
    uint64_t end_wall_time_ms;
    bool force_flush {true};
};

class IStorageService {
public:
    virtual StorageResult StartRecord(const StartRecordRequest& request) = 0;
    virtual StorageResult StopRecord(const StopRecordRequest& request) = 0;
    virtual StorageResult OnPacketFrame(const PacketFrame& frame) = 0;
    virtual StorageResult FlushAndClose() = 0;
    virtual StorageStats GetStats() const = 0;
};
```

原因：

- 不同触发来源影响文件类型、只读策略、报警索引、分段策略。
- `StartRecord()` 无参数无法表达 AOV 业务来源。
- `StopRecord()` 需要明确 end wall time，保证 DHFS 文件结束时间准确。

## 写盘流程建议

### 定时唤醒录像

```text
app/core 定时唤醒
  -> libmedia 恢复常电/低帧率取一帧或短时间编码
  -> app/storage StartRecord(PeriodicWakeup)
  -> PacketFrame 到达
  -> PS/PES 封装
  -> DHFS 写入
  -> StopRecord(end_wall_time)
  -> FlushAndClose
  -> app/core drain 完成
  -> 回 AOV 休眠
```

### 事件录像

```text
唤醒后智能检测确认目标
  -> app/alarm 生成 AlarmEvent
  -> app/core 决定进入事件录像
  -> app/storage StartRecord(AlarmEvent)
  -> PacketFrame 连续写入
  -> DHFS 写普通视频文件
  -> dh_write_alarm 写报警索引
  -> 事件结束或超时
  -> StopRecord + FlushAndClose
  -> app/core 等 storage/cloud drain 都完成
  -> idle debounce 后回 AOV 休眠
```

### App 主动唤醒录像

```text
手机 App 打开预览/控制
  -> app/core 进入 AwakeNormal
  -> 如配置需要本地录像，StartRecord(AppWakeup)
  -> PacketFrame 写盘
  -> App 退出后进入 idle debounce
  -> StopRecord + FlushAndClose
  -> app/core 判断 drain 完成后休眠
```

## 检索接口建议

建议 storage 对上层提供两个查询：

```cpp
struct RecordQueryRequest {
    int channel;
    uint64_t start_wall_time_ms;
    uint64_t end_wall_time_ms;
    RecordTriggerType filter;
};

struct RecordSegmentInfo {
    std::string session_id;
    int channel;
    RecordTriggerType trigger_type;
    uint64_t start_wall_time_ms;
    uint64_t end_wall_time_ms;
    uint32_t duration_sec;
    bool has_alarm;
    std::string alarm_id;
};

struct AlarmQueryRequest {
    int channel;
    uint64_t start_wall_time_ms;
    uint64_t end_wall_time_ms;
    uint32_t alarm_type;
};
```

普通时间轴：

```text
RecordQueryRequest
  -> dh_file_list
  -> FILE_INFO -> RecordSegmentInfo
```

事件列表：

```text
AlarmQueryRequest
  -> dh_get_alarm_list
  -> ALARM_INDEX_INFO -> AlarmSegmentInfo
```

## 实施顺序建议

1. 新增 AOV storage 结构体和接口，不接 DHFS。
2. 实现 `PsPesMuxer`，只覆盖 H264/H265 视频帧。
3. 用 stub writer 验证 `PacketFrame -> PsPesMuxer -> writer`。
4. 新增 `DhfsTime`，完成 `uint64_t wall_time_ms <-> DHTIME` 转换。
5. 接 `DhfsWriter`：
   - `dh_fs_init`
   - `dh_file_create`
   - `dh_file_write` 或 `dh_file_multi_write`
   - `dh_file_close_ex`
6. 接普通录像检索：
   - `dh_file_list`
   - `dh_file_goto`
   - `dh_file_moveto_time`
7. 接报警索引：
   - `dh_write_alarm`
   - `dh_get_alarm_list`
   - `dh_goto_alarm_video_file`
8. 板端验证 AOV 间歇录像：
   - 1 秒 1 帧
   - 3 秒 1 帧
   - 事件连续录像
   - App 主动唤醒录像
   - 断电/休眠前 close + flush + fsync

## 关键风险

1. `dh_file_write` 的 `sec_num` 在间歇录像场景下语义不明确，必须板端验证。
2. PS/PES PTS 如果使用真实墙上时间，可能导致播放器认为中间有巨大空洞。
3. 如果把 1 秒 1 帧伪造成 25fps，回放时间和真实事件时间会不一致。
4. DHFS 的 `DHTIME` 和系统时间转换必须统一时区，否则 App 检索会偏移。
5. 事件录像如果不写报警索引，App 只能按时间段找，无法按事件列表检索。
6. AOV 休眠前必须等待本地录像 close + flush + fsync 完成，否则文件索引可能损坏。

## 结论

`app/storage` 不建议直接搬旧项目 `Storage`。正确做法是：

```text
复用旧项目 PS/PES 格式经验
复用 DHFS 文件系统能力
重写 AOV 录像会话管理
重写 AOV 时间轴和检索策略
```

本模块实现完成后，`app/core` 只关心：

```text
StartRecord(trigger)
StopRecord(end_time)
FlushAndClose()
GetStats().recording/flushed
```

具体 PS/PES、DHFS、文件索引和报警索引全部收口在 `app/storage` 内部。

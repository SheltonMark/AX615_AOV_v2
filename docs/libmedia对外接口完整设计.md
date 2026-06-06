# libmedia 对外接口完整设计

本文定义 `libmedia` 面向 `app/core`、`app/packet` 的稳定 C++ 对外头文件、结构体和服务接口。文档基于当前 AOV 架构、`CS6GV2.0.xlsx` 需求配置项、已经跑通的 AX615 QSDemo 媒体链路，以及当前 `libmedia/Include`、`libmedia/Src` 代码现状。

目标是让 `libmedia` 先独立收口：可以单独编出 `libmedia.a`，对外只暴露媒体能力，不暴露 AX SDK 类型，也不依赖 `app/config`、`app/cloud`、`app/storage`、`app/packet` 的业务类型。

## 1. 设计结论

### 1.1 libmedia 的职责

`libmedia` 负责 AX615 媒体链路本身：

```text
SYS / Pool
  -> VIN / Sensor / MIPI
  -> ISP / IQ / 3A
  -> IVPS
  -> VENC / JENC
  -> AUDIO capture / encode
  -> Detect / OSD
  -> AOV 低功耗媒体参数切换
```

`libmedia` 对外输出的是编码后的裸码流帧：H264、H265、JPEG、G711A、G711U、AAC、PCM 等。这里的“裸码流”是指不添加项目私有帧头/尾，也不做 PS/PES、MP4、云 SDK 业务封装。

### 1.2 libmedia 不负责的事情

`libmedia` 不负责：

- 不保存产品配置，不包含 `app/config` 头文件。
- 不解析腾讯云物模型，不包含云端属性名。
- 不做 Packet 缓存、引用计数、背压和跨线程分发。
- 不加私有帧头/尾。
- 不做 SD 卡 PS/PES 封装和 DHFS 写盘。
- 不做云存 SDK 封装、直播推流协议封装。
- 不决定是否回 AOV 休眠，只提供休眠前媒体侧收口、唤醒后媒体侧恢复、唤醒期间低功耗取帧/常电恢复能力。

### 1.4 AOV 休眠与唤醒语义

当前项目里的“AOV 休眠”必须拆成两个状态，不能简单等同于 `libmedia::Stop()` 或“低帧率持续编码”。

```text
AOV 真正休眠态：
  - Linux 主业务基本休眠。
  - sensor 没有正常图像流进入媒体链。
  - VENC/JENC/AUDIO 不持续编码。
  - 不保存码流。
  - 只保留 PIR / RTC / TCM / 低功耗硬件唤醒配置。

AOV 唤醒态：
  - 设备被 RTC 定时、App、PIR/TCM/事件等原因唤醒。
  - libmedia 恢复必要媒体链。
  - 只要处于唤醒状态，app/storage 可以保存码流。
  - 定时唤醒按配置取 1 帧或 N 帧。
  - App 唤醒、事件唤醒可恢复常电帧率并持续编码。
```

因此 `libmedia` 需要表达三类动作：

```text
PrepareForAovSleep:
  休眠前媒体侧收口，停止普通 detect 线程和不必要编码链路，配置低功耗唤醒能力。

RestoreFromAovSleep:
  唤醒后媒体侧恢复，根据唤醒原因选择 timer/app/event profile，恢复编码和必要检测。

EnterLowPowerMode / ResumeNormalMode:
  设备已经唤醒时的媒体 profile 切换。前者用于定时唤醒短取帧或低帧率录像，后者用于 App/事件唤醒后的常电录像/预览。
```

Detect 也分两类：

```text
休眠中的唤醒检测：
  由 PIR / TCM / 低功耗硬件完成，普通 libmedia detect 线程不运行。

唤醒后的智能检测确认：
  libmedia 恢复媒体链后启动 detect service/thread，从 IVPS/VIN 帧做智能确认，输出 DetectResultSummary。
```

### 1.3 调用边界

```text
app/cloud / App 命令 / 本地持久化配置
        |
        v
app/config: 保存 DesiredConfig / ActiveConfig
        |
        v
app/core: 状态机、策略、配置生效时机
        |
        | 只转换成 libmedia DTO，不把 app/config 类型传进 libmedia
        v
libmedia public Include
        |
        v
libmedia Src service 层: 无 AX SDK 类型
        |
        v
libmedia Src/adapter/ax615: 唯一允许包含 AX_* SDK 头文件的位置
```

`app/core` 是产品配置和媒体能力之间的翻译层。比如需求里的“主码流 H265 2560x1440、VBR、1024kbps、AOV 1fps”属于 app 配置；真正调用 `libmedia` 时转换成 `VideoEncodeApplyConfig`。

## 2. 最终 Include 目录

不保留旧的平铺头文件。最终只允许 app include 以下模块化头文件：

```text
libmedia/Include/
  common/
    media_status.hpp
    media_types.hpp
    media_config.hpp
  runtime/
    i_media_runtime.hpp
  video/
    i_video_stream_service.hpp
  audio/
    i_audio_stream_service.hpp
  isp/
    i_isp_service.hpp
  osd/
    i_osd_service.hpp
  detect/
    i_detect_service.hpp
```

推荐 app 侧 include：

```cpp
#include "runtime/i_media_runtime.hpp"
#include "video/i_video_stream_service.hpp"
#include "audio/i_audio_stream_service.hpp"
#include "isp/i_isp_service.hpp"
#include "detect/i_detect_service.hpp"
#include "osd/i_osd_service.hpp"
#include "common/media_config.hpp"
#include "common/media_types.hpp"
```

## 3. 当前已有与主要缺口

当前 `libmedia/Include` 已经完成分模块目录，已有 `IVideoStreamService`、`IAudioStreamService`、`IIspService`、`IDetectService`、`IOsdService`、`IMediaRuntime` 的骨架。

主要缺口：

| 模块 | 当前情况 | 需要补齐 |
| --- | --- | --- |
| common | 已有基础枚举、帧结构、Sensor/Pool 配置 | 默认值要贴合 CS6G AOV 需求；补 StreamProfile、帧类型、错误信息、时间戳策略 |
| video | 已有 `ApplyVideoEncodeConfig`、码流回调 | 补主/子码流 profile、AOV/常电帧率、IDR、码率模式、分辨率约束 |
| audio | 已有音频配置和回调 | 默认值改 G711A/8k/16bit；补播放/对讲使能边界 |
| isp | 已有 WB/AE/Scene/LDC/Sharpen/WBC 骨架 | 按需求一次性补 brightness、contrast、saturation、hue、mirror、flip、日夜模式、补光、抗闪烁、慢快门等 |
| detect | 只有运行接口和结果摘要 | 补检测配置接口、目标类型、区域、灵敏度、布防由 app 过滤后的媒体侧参数 |
| osd | 只有更新时间/状态图标 | 补时间、星期、名称、Logo、通道名、目标框开关等需求项 |
| runtime | 已有 Init/Start/Stop/低功耗切换 | 补 AOV sleep/restore、runtime status、整体 Apply 配置入口、Snapshot、服务生命周期约束 |

## 4. common/media_status.hpp

```cpp
#pragma once

#include <string>

namespace aov::media {

enum class MediaStatusCode {
    Ok,
    InvalidArgument,
    InvalidState,
    Busy,
    Timeout,
    Unsupported,
    NoResource,
    NotFound,
    PermissionDenied,
    SdkError,
    InternalError,
};

struct MediaStatus {
    MediaStatusCode code {MediaStatusCode::Ok};
    int sdk_error {0};
    std::string message;

    bool ok() const { return code == MediaStatusCode::Ok; }
};

}  // namespace aov::media
```

说明：

- 对外接口可以继续返回 `MediaStatusCode`，但建议新增 `MediaStatus` 用于需要保留 SDK 错误码和文本的场景。
- `sdk_error` 只保存数值，不暴露 AX SDK 类型。

## 5. common/media_types.hpp

### 5.1 基础类型

```cpp
#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace aov::media {

struct Resolution {
    int width {0};
    int height {0};
};

enum class PixelFormat {
    NV12,
    YUV420P,
    YUYV,
    RGB888,
    Unknown,
};

enum class StreamProfile {
    Main,
    Sub,
    Snapshot,
    Unknown,
};

enum class VideoCodec {
    H264,
    H265,
    JPEG,
};

enum class AudioCodec {
    G711A,
    G711U,
    AAC,
    PCM,
};

enum class RcMode {
    CBR,
    VBR,
    FIXQP,
};

enum class FrameRateMode {
    Fixed,
    Interval,
};

struct FrameRateConfig {
    FrameRateMode mode {FrameRateMode::Fixed};
    int fps {15};
    int interval_sec {1};
};

}  // namespace aov::media
```

### 5.2 Sensor 与 Pool

```cpp
namespace aov::media {

enum class SensorId {
    OS04D10,
    OS04A10,
    SC4336,
    IMX415,
    Unknown,
};

struct SensorConfig {
    SensorId sensor_id {SensorId::OS04D10};
    int pipe_id {0};
    int dev_id {0};
    Resolution resolution {2688, 1520};
    int fps {25};
    std::string sensor_lib_path;
    std::string iq_bin_path {"/etc/iqfiles"};
};

enum class PoolCacheMode {
    NonCache,
    Cached,
};

struct CmPoolBlockConfig {
    uint64_t blk_size {0};
    uint32_t blk_count {0};
    uint64_t meta_size {512};
    PoolCacheMode cache_mode {PoolCacheMode::NonCache};
};

struct CmPoolConfig {
    std::vector<CmPoolBlockConfig> pools;
    static CmPoolConfig DefaultAx615();
};

}  // namespace aov::media
```

`DefaultAx615()` 的内部实现从 QSDemo 的 `COMMON_SYS_Init` / pool 配置迁移，不在头文件里暴露 AX pool 结构。

### 5.3 裸码流帧

```cpp
namespace aov::media {

enum class VideoFrameType {
    I,
    P,
    B,
    JPEG,
    Unknown,
};

struct StreamFrame {
    StreamProfile profile {StreamProfile::Main};
    int chn_id {0};
    VideoCodec codec {VideoCodec::H265};
    VideoFrameType frame_type {VideoFrameType::Unknown};
    const uint8_t* data {nullptr};
    uint32_t size {0};
    uint64_t pts_ms {0};
    uint64_t sequence {0};
    bool is_keyframe {false};
};

using StreamFrameCallback = std::function<void(const StreamFrame&)>;

struct AudioFrame {
    int chn_id {0};
    AudioCodec codec {AudioCodec::G711A};
    const uint8_t* data {nullptr};
    uint32_t size {0};
    uint64_t pts_ms {0};
    uint64_t sequence {0};
    int sample_rate_hz {8000};
    int bit_depth {16};
    int channels {1};
};

using AudioFrameCallback = std::function<void(const AudioFrame&)>;

}  // namespace aov::media
```

约束：

- `StreamFrame::data` 指向 VENC/JENC 输出的原始编码数据，不含私有帧头/尾。
- `StreamFrame::data` 默认只在回调同步期间有效；回调返回后 `libmedia` 可以立即 `ReleaseStream`。`app/packet` 如果异步分发，必须在回调内复制数据或接管可引用 buffer。
- `pts_ms` 允许 AOV 间隔录像场景不连续；连续性由后续 `app/storage` 的 PS/PES 适配层处理，不在 `libmedia` 内补时间戳。

### 5.4 检测结果与 OSD 状态

```cpp
namespace aov::media {

struct Rect {
    int x {0};
    int y {0};
    int width {0};
    int height {0};
};

enum class DetectTargetType {
    Human,
    Vehicle,
    NonMotorVehicle,
    Motion,
    Occlusion,
};

struct DetectObject {
    DetectTargetType type {DetectTargetType::Human};
    Rect box;
    int confidence {0};
};

struct DetectResultSummary {
    bool has_human {false};
    bool has_vehicle {false};
    bool has_non_motor_vehicle {false};
    bool has_motion {false};
    bool has_occlusion {false};
    int object_count {0};
    uint64_t frame_id {0};
    uint64_t timestamp_ms {0};
    std::vector<DetectObject> objects;
};

struct OsdStatusFlags {
    bool recording {false};
    bool low_battery {false};
    bool cloud_connected {false};
    bool sd_ready {false};
    bool aov_low_power {false};
};

}  // namespace aov::media
```

## 6. common/media_config.hpp

### 6.1 视频编码配置

```cpp
namespace aov::media {

struct VideoEncodeApplyConfig {
    StreamProfile profile {StreamProfile::Main};
    int chn_id {0};
    VideoCodec codec {VideoCodec::H265};
    Resolution resolution {2560, 1440};
    int fps_normal {15};       // 常电、告警、App 唤醒时帧率，需求默认 15fps
    int fps_aov {1};           // AOV 休眠录像帧率，需求可选 1/2/3fps，默认 1fps
    RcMode rc_mode {RcMode::VBR};
    int bitrate_kbps {1024};
    int gop {60};
    int fix_qp {35};
    bool enabled {true};
};

struct VideoEncodeGroupConfig {
    VideoEncodeApplyConfig main_stream;
    VideoEncodeApplyConfig sub_stream;
};

}  // namespace aov::media
```

默认建议：

- 主码流：H265、2560x1440、VBR、1024kbps、GOP 60、常电 15fps、AOV 1fps。
- 子码流：H265、768x432、VBR、256kbps、GOP 60、常电 15fps、AOV 1fps。
- 真实支持的分辨率和码率范围由 `app/config` 校验；`libmedia` 做基础合法性检查和 SDK 返回错误透传。

### 6.2 音频配置

```cpp
namespace aov::media {

struct AudioEncodeApplyConfig {
    bool enabled {true};
    int capture_dev_id {0};
    int play_dev_id {0};
    int enc_chn_id {0};
    AudioCodec codec {AudioCodec::G711A};
    int sample_rate_hz {8000};
    int bit_depth {16};
    int channels {1};
    int input_volume {100};
    int output_volume {100};
    bool enable_talkback_play {false};
};

}  // namespace aov::media
```

说明：

- 需求默认音频为 G711A、8kHz、16bit、单声道、音量 100。
- 对讲播放是音频输出能力，但是否开启对讲由 `app/core/app/cloud` 决定。

### 6.3 ISP / 图像配置

```cpp
namespace aov::media {

enum class ImageAutoMode {
    Auto,
    Manual,
};

enum class DayNightMode {
    Auto,
    Day,
    Night,
};

enum class IrCutMode {
    Auto,
    Color,
    BlackWhite,
};

enum class AntiFlickerMode {
    Off,
    Hz50,
    Hz60,
};

struct WhiteBalanceConfig {
    int pipe_id {0};
    ImageAutoMode mode {ImageAutoMode::Auto};
    int r_gain {0};
    int g_gain {0};
    int b_gain {0};
};

struct ExposureConfig {
    int pipe_id {0};
    ImageAutoMode mode {ImageAutoMode::Auto};
    int compensation {0};
    bool slow_shutter_enabled {false};
    int max_again {0};
    int max_dgain {0};
    int min_shutter_us {0};
    int max_shutter_us {0};
};

struct SceneConfig {
    int pipe_id {0};
    DayNightMode day_night_mode {DayNightMode::Auto};
    IrCutMode ir_cut_mode {IrCutMode::Auto};
    AntiFlickerMode anti_flicker {AntiFlickerMode::Hz50};
    bool white_light_enabled {false};
    int white_light_strength {0};
    bool ir_light_enabled {false};
    int ir_light_strength {0};
};

struct LdcConfig {
    int pipe_id {0};
    bool enabled {false};
    int strength {0};
};

struct SharpenConfig {
    int pipe_id {0};
    int strength {50};
};

struct WbcConfig {
    int pipe_id {0};
    bool enabled {false};
    int r_gain {0};
    int gr_gain {0};
    int gb_gain {0};
    int b_gain {0};
};

struct IspImageConfig {
    int pipe_id {0};
    int brightness {50};
    int contrast {50};
    int saturation {50};
    int hue {50};
    bool mirror {false};
    bool flip {false};
    WhiteBalanceConfig white_balance;
    ExposureConfig exposure;
    SceneConfig scene;
    LdcConfig ldc;
    SharpenConfig sharpen;
    WbcConfig wbc;
};

}  // namespace aov::media
```

实现边界：

- `Src/isp/isp_service.*` 是无 SDK 的服务层，负责参数校验、缓存当前图像配置、调用 adapter。
- `Src/adapter/ax615/isp/ax_isp_adapter.*` 才调用 `AX_ISP_IQ_Get* / AX_ISP_IQ_Set*`。
- 白平衡、AE、LDC、锐化、WBC 等需求参数先在 public DTO 中定义齐，后续每个 SDK 映射可以分批实现。

### 6.4 检测配置

```cpp
namespace aov::media {

struct DetectRegion {
    bool full_screen {true};
    std::vector<Rect> rects;
    std::vector<int> motion_cells;  // motion 网格配置，具体网格解释由 app/config 保持
};

struct DetectRuleApplyConfig {
    DetectTargetType type {DetectTargetType::Human};
    bool enabled {false};
    int pipe_id {0};
    int ivps_grp_id {0};
    int ivps_chn_id {0};
    DetectRegion region;
    int sensitivity {60};
    bool static_filter {false};
};

struct DetectApplyConfig {
    bool enabled {false};
    int pipe_id {0};
    int ivps_grp_id {0};
    int ivps_chn_id {0};
    std::vector<DetectRuleApplyConfig> rules;
};

}  // namespace aov::media
```

说明：

- 布防时间、报警去抖、录像联动、推送联动属于 `app/alarm` 和 `app/core`，不进入 `libmedia`。
- `libmedia/detect` 只关心“当前要检测什么、在哪检测、灵敏度是多少”。

### 6.5 OSD 配置

```cpp
namespace aov::media {

enum class OsdItemType {
    Time,
    Week,
    DeviceName,
    ChannelName,
    Logo,
    StatusIcon,
    DetectBox,
};

struct OsdItemConfig {
    OsdItemType type {OsdItemType::Time};
    bool enabled {false};
    int x {0};
    int y {0};
    int font_size {16};
    std::string text;
    std::string resource_path;
};

struct OsdApplyConfig {
    bool enabled {false};
    int ivps_grp_id {0};
    int ivps_chn_id {0};
    std::vector<OsdItemConfig> items;
};

}  // namespace aov::media
```

### 6.6 低功耗与抓图

```cpp
namespace aov::media {

enum class MediaRuntimeState {
    Uninitialized,
    Initialized,
    RunningNormal,
    RunningLowPower,
    PreparedForAovSleep,
    Error,
};

enum class WakeupReason {
    Timer,
    App,
    Detect,
    Manual,
    Unknown,
};

struct LowPowerRecordProfile {
    int target_frame_count {1};  // 定时唤醒时本次至少需要输出的帧数，默认 1 帧
    int record_fps {1};          // 唤醒期间低功耗编码帧率，例如 1fps / 3fps
    int normal_fps {15};         // 恢复常电时的帧率
    int gop {60};
};

struct AovSleepMediaConfig {
    bool enable_low_power_wakeup {true};
    bool enable_tcm_wakeup {false};
    int pipe_id {0};
    DetectApplyConfig wakeup_detect; // 仅表示低功耗唤醒配置，不启动普通 detect 线程
};

struct WakeupMediaProfile {
    WakeupReason reason {WakeupReason::Unknown};
    bool start_video {true};
    bool start_audio {false};
    bool start_detect_confirm {false};
    bool request_idr {true};
    LowPowerRecordProfile low_power_record;
};

struct MediaRuntimeStatus {
    MediaRuntimeState state {MediaRuntimeState::Uninitialized};
    bool initialized {false};
    bool running {false};
    bool low_power {false};
    bool video_capturing {false};
    bool audio_capturing {false};
    bool detect_running {false};
    bool prepared_for_aov_sleep {false};
    uint64_t video_frames {0};
    uint64_t audio_frames {0};
    uint64_t detect_results {0};
    MediaStatus last_error;
};

struct SnapshotRequest {
    int chn_id {0};
    int quality {80};
    bool request_keyframe_before_snapshot {false};
};

struct SnapshotResult {
    const uint8_t* data {nullptr};
    uint32_t size {0};
    uint64_t pts_ms {0};
    int width {0};
    int height {0};
};

}  // namespace aov::media
```

## 7. runtime/i_media_runtime.hpp

```cpp
#pragma once

#include "common/media_config.hpp"
#include "common/media_status.hpp"
#include "common/media_types.hpp"

namespace aov::media {

class IAudioStreamService;
class IDetectService;
class IIspService;
class IOsdService;
class IVideoStreamService;

struct MediaRuntimeInitConfig {
    SensorConfig sensor;
    CmPoolConfig pool;
    VideoEncodeGroupConfig video;
    AudioEncodeApplyConfig audio;
    IspImageConfig image;
    DetectApplyConfig detect;
    OsdApplyConfig osd;
};

class IMediaRuntime {
public:
    virtual ~IMediaRuntime() = default;

    virtual MediaStatusCode Init(const MediaRuntimeInitConfig& config) = 0;
    virtual MediaStatusCode Init(const SensorConfig& sensor, const CmPoolConfig& pool) = 0;
    virtual MediaStatusCode Start() = 0;
    virtual void Stop() = 0;

    virtual MediaStatusCode ApplyVideoConfig(const VideoEncodeGroupConfig& config) = 0;
    virtual MediaStatusCode ApplyAudioConfig(const AudioEncodeApplyConfig& config) = 0;
    virtual MediaStatusCode ApplyImageConfig(const IspImageConfig& config) = 0;
    virtual MediaStatusCode ApplyDetectConfig(const DetectApplyConfig& config) = 0;
    virtual MediaStatusCode ApplyOsdConfig(const OsdApplyConfig& config) = 0;

    // AOV true sleep boundary. PrepareForAovSleep does not decide whether the
    // system may sleep; app/core decides after storage/cloud/config drain.
    virtual MediaStatusCode PrepareForAovSleep(const AovSleepMediaConfig& config) = 0;
    virtual MediaStatusCode RestoreFromAovSleep(const WakeupMediaProfile& profile) = 0;

    // Runtime media profile switch while the system is already awake.
    virtual MediaStatusCode EnterLowPowerMode(const LowPowerRecordProfile& profile) = 0;
    virtual MediaStatusCode ResumeNormalMode() = 0;

    virtual MediaStatusCode CaptureSnapshot(const SnapshotRequest& request,
                                            SnapshotResult& result) = 0;

    virtual IIspService* GetIspService() = 0;
    virtual IVideoStreamService* GetVideoStreamService() = 0;
    virtual IAudioStreamService* GetAudioStreamService() = 0;
    virtual IDetectService* GetDetectService() = 0;
    virtual IOsdService* GetOsdService() = 0;

    virtual bool IsRunning() const = 0;
    virtual MediaRuntimeStatus GetRuntimeStatus() const = 0;
};

}  // namespace aov::media
```

说明：

- 保留 `Init(sensor, pool)` 是为了当前阶段最小启动链路；最终 app 推荐用 `MediaRuntimeInitConfig` 一次性初始化。
- `PrepareForAovSleep()` 用于真正休眠前的媒体侧收口：停止普通取流/检测线程，配置低功耗唤醒能力。它不保存码流，也不决定系统是否睡眠。
- `RestoreFromAovSleep()` 用于被 RTC/App/事件唤醒后的媒体侧恢复：按唤醒原因启动编码、检测确认和 IDR 请求。
- `EnterLowPowerMode()` 只用于唤醒状态下的低功耗取帧/低帧率编码 profile，比如定时唤醒后取 1 帧或 N 帧。
- `ResumeNormalMode()` 用于 App/事件唤醒后恢复常电媒体 profile。
- `CaptureSnapshot()` 返回 JPEG 数据，具体是否上传云端或写文件由 app 层决定。

## 8. video/i_video_stream_service.hpp

```cpp
#pragma once

#include <functional>
#include "common/media_config.hpp"
#include "common/media_status.hpp"
#include "common/media_types.hpp"

namespace aov::media {

using StreamCallback = std::function<void(int chn_id, const StreamFrame& frame)>;

class IVideoStreamService {
public:
    virtual ~IVideoStreamService() = default;

    virtual MediaStatusCode ApplyVideoEncodeConfig(const VideoEncodeApplyConfig& config) = 0;
    virtual MediaStatusCode ApplyVideoEncodeGroupConfig(const VideoEncodeGroupConfig& config) = 0;

    virtual MediaStatusCode RegisterStreamCallback(int chn_id, StreamCallback cb) = 0;
    virtual MediaStatusCode StartCapture(int chn_id) = 0;
    virtual void StopCapture(int chn_id) = 0;

    virtual MediaStatusCode RequestKeyFrame(int chn_id) = 0;
    virtual MediaStatusCode SetLowPowerFrameRate(int chn_id, int fps_aov) = 0;
    virtual MediaStatusCode SetNormalFrameRate(int chn_id, int fps_normal) = 0;

    virtual bool IsCapturing(int chn_id) const = 0;
};

}  // namespace aov::media
```

链路映射：

```text
QSDemo qs_common_venc.c / AX_VENC_*
        -> adapter/ax615/venc/AxVencAdapter
        -> Src/video/VideoStreamService
        -> IVideoStreamService callback
        -> app/packet
```

`IVideoStreamService` 不直接知道 SD 卡和云端。它只把裸码流帧回调给 app，通常由 `app/core` 注册回调后投递给 `app/packet`。

## 9. audio/i_audio_stream_service.hpp

```cpp
#pragma once

#include "common/media_config.hpp"
#include "common/media_status.hpp"
#include "common/media_types.hpp"

namespace aov::media {

class IAudioStreamService {
public:
    virtual ~IAudioStreamService() = default;

    virtual MediaStatusCode ApplyAudioEncodeConfig(const AudioEncodeApplyConfig& config) = 0;
    virtual MediaStatusCode RegisterAudioCallback(int chn_id, AudioFrameCallback cb) = 0;

    virtual MediaStatusCode StartCapture(int chn_id) = 0;
    virtual void StopCapture(int chn_id) = 0;

    virtual MediaStatusCode StartPlayback(int dev_id) = 0;
    virtual void StopPlayback(int dev_id) = 0;
    virtual MediaStatusCode SendPlaybackFrame(const AudioFrame& frame) = 0;

    virtual bool IsCapturing(int chn_id) const = 0;
};

}  // namespace aov::media
```

链路映射：

```text
QSDemo qs_common_audio.c / AX_AI / AX_AENC / AX_AO
        -> adapter/ax615/audio
        -> Src/audio/AudioStreamService
        -> IAudioStreamService callback
        -> app/packet 或 app/cloud 对讲链路
```

## 10. isp/i_isp_service.hpp

```cpp
#pragma once

#include "common/media_config.hpp"
#include "common/media_status.hpp"

namespace aov::media {

class IIspService {
public:
    virtual ~IIspService() = default;

    virtual MediaStatusCode ApplyImageConfig(const IspImageConfig& config) = 0;
    virtual MediaStatusCode GetImageConfig(int pipe_id, IspImageConfig& config) const = 0;

    virtual MediaStatusCode ApplyWhiteBalance(const WhiteBalanceConfig& config) = 0;
    virtual MediaStatusCode ApplyExposure(const ExposureConfig& config) = 0;
    virtual MediaStatusCode ApplyScene(const SceneConfig& config) = 0;
    virtual MediaStatusCode ApplyLdc(const LdcConfig& config) = 0;
    virtual MediaStatusCode ApplySharpen(const SharpenConfig& config) = 0;
    virtual MediaStatusCode ApplyWbc(const WbcConfig& config) = 0;

    virtual MediaStatusCode SetMirrorFlip(int pipe_id, bool mirror, bool flip) = 0;
    virtual MediaStatusCode SetBrightness(int pipe_id, int value) = 0;
    virtual MediaStatusCode SetContrast(int pipe_id, int value) = 0;
    virtual MediaStatusCode SetSaturation(int pipe_id, int value) = 0;
    virtual MediaStatusCode SetHue(int pipe_id, int value) = 0;
};

}  // namespace aov::media
```

AX615 SDK 预期映射：

| public 配置 | adapter 预期能力 |
| --- | --- |
| `WhiteBalanceConfig` | `AX_ISP_IQ_GetAwbParam` / `AX_ISP_IQ_SetAwbParam` |
| `ExposureConfig` | `AX_ISP_IQ_GetAeParam` / `AX_ISP_IQ_SetAeParam` |
| `SceneConfig` | `AX_ISP_IQ_GetSceneParamEx` / `AX_ISP_IQ_SetSceneParamEx` |
| `LdcConfig` | `AX_ISP_IQ_GetLdcParam` / `AX_ISP_IQ_SetLdcParam` |
| `SharpenConfig` | `AX_ISP_IQ_GetSharpenParam` / `AX_ISP_IQ_SetSharpenParam` |
| `WbcConfig` | `AX_ISP_IQ_GetWbcParam` / `AX_ISP_IQ_SetWbcParam` |
| mirror/flip | VIN/ISP/IVPS 里对应镜像翻转能力，按 SDK 实际位置适配 |

## 11. detect/i_detect_service.hpp

```cpp
#pragma once

#include <functional>
#include "common/media_config.hpp"
#include "common/media_status.hpp"
#include "common/media_types.hpp"

namespace aov::media {

using DetectCallback = std::function<void(const DetectResultSummary&)>;

class IDetectService {
public:
    virtual ~IDetectService() = default;

    virtual MediaStatusCode ApplyDetectConfig(const DetectApplyConfig& config) = 0;
    virtual MediaStatusCode RunOnceDetect(DetectResultSummary& result) = 0;
    virtual void RegisterDetectCallback(DetectCallback cb) = 0;
    virtual MediaStatusCode StartDetect() = 0;
    virtual void StopDetect() = 0;
    virtual bool IsDetecting() const = 0;
};

}  // namespace aov::media
```

说明：

- `libmedia` 输出检测事实，不输出报警决策。
- 报警去抖、布防时间、联动录像、联动白光/声音、消息推送由 `app/alarm` 和 `app/core` 处理。
- 普通 `StartDetect()` 只在系统唤醒且媒体链有图像时运行。
- 真正 AOV 休眠时不运行普通 detect 线程；休眠唤醒检测由 PIR/TCM/低功耗硬件负责，`libmedia` 只在 `PrepareForAovSleep()` 中配置相关媒体侧参数。

## 12. osd/i_osd_service.hpp

```cpp
#pragma once

#include "common/media_config.hpp"
#include "common/media_status.hpp"
#include "common/media_types.hpp"

namespace aov::media {

class IOsdService {
public:
    virtual ~IOsdService() = default;

    virtual MediaStatusCode ApplyOsdConfig(const OsdApplyConfig& config) = 0;
    virtual void UpdateDetectOverlay(const DetectResultSummary& result) = 0;
    virtual void UpdateTimestamp(uint64_t epoch_ms) = 0;
    virtual void UpdateStatusIcon(const OsdStatusFlags& flags) = 0;
    virtual void ClearAll() = 0;
};

}  // namespace aov::media
```

说明：

- 时间、星期、设备名、通道名、Logo、状态图标、目标框开关都从 `OsdApplyConfig` 表达。
- `app/core` 负责把云端/本地配置转换成 OSD item；`libmedia` 只负责把 item 叠到 IVPS/OSD 链路上。

## 13. app/core 如何调用 libmedia

### 13.1 初始化

```text
app/core 启动
  -> app/config 读取 DesiredConfig / ActiveConfig
  -> 选择 sensor profile: OS04D10 单路
  -> 构造 MediaRuntimeInitConfig
  -> IMediaRuntime::Init(config)
  -> 注册 video/audio/detect 回调
  -> IMediaRuntime::Start()
```

### 13.4 AOV 休眠与唤醒

进入真正 AOV 休眠：

```text
app/core 确认：
  - 本地录像 close + flush + fsync 完成
  - 云存成功或明确失败
  - 配置持久化完成
  - idle debounce 到期
        |
        v
libmedia::PrepareForAovSleep(aov_sleep_config)
        |
        v
libsys/power::EnterAovSleep()
```

定时唤醒取帧：

```text
RTC 唤醒
  -> app/core
  -> libmedia::RestoreFromAovSleep(reason=Timer, start_video=true, start_detect_confirm=false)
  -> libmedia::EnterLowPowerMode(target_frame_count=1/N, record_fps=1/3)
  -> StreamFrame 回调到 app/packet
  -> app/storage 写盘
  -> drain 完成
  -> libmedia::PrepareForAovSleep()
  -> libsys/power::EnterAovSleep()
```

事件唤醒确认：

```text
PIR/TCM/低功耗硬件唤醒
  -> app/core
  -> libmedia::RestoreFromAovSleep(reason=Detect, start_video=true, start_detect_confirm=true)
  -> libmedia::ResumeNormalMode()
  -> libmedia detect service 输出 DetectResultSummary
  -> app/alarm 判断是否有效事件
  -> 有效事件：本地录像 + 云存 + 告警上报
  -> 无效事件：按策略保存必要帧后回休眠
```

App 唤醒：

```text
App 请求预览/控制
  -> app/core
  -> libmedia::RestoreFromAovSleep(reason=App, start_video=true)
  -> libmedia::ResumeNormalMode()
  -> StreamFrame -> app/packet -> app/cloud 直播 / app/storage 本地保存
```

### 13.2 码流进入 Packet

```text
VENC/AENC 回调
  -> libmedia::StreamFrame / AudioFrame
  -> app/core 注册的回调
  -> app/packet PushFrame
  -> app/storage: SD 卡录像需要 PS/PES 封装后写盘
  -> app/cloud: 云直播 / 云存按云 SDK 要求封装或直接发送裸码流
```

这里 Packet 的作用是缓存、引用计数、跨线程分发、背压和多消费者并行。Packet 不需要再剥私有头，因为 libmedia 已经不加私有头。

### 13.3 配置生效

```text
云端/App 下发配置
  -> app/cloud 转成产品配置命令
  -> app/config 校验并持久化 DesiredConfig
  -> app/core 判断当前状态是否允许立即生效
  -> 转换成 libmedia DTO
  -> 调用对应 Apply 接口
  -> 成功后更新 ActiveConfig
```

AOV 休眠期间允许修改 DesiredConfig，但普通媒体配置不应该单独唤醒设备。下次 PIR、App 唤醒、定时唤醒或其他合法唤醒时，由 `app/core` 应用到 `libmedia`。

## 14. QSDemo 到 libmedia 的映射

| QSDemo 文件/能力 | libmedia 归属 | 对外接口 |
| --- | --- | --- |
| `com/qs_common_sys.c`，`AX_SYS_Init`、`AX_POOL_*` | `adapter/ax615/sys` | `IMediaRuntime::Init` |
| `com/qs_common_vin.c`，MIPI/VIN/Sensor | `adapter/ax615/vin` | `SensorConfig` |
| `vin_config.c/h`，sensor profile | `sensor_registry` + `sensors/os04d10_profile.cpp` | `SensorId::OS04D10` |
| `com/qs_common_isp.c`，ISP 初始化、3A、IQ bin | `adapter/ax615/isp` | `IIspService` |
| `com/qs_common_ivps.c`，IVPS group/channel/filter | `adapter/ax615/ivps` | runtime 内部编排 |
| `com/qs_common_venc.c`，VENC 通道、码率、取流 | `adapter/ax615/venc` + `Src/video` | `IVideoStreamService` |
| `com/qs_common_audio.c` | `adapter/ax615/audio` + `Src/audio` | `IAudioStreamService` |
| `osd/*` | `adapter/ax615/osd` + `Src/osd` | `IOsdService` |
| AOV sleep 前媒体侧收口 / sensor wakeup 配置 | `adapter/ax615/power` + `pipeline` | `IMediaRuntime::PrepareForAovSleep` |
| 唤醒后恢复媒体链 / IDR / detect confirm | `pipeline` + `adapter/ax615/power` | `IMediaRuntime::RestoreFromAovSleep` |
| 唤醒期间低功耗 fps / rc 更新 | `adapter/ax615/power` | `IMediaRuntime::EnterLowPowerMode` |

## 15. Src 目录分层要求

```text
libmedia/Src/
  adapter/ax615/
    audio/      # AX_AI / AX_AENC / AX_AO
    detect/     # AX 检测 SDK 或算法适配
    isp/        # AX_ISP / AX_ISP_IQ / 3A SDK
    ivps/       # AX_IVPS
    jenc/       # AX_JENC
    osd/        # AX OSD / IVPS OSD
    power/      # AOV fps / sensor wakeup / venc rc 切换
    sys/        # AX_SYS / AX_POOL / AX_SYS_Link
    venc/       # AX_VENC
    vin/        # AX_VIN / MIPI / sensor registry
  audio/        # 无 SDK，音频流服务
  detect/       # 无 SDK，检测服务
  isp/          # 无 SDK，图像参数服务
  osd/          # 无 SDK，OSD 服务
  pipeline/     # 媒体链路编排
  video/        # 无 SDK，视频流服务
```

规则：

- `Include/*` 和 `Src/{audio,detect,isp,osd,pipeline,video}` 不允许 include AX SDK 头。
- 只有 `Src/adapter/ax615/*` 可以 include `AX_*` SDK 头。
- `libmedia` 不允许 include `app/*`。
- public DTO 使用稳定 C++ 类型，避免把 SDK ABI 暴露给 app。

## 16. 实施顺序建议

### 阶段 A：头文件收口

1. 按本文更新 `libmedia/Include/common/media_types.hpp`、`media_config.hpp`、`media_status.hpp`。
2. 更新六个服务接口头文件。
3. 删除或不再保留旧平铺 Include 兼容头。
4. 编写一个 host 侧 public header smoke test，只 include public headers 并实例化 DTO。
5. 补 `MediaRuntimeStatus`、`AovSleepMediaConfig`、`WakeupMediaProfile` 相关合同测试。

### 阶段 B：服务层编译打通

1. `Src/video` 实现新的 `IVideoStreamService` 签名。
2. `Src/audio` 实现新的 `IAudioStreamService` 签名。
3. `Src/isp` 实现 `IIspService` 缓存和参数转发，暂未映射的 SDK 项返回 `Unsupported` 或走 stub。
4. `Src/detect`、`Src/osd` 补 Apply 配置接口。
5. `pipeline` 持有并返回各 service。
6. `pipeline` 实现 `PrepareForAovSleep()`、`RestoreFromAovSleep()`、`GetRuntimeStatus()` 的 host/stub 行为。

### 阶段 C：AX615 adapter 映射

1. VENC：按 QSDemo 映射 H265/H264、分辨率、fps、GOP、CBR/VBR/FIXQP、IDR。
2. AUDIO：按 QSDemo 映射 G711A/8k/16bit、AI/AENC/AO。
3. ISP：先映射 WB、AE、Scene、LDC、Sharpen、WBC；镜像翻转按 SDK 实际模块落地。
4. Power：实现 AOV fps、normal fps、venc rc、sensor wakeup/tcm sensor info 更新。
5. VENC：修正 Stop 顺序，确保 Stop/Start 重入可靠。

### 阶段 D：app/core 对接

1. `app/core` 把 `app/config` 的编码、图像、检测、OSD、工作模式转换成 `libmedia` DTO。
2. 注册 video/audio 回调，把裸码流送入 `app/packet`。
3. 注册 detect 回调，把检测事实送入 `app/alarm`。
4. AOV 休眠前由 `app/core` 判断云存、SD 写盘、配置持久化是否完成。
5. app/core 仅在唤醒后启动普通 detect 确认；真正休眠中不依赖 libmedia detect 线程。

## 17. 验收标准

1. public header smoke test 能在 host g++ 下编译通过。
2. `libmedia/Build/build_ax615.sh` 能交叉编译出 `libmedia.a`。
3. `arm-rel-linux-uclibcgnueabihf-ar t libmedia.a` 能看到 video/audio/isp/detect/osd/pipeline/adapter 对象文件。
4. `nm -C -u libmedia.a | grep AX_` 只出现 adapter 期望使用的 AX SDK 未解析符号。
5. `rg "app/" libmedia/Include libmedia/Src` 不应出现对 app 层头文件的 include。
6. `rg "AX_" libmedia/Include libmedia/Src/audio libmedia/Src/video libmedia/Src/isp libmedia/Src/detect libmedia/Src/osd libmedia/Src/pipeline` 不应出现 SDK 类型或 SDK 调用。
7. VENC 回调给 app 的 `StreamFrame` 不包含私有帧头/尾。
8. Packet -> storage/cloud 的链路可以并行消费同一帧；SD 卡侧后续在 `app/storage/dhfs` 做 PS/PES 和 DHFS 写盘。
9. `PrepareForAovSleep()` 后普通 video/audio/detect capture 不再运行，状态为 `PreparedForAovSleep`。
10. `RestoreFromAovSleep()` 后可按唤醒原因恢复 video capture、detect confirm 和 IDR 请求。
11. `EnterLowPowerMode()` 正确使用 `LowPowerRecordProfile::record_fps/target_frame_count/normal_fps/gop`。

## 18. 关键注意点

- `libmedia` 的 public DTO 是媒体能力 DTO，不是产品配置模型。不要把腾讯云字段名、App 页面字段名直接放进 `libmedia`。
- ISP 参数可以在头文件里一次性定义全，但 adapter 的 SDK 映射允许分批完成。未实现能力必须返回明确状态，不能静默成功。
- AOV 定时唤醒取帧可能导致输入帧时间戳不连续，`libmedia` 不负责补连续时间轴；PS/PES 和 DHFS 写盘改造放在 `app/storage/dhfs`。
- `IMediaRuntime::EnterLowPowerMode` 不等于系统休眠。它只表示唤醒状态下使用低功耗媒体 profile。
- `IMediaRuntime::PrepareForAovSleep` 也不决定系统是否休眠。系统是否回 AOV 休眠由 `app/core` 在云存、SD flush/fsync、配置持久化和 idle debounce 完成后决定。
- 普通 detect 线程只在唤醒态运行；休眠态事件唤醒依赖 PIR/TCM/低功耗硬件。
- 后续如果更换 sensor，只新增 sensor profile 和 registry 项，不应改 app/core、packet、storage、cloud 链路。

#pragma once

#include "common/media_status.hpp"

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace aov::media {

struct Resolution {
    int width = 0;
    int height = 0;
};

enum class PixelFormat {
    NV12,
    YUV420P,
    YUYV,
    RGB888,
    Unknown
};

enum class StreamProfile {
    Main,
    Sub,
    Snapshot,
    Unknown
};

enum class VideoCodec {
    H264,
    H265,
    JPEG
};

enum class AudioCodec {
    G711A,
    G711U,
    AAC,
    PCM
};

enum class RcMode {
    CBR,
    VBR,
    FIXQP
};

enum class FrameRateMode {
    Fixed,
    Interval
};

struct FrameRateConfig {
    FrameRateMode mode = FrameRateMode::Fixed;
    int fps = 15;
    int interval_sec = 1;
};

enum class PoolCacheMode {
    NonCache,
    Cached
};

struct CmPoolBlockConfig {
    uint64_t blk_size = 0;
    uint32_t blk_count = 0;
    uint64_t meta_size = 512;
    PoolCacheMode cache_mode = PoolCacheMode::NonCache;
};

struct CmPoolConfig {
    std::vector<CmPoolBlockConfig> pools;

    static CmPoolConfig DefaultAx615() {
        CmPoolConfig cfg;
        cfg.pools.push_back({3840ULL * 2160 * 3 / 2, 3, 512, PoolCacheMode::NonCache});
        cfg.pools.push_back({1920ULL * 1080 * 3 / 2, 3, 512, PoolCacheMode::NonCache});
        cfg.pools.push_back({752ULL * 576 * 3 / 2, 4, 512, PoolCacheMode::NonCache});
        return cfg;
    }
};

enum class SensorId {
    OS04D10,
    OS04A10,
    SC4336,
    IMX415,
    Unknown
};

struct SensorConfig {
    SensorId sensor_id = SensorId::OS04D10;
    int pipe_id = 0;
    int dev_id = 0;
    Resolution resolution = {2688, 1520};
    int fps = 25;
    std::string sensor_lib_path;
    std::string iq_bin_path = "/etc/iqfiles";
};

struct VencChannelConfig {
    int chnId = 0;
    VideoCodec codec = VideoCodec::H264;
    Resolution res = {1920, 1080};
    int fps = 25;
    RcMode rcMode = RcMode::CBR;
    int bitrate = 4096;
    int fixQp = 35;
    int gop = 50;
};

struct IvpsChannelConfig {
    int chnId = 0;
    Resolution outputRes = {1920, 1080};
    PixelFormat format = PixelFormat::NV12;
};

struct IvpsGroupConfig {
    int grpId = 0;
    Resolution inputRes;
    std::vector<IvpsChannelConfig> channels;
};

enum class VideoFrameType {
    I,
    P,
    B,
    JPEG,
    Unknown
};

struct StreamFrame {
    // data points to raw encoded VENC/JENC bytes without private frame headers.
    // Unless stated otherwise by a concrete implementation, the pointer is only
    // valid during the callback that delivers this frame. Async consumers such
    // as app/packet must copy or take ownership before returning.
    StreamProfile profile = StreamProfile::Main;
    int chn_id = 0;
    VideoCodec codec = VideoCodec::H265;
    VideoFrameType frame_type = VideoFrameType::Unknown;
    const uint8_t* data = nullptr;
    uint32_t size = 0;
    uint64_t pts_ms = 0;
    uint64_t sequence = 0;
    bool is_keyframe = false;
};

using StreamFrameCallback = std::function<void(const StreamFrame&)>;

struct AudioFrame {
    int chn_id = 0;
    AudioCodec codec = AudioCodec::G711A;
    const uint8_t* data = nullptr;
    uint32_t size = 0;
    uint64_t pts_ms = 0;
    uint64_t sequence = 0;
    int sample_rate_hz = 8000;
    int bit_depth = 16;
    int channels = 1;
};

using AudioFrameCallback = std::function<void(const AudioFrame&)>;

struct Rect {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
};

enum class DetectTargetType {
    Human,
    Vehicle,
    NonMotorVehicle,
    Motion,
    Occlusion
};

struct DetectObject {
    DetectTargetType type = DetectTargetType::Human;
    Rect box;
    int confidence = 0;
};

struct DetectResultSummary {
    bool has_human = false;
    bool has_vehicle = false;
    bool has_non_motor_vehicle = false;
    bool has_motion = false;
    bool has_occlusion = false;
    int object_count = 0;
    uint64_t frame_id = 0;
    uint64_t timestamp_ms = 0;
    std::vector<DetectObject> objects;
};

struct OsdStatusFlags {
    bool recording = false;
    bool low_battery = false;
    bool cloud_connected = false;
    bool sd_ready = false;
    bool aov_low_power = false;
};

struct ModLink {
    int srcMod = 0;
    int srcDev = 0;
    int srcChn = 0;
    int dstMod = 0;
    int dstDev = 0;
    int dstChn = 0;
};

enum class MediaRuntimeState {
    Uninitialized,
    Initialized,
    RunningNormal,
    RunningLowPower,
    PreparedForAovSleep,
    Error
};

enum class WakeupReason {
    Timer,
    App,
    Detect,
    Manual,
    Unknown
};

}  // namespace aov::media

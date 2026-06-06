#pragma once

#include "common/media_types.hpp"

namespace aov::media {

struct VideoEncodeApplyConfig {
    StreamProfile profile = StreamProfile::Main;
    int chn_id = 0;
    VideoCodec codec = VideoCodec::H265;
    Resolution resolution = {2560, 1440};
    int fps_normal = 15;
    int fps_aov = 1;
    RcMode rc_mode = RcMode::VBR;
    int bitrate_kbps = 1024;
    int gop = 60;
    int fix_qp = 35;
    bool enabled = true;
};

struct VideoEncodeGroupConfig {
    VideoEncodeApplyConfig main_stream;
    VideoEncodeApplyConfig sub_stream = {StreamProfile::Sub, 1, VideoCodec::H265, {768, 432}, 15, 1,
                                         RcMode::VBR, 256, 60, 35, true};
};

struct AudioEncodeApplyConfig {
    bool enabled = true;
    int capture_dev_id = 0;
    int play_dev_id = 0;
    int enc_chn_id = 0;
    AudioCodec codec = AudioCodec::G711A;
    int sample_rate_hz = 8000;
    int bit_depth = 16;
    int channels = 1;
    int input_volume = 100;
    int output_volume = 100;
    bool enable_talkback_play = false;
};

enum class ImageAutoMode {
    Auto,
    Manual
};

enum class DayNightMode {
    Auto,
    Day,
    Night
};

enum class IrCutMode {
    Auto,
    Color,
    BlackWhite
};

enum class AntiFlickerMode {
    Off,
    Hz50,
    Hz60
};

struct WhiteBalanceConfig {
    int pipe_id = 0;
    ImageAutoMode mode = ImageAutoMode::Auto;
    int r_gain = 0;
    int g_gain = 0;
    int b_gain = 0;
};

struct ExposureConfig {
    int pipe_id = 0;
    ImageAutoMode mode = ImageAutoMode::Auto;
    int compensation = 0;
    bool slow_shutter_enabled = false;
    int max_again = 0;
    int max_dgain = 0;
    int min_shutter_us = 0;
    int max_shutter_us = 0;
};

struct SceneConfig {
    int pipe_id = 0;
    DayNightMode day_night_mode = DayNightMode::Auto;
    IrCutMode ir_cut_mode = IrCutMode::Auto;
    AntiFlickerMode anti_flicker = AntiFlickerMode::Hz50;
    bool white_light_enabled = false;
    int white_light_strength = 0;
    bool ir_light_enabled = false;
    int ir_light_strength = 0;
};

struct LdcConfig {
    int pipe_id = 0;
    bool enabled = false;
    int strength = 0;
};

struct SharpenConfig {
    int pipe_id = 0;
    int strength = 50;
};

struct WbcConfig {
    int pipe_id = 0;
    bool enabled = false;
    int r_gain = 0;
    int gr_gain = 0;
    int gb_gain = 0;
    int b_gain = 0;
};

struct IspImageConfig {
    int pipe_id = 0;
    int brightness = 50;
    int contrast = 50;
    int saturation = 50;
    int hue = 50;
    bool mirror = false;
    bool flip = false;
    WhiteBalanceConfig white_balance;
    ExposureConfig exposure;
    SceneConfig scene;
    LdcConfig ldc;
    SharpenConfig sharpen;
    WbcConfig wbc;
};

struct DetectRegion {
    bool full_screen = true;
    std::vector<Rect> rects;
    std::vector<int> motion_cells;
};

struct DetectRuleApplyConfig {
    DetectTargetType type = DetectTargetType::Human;
    bool enabled = false;
    int pipe_id = 0;
    int ivps_grp_id = 0;
    int ivps_chn_id = 0;
    DetectRegion region;
    int sensitivity = 60;
    bool static_filter = false;
};

struct DetectApplyConfig {
    bool enabled = false;
    int pipe_id = 0;
    int ivps_grp_id = 0;
    int ivps_chn_id = 0;
    std::vector<DetectRuleApplyConfig> rules;
};

enum class OsdItemType {
    Time,
    Week,
    DeviceName,
    ChannelName,
    Logo,
    StatusIcon,
    DetectBox
};

struct OsdItemConfig {
    OsdItemType type = OsdItemType::Time;
    bool enabled = false;
    int x = 0;
    int y = 0;
    int font_size = 16;
    std::string text;
    std::string resource_path;
};

struct OsdApplyConfig {
    bool enabled = false;
    int ivps_grp_id = 0;
    int ivps_chn_id = 0;
    std::vector<OsdItemConfig> items;
};

struct LowPowerRecordProfile {
    int target_frame_count = 1;
    int record_fps = 1;
    int normal_fps = 15;
    int gop = 60;
};

struct AovSleepMediaConfig {
    bool enable_low_power_wakeup = true;
    bool enable_tcm_wakeup = false;
    int pipe_id = 0;
    DetectApplyConfig wakeup_detect;
};

struct WakeupMediaProfile {
    WakeupReason reason = WakeupReason::Unknown;
    bool start_video = true;
    bool start_audio = false;
    bool start_detect_confirm = false;
    bool request_idr = true;
    LowPowerRecordProfile low_power_record;
};

struct SnapshotRequest {
    int chn_id = 0;
    int quality = 80;
    bool request_keyframe_before_snapshot = false;
};

struct SnapshotResult {
    const uint8_t* data = nullptr;
    uint32_t size = 0;
    uint64_t pts_ms = 0;
    int width = 0;
    int height = 0;
};

struct MediaRuntimeInitConfig {
    SensorConfig sensor;
    CmPoolConfig pool;
    VideoEncodeGroupConfig video;
    AudioEncodeApplyConfig audio;
    IspImageConfig image;
    DetectApplyConfig detect;
    OsdApplyConfig osd;
};

struct MediaRuntimeStatus {
    MediaRuntimeState state = MediaRuntimeState::Uninitialized;
    bool initialized = false;
    bool running = false;
    bool low_power = false;
    bool video_capturing = false;
    bool audio_capturing = false;
    bool detect_running = false;
    bool prepared_for_aov_sleep = false;
    uint64_t video_frames = 0;
    uint64_t audio_frames = 0;
    uint64_t detect_results = 0;
    MediaStatus last_error;
};

}  // namespace aov::media

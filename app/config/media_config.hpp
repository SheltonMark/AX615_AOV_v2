#pragma once

#include <string>

namespace aov::app::config {

enum class StreamProfile {
    Main,
    Sub,
};

enum class VideoEncodeType {
    H264,
    H265,
};

enum class BitrateControlMode {
    CBR,
    VBR,
};

struct VideoStreamConfig {
    StreamProfile stream {StreamProfile::Main};
    VideoEncodeType codec {VideoEncodeType::H265};
    int width {2560};
    int height {1440};
    int fps_normal {15};
    int fps_aov {1};
    BitrateControlMode bitrate_mode {BitrateControlMode::VBR};
    int bitrate_kbps {1024};
    int gop {60};
};

enum class AudioEncodeType {
    G711A,
    G711U,
    AAC,
};

struct AudioConfig {
    bool enabled {true};
    AudioEncodeType codec {AudioEncodeType::G711A};
    int sample_rate_khz {8};
    int bit_depth {16};
    int input_volume {100};
    int output_volume {100};
};

struct ImageParamConfig {
    int brightness {50};
    int contrast {50};
    int saturation {50};
    int sharpness {50};
    bool mirror {false};
    bool anti_flicker {false};
    std::string video_standard {"NTSC"};
};

struct OsdConfig {
    bool show_name {true};
    bool show_time {true};
    bool show_week {true};
    bool show_tenda_logo {true};
    std::string channel_name {"CS6GV2.0"};
    std::string time_format {"24h"};
    std::string date_format {"YYYY-MM-DD"};
    std::string font_size {"auto"};
};

enum class FillLightMode {
    Off,
    Auto,
    Manual,
};

struct FillLightConfig {
    FillLightMode mode {FillLightMode::Auto};
    int brightness {50};
};

struct MediaEncodeConfig {
    VideoStreamConfig main_stream;
    VideoStreamConfig sub_stream;
    AudioConfig audio;
};

inline VideoStreamConfig MakeDefaultSubStreamConfig() {
    VideoStreamConfig config;
    config.stream = StreamProfile::Sub;
    config.width = 768;
    config.height = 432;
    config.bitrate_kbps = 256;
    return config;
}

} // namespace aov::app::config

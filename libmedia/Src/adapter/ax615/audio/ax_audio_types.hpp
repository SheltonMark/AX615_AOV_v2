#pragma once

#include "common/media_types.hpp"

namespace aov::media::ax615 {

struct AxAudioCaptureConfig {
    int card_id = 0;
    int device_id = 0;
    int aenc_chn_id = 0;
    int sample_rate = 16000;
    int channels = 1;
    int bitrate_bps = 48000;
    AudioCodec codec = AudioCodec::AAC;
};

struct AxAudioPlayConfig {
    int card_id = 0;
    int device_id = 0;
    int adec_chn_id = 0;
    int sample_rate = 16000;
    int channels = 1;
    AudioCodec codec = AudioCodec::G711A;
};

}  // namespace aov::media::ax615

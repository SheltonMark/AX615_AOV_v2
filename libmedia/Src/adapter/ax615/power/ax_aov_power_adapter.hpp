#pragma once

#include "common/media_types.hpp"

#include <vector>

extern "C" {
#include "ax_tcm_api.h"
#include "ax_venc_api.h"
#include "ax_vin_api.h"
}

namespace aov::media::ax615 {

struct AovPipeConfig {
    int pipe_id = 0;
    bool enable_tcm = false;
};

struct AovVencConfig {
    int chn_id = 0;
    VideoCodec codec = VideoCodec::H264;
    uint32_t width = 1920;
    uint32_t height = 1080;
};

struct AovPowerConfig {
    float normal_fps = 15.0F;
    float aov_pipe_dst_fps = 0.0F;
    float aov_pipe_src_fps = 60.0F;
    float aov_venc_fps = 1.0F;
    uint32_t normal_gop = 30;
    uint32_t aov_gop = 2;
    uint32_t tcm_cap_frame_num = 1;
    float aov_bitrate_ratio = 3.0F;
    std::vector<AovPipeConfig> pipes;
    std::vector<AovVencConfig> venc_channels;
};

class AxAovPowerAdapter {
public:
    AxAovPowerAdapter() = default;
    ~AxAovPowerAdapter() = default;

    AxAovPowerAdapter(const AxAovPowerAdapter&) = delete;
    AxAovPowerAdapter& operator=(const AxAovPowerAdapter&) = delete;

    MediaStatusCode ApplyLowPower(const AovPowerConfig& cfg);
    MediaStatusCode RestoreNormal(const AovPowerConfig& cfg);
    MediaStatusCode WakeupSensors(const std::vector<AovPipeConfig>& pipes);

    bool IsInLowPower() const { return low_power_; }

private:
    MediaStatusCode UpdatePipeFps(const AovPowerConfig& cfg, bool low_power);
    MediaStatusCode UpdateVencRc(const AovPowerConfig& cfg, bool low_power);
    MediaStatusCode UpdateTcmSensorInfo(const AovPowerConfig& cfg, bool low_power);
    MediaStatusCode WakeupTcmSensors(const std::vector<AovPipeConfig>& pipes);

    bool low_power_ = false;
};

}  // namespace aov::media::ax615

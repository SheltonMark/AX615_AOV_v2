#include "adapter/ax615/power/ax_aov_power_adapter.hpp"

int main()
{
    using namespace aov::media;
    using namespace aov::media::ax615;

    AxAovPowerAdapter adapter;

    AovPowerConfig cfg;
    cfg.normal_fps = 15.0F;
    cfg.aov_pipe_dst_fps = 0.0F;
    cfg.aov_pipe_src_fps = 60.0F;
    cfg.aov_venc_fps = 1.0F;
    cfg.aov_gop = 2;
    cfg.tcm_cap_frame_num = 1;

    AovPipeConfig pipe;
    pipe.pipe_id = 0;
    pipe.enable_tcm = true;
    cfg.pipes.push_back(pipe);

    AovVencConfig venc;
    venc.chn_id = 0;
    venc.codec = PT_H264;
    venc.width = 1920;
    venc.height = 1080;
    cfg.venc_channels.push_back(venc);

    if (adapter.IsInLowPower()) {
        return 1;
    }
    if (adapter.ApplyLowPower({}) != MediaStatusCode::InvalidArgument) {
        return 2;
    }
    if (adapter.WakeupSensors({}) != MediaStatusCode::InvalidArgument) {
        return 3;
    }

    return 0;
}

#include "adapter/ax615/venc/ax_venc_adapter.hpp"

#include <cstdint>

int main()
{
    using namespace aov::media;
    using namespace aov::media::ax615;

    AxVencAdapter adapter;

    aov::media::ax615::VencChannelConfig cfg;
    cfg.chn_id = 0;
    cfg.width = 1920;
    cfg.height = 1080;
    cfg.fps = 15;
    cfg.bitrate_kbps = 2048;
    cfg.gop = 30;
    cfg.codec = PT_H264;
    cfg.rc_mode = AX_VENC_RC_MODE_H264CBR;

    bool callback_seen = false;
    adapter.SetFrameCallback(cfg.chn_id, [&](const StreamFrame& frame) {
        callback_seen = true;
        return;
    });

    if (adapter.IsRunning(cfg.chn_id)) {
        return 1;
    }

    const auto status = adapter.RequestKeyFrame(cfg.chn_id);
    if (status != MediaStatusCode::InvalidState) {
        return 2;
    }

    return callback_seen ? 3 : 0;
}

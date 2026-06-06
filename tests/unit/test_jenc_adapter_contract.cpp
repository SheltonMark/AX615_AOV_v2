#include "adapter/ax615/jenc/ax_jenc_adapter.hpp"

int main()
{
    using namespace aov::media;
    using namespace aov::media::ax615;

    AxJencAdapter adapter;

    JencChannelConfig cfg;
    cfg.chn_id = 8;
    cfg.width = 1920;
    cfg.height = 1080;
    cfg.sensor_fps = 15;
    cfg.capture_fps = 1;
    cfg.quality = 60;

    bool callback_seen = false;
    adapter.SetFrameCallback(cfg.chn_id, [&](const StreamFrame& frame) {
        callback_seen = true;
        if (frame.codec != VideoCodec::JPEG) {
            callback_seen = false;
        }
    });

    if (adapter.IsOpen(cfg.chn_id)) {
        return 1;
    }
    if (adapter.CaptureOnce(cfg.chn_id) != MediaStatusCode::InvalidState) {
        return 2;
    }

    return callback_seen ? 3 : 0;
}

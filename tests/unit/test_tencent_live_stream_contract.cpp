#include <cstdint>
#include <iostream>

#include "../../app/cloud/tencent/tencent_live_stream.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

} // namespace

int main() {
    using namespace aov::app::cloud::tencent;

    TencentLiveStream live;

    iv_cm_venc_pack_s pack {};
    std::uint8_t payload[] = {0x00, 0x00, 0x00, 0x01, 0x65};
    pack.pu8Addr = payload;
    pack.u32Len = sizeof(payload);
    pack.u64PTS = 100;
    pack.u32Seq = 3;
    pack.eFrameType = IV_CM_FRAME_TYPE_I;

    if (live.SendVideoPack(pack).code != TencentCloudStatusCode::InvalidState ||
        live.GetStats().dropped_frames != 1) {
        return Fail("live stream must reject video pack before session starts");
    }

    TencentLiveSession session;
    session.visitor = 7;
    session.channel = 0;
    session.video_res_type = IV_AVT_VIDEO_RES_HD;
    if (live.Start(session) != TencentCloudStatusCode::Ok ||
        !live.GetStats().active) {
        return Fail("live stream start must activate session");
    }

    if (live.SendVideoPack(pack).code != TencentCloudStatusCode::Ok) {
        return Fail("live stream must accept video pack after session starts");
    }

    const auto stats = live.GetStats();
    if (stats.sent_video_frames != 1 ||
        stats.sent_video_bytes != sizeof(payload) ||
        stats.last_video_pts_ms != 100 ||
        stats.last_video_sequence != 3) {
        return Fail("live stream stats mismatch after sending video pack");
    }

    if (live.Stop().code != TencentCloudStatusCode::Ok ||
        live.GetStats().active) {
        return Fail("live stream stop must clear active session");
    }

    std::cout << "tencent live stream contract passed\n";
    return 0;
}

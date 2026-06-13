#include <iostream>
#include <memory>
#include <vector>

#include "../../app/cloud/tencent/tencent_stream_adapter.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

} // namespace

int main() {
    using namespace aov::app::cloud::tencent;
    using namespace aov::app::packet;

    PacketFrame frame;
    frame.chn_id = 0;
    frame.codec = aov::media::VideoCodec::H265;
    frame.pts_ms = 1234;
    frame.sequence = 7;
    frame.is_keyframe = true;
    frame.payload = std::make_shared<const std::vector<std::uint8_t>>(
        std::vector<std::uint8_t>{0x00, 0x00, 0x01, 0x26});

    TencentStreamAdapter adapter;
    auto venc = adapter.ToVencPack(frame);
    if (!venc.ok ||
        venc.pack.pu8Addr != frame.data() ||
        venc.pack.u32Len != frame.size() ||
        venc.pack.u64PTS != frame.pts_ms ||
        venc.pack.u32Seq != frame.sequence ||
        venc.pack.eFrameType != IV_CM_FRAME_TYPE_I) {
        return Fail("packet frame must map to Tencent venc pack");
    }

    frame.is_keyframe = false;
    venc = adapter.ToVencPack(frame);
    if (!venc.ok || venc.pack.eFrameType != IV_CM_FRAME_TYPE_P) {
        return Fail("non-keyframe must map to Tencent P frame");
    }

    aov::app::config::MediaEncodeConfig media;
    media.main_stream.codec = aov::app::config::VideoEncodeType::H265;
    media.main_stream.width = 2560;
    media.main_stream.height = 1440;
    media.main_stream.fps_normal = 15;
    media.audio.codec = aov::app::config::AudioEncodeType::G711A;
    media.audio.sample_rate_khz = 8;
    media.audio.bit_depth = 16;

    auto info = adapter.ToAvDataInfo(media);
    if (!info.ok ||
        info.info.eVideoType != IV_CM_VENC_TYPE_H265 ||
        info.info.u32VideoWidth != 2560 ||
        info.info.u32VideoHeight != 1440 ||
        info.info.u32Framerate != 15 ||
        info.info.eAudioType != IV_CM_AENC_TYPE_G711A ||
        info.info.eAudioSampleRate != IV_CM_AENC_SAMPLE_RATE_8000 ||
        info.info.eAudioBitWidth != IV_CM_AENC_BIT_WIDTH_16) {
        return Fail("media encode config must map to Tencent av data info");
    }

    frame.payload = {};
    venc = adapter.ToVencPack(frame);
    if (venc.ok) {
        return Fail("empty packet payload must reject Tencent venc pack mapping");
    }

    std::cout << "tencent stream adapter contract passed\n";
    return 0;
}

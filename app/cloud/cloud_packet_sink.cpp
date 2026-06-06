#include "app/cloud/cloud_packet_sink.hpp"

namespace aov::app::cloud {

CloudPacketSink::CloudPacketSink(tencent::TencentLiveStream* live_stream)
    : live_stream_(live_stream) {
}

CloudPacketSinkResult CloudPacketSink::OnPacketFrame(
    const aov::app::packet::PacketFrame& frame) {
    ++stats_.received_frames;

    const auto pack_result = stream_adapter_.ToVencPack(frame);
    if (!pack_result.ok) {
        ++stats_.dropped_frames;
        return CloudPacketSinkResult::Error("failed to convert packet frame to tencent video pack");
    }

    stats_.last_video_pack = pack_result.pack;
    ++stats_.converted_video_frames;

    if (live_stream_) {
        const auto send_result = live_stream_->SendVideoPack(stats_.last_video_pack);
        if (!send_result.ok()) {
            ++stats_.dropped_frames;
            return CloudPacketSinkResult::Error(send_result.message);
        }
    }

    return CloudPacketSinkResult::Ok();
}

CloudPacketSinkStats CloudPacketSink::GetStats() const {
    return stats_;
}

void CloudPacketSink::Reset() {
    stats_ = {};
}

CloudPacketSinkResult BindCloudToPacket(aov::app::packet::MediaPacketRouter& router,
                                        CloudPacketSink& sink) {
    const auto result = router.BindCloudSink(
        [&sink](const aov::app::packet::PacketFrame& frame) {
            sink.OnPacketFrame(frame);
        });

    if (!result.ok()) {
        return CloudPacketSinkResult::Error(result.message);
    }
    return CloudPacketSinkResult::Ok();
}

} // namespace aov::app::cloud

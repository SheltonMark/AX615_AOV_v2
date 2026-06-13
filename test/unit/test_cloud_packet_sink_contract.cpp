#include <cstdint>
#include <iostream>

#include "../../app/cloud/cloud_packet_sink.hpp"
#include "../../app/packet/media_packet_router.hpp"
#include "../../app/packet/packet_service.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

} // namespace

int main() {
    using namespace aov::app;

    packet::PacketService packet_service;
    if (!packet_service.Init(packet::PacketConfig {}).ok()) {
        return Fail("packet service init failed");
    }

    packet::MediaPacketRouter router(packet_service);
    cloud::CloudPacketSink sink;

    const auto bind_result = cloud::BindCloudToPacket(router, sink);
    if (!bind_result.ok()) {
        return Fail(bind_result.message.c_str());
    }

    const std::uint8_t payload[] = {
        0x00, 0x00, 0x00, 0x01, 0x65, 0x88, 0x84, 0x21
    };

    aov::media::StreamFrame frame;
    frame.chn_id = 0;
    frame.codec = aov::media::VideoCodec::H264;
    frame.data = payload;
    frame.size = sizeof(payload);
    frame.pts_ms = 5678;
    frame.sequence = 9;
    frame.is_keyframe = true;

    const auto packet_result = router.OnMediaFrame(frame);
    if (!packet_result.ok()) {
        return Fail(packet_result.message.c_str());
    }

    const auto stats = sink.GetStats();
    if (stats.received_frames != 1 ||
        stats.converted_video_frames != 1 ||
        stats.dropped_frames != 0 ||
        stats.last_video_pack.u32Len != sizeof(payload) ||
        stats.last_video_pack.u64PTS != 5678 ||
        stats.last_video_pack.u32Seq != 9) {
        return Fail("cloud packet sink did not convert the packet frame");
    }

    cloud::tencent::TencentLiveStream live;
    cloud::tencent::TencentLiveSession session;
    session.visitor = 11;
    if (live.Start(session) != cloud::tencent::TencentCloudStatusCode::Ok) {
        return Fail("live stream start failed");
    }

    cloud::CloudPacketSink live_sink(&live);
    if (!cloud::BindCloudLiveToPacket(router, live_sink).ok()) {
        return Fail("bind live cloud sink failed");
    }
    if (!router.OnMediaFrame(frame).ok()) {
        return Fail("publish frame to live cloud sink failed");
    }
    if (live.GetStats().sent_video_frames != 1 ||
        live.GetStats().sent_video_bytes != sizeof(payload)) {
        return Fail("cloud packet sink did not forward frame to live stream");
    }

    cloud::tencent::TencentCloudStorage cloud_storage;
    cloud::tencent::TencentCloudStorageSession cloud_storage_session;
    cloud_storage_session.event_id = 12;
    if (cloud_storage.StartEvent(cloud_storage_session) !=
        cloud::tencent::TencentCloudStatusCode::Ok) {
        return Fail("cloud storage start failed");
    }

    cloud::CloudPacketSink storage_sink(nullptr, &cloud_storage);
    if (!cloud::BindCloudStorageToPacket(router, storage_sink).ok()) {
        return Fail("bind cloud storage sink failed");
    }
    if (!router.OnMediaFrame(frame).ok()) {
        return Fail("publish frame to cloud storage sink failed");
    }
    if (live.GetStats().sent_video_frames != 2 ||
        cloud_storage.GetStats().sent_video_frames != 1 ||
        cloud_storage.GetStats().sent_video_bytes != sizeof(payload)) {
        return Fail("cloud live and cloud storage sinks must receive independently");
    }

    std::cout << "cloud packet sink contract passed\n";
    return 0;
}

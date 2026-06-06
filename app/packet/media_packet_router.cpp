#include "media_packet_router.hpp"

namespace aov::app::packet {

MediaPacketRouter::MediaPacketRouter(IPacketService& packet_service)
    : packet_service_(packet_service) {
}

PacketResult MediaPacketRouter::BindStorageSink(PacketFrameCallback callback) {
    return packet_service_.Subscribe(PacketConsumerId::Storage, std::move(callback));
}

PacketResult MediaPacketRouter::BindCloudSink(PacketFrameCallback callback) {
    return packet_service_.Subscribe(PacketConsumerId::Cloud, std::move(callback));
}

void MediaPacketRouter::UnbindStorageSink() {
    packet_service_.Unsubscribe(PacketConsumerId::Storage);
}

void MediaPacketRouter::UnbindCloudSink() {
    packet_service_.Unsubscribe(PacketConsumerId::Cloud);
}

PacketResult MediaPacketRouter::OnMediaFrame(const aov::media::StreamFrame& frame) {
    return packet_service_.PublishFrame(frame);
}

} // namespace aov::app::packet

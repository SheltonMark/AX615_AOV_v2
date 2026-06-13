#include "media_packet_router.hpp"

namespace aov::app::packet {

MediaPacketRouter::MediaPacketRouter(IPacketService& packet_service)
    : packet_service_(packet_service) {
}

PacketResult MediaPacketRouter::BindStorageSink(PacketFrameCallback callback) {
    return packet_service_.Subscribe(PacketConsumerId::Storage, std::move(callback));
}

PacketResult MediaPacketRouter::BindCloudSink(PacketFrameCallback callback) {
    return BindCloudLiveSink(std::move(callback));
}

PacketResult MediaPacketRouter::BindCloudLiveSink(PacketFrameCallback callback) {
    return packet_service_.Subscribe(PacketConsumerId::CloudLive, std::move(callback));
}

PacketResult MediaPacketRouter::BindCloudStorageSink(PacketFrameCallback callback) {
    return packet_service_.Subscribe(PacketConsumerId::CloudStorage, std::move(callback));
}

void MediaPacketRouter::UnbindStorageSink() {
    packet_service_.Unsubscribe(PacketConsumerId::Storage);
}

void MediaPacketRouter::UnbindCloudSink() {
    UnbindCloudLiveSink();
}

void MediaPacketRouter::UnbindCloudLiveSink() {
    packet_service_.Unsubscribe(PacketConsumerId::CloudLive);
}

void MediaPacketRouter::UnbindCloudStorageSink() {
    packet_service_.Unsubscribe(PacketConsumerId::CloudStorage);
}

PacketResult MediaPacketRouter::OnMediaFrame(const aov::media::StreamFrame& frame) {
    return packet_service_.PublishFrame(frame);
}

} // namespace aov::app::packet

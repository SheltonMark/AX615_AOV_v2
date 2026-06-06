#pragma once

#include "i_packet_service.hpp"

namespace aov::app::packet {

class MediaPacketRouter {
public:
    explicit MediaPacketRouter(IPacketService& packet_service);

    PacketResult BindStorageSink(PacketFrameCallback callback);
    PacketResult BindCloudSink(PacketFrameCallback callback);
    void UnbindStorageSink();
    void UnbindCloudSink();

    PacketResult OnMediaFrame(const aov::media::StreamFrame& frame);

private:
    IPacketService& packet_service_;
};

} // namespace aov::app::packet

#pragma once

#include <functional>

#include "packet_types.hpp"

namespace aov::app::packet {

using PacketFrameCallback = std::function<void(const PacketFrame&)>;

class IPacketService {
public:
    virtual ~IPacketService() = default;

    virtual PacketResult Init(const PacketConfig& config) = 0;
    virtual void Deinit() = 0;

    virtual PacketResult Subscribe(PacketConsumerId consumer,
                                   PacketFrameCallback callback) = 0;
    virtual void Unsubscribe(PacketConsumerId consumer) = 0;

    virtual PacketResult PublishFrame(const aov::media::StreamFrame& frame) = 0;
    virtual PacketStats GetStats() const = 0;
};

} // namespace aov::app::packet

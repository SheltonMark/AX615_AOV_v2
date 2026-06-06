#pragma once

#include <mutex>
#include <unordered_map>

#include "i_packet_service.hpp"

namespace aov::app::packet {

class PacketServiceStub final : public IPacketService {
public:
    PacketServiceStub() = default;
    ~PacketServiceStub() override;

    PacketResult Init(const PacketConfig& config) override;
    void Deinit() override;

    PacketResult Subscribe(PacketConsumerId consumer,
                           PacketFrameCallback callback) override;
    void Unsubscribe(PacketConsumerId consumer) override;

    PacketResult PublishFrame(const aov::media::StreamFrame& frame) override;
    PacketStats GetStats() const override;

private:
    PacketFrame CopyFrame(const aov::media::StreamFrame& frame) const;

    mutable std::mutex mutex_;
    bool initialized_ {false};
    PacketConfig config_;
    PacketStats stats_;
    std::unordered_map<PacketConsumerId, PacketFrameCallback> subscribers_;
};

} // namespace aov::app::packet

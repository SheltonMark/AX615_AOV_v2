#pragma once

#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "i_packet_service.hpp"

namespace aov::app::packet {

class PacketService final : public IPacketService {
public:
    PacketService() = default;
    ~PacketService() override;

    PacketResult Init(const PacketConfig& config) override;
    void Deinit() override;

    PacketResult Subscribe(PacketConsumerId consumer,
                           PacketFrameCallback callback) override;
    void Unsubscribe(PacketConsumerId consumer) override;

    PacketResult PublishFrame(const aov::media::StreamFrame& frame) override;
    PacketStats GetStats() const override;

    PacketResult SetConsumerConfig(PacketConsumerId consumer,
                                   const PacketConsumerConfig& config);
    PacketConsumerConfig GetConsumerConfig(PacketConsumerId consumer) const;
    PacketConsumerStats GetConsumerStats(PacketConsumerId consumer) const;
    PacketResult SetPressureCallback(PacketPressureCallback callback);
    PacketResult Dispatch(PacketConsumerId consumer, std::uint32_t max_frames);
    PacketResult AcquireBuffer(std::uint32_t size, PacketWritableBuffer& out);
    PacketResult PublishBuffer(PacketWritableBuffer& buffer,
                               const PacketFrameInfo& info);
    void ReleaseBuffer(PacketWritableBuffer& buffer);
    bool HasCapacity(std::uint32_t size) const;
    std::string DumpStats() const;

private:
    struct ConsumerState {
        PacketFrameCallback callback;
        PacketConsumerConfig config;
        PacketConsumerStats stats;
        std::deque<PacketFrame> queue;
    };

    struct PoolState {
        std::mutex mutex;
        std::uint32_t max_buffers {0};
        std::uint32_t buffer_size {0};
        std::uint32_t allocated_buffers {0};
        std::deque<std::unique_ptr<std::vector<std::uint8_t>>> available;
    };

    static PacketConsumerId NormalizeConsumer(PacketConsumerId consumer);
    static const char* ConsumerName(PacketConsumerId consumer);
    static PacketConsumerConfig MakeDefaultConsumerConfig(PacketConsumerId consumer,
                                                         const PacketConfig& global);
    PacketFrame CopyFrame(const aov::media::StreamFrame& frame) const;
    PacketFrame MakeFrameFromBuffer(const PacketWritableBuffer& buffer,
                                    const PacketFrameInfo& info) const;
    std::shared_ptr<std::vector<std::uint8_t>> AcquirePayload(std::uint32_t size) const;
    PacketResult PublishPacketFrame(const PacketFrame& packet_frame);
    PacketResult EnqueueForConsumer(PacketConsumerId consumer,
                                    ConsumerState& state,
                                    const PacketFrame& frame);
    void DropFront(ConsumerState& state);
    void NotifyPressureLocked(PacketConsumerId consumer,
                              PacketPressureEventType type,
                              PacketStatusCode status,
                              const std::string& reason) const;

    mutable std::mutex mutex_;
    PacketConfig config_ {};
    PacketStats stats_ {};
    bool initialized_ {false};
    std::unordered_map<PacketConsumerId, ConsumerState> consumers_;
    std::shared_ptr<PoolState> pool_;
    PacketPressureCallback pressure_callback_;
};

} // namespace aov::app::packet

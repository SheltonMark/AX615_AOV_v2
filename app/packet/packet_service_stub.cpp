#include "packet_service_stub.hpp"

#include <algorithm>

namespace aov::app::packet {

PacketServiceStub::~PacketServiceStub() {
    Deinit();
}

PacketResult PacketServiceStub::Init(const PacketConfig& config) {
    if (config.max_frames == 0 || config.max_bytes == 0) {
        return PacketResult::Error(PacketStatusCode::InvalidArgument,
                                   "packet buffer limits must be non-zero");
    }

    std::lock_guard<std::mutex> lock(mutex_);
    config_ = config;
    stats_ = {};
    subscribers_.clear();
    initialized_ = true;
    return PacketResult::Ok();
}

void PacketServiceStub::Deinit() {
    std::lock_guard<std::mutex> lock(mutex_);
    subscribers_.clear();
    initialized_ = false;
}

PacketResult PacketServiceStub::Subscribe(PacketConsumerId consumer,
                                          PacketFrameCallback callback) {
    if (!callback || consumer == PacketConsumerId::Unknown) {
        return PacketResult::Error(PacketStatusCode::InvalidArgument,
                                   "invalid packet subscriber");
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (!initialized_) {
        return PacketResult::Error(PacketStatusCode::InvalidState,
                                   "packet service is not initialized");
    }

    subscribers_[consumer] = std::move(callback);
    stats_.subscriber_count = static_cast<std::uint32_t>(subscribers_.size());
    return PacketResult::Ok();
}

void PacketServiceStub::Unsubscribe(PacketConsumerId consumer) {
    std::lock_guard<std::mutex> lock(mutex_);
    subscribers_.erase(consumer);
    stats_.subscriber_count = static_cast<std::uint32_t>(subscribers_.size());
}

PacketResult PacketServiceStub::PublishFrame(const aov::media::StreamFrame& frame) {
    if (frame.data == nullptr || frame.size == 0) {
        return PacketResult::Error(PacketStatusCode::InvalidArgument,
                                   "empty media frame");
    }

    std::unordered_map<PacketConsumerId, PacketFrameCallback> subscribers;
    PacketFrame packet_frame;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!initialized_) {
            return PacketResult::Error(PacketStatusCode::InvalidState,
                                       "packet service is not initialized");
        }
        if (subscribers_.empty()) {
            ++stats_.dropped_frames;
            return PacketResult::Error(PacketStatusCode::NoSubscriber,
                                       "no packet subscriber");
        }
        if (frame.size > config_.max_bytes) {
            ++stats_.dropped_frames;
            return PacketResult::Error(PacketStatusCode::Backpressure,
                                       "frame exceeds packet buffer limit");
        }

        packet_frame = CopyFrame(frame);
        subscribers = subscribers_;
        ++stats_.published_frames;
        stats_.buffered_frames = 1;
        stats_.buffered_bytes = packet_frame.size();
    }

    for (const auto& entry : subscribers) {
        entry.second(packet_frame);
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        stats_.delivered_frames += static_cast<std::uint64_t>(subscribers.size());
        stats_.buffered_frames = 0;
        stats_.buffered_bytes = 0;
    }

    return PacketResult::Ok();
}

PacketStats PacketServiceStub::GetStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    PacketStats stats = stats_;
    stats.subscriber_count = static_cast<std::uint32_t>(subscribers_.size());
    return stats;
}

PacketFrame PacketServiceStub::CopyFrame(const aov::media::StreamFrame& frame) const {
    auto payload = std::make_shared<std::vector<std::uint8_t>>(frame.data,
                                                               frame.data + frame.size);

    PacketFrame packet_frame;
    packet_frame.chn_id = frame.chn_id;
    packet_frame.codec = frame.codec;
    packet_frame.pts_ms = frame.pts_ms;
    packet_frame.sequence = frame.sequence;
    packet_frame.is_keyframe = frame.is_keyframe;
    packet_frame.payload = std::move(payload);
    return packet_frame;
}

} // namespace aov::app::packet

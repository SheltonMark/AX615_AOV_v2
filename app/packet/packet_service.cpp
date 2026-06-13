#include "app/packet/packet_service.hpp"

#include <algorithm>
#include <sstream>

namespace aov::app::packet {

PacketService::~PacketService() {
    Deinit();
}

PacketResult PacketService::Init(const PacketConfig& config) {
    if (config.max_frames == 0 ||
        config.max_bytes == 0 ||
        config.pool_buffer_count == 0 ||
        config.pool_buffer_size == 0) {
        return PacketResult::Error(PacketStatusCode::InvalidArgument,
                                   "packet buffer limits must be non-zero");
    }

    std::lock_guard<std::mutex> lock(mutex_);
    config_ = config;
    stats_ = {};
    consumers_.clear();
    pool_ = std::make_shared<PoolState>();
    pool_->max_buffers = config.pool_buffer_count;
    pool_->buffer_size = config.pool_buffer_size;
    initialized_ = true;
    return PacketResult::Ok();
}

void PacketService::Deinit() {
    std::lock_guard<std::mutex> lock(mutex_);
    consumers_.clear();
    pool_.reset();
    initialized_ = false;
}

PacketResult PacketService::Subscribe(PacketConsumerId consumer,
                                      PacketFrameCallback callback) {
    consumer = NormalizeConsumer(consumer);
    if (!callback || consumer == PacketConsumerId::Unknown) {
        return PacketResult::Error(PacketStatusCode::InvalidArgument,
                                   "invalid packet subscriber");
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (!initialized_) {
        return PacketResult::Error(PacketStatusCode::InvalidState,
                                   "packet service is not initialized");
    }

    ConsumerState state;
    state.callback = std::move(callback);
    state.config = MakeDefaultConsumerConfig(consumer, config_);
    consumers_[consumer] = std::move(state);
    stats_.subscriber_count = static_cast<std::uint32_t>(consumers_.size());
    stats_.active_consumers = stats_.subscriber_count;
    return PacketResult::Ok();
}

void PacketService::Unsubscribe(PacketConsumerId consumer) {
    consumer = NormalizeConsumer(consumer);
    std::lock_guard<std::mutex> lock(mutex_);
    consumers_.erase(consumer);
    stats_.subscriber_count = static_cast<std::uint32_t>(consumers_.size());
    stats_.active_consumers = stats_.subscriber_count;
}

PacketResult PacketService::PublishFrame(const aov::media::StreamFrame& frame) {
    if (frame.data == nullptr || frame.size == 0) {
        return PacketResult::Error(PacketStatusCode::InvalidArgument,
                                   "empty media frame");
    }

    PacketFrame packet_frame;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!initialized_) {
            return PacketResult::Error(PacketStatusCode::InvalidState,
                                       "packet service is not initialized");
        }
        if (frame.size > config_.pool_buffer_size) {
            return PacketResult::Error(PacketStatusCode::Backpressure,
                                       "frame exceeds packet buffer size");
        }
        if (consumers_.empty()) {
            ++stats_.dropped_frames;
            stats_.dropped_bytes += frame.size;
            return PacketResult::Error(PacketStatusCode::NoSubscriber,
                                       "no packet subscriber");
        }

        packet_frame = CopyFrame(frame);
        if (!packet_frame.payload) {
            ++stats_.dropped_frames;
            stats_.dropped_bytes += frame.size;
            NotifyPressureLocked(PacketConsumerId::Unknown,
                                 PacketPressureEventType::PoolExhausted,
                                 PacketStatusCode::Backpressure,
                                 "packet buffer pool exhausted");
            return PacketResult::Error(PacketStatusCode::Backpressure,
                                       "packet buffer pool exhausted");
        }
    }

    return PublishPacketFrame(packet_frame);
}

PacketStats PacketService::GetStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    PacketStats stats = stats_;
    stats.subscriber_count = static_cast<std::uint32_t>(consumers_.size());
    stats.active_consumers = stats.subscriber_count;
    stats.buffered_frames = 0;
    stats.buffered_bytes = 0;
    for (const auto& entry : consumers_) {
        stats.buffered_frames += entry.second.stats.queued_frames;
        stats.buffered_bytes += entry.second.stats.queued_bytes;
    }
    return stats;
}

PacketResult PacketService::SetConsumerConfig(PacketConsumerId consumer,
                                              const PacketConsumerConfig& config) {
    consumer = NormalizeConsumer(consumer);
    if (consumer == PacketConsumerId::Unknown ||
        config.max_frames == 0 ||
        config.max_bytes == 0) {
        return PacketResult::Error(PacketStatusCode::InvalidArgument,
                                   "invalid packet consumer config");
    }

    std::lock_guard<std::mutex> lock(mutex_);
    auto it = consumers_.find(consumer);
    if (it == consumers_.end()) {
        return PacketResult::Error(PacketStatusCode::InvalidState,
                                   "packet consumer is not subscribed");
    }
    it->second.config = config;
    while (it->second.stats.queued_frames > config.max_frames ||
           it->second.stats.queued_bytes > config.max_bytes) {
        DropFront(it->second);
        ++stats_.dropped_frames;
    }
    return PacketResult::Ok();
}

PacketConsumerStats PacketService::GetConsumerStats(PacketConsumerId consumer) const {
    consumer = NormalizeConsumer(consumer);
    std::lock_guard<std::mutex> lock(mutex_);
    const auto it = consumers_.find(consumer);
    if (it == consumers_.end()) {
        return {};
    }
    return it->second.stats;
}

PacketConsumerConfig PacketService::GetConsumerConfig(PacketConsumerId consumer) const {
    consumer = NormalizeConsumer(consumer);
    std::lock_guard<std::mutex> lock(mutex_);
    const auto it = consumers_.find(consumer);
    if (it == consumers_.end()) {
        return {};
    }
    return it->second.config;
}

PacketResult PacketService::SetPressureCallback(PacketPressureCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    pressure_callback_ = std::move(callback);
    return PacketResult::Ok();
}

PacketResult PacketService::Dispatch(PacketConsumerId consumer,
                                     const std::uint32_t max_frames) {
    consumer = NormalizeConsumer(consumer);
    if (consumer == PacketConsumerId::Unknown) {
        return PacketResult::Error(PacketStatusCode::InvalidArgument,
                                   "invalid packet consumer");
    }
    if (max_frames == 0) {
        return PacketResult::Ok();
    }

    std::deque<PacketFrame> frames;
    PacketFrameCallback callback;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = consumers_.find(consumer);
        if (it == consumers_.end()) {
            return PacketResult::Error(PacketStatusCode::InvalidState,
                                       "packet consumer is not subscribed");
        }

        ConsumerState& state = it->second;
        callback = state.callback;
        while (!state.queue.empty() &&
               frames.size() < max_frames) {
            PacketFrame frame = state.queue.front();
            state.queue.pop_front();
            state.stats.queued_frames -= 1;
            state.stats.queued_bytes -= frame.size();
            frames.push_back(std::move(frame));
        }
    }

    for (const auto& frame : frames) {
        callback(frame);
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = consumers_.find(consumer);
        if (it != consumers_.end()) {
            it->second.stats.delivered_frames += frames.size();
        }
        stats_.delivered_frames += frames.size();
        if (!frames.empty()) {
            NotifyPressureLocked(PacketConsumerId::Unknown,
                                 PacketPressureEventType::PoolRecovered,
                                 PacketStatusCode::Ok,
                                 "packet buffer released after dispatch");
        }
    }

    return PacketResult::Ok();
}

PacketResult PacketService::AcquireBuffer(const std::uint32_t size,
                                          PacketWritableBuffer& out) {
    out = {};
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!initialized_) {
            return PacketResult::Error(PacketStatusCode::InvalidState,
                                       "packet service is not initialized");
        }
    }

    auto payload = AcquirePayload(size);
    if (!payload) {
        std::lock_guard<std::mutex> lock(mutex_);
        NotifyPressureLocked(PacketConsumerId::Unknown,
                             PacketPressureEventType::PoolExhausted,
                             PacketStatusCode::Backpressure,
                             "packet buffer pool exhausted");
        return PacketResult::Error(PacketStatusCode::Backpressure,
                                   "packet buffer pool exhausted");
    }

    out.payload = std::move(payload);
    out.data = out.payload->data();
    out.capacity = static_cast<std::uint32_t>(out.payload->size());
    out.size = 0;
    return PacketResult::Ok();
}

PacketResult PacketService::PublishBuffer(PacketWritableBuffer& buffer,
                                          const PacketFrameInfo& info) {
    if (!buffer.payload || buffer.data == nullptr || buffer.size == 0) {
        return PacketResult::Error(PacketStatusCode::InvalidArgument,
                                   "invalid writable packet buffer");
    }
    if (buffer.size > buffer.capacity) {
        return PacketResult::Error(PacketStatusCode::InvalidArgument,
                                   "writable packet buffer size exceeds capacity");
    }

    PacketFrame frame = MakeFrameFromBuffer(buffer, info);
    buffer = {};
    return PublishPacketFrame(frame);
}

void PacketService::ReleaseBuffer(PacketWritableBuffer& buffer) {
    buffer = {};
    std::lock_guard<std::mutex> lock(mutex_);
    NotifyPressureLocked(PacketConsumerId::Unknown,
                         PacketPressureEventType::PoolRecovered,
                         PacketStatusCode::Ok,
                         "packet writable buffer released");
}

bool PacketService::HasCapacity(const std::uint32_t size) const {
    if (!pool_ || size > pool_->buffer_size) {
        return false;
    }
    std::lock_guard<std::mutex> lock(pool_->mutex);
    return !pool_->available.empty() ||
        pool_->allocated_buffers < pool_->max_buffers;
}

std::string PacketService::DumpStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ostringstream oss;
    PacketStats stats = stats_;
    stats.subscriber_count = static_cast<std::uint32_t>(consumers_.size());
    stats.active_consumers = stats.subscriber_count;
    for (const auto& entry : consumers_) {
        stats.buffered_frames += entry.second.stats.queued_frames;
        stats.buffered_bytes += entry.second.stats.queued_bytes;
    }

    oss << "packet"
        << " published=" << stats.published_frames
        << " delivered=" << stats.delivered_frames
        << " dropped=" << stats.dropped_frames
        << " buffered=" << stats.buffered_frames
        << " consumers=" << stats.active_consumers;
    for (const auto& entry : consumers_) {
        const PacketConsumerStats& consumer = entry.second.stats;
        oss << "\nconsumer=" << ConsumerName(entry.first)
            << " queued=" << consumer.queued_frames
            << " delivered=" << consumer.delivered_frames
            << " dropped=" << consumer.dropped_frames
            << " waiting_keyframe=" << (consumer.waiting_keyframe ? 1 : 0);
    }
    return oss.str();
}

PacketConsumerId PacketService::NormalizeConsumer(const PacketConsumerId consumer) {
    if (consumer == PacketConsumerId::Cloud) {
        return PacketConsumerId::CloudLive;
    }
    return consumer;
}

const char* PacketService::ConsumerName(const PacketConsumerId consumer) {
    switch (NormalizeConsumer(consumer)) {
        case PacketConsumerId::Storage:
            return "Storage";
        case PacketConsumerId::CloudLive:
            return "CloudLive";
        case PacketConsumerId::CloudStorage:
            return "CloudStorage";
        case PacketConsumerId::Preview:
            return "Preview";
        case PacketConsumerId::Snapshot:
            return "Snapshot";
        case PacketConsumerId::Debug:
            return "Debug";
        case PacketConsumerId::Cloud:
            return "CloudLive";
        case PacketConsumerId::Unknown:
            return "Unknown";
    }
    return "Unknown";
}

PacketConsumerConfig PacketService::MakeDefaultConsumerConfig(
    const PacketConsumerId consumer,
    const PacketConfig& global) {
    PacketConsumerConfig config;
    config.max_frames = global.max_frames;
    config.max_bytes = global.max_bytes;
    config.drop_non_keyframe_first = global.drop_non_keyframe_first;
    config.wait_next_keyframe_after_drop = global.wait_next_keyframe_after_drop;

    switch (NormalizeConsumer(consumer)) {
        case PacketConsumerId::Storage:
            config.max_frames = std::max<std::uint32_t>(global.max_frames, 128);
            config.max_bytes = std::max<std::uint32_t>(global.max_bytes, 8 * 1024 * 1024);
            config.drop_non_keyframe_first = false;
            config.wait_next_keyframe_after_drop = false;
            break;
        case PacketConsumerId::CloudLive:
            config.max_frames = std::min<std::uint32_t>(global.max_frames, 8);
            config.max_bytes = std::min<std::uint32_t>(global.max_bytes, 1 * 1024 * 1024);
            config.drop_non_keyframe_first = true;
            config.wait_next_keyframe_after_drop = true;
            break;
        case PacketConsumerId::CloudStorage:
            config.max_frames = std::min<std::uint32_t>(
                std::max<std::uint32_t>(global.max_frames, 64), 64);
            config.max_bytes = std::min<std::uint32_t>(
                std::max<std::uint32_t>(global.max_bytes, 4 * 1024 * 1024),
                4 * 1024 * 1024);
            config.drop_non_keyframe_first = true;
            config.wait_next_keyframe_after_drop = true;
            break;
        case PacketConsumerId::Preview:
        case PacketConsumerId::Snapshot:
        case PacketConsumerId::Debug:
        case PacketConsumerId::Cloud:
        case PacketConsumerId::Unknown:
            break;
    }

    config.max_frames = std::max<std::uint32_t>(config.max_frames, 1);
    config.max_bytes = std::max<std::uint32_t>(config.max_bytes, 1);
    return config;
}

PacketFrame PacketService::CopyFrame(const aov::media::StreamFrame& frame) const {
    auto payload = AcquirePayload(frame.size);
    if (!payload) {
        return {};
    }
    std::copy(frame.data, frame.data + frame.size, payload->begin());
    payload->resize(frame.size);

    PacketFrame packet_frame;
    packet_frame.media_type = PacketMediaType::Video;
    packet_frame.chn_id = frame.chn_id;
    packet_frame.codec = frame.codec;
    packet_frame.pts_ms = frame.pts_ms;
    packet_frame.sequence = frame.sequence;
    packet_frame.is_keyframe = frame.is_keyframe;
    packet_frame.payload = std::move(payload);
    return packet_frame;
}

PacketFrame PacketService::MakeFrameFromBuffer(const PacketWritableBuffer& buffer,
                                               const PacketFrameInfo& info) const {
    buffer.payload->resize(buffer.size);

    PacketFrame frame;
    frame.media_type = info.media_type;
    frame.chn_id = info.chn_id;
    frame.codec = info.codec;
    frame.audio_codec = info.audio_codec;
    frame.sample_rate = info.sample_rate;
    frame.channels = info.channels;
    frame.bit_width = info.bit_width;
    frame.pts_ms = info.pts_ms;
    frame.sequence = info.sequence;
    frame.is_keyframe = info.is_keyframe;
    frame.payload = buffer.payload;
    return frame;
}

std::shared_ptr<std::vector<std::uint8_t>> PacketService::AcquirePayload(
    const std::uint32_t size) const {
    if (!pool_ || size > pool_->buffer_size) {
        return {};
    }

    std::unique_ptr<std::vector<std::uint8_t>> buffer;
    {
        std::lock_guard<std::mutex> lock(pool_->mutex);
        if (!pool_->available.empty()) {
            buffer = std::move(pool_->available.front());
            pool_->available.pop_front();
        } else if (pool_->allocated_buffers < pool_->max_buffers) {
            buffer.reset(new std::vector<std::uint8_t>(pool_->buffer_size));
            ++pool_->allocated_buffers;
        } else {
            return {};
        }
    }

    buffer->resize(pool_->buffer_size);
    std::shared_ptr<PoolState> pool = pool_;
    return std::shared_ptr<std::vector<std::uint8_t>>(
        buffer.release(),
        [pool](std::vector<std::uint8_t>* released) {
            if (!released || !pool) {
                delete released;
                return;
            }
            std::unique_ptr<std::vector<std::uint8_t>> owned(released);
            std::lock_guard<std::mutex> lock(pool->mutex);
            pool->available.push_back(std::move(owned));
        });
}

PacketResult PacketService::PublishPacketFrame(const PacketFrame& packet_frame) {
    if (!packet_frame.payload || packet_frame.size() == 0) {
        return PacketResult::Error(PacketStatusCode::InvalidArgument,
                                   "empty packet frame");
    }

    std::deque<PacketConsumerId> dispatch_consumers;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!initialized_) {
            return PacketResult::Error(PacketStatusCode::InvalidState,
                                       "packet service is not initialized");
        }
        if (consumers_.empty()) {
            ++stats_.dropped_frames;
            stats_.dropped_bytes += packet_frame.size();
            return PacketResult::Error(PacketStatusCode::NoSubscriber,
                                       "no packet subscriber");
        }

        ++stats_.published_frames;
        stats_.published_bytes += packet_frame.size();
        for (auto& entry : consumers_) {
            const PacketResult result = EnqueueForConsumer(entry.first,
                                                           entry.second,
                                                           packet_frame);
            if (!result.ok()) {
                ++stats_.dropped_frames;
                stats_.dropped_bytes += packet_frame.size();
            }
            dispatch_consumers.push_back(entry.first);
        }
        stats_.subscriber_count = static_cast<std::uint32_t>(consumers_.size());
        stats_.active_consumers = stats_.subscriber_count;
    }

    if (config_.auto_dispatch) {
        for (const PacketConsumerId consumer : dispatch_consumers) {
            (void)Dispatch(consumer, 1);
        }
    }

    return PacketResult::Ok();
}

PacketResult PacketService::EnqueueForConsumer(PacketConsumerId consumer,
                                               ConsumerState& state,
                                               const PacketFrame& frame) {
    if (state.stats.waiting_keyframe && !frame.is_keyframe) {
        ++state.stats.dropped_frames;
        state.stats.dropped_bytes += frame.size();
        NotifyPressureLocked(consumer,
                             PacketPressureEventType::ConsumerOverflow,
                             PacketStatusCode::Backpressure,
                             "consumer waits for keyframe");
        return PacketResult::Error(PacketStatusCode::Backpressure,
                                   "consumer waits for keyframe");
    }
    if (state.stats.waiting_keyframe && frame.is_keyframe) {
        state.stats.waiting_keyframe = false;
        NotifyPressureLocked(consumer,
                             PacketPressureEventType::ConsumerRecovered,
                             PacketStatusCode::Ok,
                             "consumer recovered on keyframe");
    }

    while (state.stats.queued_frames >= state.config.max_frames ||
           state.stats.queued_bytes + frame.size() > state.config.max_bytes) {
        if (state.queue.empty()) {
            ++state.stats.dropped_frames;
            state.stats.dropped_bytes += frame.size();
            if (state.config.wait_next_keyframe_after_drop && !frame.is_keyframe) {
                state.stats.waiting_keyframe = true;
            }
            NotifyPressureLocked(consumer,
                                 PacketPressureEventType::ConsumerOverflow,
                                 PacketStatusCode::Backpressure,
                                 "consumer queue is full");
            return PacketResult::Error(PacketStatusCode::Backpressure,
                                       "consumer queue is full");
        }
        DropFront(state);
        if (state.config.wait_next_keyframe_after_drop) {
            state.stats.waiting_keyframe = true;
        }
        NotifyPressureLocked(consumer,
                             PacketPressureEventType::ConsumerOverflow,
                             PacketStatusCode::Backpressure,
                             "consumer queue overflow dropped front frame");
    }

    if (state.stats.waiting_keyframe && !frame.is_keyframe) {
        ++state.stats.dropped_frames;
        state.stats.dropped_bytes += frame.size();
        NotifyPressureLocked(consumer,
                             PacketPressureEventType::ConsumerOverflow,
                             PacketStatusCode::Backpressure,
                             "consumer waits for keyframe after drop");
        return PacketResult::Error(PacketStatusCode::Backpressure,
                                   "consumer waits for keyframe after drop");
    }
    if (state.stats.waiting_keyframe && frame.is_keyframe) {
        state.stats.waiting_keyframe = false;
        NotifyPressureLocked(consumer,
                             PacketPressureEventType::ConsumerRecovered,
                             PacketStatusCode::Ok,
                             "consumer recovered on keyframe after drop");
    }

    state.queue.push_back(frame);
    ++state.stats.enqueued_frames;
    ++state.stats.queued_frames;
    state.stats.queued_bytes += frame.size();
    return PacketResult::Ok();
}

void PacketService::DropFront(ConsumerState& state) {
    if (state.queue.empty()) {
        return;
    }
    const PacketFrame dropped = state.queue.front();
    state.queue.pop_front();
    --state.stats.queued_frames;
    state.stats.queued_bytes -= dropped.size();
    ++state.stats.dropped_frames;
    state.stats.dropped_bytes += dropped.size();
}

void PacketService::NotifyPressureLocked(const PacketConsumerId consumer,
                                         const PacketPressureEventType type,
                                         const PacketStatusCode status,
                                         const std::string& reason) const {
    if (!pressure_callback_) {
        return;
    }

    PacketPressureEvent event;
    event.consumer = NormalizeConsumer(consumer);
    event.type = type;
    event.status = status;
    event.reason = reason;
    pressure_callback_(event);
}

} // namespace aov::app::packet

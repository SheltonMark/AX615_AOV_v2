#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "common/media_types.hpp"

namespace aov::app::packet {

enum class PacketStatusCode {
    Ok,
    InvalidArgument,
    InvalidState,
    NoSubscriber,
    Backpressure,
    NoMemory,
    InternalError,
};

enum class PacketConsumerId {
    Storage,
    CloudLive,
    CloudStorage,
    // Backward-compatible alias for older code paths that only had one cloud sink.
    Cloud,
    Preview,
    Snapshot,
    Debug,
    Unknown,
};

enum class PacketMediaType {
    Video,
    Audio,
    Metadata,
};

enum class PacketAudioCodec {
    G711A,
    G711U,
    AAC,
    PCM,
    Unknown,
};

enum class PacketPressureEventType {
    PoolExhausted,
    PoolRecovered,
    ConsumerOverflow,
    ConsumerRecovered,
};

struct PacketConfig {
    std::uint32_t max_frames {128};
    std::uint32_t max_bytes {4 * 1024 * 1024};
    std::uint32_t pool_buffer_count {128};
    std::uint32_t pool_buffer_size {4 * 1024 * 1024};
    bool drop_non_keyframe_first {true};
    bool wait_next_keyframe_after_drop {true};
    bool auto_dispatch {true};
};

struct PacketConsumerConfig {
    std::uint32_t max_frames {64};
    std::uint32_t max_bytes {2 * 1024 * 1024};
    bool drop_non_keyframe_first {true};
    bool wait_next_keyframe_after_drop {true};
};

struct PacketFrame {
    PacketMediaType media_type {PacketMediaType::Video};
    int chn_id {0};
    aov::media::VideoCodec codec {aov::media::VideoCodec::H264};
    PacketAudioCodec audio_codec {PacketAudioCodec::Unknown};
    std::uint32_t sample_rate {0};
    std::uint32_t channels {0};
    std::uint32_t bit_width {0};
    std::uint64_t pts_ms {0};
    std::uint64_t sequence {0};
    bool is_keyframe {false};
    std::shared_ptr<const std::vector<std::uint8_t>> payload;

    const std::uint8_t* data() const {
        return payload && !payload->empty() ? payload->data() : nullptr;
    }

    std::uint32_t size() const {
        return payload ? static_cast<std::uint32_t>(payload->size()) : 0;
    }
};

struct PacketFrameInfo {
    PacketMediaType media_type {PacketMediaType::Video};
    int chn_id {0};
    aov::media::VideoCodec codec {aov::media::VideoCodec::H264};
    PacketAudioCodec audio_codec {PacketAudioCodec::Unknown};
    std::uint32_t sample_rate {0};
    std::uint32_t channels {0};
    std::uint32_t bit_width {0};
    std::uint64_t pts_ms {0};
    std::uint64_t sequence {0};
    bool is_keyframe {false};
};

struct PacketWritableBuffer {
    std::uint8_t* data {nullptr};
    std::uint32_t capacity {0};
    std::uint32_t size {0};
    std::shared_ptr<std::vector<std::uint8_t>> payload;
};

struct PacketStats {
    std::uint64_t published_frames {0};
    std::uint64_t published_bytes {0};
    // One frame delivered to both storage and cloud counts as two deliveries.
    std::uint64_t delivered_frames {0};
    std::uint64_t dropped_frames {0};
    std::uint64_t dropped_bytes {0};
    std::uint32_t subscriber_count {0};
    std::uint32_t active_consumers {0};
    std::uint32_t buffered_frames {0};
    std::uint32_t buffered_bytes {0};
};

struct PacketConsumerStats {
    std::uint64_t enqueued_frames {0};
    std::uint64_t delivered_frames {0};
    std::uint64_t dropped_frames {0};
    std::uint64_t dropped_bytes {0};
    std::uint32_t queued_frames {0};
    std::uint32_t queued_bytes {0};
    bool waiting_keyframe {false};
};

struct PacketPressureEvent {
    PacketConsumerId consumer {PacketConsumerId::Unknown};
    PacketPressureEventType type {PacketPressureEventType::PoolExhausted};
    PacketStatusCode status {PacketStatusCode::Ok};
    std::string reason;
};

using PacketPressureCallback = std::function<void(const PacketPressureEvent&)>;

struct PacketResult {
    PacketStatusCode code {PacketStatusCode::Ok};
    std::string message;

    static PacketResult Ok() {
        return {};
    }

    static PacketResult Error(PacketStatusCode status, std::string msg) {
        PacketResult result;
        result.code = status;
        result.message = std::move(msg);
        return result;
    }

    bool ok() const {
        return code == PacketStatusCode::Ok;
    }
};

} // namespace aov::app::packet

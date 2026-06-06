#pragma once

#include <cstdint>
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
    Cloud,
    Preview,
    Snapshot,
    Unknown,
};

struct PacketConfig {
    std::uint32_t max_frames {128};
    std::uint32_t max_bytes {4 * 1024 * 1024};
    bool drop_non_keyframe_first {true};
};

struct PacketFrame {
    int chn_id {0};
    aov::media::VideoCodec codec {aov::media::VideoCodec::H264};
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

struct PacketStats {
    std::uint64_t published_frames {0};
    // One frame delivered to both storage and cloud counts as two deliveries.
    std::uint64_t delivered_frames {0};
    std::uint64_t dropped_frames {0};
    std::uint32_t subscriber_count {0};
    std::uint32_t buffered_frames {0};
    std::uint32_t buffered_bytes {0};
};

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

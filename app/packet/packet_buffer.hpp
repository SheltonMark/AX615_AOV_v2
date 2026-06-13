#pragma once

#include <cstdint>
#include <memory>
#include <vector>

namespace aov::app::packet {

struct PacketBuffer {
    explicit PacketBuffer(const std::uint32_t capacity)
        : bytes(capacity) {
    }

    std::vector<std::uint8_t> bytes;
    std::uint32_t size {0};
};

using PacketBufferPtr = std::shared_ptr<PacketBuffer>;

} // namespace aov::app::packet

#pragma once

#include "../../packet/packet_types.hpp"
#include "dhfs_types.hpp"

namespace aov::app::storage::dhfs {

class IDhfsWriter {
public:
    virtual ~IDhfsWriter() = default;

    virtual DhfsResult Open(const DhfsWriterConfig& config) = 0;
    virtual DhfsResult WriteFrame(const aov::app::packet::PacketFrame& frame) = 0;
    virtual DhfsResult FlushAndClose() = 0;
    virtual DhfsWriterStats GetStats() const = 0;
};

} // namespace aov::app::storage::dhfs

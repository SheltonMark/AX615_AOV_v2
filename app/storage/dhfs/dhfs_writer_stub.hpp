#pragma once

#include <mutex>

#include "i_dhfs_writer.hpp"

namespace aov::app::storage::dhfs {

class DhfsWriterStub final : public IDhfsWriter {
public:
    DhfsResult Open(const DhfsWriterConfig& config) override;
    DhfsResult WriteFrame(const aov::app::packet::PacketFrame& frame) override;
    DhfsResult FlushAndClose() override;
    DhfsWriterStats GetStats() const override;

private:
    mutable std::mutex mutex_;
    DhfsWriterConfig config_;
    DhfsWriterStats stats_;
};

} // namespace aov::app::storage::dhfs

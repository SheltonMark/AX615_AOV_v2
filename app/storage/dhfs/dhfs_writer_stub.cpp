#include "dhfs_writer_stub.hpp"

namespace aov::app::storage::dhfs {

DhfsResult DhfsWriterStub::Open(const DhfsWriterConfig& config) {
    if (config.root_dir.empty()) {
        return DhfsResult::Error(DhfsStatusCode::InvalidArgument,
                                 "dhfs root directory is empty");
    }

    std::lock_guard<std::mutex> lock(mutex_);
    config_ = config;
    stats_ = {};
    stats_.opened = true;
    stats_.flushed = false;
    return DhfsResult::Ok();
}

DhfsResult DhfsWriterStub::WriteFrame(const aov::app::packet::PacketFrame& frame) {
    if (frame.data() == nullptr || frame.size() == 0) {
        return DhfsResult::Error(DhfsStatusCode::InvalidArgument,
                                 "empty packet frame");
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (!stats_.opened) {
        return DhfsResult::Error(DhfsStatusCode::InvalidState,
                                 "dhfs writer is not opened");
    }

    ++stats_.written_frames;
    stats_.written_bytes += frame.size();
    stats_.flushed = false;
    return DhfsResult::Ok();
}

DhfsResult DhfsWriterStub::FlushAndClose() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!stats_.opened) {
        stats_.flushed = true;
        return DhfsResult::Ok();
    }

    stats_.opened = false;
    stats_.flushed = true;
    return DhfsResult::Ok();
}

DhfsWriterStats DhfsWriterStub::GetStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return stats_;
}

} // namespace aov::app::storage::dhfs

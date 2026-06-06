#include "storage_service_stub.hpp"

#include <utility>

namespace aov::app::storage {

StorageServiceStub::StorageServiceStub(std::unique_ptr<dhfs::IDhfsWriter> writer)
    : writer_(std::move(writer)) {
}

StorageServiceStub::~StorageServiceStub() {
    Deinit();
}

StorageResult StorageServiceStub::Init(const StorageConfig& config) {
    if (!writer_) {
        return StorageResult::Error(StorageStatusCode::InvalidArgument,
                                    "storage writer is null");
    }
    if (config.mount_point.empty() || config.record_dir.empty()) {
        return StorageResult::Error(StorageStatusCode::InvalidArgument,
                                    "storage path is empty");
    }

    std::lock_guard<std::mutex> lock(mutex_);
    config_ = config;
    stats_ = {};
    stats_.flushed = true;
    initialized_ = true;
    return StorageResult::Ok();
}

void StorageServiceStub::Deinit() {
    FlushAndClose();
    std::lock_guard<std::mutex> lock(mutex_);
    initialized_ = false;
}

StorageResult StorageServiceStub::StartRecord() {
    dhfs::DhfsWriterConfig writer_config;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!initialized_) {
            return StorageResult::Error(StorageStatusCode::InvalidState,
                                        "storage service is not initialized");
        }
        writer_config = BuildWriterConfig();
    }

    const auto result = writer_->Open(writer_config);
    if (!result.ok()) {
        return FromDhfsResult(result);
    }

    std::lock_guard<std::mutex> lock(mutex_);
    stats_.recording = true;
    stats_.flushed = false;
    return StorageResult::Ok();
}

StorageResult StorageServiceStub::StopRecord() {
    return FlushAndClose();
}

StorageResult StorageServiceStub::OnPacketFrame(const aov::app::packet::PacketFrame& frame) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!initialized_) {
            return StorageResult::Error(StorageStatusCode::InvalidState,
                                        "storage service is not initialized");
        }
        if (!stats_.recording) {
            ++stats_.dropped_frames;
            return StorageResult::Error(StorageStatusCode::InvalidState,
                                        "storage is not recording");
        }
        ++stats_.received_frames;
    }

    const auto result = writer_->WriteFrame(frame);

    std::lock_guard<std::mutex> lock(mutex_);
    if (!result.ok()) {
        ++stats_.dropped_frames;
        return FromDhfsResult(result);
    }

    const auto writer_stats = writer_->GetStats();
    stats_.written_frames = writer_stats.written_frames;
    stats_.written_bytes = writer_stats.written_bytes;
    stats_.flushed = writer_stats.flushed;
    return StorageResult::Ok();
}

StorageResult StorageServiceStub::FlushAndClose() {
    dhfs::IDhfsWriter* writer = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        writer = writer_.get();
    }

    if (writer == nullptr) {
        return StorageResult::Ok();
    }

    const auto result = writer->FlushAndClose();
    std::lock_guard<std::mutex> lock(mutex_);
    stats_.recording = false;
    stats_.flushed = true;
    if (!result.ok()) {
        return FromDhfsResult(result);
    }
    return StorageResult::Ok();
}

StorageStats StorageServiceStub::GetStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return stats_;
}

StorageResult StorageServiceStub::FromDhfsResult(const dhfs::DhfsResult& result) {
    if (result.ok()) {
        return StorageResult::Ok();
    }
    return StorageResult::Error(StorageStatusCode::WriteFailed, result.message);
}

dhfs::DhfsWriterConfig StorageServiceStub::BuildWriterConfig() const {
    dhfs::DhfsWriterConfig writer_config;
    writer_config.root_dir = config_.mount_point + "/" + config_.record_dir;
    writer_config.channel = 0;
    writer_config.enable_ps_pes = true;
    return writer_config;
}

} // namespace aov::app::storage

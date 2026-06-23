#pragma once

#include <memory>
#include <mutex>

#include "dhfs/i_dhfs_writer.hpp"
#include "i_storage_service.hpp"

namespace aov::app::storage {

class StorageServiceStub final : public IStorageService {
public:
    explicit StorageServiceStub(std::unique_ptr<dhfs::IDhfsWriter> writer);
    ~StorageServiceStub() override;

    StorageResult Init(const StorageConfig& config) override;
    void Deinit() override;

    StorageResult StartRecord() override;
    StorageResult StartRecord(RecordMode mode) override;
    StorageResult StopRecord() override;

    StorageResult OnPacketFrame(const aov::app::packet::PacketFrame& frame) override;
    StorageResult FlushAndClose() override;

    StorageStats GetStats() const override;

private:
    static StorageResult FromDhfsResult(const dhfs::DhfsResult& result);
    dhfs::DhfsWriterConfig BuildWriterConfig() const;

    mutable std::mutex mutex_;
    std::unique_ptr<dhfs::IDhfsWriter> writer_;
    StorageConfig config_;
    StorageStats stats_;
    bool initialized_ {false};
};

} // namespace aov::app::storage

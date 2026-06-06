#pragma once

#include "../packet/packet_types.hpp"
#include "storage_types.hpp"

namespace aov::app::storage {

class IStorageService {
public:
    virtual ~IStorageService() = default;

    virtual StorageResult Init(const StorageConfig& config) = 0;
    virtual void Deinit() = 0;

    virtual StorageResult StartRecord() = 0;
    virtual StorageResult StopRecord() = 0;

    virtual StorageResult OnPacketFrame(const aov::app::packet::PacketFrame& frame) = 0;
    virtual StorageResult FlushAndClose() = 0;

    virtual StorageStats GetStats() const = 0;
};

} // namespace aov::app::storage

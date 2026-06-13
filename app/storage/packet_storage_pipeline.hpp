#pragma once

#include <memory>

#include "../packet/media_packet_router.hpp"
#include "../packet/packet_service.hpp"
#include "dhfs/dhfs_writer_stub.hpp"
#include "storage_service_stub.hpp"

namespace aov::app::storage {

struct PacketStoragePipeline {
    aov::app::packet::PacketService packet_service;
    aov::app::packet::MediaPacketRouter router;
    StorageServiceStub storage_service;

    PacketStoragePipeline();

    StorageResult Init(const aov::app::packet::PacketConfig& packet_config,
                       const StorageConfig& storage_config);
    void Deinit();
};

} // namespace aov::app::storage

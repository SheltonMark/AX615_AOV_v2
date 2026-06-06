#include "packet_storage_pipeline.hpp"

#include "storage_packet_sink.hpp"

namespace aov::app::storage {

PacketStoragePipeline::PacketStoragePipeline()
    : router(packet_service),
      storage_service(std::unique_ptr<dhfs::IDhfsWriter>(new dhfs::DhfsWriterStub())) {
}

StorageResult PacketStoragePipeline::Init(const aov::app::packet::PacketConfig& packet_config,
                                          const StorageConfig& storage_config) {
    const auto packet_result = packet_service.Init(packet_config);
    if (!packet_result.ok()) {
        return StorageResult::Error(StorageStatusCode::InvalidState, packet_result.message);
    }

    auto storage_result = storage_service.Init(storage_config);
    if (!storage_result.ok()) {
        return storage_result;
    }

    storage_result = storage_service.StartRecord();
    if (!storage_result.ok()) {
        return storage_result;
    }

    return BindStorageToPacket(router, storage_service);
}

void PacketStoragePipeline::Deinit() {
    storage_service.Deinit();
    packet_service.Deinit();
}

} // namespace aov::app::storage

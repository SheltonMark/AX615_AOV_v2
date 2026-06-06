#include "storage_packet_sink.hpp"

namespace aov::app::storage {

StorageResult BindStorageToPacket(aov::app::packet::MediaPacketRouter& router,
                                  IStorageService& storage_service) {
    const auto result = router.BindStorageSink(
        [&storage_service](const aov::app::packet::PacketFrame& frame) {
            storage_service.OnPacketFrame(frame);
        });

    if (!result.ok()) {
        return StorageResult::Error(StorageStatusCode::InvalidState, result.message);
    }
    return StorageResult::Ok();
}

} // namespace aov::app::storage

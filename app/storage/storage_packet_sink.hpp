#pragma once

#include "../packet/media_packet_router.hpp"
#include "i_storage_service.hpp"

namespace aov::app::storage {

StorageResult BindStorageToPacket(aov::app::packet::MediaPacketRouter& router,
                                  IStorageService& storage_service);

} // namespace aov::app::storage

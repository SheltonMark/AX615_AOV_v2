#pragma once

#include "app/alarm/alarm_service_stub.hpp"
#include "app/cloud/cloud_packet_sink.hpp"
#include "app/cloud/i_cloud_service.hpp"
#include "app/config/config_service_stub.hpp"
#include "app/core/i_aov_orchestrator.hpp"
#include "app/packet/media_packet_router.hpp"
#include "app/packet/i_packet_service.hpp"
#include "app/storage/storage_packet_sink.hpp"
#include "app/storage/i_storage_service.hpp"

namespace aov::app::core {

struct AovAppRuntimeStatus {
    bool initialized {false};
    bool started {false};
};

class AovAppRuntime final {
public:
    AovAppRuntime(config::ConfigServiceStub& config,
                  IAovOrchestrator& orchestrator,
                  ICloudService& cloud,
                  AlarmServiceStub& alarm,
                  packet::IPacketService& packet,
                  storage::IStorageService& storage);

    AovStatusCode Init();
    AovStatusCode Start();
    AovStatusCode Stop();

    AovAppRuntimeStatus GetStatus() const;

private:
    static AovStatusCode ToAovStatus(const config::ConfigResult& result);
    static AovStatusCode ToAovStatus(const packet::PacketResult& result);
    static AovStatusCode ToAovStatus(const storage::StorageResult& result);

    config::ConfigServiceStub& config_;
    IAovOrchestrator& orchestrator_;
    ICloudService& cloud_;
    AlarmServiceStub& alarm_;
    packet::IPacketService& packet_;
    storage::IStorageService& storage_;
    packet::MediaPacketRouter router_;
    cloud::CloudPacketSink cloud_packet_sink_ {};
    AovAppRuntimeStatus status_ {};
};

} // namespace aov::app::core

#pragma once

#include <cstddef>

#include "app/alarm/alarm_service_stub.hpp"
#include "app/cloud/cloud_core_command_bridge.hpp"
#include "app/cloud/cloud_packet_sink.hpp"
#include "app/cloud/i_cloud_service.hpp"
#include "app/config/config_service_stub.hpp"
#include "app/core/core_apply_coordinator.hpp"
#include "app/core/core_command_executor.hpp"
#include "app/core/core_command_queue.hpp"
#include "app/core/i_aov_orchestrator.hpp"
#include "app/packet/media_packet_router.hpp"
#include "app/packet/i_packet_service.hpp"
#include "app/storage/storage_packet_sink.hpp"
#include "app/storage/i_storage_service.hpp"
#include "libmedia/Include/runtime/i_media_runtime.hpp"

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
    AovAppRuntime(config::ConfigServiceStub& config,
                  IAovOrchestrator& orchestrator,
                  ICloudService& cloud,
                  AlarmServiceStub& alarm,
                  packet::IPacketService& packet,
                  storage::IStorageService& storage,
                  aov::media::IMediaRuntime& media_runtime);

    AovStatusCode Init();
    AovStatusCode Start();
    AovStatusCode Stop();
    AovStatusCode OnWakeupEvent(const WakeupEvent& event);
    AovStatusCode OnDetectResult(const DetectResultSummary& result);
    AovStatusCode ApplyPendingConfig();
    cloud::CloudActionMapResult SubmitCloudAction(const cloud::CloudActionRequest& request);
    CoreCommandQueueResult PopCoreCommand(CoreCommand& out);
    std::size_t GetPendingCoreCommandCount() const;
    CoreCommandExecResult ExecuteCoreCommand(
        const CoreCommand& command,
        aov::sys::IStorageService* sys_storage_service = nullptr,
        aov::sys::IDeviceControlService* device_control_service = nullptr);
    CoreCommandExecResult DrainCoreCommands(
        std::size_t max_count,
        aov::sys::IStorageService* sys_storage_service = nullptr,
        aov::sys::IDeviceControlService* device_control_service = nullptr);

    AovAppRuntimeStatus GetStatus() const;

private:
    static AovStatusCode ToAovStatus(const config::ConfigResult& result);
    static AovStatusCode ToAovStatus(const packet::PacketResult& result);
    static AovStatusCode ToAovStatus(const storage::StorageResult& result);
    static AovStatusCode ToAovStatus(aov::media::MediaStatusCode status);
    AovStatusCode BindMediaToPacket();

    config::ConfigServiceStub& config_;
    IAovOrchestrator& orchestrator_;
    ICloudService& cloud_;
    AlarmServiceStub& alarm_;
    packet::IPacketService& packet_;
    storage::IStorageService& storage_;
    aov::media::IMediaRuntime* media_runtime_ {nullptr};
    packet::MediaPacketRouter router_;
    cloud::CloudPacketSink cloud_packet_sink_ {};
    CoreCommandQueue command_queue_ {};
    cloud::CloudCoreCommandBridge cloud_command_bridge_;
    AovAppRuntimeStatus status_ {};
};

} // namespace aov::app::core

#include "app/core/aov_app_runtime.hpp"

namespace aov::app::core {

AovAppRuntime::AovAppRuntime(config::ConfigServiceStub& config,
                             IAovOrchestrator& orchestrator,
                             ICloudService& cloud,
                             AlarmServiceStub& alarm,
                             packet::IPacketService& packet,
                             storage::IStorageService& storage)
    : config_(config),
      orchestrator_(orchestrator),
      cloud_(cloud),
      alarm_(alarm),
      packet_(packet),
      storage_(storage),
      router_(packet) {
}

AovStatusCode AovAppRuntime::Init() {
    if (status_.initialized) {
        return AovStatusCode::Ok;
    }

    AovStatusCode status = ToAovStatus(config_.LoadFactoryConfig());
    if (status != AovStatusCode::Ok) {
        return status;
    }

    status = orchestrator_.Init(config_.GetActiveWorkModePolicy());
    if (status != AovStatusCode::Ok) {
        return status;
    }

    status = ToAovStatus(packet_.Init(packet::PacketConfig {}));
    if (status != AovStatusCode::Ok) {
        return status;
    }

    status = ToAovStatus(storage_.Init(storage::StorageConfig {}));
    if (status != AovStatusCode::Ok) {
        packet_.Deinit();
        return status;
    }

    status = ToAovStatus(storage::BindStorageToPacket(router_, storage_));
    if (status != AovStatusCode::Ok) {
        storage_.Deinit();
        packet_.Deinit();
        return status;
    }

    const auto cloud_bind_result = cloud::BindCloudToPacket(router_, cloud_packet_sink_);
    if (!cloud_bind_result.ok()) {
        storage_.Deinit();
        packet_.Deinit();
        return AovStatusCode::InvalidState;
    }

    (void)alarm_;
    status_.initialized = true;
    return AovStatusCode::Ok;
}

AovStatusCode AovAppRuntime::Start() {
    if (!status_.initialized) {
        const AovStatusCode status = Init();
        if (status != AovStatusCode::Ok) {
            return status;
        }
    }
    if (status_.started) {
        return AovStatusCode::Ok;
    }

    AovStatusCode status = cloud_.Start();
    if (status != AovStatusCode::Ok) {
        return status;
    }

    status = orchestrator_.Start();
    if (status != AovStatusCode::Ok) {
        cloud_.Stop();
        return status;
    }

    status = cloud_.ReportDeviceState();
    if (status != AovStatusCode::Ok) {
        orchestrator_.Stop();
        cloud_.Stop();
        return status;
    }

    status_.started = true;
    return AovStatusCode::Ok;
}

AovStatusCode AovAppRuntime::Stop() {
    if (status_.started) {
        storage_.FlushAndClose();
        orchestrator_.Stop();
        cloud_.Stop();
    }

    storage_.Deinit();
    packet_.Deinit();
    status_.started = false;
    status_.initialized = false;
    return AovStatusCode::Ok;
}

AovAppRuntimeStatus AovAppRuntime::GetStatus() const {
    return status_;
}

AovStatusCode AovAppRuntime::ToAovStatus(const config::ConfigResult& result) {
    return result.ok() ? AovStatusCode::Ok : AovStatusCode::InvalidArgument;
}

AovStatusCode AovAppRuntime::ToAovStatus(const packet::PacketResult& result) {
    if (result.ok()) {
        return AovStatusCode::Ok;
    }
    if (result.code == packet::PacketStatusCode::InvalidArgument) {
        return AovStatusCode::InvalidArgument;
    }
    if (result.code == packet::PacketStatusCode::InvalidState) {
        return AovStatusCode::InvalidState;
    }
    if (result.code == packet::PacketStatusCode::Backpressure) {
        return AovStatusCode::Busy;
    }
    return AovStatusCode::InternalError;
}

AovStatusCode AovAppRuntime::ToAovStatus(const storage::StorageResult& result) {
    if (result.ok()) {
        return AovStatusCode::Ok;
    }
    if (result.code == storage::StorageStatusCode::InvalidArgument) {
        return AovStatusCode::InvalidArgument;
    }
    if (result.code == storage::StorageStatusCode::InvalidState) {
        return AovStatusCode::InvalidState;
    }
    if (result.code == storage::StorageStatusCode::NotMounted) {
        return AovStatusCode::NotFound;
    }
    if (result.code == storage::StorageStatusCode::NoSpace) {
        return AovStatusCode::Busy;
    }
    return AovStatusCode::InternalError;
}

} // namespace aov::app::core

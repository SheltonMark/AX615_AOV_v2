#include <iostream>
#include <memory>

#include "../../app/alarm/alarm_service_stub.hpp"
#include "../../app/cloud/cloud_service_stub.hpp"
#include "../../app/config/config_service_stub.hpp"
#include "../../app/core/aov_app_runtime.hpp"
#include "../../app/core/aov_orchestrator_stub.hpp"
#include "../../app/packet/packet_service_stub.hpp"
#include "../../app/storage/dhfs/dhfs_writer_stub.hpp"
#include "../../app/storage/storage_service_stub.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

} // namespace

int main() {
    using namespace aov::app;

    ConfigServiceStub config;
    AovOrchestratorStub orchestrator;
    CloudServiceStub cloud;
    AlarmServiceStub alarm;
    packet::PacketServiceStub packet;
    storage::StorageServiceStub storage(
        std::make_unique<storage::dhfs::DhfsWriterStub>());

    core::AovAppRuntime runtime(config,
                                orchestrator,
                                cloud,
                                alarm,
                                packet,
                                storage);

    if (runtime.GetStatus().initialized || runtime.GetStatus().started) {
        return Fail("runtime must start as stopped");
    }

    if (runtime.Init() != AovStatusCode::Ok ||
        !runtime.GetStatus().initialized) {
        return Fail("runtime init must load config and initialize modules");
    }

    if (runtime.Start() != AovStatusCode::Ok ||
        !runtime.GetStatus().started ||
        !cloud.GetRuntimeStatus().connected ||
        orchestrator.GetRuntimeWorkState() == RuntimeWorkState::LowBatteryProtection) {
        return Fail("runtime start must start core and cloud");
    }

    if (runtime.Start() != AovStatusCode::Ok) {
        return Fail("runtime start must be idempotent");
    }

    if (runtime.Stop() != AovStatusCode::Ok ||
        runtime.GetStatus().started ||
        cloud.GetRuntimeStatus().connected ||
        storage.GetStats().recording) {
        return Fail("runtime stop must stop services and clear running state");
    }

    if (runtime.Stop() != AovStatusCode::Ok) {
        return Fail("runtime stop must be idempotent");
    }

    std::cout << "aov app runtime contract passed\n";
    return 0;
}

/**
 * @file main.cpp
 * @brief AOV AX615 设备端主入口（阶段0：stub 空回路）
 *
 * 阶段0目标：把16个stub串成可编译的空回路。
 * 无真实SDK依赖，全程host可编译。
 */

#include <csignal>
#include <cstdio>
#include <atomic>
#include <memory>
#include <thread>
#include <chrono>

// ── app layer ──
#include "app/alarm/alarm_service_stub.hpp"
#include "app/cloud/cloud_service_stub.hpp"
#include "app/config/config_service_stub.hpp"
#include "app/core/aov_app_runtime.hpp"
#include "app/core/aov_orchestrator_stub.hpp"
#include "app/packet/packet_service_stub.hpp"
#include "app/storage/dhfs/dhfs_writer_stub.hpp"
#include "app/storage/storage_service_stub.hpp"

// ── libmedia layer ──

// ── libsys layer ──
#include "libsys/Src/device/device_control_service_stub.hpp"
#include "libsys/Src/modem/modem_service_stub.hpp"
#include "libsys/Src/network/network_service_stub.hpp"
#include "libsys/Src/power/power_service_stub.hpp"
#include "libsys/Src/storage/storage_service_stub.hpp"

static std::atomic<bool> g_running{true};

static void signal_handler(int /*sig*/) {
    g_running.store(false);
}

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;

    signal(SIGINT,  signal_handler);
    signal(SIGTERM, signal_handler);

    fprintf(stdout, "[AOV] stub bootstrap starting...\n");

    aov::app::AlarmServiceStub       alarm;
    aov::app::CloudServiceStub        cloud;
    aov::app::ConfigServiceStub       config;
    aov::app::AovOrchestratorStub     orchestrator;
    aov::app::packet::PacketServiceStub packet;
    aov::app::storage::StorageServiceStub app_storage(
        std::make_unique<aov::app::storage::dhfs::DhfsWriterStub>());
    aov::sys::DeviceControlServiceStub device_ctrl;
    aov::sys::ModemServiceStub         modem;
    aov::sys::NetworkServiceStub       network;
    aov::sys::PowerServiceStub         power;
    aov::sys::StorageServiceStub       storage;

    (void)device_ctrl;
    (void)modem;
    (void)network;
    (void)power;
    (void)storage;

    aov::app::core::AovAppRuntime runtime(config,
                                          orchestrator,
                                          cloud,
                                          alarm,
                                          packet,
                                          app_storage);

    auto core_rc = runtime.Init();
    fprintf(stdout, "[AOV] runtime.Init => %d\n", static_cast<int>(core_rc));
    if (core_rc != aov::app::AovStatusCode::Ok) {
        return 1;
    }

    core_rc = runtime.Start();
    fprintf(stdout, "[AOV] runtime.Start => %d\n", static_cast<int>(core_rc));
    if (core_rc != aov::app::AovStatusCode::Ok) {
        runtime.Stop();
        return 1;
    }

    fprintf(stdout, "[AOV] all stubs initialized. running...\n");

    while (g_running.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    runtime.Stop();
    fprintf(stdout, "[AOV] clean exit.\n");
    return 0;
}

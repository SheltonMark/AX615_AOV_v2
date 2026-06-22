/**
 * @file main.cpp
 * @brief AOV AX615 设备端主入口（阶段1：事件循环测试）
 *
 * 阶段1目标：测试主事件循环和状态机。
 * 使用新的 AovOrchestrator 替代 Stub。
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
#include "app/core/aov_orchestrator.hpp"
#include "app/packet/packet_service.hpp"
#include "app/storage/dhfs/dhfs_writer_stub.hpp"
#include "app/storage/storage_service_stub.hpp"

// ── libmedia layer ──

static std::atomic<bool> g_running{true};

static void signal_handler(int /*sig*/) {
    g_running.store(false);
}

// 模拟定时唤醒事件（测试用）
static void simulate_periodic_wakeup(aov::app::core::AovOrchestrator& orchestrator) {
    using namespace std::chrono_literals;

    fprintf(stdout, "\n[TEST] Simulating periodic wakeup events...\n");

    for (int i = 0; i < 3 && g_running.load(); ++i) {
        std::this_thread::sleep_for(3s);

        aov::app::WakeupEvent event;
        event.source = aov::app::WakeupSource::Timer;
        event.timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();

        fprintf(stdout, "\n[TEST] Posting WakeupEvent #%d\n", i + 1);
        orchestrator.OnWakeupEvent(event);

        // 模拟检测结果（第2次有目标）
        std::this_thread::sleep_for(500ms);
        aov::app::DetectResultSummary detect;
        detect.has_person = (i == 1);  // 第2次检测到人
        detect.object_count = (i == 1) ? 1 : 0;
        detect.timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();

        fprintf(stdout, "[TEST] Posting DetectResult: has_person=%d\n", detect.has_person);
        orchestrator.OnDetectResult(detect);
    }

    fprintf(stdout, "\n[TEST] Simulation complete. Press Ctrl+C to exit.\n");
}

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;

    signal(SIGINT,  signal_handler);
    signal(SIGTERM, signal_handler);

    fprintf(stdout, "[AOV] Event loop test starting...\n");

    aov::app::AlarmServiceStub       alarm;
    aov::app::CloudServiceStub        cloud;
    aov::app::ConfigServiceStub       config;
    aov::app::core::AovOrchestrator   orchestrator;  // 使用新实现
    aov::app::packet::PacketService packet;
    aov::app::storage::StorageServiceStub app_storage(
        std::make_unique<aov::app::storage::dhfs::DhfsWriterStub>());
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

    fprintf(stdout, "[AOV] Event loop running. Starting test simulation...\n\n");

    // 启动模拟测试线程
    std::thread test_thread(simulate_periodic_wakeup, std::ref(orchestrator));

    while (g_running.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    test_thread.join();
    runtime.Stop();
    fprintf(stdout, "\n[AOV] clean exit.\n");
    return 0;
}

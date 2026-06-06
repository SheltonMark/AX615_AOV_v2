cmake_minimum_required(VERSION 3.10)

project(ax615_aov_device LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_LIBMEDIA "Build libmedia module" ON)
option(BUILD_APP_STUBS "Build app/libsys stub executable and tests" ON)
option(AOV_LINK_PREBUILT_DEPS "Link AX615 prebuilt libraries from Depend into Kylin" OFF)

if(BUILD_LIBMEDIA)
    add_subdirectory(libmedia)
endif()

if(BUILD_APP_STUBS)
set(AOV_DEPEND_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Depend)
set(AOV_AX_SDK_DIR ${AOV_DEPEND_DIR}/ax_sdk/arm_uclibc)
set(AOV_THIRD_PARTY_DIR ${AOV_DEPEND_DIR}/third_party)
set(AOV_TENCENT_CLOUD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/app/cloud/TencentCloud_SDK)
# ── stub 静态库 ────────────────────────────────────────────────
add_library(ax615_aov_device_stubs STATIC
    app/alarm/alarm_service_stub.cpp
    app/cloud/cloud_action_mapper.cpp
    app/cloud/cloud_packet_sink.cpp
    app/cloud/cloud_property_mapper.cpp
    app/cloud/cloud_report_builder.cpp
    app/cloud/cloud_service_stub.cpp
    app/cloud/tencent/tencent_cloud_service.cpp
    app/cloud/tencent/tencent_generated_callback_dispatcher.cpp
    app/cloud/tencent/tencent_generated_callback_bridge.cpp
    app/cloud/tencent/tencent_live_stream.cpp
    app/cloud/tencent/tencent_product_model_bridge.cpp
    app/cloud/tencent/tencent_sdk_adapter.cpp
    app/cloud/tencent/tencent_stream_adapter.cpp
    app/config/config_core_mapper.cpp
    app/config/config_diff.cpp
    app/config/config_service_stub.cpp
    app/config/config_validation.cpp
    app/config/device_config.cpp
    app/core/aov_app_runtime.cpp
    app/core/cloud_core_drain_mapper.cpp
    app/core/core_apply_coordinator.cpp
    app/core/core_wakeup_planner.cpp
    app/core/idle_sleep_controller.cpp
    app/core/storage_core_drain_mapper.cpp
    app/core/core_runtime_facade.cpp
    app/core/system_sleep_coordinator.cpp
    app/core/aov_orchestrator_stub.cpp
    app/core/work_mode_state_machine.cpp
    app/packet/media_packet_router.cpp
    app/packet/packet_service_stub.cpp
    app/storage/dhfs/dhfs_writer_stub.cpp
    app/storage/packet_storage_pipeline.cpp
    app/storage/storage_packet_sink.cpp
    app/storage/storage_service_stub.cpp
    libsys/Src/device/device_control_service_stub.cpp
    libsys/Src/modem/modem_service_stub.cpp
    libsys/Src/network/network_service_stub.cpp
    libsys/Src/power/power_service_stub.cpp
    libsys/Src/storage/storage_service_stub.cpp
)

target_include_directories(ax615_aov_device_stubs
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/libmedia/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/libsys/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/app/cloud/TencentCloud_SDK/include
        ${AOV_AX_SDK_DIR}/include
        ${AOV_THIRD_PARTY_DIR}/include
)

# ── entry 可执行文件（空回路验证） ──────────────────────────────
add_executable(Kylin
    app/entry/main.cpp
)
target_link_libraries(Kylin PRIVATE ax615_aov_device_stubs)

if(AOV_LINK_PREBUILT_DEPS)
    file(GLOB AOV_THIRD_PARTY_LIBS
        ${AOV_THIRD_PARTY_DIR}/lib/*.a
    )
    file(GLOB AOV_TENCENT_CLOUD_LIBS
        ${AOV_TENCENT_CLOUD_DIR}/lib/*.a
    )
    file(GLOB AOV_AX_SDK_LIBS
        ${AOV_AX_SDK_DIR}/lib/*.a
    )

    target_link_libraries(Kylin
        PRIVATE
            -Wl,--start-group
            ${AOV_THIRD_PARTY_LIBS}
            ${AOV_TENCENT_CLOUD_LIBS}
            ${AOV_AX_SDK_LIBS}
            -Wl,--end-group
            pthread
            dl
            m
            rt
    )
endif()

# ── GoogleTest（vendored，离线） ────────────────────────────────
set(GTEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Depend/third_party/googletest)
if(EXISTS ${GTEST_DIR}/CMakeLists.txt)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    add_subdirectory(${GTEST_DIR} googletest EXCLUDE_FROM_ALL)

    enable_testing()

    # 单测可执行文件
    add_executable(aov_unit_tests
        tests/unit/test_aov_orchestrator.cpp
    )
    target_link_libraries(aov_unit_tests
        PRIVATE
            ax615_aov_device_stubs
            gtest
            gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(aov_unit_tests)
else()
    message(STATUS "GoogleTest not found at ${GTEST_DIR} — unit tests disabled")
    message(STATUS "Run: git clone https://github.com/google/googletest Depend/third_party/googletest")
endif()
endif()

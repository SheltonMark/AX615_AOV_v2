cmake_minimum_required(VERSION 3.10)

project(ax615_aov_device LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_LIBMEDIA "Build libmedia module" ON)
option(BUILD_APP_STUBS "Build app executable and tests" ON)
option(AOV_LINK_PREBUILT_DEPS "Link AX615 prebuilt libraries from Depend into Kylin" OFF)
option(AOV_ENABLE_TENCENT_SDK "Enable real Tencent cloud SDK calls" OFF)

if(BUILD_LIBMEDIA)
    add_subdirectory(libmedia)
endif()

if(BUILD_APP_STUBS)
set(AOV_DEPEND_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Depend)
set(AOV_AX_SDK_DIR ${AOV_DEPEND_DIR}/ax_sdk/arm_uclibc)
set(AOV_THIRD_PARTY_DIR ${AOV_DEPEND_DIR}/third_party)
set(AOV_PREBUILT_DIR ${AOV_DEPEND_DIR}/prebuilt)
set(AOV_TENCENT_CLOUD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/app/cloud/TencentCloud_SDK)
set(AOV_APP_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libmedia/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/libsys/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/app/cloud/TencentCloud_SDK/include
    ${AOV_AX_SDK_DIR}/include
    ${AOV_THIRD_PARTY_DIR}/include
)

# ── app object 集合：直接链接进 Kylin，避免额外归档 libax615_aov_app_core.a ──
add_library(ax615_aov_app_core OBJECT
    app/alarm/alarm_service_stub.cpp
    app/cloud/cloud_action_mapper.cpp
    app/cloud/cloud_core_command_bridge.cpp
    app/cloud/cloud_packet_sink.cpp
    app/cloud/cloud_property_mapper.cpp
    app/cloud/cloud_report_builder.cpp
    app/cloud/cloud_service_stub.cpp
    app/cloud/tencent/tencent_cloud_service.cpp
    app/cloud/tencent/tencent_cloud_storage.cpp
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
    app/core/core_command_queue.cpp
    app/core/core_command_executor.cpp
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
)

if(AOV_ENABLE_TENCENT_SDK)
    target_sources(ax615_aov_app_core
        PRIVATE
            app/cloud/TencentCloud_SDK/os_linux_impl.c
    )
    target_compile_definitions(ax615_aov_app_core
        PUBLIC
            AOV_ENABLE_TENCENT_SDK=1
    )
endif()

target_include_directories(ax615_aov_app_core
    PUBLIC
        ${AOV_APP_INCLUDE_DIRS}
)

# ── entry 可执行文件（空回路验证） ──────────────────────────────
add_executable(Kylin
    app/entry/main.cpp
    $<TARGET_OBJECTS:ax615_aov_app_core>
)
target_include_directories(Kylin
    PRIVATE
        ${AOV_APP_INCLUDE_DIRS}
)

if(AOV_LINK_PREBUILT_DEPS)
    set(AOV_PREBUILT_LIBMEDIA ${AOV_PREBUILT_DIR}/lib/libmedia.a)
    set(AOV_PREBUILT_LIBSYS ${AOV_PREBUILT_DIR}/lib/libsys.a)
    if(NOT EXISTS ${AOV_PREBUILT_LIBMEDIA})
        message(FATAL_ERROR "AOV_LINK_PREBUILT_DEPS=ON but libmedia.a not found: ${AOV_PREBUILT_LIBMEDIA}")
    endif()
    if(NOT EXISTS ${AOV_PREBUILT_LIBSYS})
        message(FATAL_ERROR "AOV_LINK_PREBUILT_DEPS=ON but libsys.a not found: ${AOV_PREBUILT_LIBSYS}")
    endif()

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
            ${AOV_PREBUILT_LIBMEDIA}
            ${AOV_PREBUILT_LIBSYS}
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
        $<TARGET_OBJECTS:ax615_aov_app_core>
    )
    target_include_directories(aov_unit_tests
        PRIVATE
            ${AOV_APP_INCLUDE_DIRS}
    )
    target_link_libraries(aov_unit_tests
        PRIVATE
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

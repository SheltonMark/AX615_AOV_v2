cmake_minimum_required(VERSION 3.10)

project(libmedia VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(LIBMEDIA_BUILD_AX615_ADAPTERS "Build AX615 SDK adapters" OFF)

set(LIBMEDIA_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(CODE_V2_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(AX_SDK_ROOT ${CODE_V2_ROOT}/Depend/ax_sdk/arm_uclibc CACHE PATH "AX615 arm_uclibc SDK root")
if(DEFINED AX615_SDK_INC)
    set(AX_SDK_INCLUDE_DIR ${AX615_SDK_INC})
else()
    set(AX_SDK_INCLUDE_DIR ${AX_SDK_ROOT}/include)
endif()
if(DEFINED AX615_SDK_LIB)
    set(AX_SDK_LIB_DIR ${AX615_SDK_LIB})
else()
    set(AX_SDK_LIB_DIR ${AX_SDK_ROOT}/lib)
endif()

set(LIBMEDIA_PUBLIC_HEADERS
    Include/common/media_status.hpp
    Include/common/media_types.hpp
    Include/common/media_config.hpp
    Include/runtime/i_media_runtime.hpp
    Include/video/i_video_stream_service.hpp
    Include/audio/i_audio_stream_service.hpp
    Include/isp/i_isp_service.hpp
    Include/osd/i_osd_service.hpp
    Include/detect/i_detect_service.hpp
)

set(LIBMEDIA_SOURCES
    Src/audio/audio_stream_service.cpp
    Src/detect/detect_service.cpp
    Src/isp/isp_service.cpp
    Src/osd/osd_service.cpp
    Src/pipeline/media_pipeline_factory.cpp
    Src/pipeline/media_pipeline_manager.cpp
    Src/video/video_stream_service.cpp
)

if(LIBMEDIA_BUILD_AX615_ADAPTERS)
    list(APPEND LIBMEDIA_SOURCES
        Src/adapter/ax615/sys/ax_sys_adapter.cpp
        Src/adapter/ax615/sys/ax_link_manager.cpp
        Src/adapter/ax615/audio/ax_audio_capture_adapter.cpp
        Src/adapter/ax615/audio/ax_audio_play_adapter.cpp
        Src/adapter/ax615/detect/ax_detect_adapter.cpp
        Src/adapter/ax615/vin/ax_vin_adapter.cpp
        Src/adapter/ax615/vin/sensor_registry.cpp
        Src/adapter/ax615/vin/sensors/os04d10_profile.cpp
        Src/adapter/ax615/isp/ax_isp_adapter.cpp
        Src/adapter/ax615/ivps/ax_ivps_adapter.cpp
        Src/adapter/ax615/osd/ax_osd_adapter.cpp
        Src/adapter/ax615/venc/ax_venc_adapter.cpp
        Src/adapter/ax615/jenc/ax_jenc_adapter.cpp
        Src/adapter/ax615/power/ax_aov_power_adapter.cpp
    )
endif()

add_library(media STATIC ${LIBMEDIA_SOURCES} ${LIBMEDIA_PUBLIC_HEADERS})
add_library(media::media ALIAS media)

target_include_directories(media
    PUBLIC
        ${LIBMEDIA_ROOT}/Include
    PRIVATE
        ${LIBMEDIA_ROOT}/Src
)

if(LIBMEDIA_BUILD_AX615_ADAPTERS)
    target_compile_definitions(media PRIVATE LIBMEDIA_BUILD_AX615_ADAPTERS=1)
    target_include_directories(media PRIVATE ${AX_SDK_INCLUDE_DIR})
    if(COMMAND target_link_directories)
        target_link_directories(media PUBLIC ${AX_SDK_LIB_DIR})
    else()
        target_link_libraries(media PUBLIC "-L${AX_SDK_LIB_DIR}")
    endif()
    target_link_libraries(media
        PUBLIC
            ax_sys
            ax_audio
            ax_audio_3a
            ax_ivps
            ax_venc
            ax_mipi
            ax_tcm
            ax_ae
            ax_awb
            ax_skel
            ax_ive
            ax_engine
            ax_npu_isp
            ax_faac
            sns_os04d10
            pthread
            dl
    )
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(media PRIVATE -Wall -Wextra -Wno-unused-parameter)
endif()





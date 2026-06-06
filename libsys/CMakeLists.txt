cmake_minimum_required(VERSION 3.10)

project(libsys VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(LIBSYS_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(LIBSYS_PUBLIC_HEADERS
    include/common/sys_types.hpp
    include/runtime/i_sys_runtime.hpp
    include/power/i_power_service.hpp
    include/modem/i_modem_service.hpp
    include/device/i_device_control_service.hpp
    include/network/i_network_service.hpp
    include/storage/i_storage_service.hpp
)

set(LIBSYS_SOURCES
    Src/device/device_control_service_stub.cpp
    Src/modem/modem_service_stub.cpp
    Src/network/network_service_stub.cpp
    Src/power/power_service_stub.cpp
    Src/runtime/sys_runtime.cpp
    Src/runtime/sys_runtime_factory.cpp
    Src/storage/storage_service_stub.cpp
)

add_library(sys STATIC ${LIBSYS_SOURCES} ${LIBSYS_PUBLIC_HEADERS})
add_library(sys::sys ALIAS sys)

target_include_directories(sys
    PUBLIC
        ${LIBSYS_ROOT}/include
        ${LIBSYS_ROOT}/..
    PRIVATE
        ${LIBSYS_ROOT}/Src
)

set_target_properties(sys PROPERTIES OUTPUT_NAME sys)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(sys PRIVATE -Wall -Wextra -Wno-unused-parameter)
endif()

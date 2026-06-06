cmake_minimum_required(VERSION 3.16)

# Unit tests for AOV AX615

# Find GTest
find_package(GTest QUIET)

if(NOT GTest_FOUND)
    # Try to use FetchContent
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0
    )
    FetchContent_MakeAvailable(googletest)
endif()

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/app
    ${CMAKE_SOURCE_DIR}/app/aov
    ${CMAKE_SOURCE_DIR}/app/isp
    ${CMAKE_SOURCE_DIR}/app/codec
    ${CMAKE_SOURCE_DIR}/app/network
)

# Test: AOV Orchestrator
add_executable(test_aov_orchestrator
    test_aov_orchestrator.cpp
)
target_link_libraries(test_aov_orchestrator
    GTest::gtest
    GTest::gtest_main
)
target_compile_definitions(test_aov_orchestrator PRIVATE
    UNIT_TEST_STUB=1
)

# Register tests with CTest
include(GoogleTest)
gtest_discover_tests(test_aov_orchestrator)

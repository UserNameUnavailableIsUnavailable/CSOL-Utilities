cmake_minimum_required(VERSION 3.20)

find_package(GTest CONFIG REQUIRED)

add_executable(
    ControllerTesting
    TestUtilities.cpp

    Model.cpp
)

target_link_libraries(
    ControllerTesting
    PRIVATE
    ControllerLib
    GTest::gtest_main
)

target_compile_definitions(
    ControllerTesting
    PRIVATE
    CONTROLLER_VERSION="${VERSION}"
    SOURCE_DIR="${CMAKE_SOURCE_DIR}"
    CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
    CURRENT_BUILD_DIR="${CMAKE_CURRENT_BINARY_DIR}"
)

include(GoogleTest)
gtest_discover_tests(ControllerTesting)


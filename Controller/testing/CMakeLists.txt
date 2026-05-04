cmake_minimum_required(VERSION 3.20)

find_package(GTest CONFIG REQUIRED)

file(GLOB CONTROLLER_TEST_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_executable(
    ControllerTesting
    ${CONTROLLER_TEST_SOURCES}
)

target_include_directories(
    ControllerTesting
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${DEPENDENCIES_DIR}/include
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
)

target_precompile_headers(ControllerTesting REUSE_FROM ControllerLib)

include(GoogleTest)
gtest_discover_tests(ControllerTesting)

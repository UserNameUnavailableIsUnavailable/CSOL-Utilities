cmake_minimum_required(VERSION 3.20)

message(STATUS "vcpkg root: $ENV{VCPKG_ROOT}")
# vcpkg toolchain must be specified before the first project()/enable_language so its hooks add the package paths
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "vcpkg toolchain file")

project(CSOL-Utilities LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows" CACHE STRING "vcpkg prefix path" FORCE)

set(DEPENDENCIES_DIR "${CMAKE_BINARY_DIR}/dependencies")
set(DOWNLOADS_DIR "${CMAKE_BINARY_DIR}/downloads")
file(MAKE_DIRECTORY "${DOWNLOADS_DIR}")
file(MAKE_DIRECTORY "${DEPENDENCIES_DIR}")

add_subdirectory(Controller)
add_subdirectory(Tool)

cmake_minimum_required(VERSION 3.20)

# Prefer VERSION from -DVERSION=...; fallback to VERSION.txt.
if(NOT DEFINED VERSION OR VERSION STREQUAL "")
    file(READ "${CMAKE_SOURCE_DIR}/VERSION.txt" VERSION)
    string(STRIP "${VERSION}" VERSION)
endif()
string(REGEX REPLACE "^\"(.*)\"$" "\\1" VERSION "${VERSION}")
if (VERSION STREQUAL "")
    message(FATAL_ERROR "VERSION is empty. Pass -DVERSION=... or set VERSION.txt")
endif()
message(STATUS "CSOL-Utilities version: ${VERSION}")

set(VCPKG_OVERLAY_TRIPLETS "${CMAKE_SOURCE_DIR}/vcpkg/triplets")
set(VCPKG_TARGET_TRIPLET "x64-windows-mixed")

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/cmake")

project(CSOL-Utilities VERSION ${VERSION} LANGUAGES C CXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Check MSVC
if(MSVC)
    add_compile_options(/utf-8 /W4 /permissive-)
endif()

add_compile_definitions(
    $<$<CONFIG:Debug>:_DEBUG=1>
)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/dependencies/include
)

add_subdirectory(utilities)
# add_subdirectory(controller)
add_subdirectory(lghubex)
add_subdirectory(executor)
enable_testing()
add_subdirectory(testing)

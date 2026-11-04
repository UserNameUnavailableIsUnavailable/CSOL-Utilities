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

project(CSOL-Utilities VERSION ${VERSION} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Check MSVC
if(MSVC)
    add_compile_options(/utf-8 /W4 /permissive-)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(_DEBUG=1)
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)
add_subdirectory(utilities)
add_subdirectory(controller)
add_subdirectory(lghubex)
enable_testing()
add_subdirectory(testing)
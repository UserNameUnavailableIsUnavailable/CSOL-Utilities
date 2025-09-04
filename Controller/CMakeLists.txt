# Note:
# If you use vcpkg bundled in Visual Studio from Visual Studio's Developer Shell, then you do not have to manually set `VCPKG_ROOT`.

cmake_minimum_required(VERSION 3.20)

message(STATUS "vcpkg root: $ENV{VCPKG_ROOT}")
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "vcpkg toolchain file")

project(Controller LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(DEPENDENCIES_DIR "${CMAKE_BINARY_DIR}/dependencies")

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/cmake")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows")

find_package(onnxruntime CONFIG REQUIRED)
find_package(aho_corasick CONFIG REQUIRED)
find_package(OpenCV CONFIG REQUIRED)
find_package(Clipper2 CONFIG REQUIRED)
find_package(CLI11 CONFIG REQUIRED)

include_directories(
    Controller
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${DEPENDENCIES_DIR}/include
)
get_target_property(OPENCV_INCLUDES opencv_core INTERFACE_INCLUDE_DIRECTORIES)
message(STATUS "OpenCV include path: ${OPENCV_INCLUDES}")

add_executable(
    Controller
    source/Main.cpp
    source/Driver.cpp
    source/Command.cpp
    source/CommandDispatcher.cpp
    source/Console.cpp
    source/CRNN.cpp
    source/CSOBannerSuppressor.cpp
    source/Global.cpp
    source/Hotkey.cpp
    source/LowLevelKeyboardHook.cpp
    source/IdleEngine.cpp
    source/OCR.cpp
    source/DBNet.cpp
    source/OCRIdleEngine.cpp
    source/OCRUtilities.cpp
    source/pch.cpp
    source/Utilities.cpp
)

target_link_libraries(Controller
    PRIVATE
    onnxruntime::onnxruntime
    opencv_core
    opencv_imgcodecs
    opencv_imgproc
    Clipper2::Clipper2
    CLI11::CLI11
)

target_precompile_headers(Controller
    PRIVATE
    include/pch.hpp
)

file(GLOB dll_files "${DEPENDENCIES_DIR}/bin/*.dll")
file(GLOB pdb_files "${DEPENDENCIES_DIR}/bin/*.pdb")
list(APPEND dependencies ${dll_files})

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    list(APPEND dependencies ${pdb_files})
endif()

message(STATUS "Dependencies: ${dependencies}")

if (dependencies)
    add_custom_command(TARGET Controller POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${dependencies}
            $<TARGET_FILE_DIR:Controller>
    )
endif()

add_custom_command(TARGET Controller POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/locales
        $<TARGET_FILE_DIR:Controller>/locales
)

add_custom_command(TARGET Controller POST_BUILD
    COMMAND pwsh -NoLogo -NoProfile -File "${CMAKE_SOURCE_DIR}/InstallModels.ps1" -Target "$<CONFIG>"
)

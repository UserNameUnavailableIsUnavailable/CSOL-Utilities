find_package(CLI11 CONFIG REQUIRED)
find_package(onnxruntime CONFIG REQUIRED)
find_package(OpenCV CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_library(
    ControllerLib
    STATIC
    source/pch.cpp
    source/injector.cpp
)

# Compile startup initialization into consumers so archive extraction cannot omit it.
target_sources(
    ControllerLib
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/source/preset.cpp
)

target_precompile_headers(
    ControllerLib
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include/controller/pch.hpp
)

target_link_libraries(
    ControllerLib
    PUBLIC
    UtilitiesLib
    CLI11::CLI11
    opencv_core
    opencv_imgcodecs
    opencv_imgproc
    onnxruntime::onnxruntime
    spdlog::spdlog
)

add_executable(
    Controller
    source/main.cpp
)

target_link_libraries(
    Controller
    PRIVATE
    ControllerLib
)

find_package(CLI11 CONFIG REQUIRED)
find_package(OpenCV CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_library(
    ControllerLib
    STATIC
    source/pch.cpp
)

target_precompile_headers(
    ControllerLib
    PUBLIC
    source/pch.hpp
)

target_link_libraries(
    ControllerLib
    PUBLIC
    UtilitiesLib
    CLI11::CLI11
    opencv_core
    opencv_imgcodecs
    opencv_imgproc
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

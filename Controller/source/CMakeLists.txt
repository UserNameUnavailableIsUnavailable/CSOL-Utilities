cmake_minimum_required(VERSION 3.20)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(CLI11 CONFIG REQUIRED)
find_package(OpenCV CONFIG REQUIRED)
find_package(Clipper2 CONFIG REQUIRED)
find_package(onnxruntime CONFIG REQUIRED)
find_package(aho_corasick CONFIG REQUIRED)
get_target_property(OPENCV_INCLUDES opencv_core INTERFACE_INCLUDE_DIRECTORIES)
find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)


# ControllerLib is shared by normal build and testing
add_library(
    ControllerLib
    Configuration.cpp
    Configuration/Game.cpp
    Configuration/Locale.cpp
    Configuration/Idle.cpp
    Configuration/Executor.cpp
    Driver.cpp
    Command.cpp
    CommandDispatcher.cpp
    Console.cpp
    CSOBannerSuppressor.cpp
    Global.cpp
    Hotkey.cpp
    LowLevelKeyboardHook.cpp
    IdleEngine.cpp
    ModelUtilities.cpp
    OCR.cpp
    OCR_Detector.cpp
    OCR_Recognizer.cpp
    OCRIdleEngine.cpp
    OCRUtilities.cpp
    pch.cpp
    Utilities.cpp
    Classifier.cpp
    ClassifierIdleEngine.cpp
    Server.cpp
)

target_include_directories(
    ControllerLib
    PUBLIC # make ControllerLib available to ControllerTesting
    "${PROJECT_SOURCE_DIR}/include"
)

target_link_libraries(
    ControllerLib # shared by normal build and testing
    PUBLIC
    onnxruntime::onnxruntime
    opencv_core
    opencv_imgcodecs
    opencv_imgproc
    Clipper2::Clipper2
    CLI11::CLI11
    # Custom lib
    Protocol
)

target_compile_definitions(
    ControllerLib
    PUBLIC
    CONTROLLER_VERSION="${VERSION}"
)

target_precompile_headers(
    ControllerLib
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include/pch.hpp
)

message(STATUS "Controller version: ${VERSION}")

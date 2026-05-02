cmake_minimum_required(VERSION 3.20)

message(STATUS "NativePanel CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "NativePanel CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")

find_package(Qt6 COMPONENTS Core Widgets REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)

set(CMAKE_AUTOMOC ON)

add_executable(NativePanel WIN32
    source/main.cpp
    source/Communication.cpp
    source/QMainWindow.cpp
)

generate_controller_proto(NativePanel)

target_include_directories(NativePanel
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}"          # exposes Protocol/Controller.hpp
)

target_link_libraries(NativePanel
    PRIVATE
    Qt6::Core
    Qt6::Widgets
    gRPC::grpc++
    protobuf::libprotobuf
)

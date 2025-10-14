cmake_minimum_required(VERSION 3.20)

project(TOOL LANGUAGES CXX)

add_library(
    Tool SHARED
    source/dll/Tool.h
    source/dll/Export.h
    source/dll/Export.cpp
    source/dll/WindowUtils.h
    source/dll/WindowUtils.cpp
)

target_link_libraries(
    Tool
    PRIVATE
    winmm
)
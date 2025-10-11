cmake_minimum_required(VERSION 3.20)

project(TOOL LANGUAGES CXX)

add_library(
    Tool SHARED
    source/dll/CursorClipper.hpp
    source/dll/CursorClipper.cpp
    source/dll/Exports.cpp
    source/dll/ToolException.hpp
)
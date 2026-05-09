cmake_minimum_required(VERSION 3.20)

project(TOOL LANGUAGES CXX)

include_directories(
    Tool
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_library(
    Tool SHARED
    source/Export.cpp
    source/WindowUtils.cpp
)

target_link_libraries(
    Tool
    PRIVATE
    winmm
)

# FIXME: Currently *.exp and *.lib is also copied into dist dir.
install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/"
    DESTINATION .
    COMPONENT Tool
)

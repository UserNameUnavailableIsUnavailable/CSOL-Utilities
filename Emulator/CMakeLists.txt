# Emulator.dll

cmake_minimum_required(VERSION 3.20)

find_package(Lua REQUIRED)

add_library(
    Emulator
    SHARED
    emulator.c
    emulator.h
)

target_include_directories(Emulator PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(Emulator PRIVATE ${LUA_LIBRARIES})


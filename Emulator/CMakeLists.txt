# LGHUB_Emulator.dll

cmake_minimum_required(VERSION 3.20)

find_package(lua CONFIG REQUIRED)

add_library(
    Emulator
    SHARED
    emulator.cpp
    emulator.h
)

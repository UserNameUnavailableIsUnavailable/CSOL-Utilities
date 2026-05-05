# LGHUB_Emulator.dll

cmake_minimum_required(VERSION 3.20)

find_package(lua CONFIG REQUIRED)

add_library(
    LGHUB_Emulator SHARED
    source/LGHUB_Emulator.cpp
    source/LGHUB_Emulator.h
)
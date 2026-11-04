add_library(
    UtilitiesLib
    STATIC
    source/string.cpp
    source/process_thread.cpp
)

find_package(spdlog CONFIG REQUIRED)

target_precompile_headers(
    UtilitiesLib
    PUBLIC
    source/pch.hpp
)

target_link_libraries(
    UtilitiesLib
    PRIVATE spdlog::spdlog
)
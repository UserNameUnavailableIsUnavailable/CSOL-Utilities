add_library(
    UtilitiesLib
    STATIC
    string.cpp
    process_thread.cpp
)

find_package(spdlog CONFIG REQUIRED)

target_precompile_headers(
    UtilitiesLib
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include/utilities/pch.hpp
)

target_link_libraries(
    UtilitiesLib
    PRIVATE spdlog::spdlog
)
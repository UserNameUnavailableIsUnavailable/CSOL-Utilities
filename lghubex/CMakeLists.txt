add_executable(
    lghubex_app
    source/pch.cpp
    source/injector.cpp
    source/main.cpp
)

add_library(
    lghubex_dll
    SHARED
    source/pch.cpp
    source/search_pattern.cpp
    source/dll.cpp
    source/runtime.cpp
)

find_package(spdlog CONFIG REQUIRED)

target_precompile_headers(
    lghubex_app
    PRIVATE
    source/pch.hpp
)

target_precompile_headers(
    lghubex_dll
    REUSE_FROM
    lghubex_app
)

target_link_libraries(
    lghubex_app
    PRIVATE
    spdlog::spdlog
    UtilitiesLib
)

set_target_properties(lghubex_app PROPERTIES OUTPUT_NAME "lghubex")
set_target_properties(lghubex_dll PROPERTIES OUTPUT_NAME "lghubex")

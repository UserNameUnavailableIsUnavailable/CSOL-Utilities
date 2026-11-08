include("${CMAKE_SOURCE_DIR}/cmake/lua.cmake")

add_library(
    lghubex_app_impl
    INTERFACE
    source/app/pch.cpp
    source/app/injector.cpp
)

add_library(
    lghubex_dll_impl
    INTERFACE
    source/dll/pattern.c
    source/dll/hook.c
    source/dll/features.c
)

add_executable(
    lghubex_app
    source/app/main.cpp
)
target_link_libraries(
    lghubex_app
    PRIVATE
    lghubex_app_impl
)

add_library(
    lghubex_dll
    SHARED
    source/dll/dll.c
)

target_link_libraries(
    lghubex_dll
    PRIVATE
    lghubex_dll_impl
)

find_package(CLI11 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(minhook CONFIG REQUIRED)
find_package(Lua CONFIG REQUIRED)

target_precompile_headers(
    lghubex_app_impl
    PRIVATE
    source/app/pch.hpp
)

target_precompile_headers(
    lghubex_app
    REUSE_FROM
    lghubex_app_impl
)

target_link_libraries(
    lghubex_app_impl
    PUBLIC
    spdlog::spdlog
    UtilitiesLib
    CLI11::CLI11
)

target_link_libraries(
    lghubex_dll_impl
    PUBLIC
    minhook::minhook
    UtilitiesLib
    Lua::Static
)

target_compile_definitions(
    lghubex_dll_impl
    PRIVATE
    $<$<CONFIG:Debug>:PROJECT_ROOT="${PROJECT_SOURCE_DIR}">
)

set_target_properties(lghubex_app PROPERTIES OUTPUT_NAME "lghubex")
set_target_properties(lghubex_dll PROPERTIES OUTPUT_NAME "lghubex")

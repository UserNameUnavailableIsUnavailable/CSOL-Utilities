add_executable(
    lghubex_app_test
    injector.cpp
)

target_link_libraries(
    lghubex_app_test
    PRIVATE
    lghubex_app_impl
)

add_executable(
    lghubex_dll_test
    dll.cpp
)

target_link_libraries(
    lghubex_dll_test
    PRIVATE
    lghubex_dll_impl
)

target_compile_definitions(
    lghubex_dll_test
    PRIVATE
    $<$<CONFIG:Debug>:PROJECT_ROOT="${PROJECT_SOURCE_DIR}">
)

register_test(lghubex_app_test)
register_test(lghubex_dll_test)
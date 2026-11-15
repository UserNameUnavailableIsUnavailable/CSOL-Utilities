add_executable(
    executor_native_test
    native.cpp
    async.cpp
    "${PROJECT_SOURCE_DIR}/executor/Native/dll.cpp"
)

target_link_libraries(
    executor_native_test
    PRIVATE
    Native_impl
    lghubex_dll_impl
)

register_test(executor_native_test)

target_compile_definitions(
    executor_native_test
    PRIVATE NATIVE_DLL_PATH="$<TARGET_FILE:Native>"
    TEST_PROJECT_ROOT="${PROJECT_SOURCE_DIR}"
)
add_dependencies(executor_native_test Native)

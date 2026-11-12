add_library(
    Native_impl
    STATIC
    native/time.cpp
    native/keyboard.cpp
    native/mouse.cpp
    native/builtin.cpp
    native/input.cpp
    native/storage.cpp
)

add_library(
    Native
    SHARED
    native/dll.cpp
)

find_package(Lua CONFIG REQUIRED)

target_link_libraries(Native_impl PUBLIC Lua::Static)

target_link_libraries(
    Native
    Native_impl
    Lua::Static
)

add_custom_command(
    TARGET Native POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "$<TARGET_FILE:Native>"
        "${CMAKE_SOURCE_DIR}/executor/$<TARGET_FILE_NAME:Native>"
    VERBATIM
)

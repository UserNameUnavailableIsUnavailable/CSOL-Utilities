add_library(
    Native_impl
    STATIC
    core/Native/time.cpp
    core/Native/keyboard.cpp
    core/Native/mouse.cpp
    core/Native/raw.cpp
    core/Native/input.cpp
    core/Native/storage.cpp
    core/Native/async.cpp
    core/Native/waitable.cpp
    core/Native/channel.cpp
    core/Native/multiplexer.cpp
    core/Native/runtime.cpp
)

add_library(
    Native
    SHARED
    core/Native/dll.cpp
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
        "${CMAKE_SOURCE_DIR}/executor/core/$<TARGET_FILE_NAME:Native>"
    VERBATIM
)

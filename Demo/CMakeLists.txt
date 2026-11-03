cmake_minimum_required(VERSION 3.20)

add_subdirectory(
    "${CMAKE_SOURCE_DIR}/Controller/dependencies/logitech-ghub-api-extender"
    "${CMAKE_BINARY_DIR}/logitech-ghub-api-extender"
    EXCLUDE_FROM_ALL
)

add_executable(LGHUBHookDemo LGHUBHookDemo.cpp)
add_dependencies(LGHUBHookDemo LGExtenderLib)

add_custom_command(
    TARGET LGHUBHookDemo
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:LGExtenderLib>"
        "$<TARGET_FILE_DIR:LGHUBHookDemo>"
)

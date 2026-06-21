cmake_minimum_required(VERSION 3.20)

project(Controller)

list(APPEND CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/cmake")

message(STATUS "Source dir: ${PROJECT_SOURCE_DIR}")

# Shared lib
add_subdirectory(source)
add_subdirectory(testing)

add_executable(
    Controller
    Main.cpp
    Controller.rc
)

# since the rc file already includes a manifest, we need to disable automatic manifest generation to avoid conflicts
target_link_options(
    Controller PRIVATE /MANIFEST:NO
)

target_link_libraries(
    Controller
    PRIVATE
    ControllerLib
)

# deal with deps

list(APPEND dependencies "C:/Windows/System32/vcruntime140.dll")
list(APPEND dependencies "C:/Windows/System32/vcruntime140_1.dll")
list(APPEND dependencies "C:/Windows/System32/msvcp140.dll")
list(APPEND dependencies "C:/Windows/System32/msvcp140_1.dll")
list(APPEND dependencies "C:/Windows/System32/msvcp140_atomic_wait.dll")
file(GLOB dll_files "${DEPENDENCIES_DIR}/bin/*.dll")
file(GLOB pdb_files "${DEPENDENCIES_DIR}/bin/*.pdb")
list(APPEND dependencies ${dll_files})

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    list(APPEND dependencies ${pdb_files})
endif()

message(STATUS "Dependencies: ${dependencies}")

if (dependencies)
    add_custom_command(TARGET Controller POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${dependencies}
            "$<TARGET_FILE_DIR:Controller>"
    )

# assets: locales
# copy ${CMAKE_CURRENT_SOURCE_DIR}/locales to $<TARGET_FILE_DIR:Controller>/locales
add_custom_command(TARGET Controller POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/locales"
        "$<TARGET_FILE_DIR:Controller>/locales"
)
endif()

install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/"
    DESTINATION .
    #ignore *.lib, *.exp files
    COMPONENT Controller
    PATTERN "*.lib" EXCLUDE
    PATTERN "*.exp" EXCLUDE
)

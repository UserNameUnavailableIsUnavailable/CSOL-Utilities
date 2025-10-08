cmake_minimum_required(VERSION 3.20)

if (aho_corasick_installed)
    return()
endif()

set(URL "https://github.com/cjgdev/aho_corasick")

execute_process(
    COMMAND git clone ${URL} "${DOWNLOADS_DIR}/aho_corasick"
    RESULT_VARIABLE GIT_CLONE_RESULT
    OUTPUT_VARIABLE GIT_CLONE_OUTPUT
    ERROR_VARIABLE GIT_CLONE_ERROR
)

if(NOT ${GIT_CLONE_RESULT} EQUAL 0)
    message(FATAL_ERROR "Failed to clone ${URL}: ${GIT_CLONE_ERROR}")
endif()

file(COPY "${DOWNLOADS_DIR}/aho_corasick/src/aho_corasick/aho_corasick.hpp" DESTINATION "${DEPENDENCIES_DIR}/include/aho_corasick")
set(aho_corasick_installed ON CACHE BOOL "Aho-Corasick installed" FORCE)
cmake_minimum_required(VERSION 3.20)

if (aho_corasick_installed)
    return()
endif()
set(URL "https://github.com/cjgdev/aho_corasick")
execute_process(
    COMMAND git clone ${URL} "${CMAKE_BINARY_DIR}/downloads/aho_corasick"
)
file(COPY "${CMAKE_BINARY_DIR}/downloads/aho_corasick/src/aho_corasick/aho_corasick.hpp" DESTINATION "${DEPENDENCIES_DIR}/include/aho_corasick")
set(aho_corasick_installed ON CACHE BOOL "Aho-Corasick installed" FORCE)
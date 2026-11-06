include_guard(GLOBAL)

include(FetchContent)

set(LUA_VERSION "5.4.0")
set(LUA_DEPENDENCIES_ROOT "${CMAKE_SOURCE_DIR}/dependencies")
set(LUA_SOURCE_ROOT "${LUA_DEPENDENCIES_ROOT}/src/lua-${LUA_VERSION}")
set(LUA_INCLUDE_ROOT "${LUA_DEPENDENCIES_ROOT}/include")
set(LUA_LIBRARY_ROOT "${LUA_DEPENDENCIES_ROOT}/lib/lua-${LUA_VERSION}")
set(LUA_BINARY_ROOT "${LUA_DEPENDENCIES_ROOT}/bin/lua-${LUA_VERSION}")

FetchContent_Declare(
    Lua
    URL "https://www.lua.org/ftp/lua-${LUA_VERSION}.tar.gz"
    URL_HASH
        "SHA256=eac0836eb7219e421a96b7ee3692b93f0629e4cdb0c788432e3d10ce9ed47e28"
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    SOURCE_DIR "${LUA_SOURCE_ROOT}"
)
FetchContent_MakeAvailable(Lua)

set(LUA_SOURCE_DIR "${LUA_SOURCE_ROOT}/src")
file(
    GLOB LUA_LIBRARY_SOURCES
    CONFIGURE_DEPENDS
    "${LUA_SOURCE_DIR}/*.c"
)
list(
    REMOVE_ITEM LUA_LIBRARY_SOURCES
    "${LUA_SOURCE_DIR}/lua.c"
    "${LUA_SOURCE_DIR}/luac.c"
)

file(MAKE_DIRECTORY "${LUA_INCLUDE_ROOT}")
file(
    COPY
        "${LUA_SOURCE_DIR}/lua.h"
        "${LUA_SOURCE_DIR}/lua.hpp"
        "${LUA_SOURCE_DIR}/luaconf.h"
        "${LUA_SOURCE_DIR}/lauxlib.h"
        "${LUA_SOURCE_DIR}/lualib.h"
    DESTINATION "${LUA_INCLUDE_ROOT}"
)

function(lua_set_output_directory target property directory)
    set_target_properties(
        "${target}"
        PROPERTIES "${property}_OUTPUT_DIRECTORY" "${directory}"
    )
    foreach(config Debug Release RelWithDebInfo MinSizeRel)
        string(TOUPPER "${config}" config_upper)
        set_target_properties(
            "${target}"
            PROPERTIES
                "${property}_OUTPUT_DIRECTORY_${config_upper}" "${directory}"
        )
    endforeach()
endfunction()

add_library(lua_static STATIC ${LUA_LIBRARY_SOURCES})
add_library(lua_shared SHARED ${LUA_LIBRARY_SOURCES})

foreach(target lua_static lua_shared)
    target_include_directories(
        "${target}"
        PUBLIC
            "$<BUILD_INTERFACE:${LUA_SOURCE_DIR}>"
            "$<INSTALL_INTERFACE:include>"
    )
    target_compile_definitions("${target}" PRIVATE NDEBUG)
    if(MSVC)
        set_property(
            TARGET "${target}"
            PROPERTY MSVC_RUNTIME_LIBRARY MultiThreadedDLL
        )
    endif()
endforeach()

target_compile_definitions(
    lua_shared
    PRIVATE LUA_BUILD_AS_DLL
    INTERFACE LUA_BUILD_AS_DLL
)

set_target_properties(lua_static lua_shared PROPERTIES OUTPUT_NAME lua)
lua_set_output_directory(lua_static ARCHIVE "${LUA_LIBRARY_ROOT}")
lua_set_output_directory(lua_shared ARCHIVE "${LUA_BINARY_ROOT}")
lua_set_output_directory(lua_shared LIBRARY "${LUA_BINARY_ROOT}")
lua_set_output_directory(lua_shared RUNTIME "${LUA_BINARY_ROOT}")

add_executable(lua "${LUA_SOURCE_DIR}/lua.c")
add_executable(luac "${LUA_SOURCE_DIR}/luac.c")
target_link_libraries(lua PRIVATE lua_static)
target_link_libraries(luac PRIVATE lua_static)
lua_set_output_directory(lua RUNTIME "${LUA_BINARY_ROOT}")
lua_set_output_directory(luac RUNTIME "${LUA_BINARY_ROOT}")

add_library(Lua::Static ALIAS lua_static)
add_library(Lua::Shared ALIAS lua_shared)

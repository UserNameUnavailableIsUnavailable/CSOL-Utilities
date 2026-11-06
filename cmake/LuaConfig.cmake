include_guard(GLOBAL)

if(TARGET lua_static OR TARGET lua_shared)
    return()
endif()

set(LUA_VERSION "5.4.0")
get_filename_component(
    LUA_DEPENDENCIES_ROOT
    "${CMAKE_CURRENT_LIST_DIR}/../dependencies"
    ABSOLUTE
)
set(LUA_INCLUDE_ROOT "${LUA_DEPENDENCIES_ROOT}/include")
set(LUA_LIBRARY_ROOT "${LUA_DEPENDENCIES_ROOT}/lib/lua-${LUA_VERSION}")
set(LUA_BINARY_ROOT "${LUA_DEPENDENCIES_ROOT}/bin/lua-${LUA_VERSION}")

add_library(lua_static STATIC IMPORTED)
set_target_properties(
    lua_static
    PROPERTIES
        IMPORTED_LOCATION
            "${LUA_LIBRARY_ROOT}/${CMAKE_STATIC_LIBRARY_PREFIX}lua${CMAKE_STATIC_LIBRARY_SUFFIX}"
        INTERFACE_INCLUDE_DIRECTORIES "${LUA_INCLUDE_ROOT}"
)

add_library(lua_shared SHARED IMPORTED)
set_target_properties(
    lua_shared
    PROPERTIES
        IMPORTED_LOCATION
            "${LUA_BINARY_ROOT}/${CMAKE_SHARED_LIBRARY_PREFIX}lua${CMAKE_SHARED_LIBRARY_SUFFIX}"
        INTERFACE_COMPILE_DEFINITIONS LUA_BUILD_AS_DLL
        INTERFACE_INCLUDE_DIRECTORIES "${LUA_INCLUDE_ROOT}"
)
if(WIN32)
    set_target_properties(
        lua_shared
        PROPERTIES
            IMPORTED_IMPLIB
                "${LUA_BINARY_ROOT}/${CMAKE_IMPORT_LIBRARY_PREFIX}lua${CMAKE_IMPORT_LIBRARY_SUFFIX}"
    )
endif()

add_library(Lua::Static ALIAS lua_static)
add_library(Lua::Shared ALIAS lua_shared)

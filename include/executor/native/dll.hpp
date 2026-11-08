#pragma once

#define EXPORT __declspec(dllexport)
#define NATIVE_API extern "C"

NATIVE_API {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
EXPORT int luaopen_executor_Native(lua_State *L);

EXPORT int Native_GetMonotonicTimestamp(lua_State *L);
EXPORT int Native_GetSystemTimestamp(lua_State* L);
}
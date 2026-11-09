#pragma once

#define EXPORT __declspec(dllexport)

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

EXPORT int luaopen_executor_Native(struct lua_State *L);

EXPORT int Native_GetMonotonicTimepoint(struct lua_State *L);
EXPORT int Native_GetSystemTimepoint(struct lua_State *L);
EXPORT int Native_Sleep(struct lua_State *L);
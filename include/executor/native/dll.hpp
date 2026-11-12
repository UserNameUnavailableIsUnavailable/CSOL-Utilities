#pragma once

#define EXPORT __declspec(dllexport)
#define NATIVE extern "C"

extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}

NATIVE EXPORT int luaopen_executor_Native(lua_State* L);

void InitializeNamespace(lua_State* L);
void InitializeKeyboard(lua_State* L);
void InitializeMouse(lua_State* L);
void InitializeTime(lua_State* L);

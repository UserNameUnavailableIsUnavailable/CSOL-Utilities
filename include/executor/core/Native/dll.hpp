#pragma once

#define EXPORT __declspec(dllexport)
#define NATIVE extern "C"

extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}

NATIVE EXPORT int luaopen_executor_core_Native(lua_State* L);

void InitializeRaw(lua_State* L);
void InitializeKeyboard(lua_State* L);
void InitializeMouse(lua_State* L);
void InitializeTime(lua_State* L);
void InitializeDebug(lua_State* L);
void InitializeRuntime(lua_State* L);
void InitializeStorage(lua_State* L);
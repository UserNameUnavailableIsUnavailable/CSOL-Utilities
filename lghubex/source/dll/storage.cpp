#include <lghubex/dll/hook.h>

#include <lua.hpp>

namespace {

void* g_NativeStorage = nullptr;

int GetNativeStorage(lua_State* state) {
  if (g_NativeStorage == nullptr) {
    lua_pushnil(state);
  } else {
    lua_pushlightuserdata(state, g_NativeStorage);
  }
  return 1;
}

int SetNativeStorage(lua_State* state) {
  luaL_checktype(state, 1, LUA_TLIGHTUSERDATA);
  void* storage = lua_touserdata(state, 1);
  luaL_argcheck(state, storage != nullptr, 1, "storage must not be null");
  luaL_argcheck(state, g_NativeStorage == nullptr || g_NativeStorage == storage,
                1, "Native storage is initialized once and cannot be replaced");
  g_NativeStorage = storage;
  return 0;
}

}  // namespace

void InstallNativeStorage(lua_State* state) {
  lua_newtable(state);
  lua_pushcfunction(state, GetNativeStorage);
  lua_setfield(state, -2, "GetStorage");
  lua_pushcfunction(state, SetNativeStorage);
  lua_setfield(state, -2, "SetStorage");
  lua_setfield(state, LUA_REGISTRYINDEX, "executor.core.Native.Storage");
}

#include <lghubex/dll/hook.h>

#include <executor/native/global_storage.hpp>

namespace {

NativeStorage& GetGlobalStorage() {
  static NativeStorage storage;
  return storage;
}

int GetNativeStorage(lua_State* state) {
  lua_pushlightuserdata(state, &GetGlobalStorage());
  return 1;
}

int SetNativeStorage(lua_State* state) {
  luaL_checktype(state, 1, LUA_TLIGHTUSERDATA);
  luaL_argcheck(state, lua_touserdata(state, 1) == &GetGlobalStorage(), 1,
                "Native storage is initialized once and cannot be replaced");
  return 0;
}

}  // namespace

void InstallNativeStorage(lua_State* state) {
  GetGlobalStorage();
  lua_newtable(state);
  lua_pushcfunction(state, GetNativeStorage);
  lua_setfield(state, -2, "GetStorage");
  lua_pushcfunction(state, SetNativeStorage);
  lua_setfield(state, -2, "SetStorage");
  lua_setfield(state, LUA_REGISTRYINDEX, kNativeStorageRegistryKey);
}

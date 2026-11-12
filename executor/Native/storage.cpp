#include <executor/native/global_storage.hpp>

NativeStorage& GetStorage(lua_State* state) {
  lua_getfield(state, LUA_REGISTRYINDEX, kNativeStorageRegistryKey);
  if (!lua_istable(state, -1)) {
    luaL_error(state,
               "Native storage accessors are unavailable; update the hook DLL");
  }
  lua_getfield(state, -1, "GetStorage");
  lua_call(state, 0, 1);
  if (!lua_islightuserdata(state, -1) || lua_touserdata(state, -1) == nullptr) {
    luaL_error(state, "hook returned invalid Native storage");
  }
  auto* storage = static_cast<NativeStorage*>(lua_touserdata(state, -1));
  lua_pop(state, 2);
  return *storage;
}

void SetStorage(lua_State* state, NativeStorage* storage) {
  lua_getfield(state, LUA_REGISTRYINDEX, kNativeStorageRegistryKey);
  if (!lua_istable(state, -1)) {
    luaL_error(state,
               "Native storage accessors are unavailable; update the hook DLL");
  }
  lua_getfield(state, -1, "SetStorage");
  lua_pushlightuserdata(state, storage);
  lua_call(state, 1, 0);
  lua_pop(state, 1);
}

#include <executor/core/Native/global_storage.hpp>
#include <executor/core/Native/runtime.hpp>

namespace {

NativeStorage* GetStoragePointer(lua_State* state) {
  lua_getfield(state, LUA_REGISTRYINDEX, kNativeStorageRegistryKey);
  if (!lua_istable(state, -1)) {
    luaL_error(state,
               "Native storage interface is unavailable; update the hook DLL");
  }
  lua_getfield(state, -1, "GetStorage");
  if (!lua_isfunction(state, -1)) {
    luaL_error(state, "hook Native storage getter is unavailable");
  }
  lua_call(state, 0, 1);
  NativeStorage* storage = nullptr;
  if (lua_islightuserdata(state, -1)) {
    storage = static_cast<NativeStorage*>(lua_touserdata(state, -1));
  } else if (!lua_isnil(state, -1)) {
    luaL_error(state, "hook returned invalid Native storage");
  }
  lua_pop(state, 2);
  return storage;
}

void StoreNativeStorage(lua_State* state, NativeStorage* storage) {
  lua_getfield(state, LUA_REGISTRYINDEX, kNativeStorageRegistryKey);
  if (!lua_istable(state, -1)) {
    luaL_error(state,
               "Native storage interface is unavailable; update the hook DLL");
  }
  lua_getfield(state, -1, "SetStorage");
  if (!lua_isfunction(state, -1)) {
    luaL_error(state, "hook Native storage setter is unavailable");
  }
  lua_pushlightuserdata(state, storage);
  lua_call(state, 1, 0);
  lua_pop(state, 1);
}

}  // namespace

NativeStorage::NativeStorage() = default;
NativeStorage::~NativeStorage() = default;

void InitializeStorage(lua_State* state) {
  if (GetStoragePointer(state) == nullptr) {
    StoreNativeStorage(state, new NativeStorage());
  }
}

NativeStorage& GetStorage(lua_State* state) {
  NativeStorage* storage = GetStoragePointer(state);
  if (storage == nullptr) {
    luaL_error(state, "Native storage has not been initialized");
  }
  return *storage;
}

void SetStorage(lua_State* state, NativeStorage* storage) {
  StoreNativeStorage(state, storage);
}

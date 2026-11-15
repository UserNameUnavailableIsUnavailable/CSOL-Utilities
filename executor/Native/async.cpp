#include <executor/native/global_storage.hpp>

namespace {

int CallRuntime(lua_State* state, const char* name) {
  const int count = lua_gettop(state);
  lua_getfield(state, LUA_REGISTRYINDEX, kNativeStorageRegistryKey);
  if (!lua_istable(state, -1)) {
    return luaL_error(state, "Native storage accessors are unavailable");
  }
  lua_getfield(state, -1, name);
  if (!lua_isfunction(state, -1)) {
    return luaL_error(state, "Native runtime accessor %s is unavailable", name);
  }
  lua_remove(state, -2);
  lua_insert(state, 1);
  lua_call(state, count, LUA_MULTRET);
  return lua_gettop(state);
}

int Sleep(lua_State* state) { return CallRuntime(state, "Runtime_sleep"); }

int Poll(lua_State* state) { return CallRuntime(state, "Runtime_poll"); }

int Close(lua_State* state) { return CallRuntime(state, "Runtime_close"); }

int Open(lua_State* state) { return CallRuntime(state, "Runtime_open"); }

}  // namespace

void InitializeRuntime(lua_State* state) {
  // Keep the module table separate from the accessor's argument stack.
  lua_pushcfunction(state, Open);
  lua_call(state, 0, 0);
  const luaL_Reg kFunctions[] = {
      {"Runtime_sleep", Sleep}, {"Runtime_poll", Poll},
      {"Runtime_close", Close}, {"Runtime_open", Open},
      {nullptr, nullptr},
  };
  luaL_setfuncs(state, kFunctions, 0);
}

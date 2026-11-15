#include <executor/native/dll.hpp>
#include <executor/native/global_storage.hpp>

NATIVE EXPORT int luaopen_executor_Native(lua_State* state) {
  lua_newtable(state);
  InitializeRaw(state);
  InitializeKeyboard(state);
  InitializeMouse(state);
  InitializeTime(state);
  InitializeRuntime(state);
  return 1;
}

#include <executor/core/Native/dll.hpp>
#include <executor/core/Native/global_storage.hpp>

NATIVE EXPORT int luaopen_executor_core_Native(lua_State* state) {
  InitializeStorage(state);
  lua_newtable(state);
  InitializeRaw(state);
  InitializeKeyboard(state);
  InitializeMouse(state);
  InitializeTime(state);
  InitializeRuntime(state);
  return 1;
}

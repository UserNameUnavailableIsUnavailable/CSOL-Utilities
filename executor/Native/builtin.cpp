#include <executor/native/input.hpp>

namespace {

int Sleep(lua_State* state) { return input::CallGlobal(state, "Sleep"); }

}  // namespace

void InitializeNamespace(lua_State* state) {
  const luaL_Reg kFunctions[] = {
      {"Sleep", Sleep},
      {nullptr, nullptr},
  };
  luaL_setfuncs(state, kFunctions, 0);
}

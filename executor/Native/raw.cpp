#include <executor/native/dll.hpp>

#include <thread>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace {

int Sleep(lua_State* state) {
  auto ms = luaL_checkinteger(state, 1);
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
  return 0;
}

int GetThreadId(lua_State* state) {
  auto tid = GetCurrentThreadId();
  lua_pushinteger(state, tid);
  return 1;
}

}  // namespace

void InitializeRaw(lua_State* state) {
  const luaL_Reg kFunctions[] = {
      {"Raw_sleep", Sleep},
      {"Raw_get_thread_id", GetThreadId},
      {nullptr, nullptr},
  };
  luaL_setfuncs(state, kFunctions, 0);
}

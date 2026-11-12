#include <chrono>
#include <executor/native/dll.hpp>
#include <executor/native/global_storage.hpp>

namespace {

int GetMonotonicTimestamp(lua_State* state) {
  return GetStorage(state).time.GetMonotonicTimestamp(state);
}

int GetSystemTimestamp(lua_State* state) {
  return GetStorage(state).time.GetSystemTimestamp(state);
}

}  // namespace

int Time::GetMonotonicTimestamp(lua_State* state) const {
  const auto now = std::chrono::steady_clock::now();
  const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                           now - monotonic_origin_)
                           .count();
  lua_pushinteger(state, elapsed);
  return 1;
}

int Time::GetSystemTimestamp(lua_State* state) const {
  const auto now = std::chrono::system_clock::now();
  const auto timepoint = std::chrono::duration_cast<std::chrono::milliseconds>(
                             now.time_since_epoch())
                             .count();
  lua_pushinteger(state, timepoint);
  return 1;
}

void InitializeTime(lua_State* state) {
  const luaL_Reg kFunctions[] = {
      {"Time_GetMonotonicTimestamp", GetMonotonicTimestamp},
      {"Time_GetSystemTimestamp", GetSystemTimestamp},
      {nullptr, nullptr},
  };
  luaL_setfuncs(state, kFunctions, 0);
}

#include <lghubex/dll/hook.h>

#include <exception>
#include <executor/native/global_storage.hpp>
#include <executor/native/runtime.hpp>
#include <string>

NativeStorage::NativeStorage() = default;
NativeStorage::~NativeStorage() = default;

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

lua_State* MainThread(lua_State* state) {
  lua_rawgeti(state, LUA_REGISTRYINDEX, LUA_RIDX_MAINTHREAD);
  lua_State* main = lua_tothread(state, -1);
  lua_pop(state, 1);
  return main;
}

AsyncRuntime& RequireRuntime(lua_State* state) {
  auto& storage = GetGlobalStorage();
  if (storage.runtime_owner != MainThread(state) || !storage.runtime) {
    throw std::runtime_error(
        "async runtime is closed or belongs to another VM");
  }
  return *storage.runtime;
}

// Lua errors longjmp: finish unwinding C++ objects before raising an error.
template <typename Function>
int RuntimeCall(lua_State* state, Function function) {
  bool failed = false;
  int results = 0;
  {
    std::string message;
    try {
      results = function();
    } catch (const std::exception& error) {
      failed = true;
      message = error.what();
    }
    if (failed) {
      lua_pushlstring(state, message.data(), message.size());
    }
  }
  return failed ? lua_error(state) : results;
}

int AttachRuntime(lua_State* state) {
  return RuntimeCall(state, [state] {
    auto& storage = GetGlobalStorage();
    auto* owner = MainThread(state);
    if (storage.runtime_owner != owner) {
      throw std::runtime_error("async runtime belongs to another VM");
    }
    if (!storage.runtime) {
      storage.runtime = std::make_unique<AsyncRuntime>();
    }
    return 0;
  });
}

int SleepRuntime(lua_State* state) {
  luaL_argcheck(state, lua_gettop(state) == 2, 1, "expected task id and delay");
  luaL_checktype(state, 1, LUA_TNUMBER);
  luaL_checktype(state, 2, LUA_TNUMBER);
  const lua_Integer task_id = luaL_checkinteger(state, 1);
  const lua_Integer delay = luaL_checkinteger(state, 2);
  return RuntimeCall(state, [state, task_id, delay] {
    RequireRuntime(state).Sleep(task_id, std::chrono::milliseconds(delay));
    return 0;
  });
}

int PollRuntime(lua_State* state) {
  luaL_argcheck(state, lua_gettop(state) <= 1, 1,
                "expected an optional timeout");
  lua_Integer timeout = -1;
  if (lua_gettop(state) == 1) {
    luaL_checktype(state, 1, LUA_TNUMBER);
    timeout = luaL_checkinteger(state, 1);
  }
  luaL_argcheck(state, timeout >= -1 && timeout <= INT32_MAX, 1,
                "timeout must be -1 or from 0 to 2147483647 ms");
  return RuntimeCall(state, [state, timeout] {
    auto ready = RequireRuntime(state).Poll(static_cast<std::int32_t>(timeout));
    lua_createtable(state, static_cast<int>(ready.size()), 0);
    lua_Integer index = 1;
    for (const auto id : ready) {
      lua_pushinteger(state, id);
      lua_rawseti(state, -2, index++);
    }
    return 1;
  });
}

int CloseRuntime(lua_State* state) {
  return RuntimeCall(state, [state] {
    auto& storage = GetGlobalStorage();
    if (storage.runtime_owner != MainThread(state)) {
      throw std::runtime_error("async runtime belongs to another VM");
    }
    storage.runtime.reset();
    return 0;
  });
}

}  // namespace

void InstallNativeStorage(lua_State* state) {
  lua_getfield(state, LUA_REGISTRYINDEX, kNativeStorageRegistryKey);
  const bool already_attached = lua_istable(state, -1);
  lua_pop(state, 1);
  auto& storage = GetGlobalStorage();
  if (!already_attached) {
    storage.runtime.reset();
    storage.runtime_owner = MainThread(state);
  }
  lua_newtable(state);
  lua_pushcfunction(state, GetNativeStorage);
  lua_setfield(state, -2, "GetStorage");
  lua_pushcfunction(state, SetNativeStorage);
  lua_setfield(state, -2, "SetStorage");
  const luaL_Reg kRuntimeFunctions[] = {
      {"Runtime_open", AttachRuntime},
      {"Runtime_sleep", SleepRuntime},
      {"Runtime_poll", PollRuntime},
      {"Runtime_close", CloseRuntime},
      {nullptr, nullptr},
  };
  luaL_setfuncs(state, kRuntimeFunctions, 0);
  lua_setfield(state, LUA_REGISTRYINDEX, kNativeStorageRegistryKey);
}

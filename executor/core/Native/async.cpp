#include <exception>
#include <executor/core/Native/global_storage.hpp>
#include <executor/core/Native/runtime.hpp>
#include <string>

namespace {

constexpr char kRuntimeFinalizerRegistryKey[] =
    "executor.core.Native.RuntimeFinalizer";

lua_State* MainThread(lua_State* state) {
  lua_rawgeti(state, LUA_REGISTRYINDEX, LUA_RIDX_MAINTHREAD);
  lua_State* main = lua_tothread(state, -1);
  lua_pop(state, 1);
  return main;
}

void EnsureRuntime(lua_State* state) {
  auto& storage = GetStorage(state);
  lua_State* owner = MainThread(state);
  if (storage.runtime_owner != nullptr && storage.runtime_owner != owner) {
    throw std::runtime_error("async runtime belongs to another VM");
  }
  if (storage.runtime_owner == nullptr) {
    storage.runtime_owner = owner;
  }
  if (!storage.runtime) {
    storage.runtime = std::make_unique<AsyncRuntime>();
  }
}

void AttachRuntime(lua_State* state) {
  auto& storage = GetStorage(state);
  storage.runtime.reset();
  storage.runtime_owner = MainThread(state);
  storage.runtime = std::make_unique<AsyncRuntime>();
}

AsyncRuntime& RequireRuntime(lua_State* state) {
  auto& storage = GetStorage(state);
  if (storage.runtime_owner != MainThread(state) || !storage.runtime) {
    throw std::runtime_error(
        "async runtime is closed or belongs to another VM");
  }
  return *storage.runtime;
}

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

int Open(lua_State* state) {
  return RuntimeCall(state, [state] {
    EnsureRuntime(state);
    return 0;
  });
}

int Attach(lua_State* state) {
  return RuntimeCall(state, [state] {
    AttachRuntime(state);
    return 0;
  });
}

int Sleep(lua_State* state) {
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

int Poll(lua_State* state) {
  luaL_argcheck(state, lua_gettop(state) <= 1, 1,
                "expected an optional timeout");
  const lua_Integer timeout =
      lua_gettop(state) == 0 ? -1 : luaL_checkinteger(state, 1);
  if (lua_gettop(state) == 1) {
    luaL_checktype(state, 1, LUA_TNUMBER);
  }
  luaL_argcheck(state, timeout >= -1 && timeout <= INT32_MAX, 1,
                "timeout must be -1 or from 0 to 2147483647 ms");
  return RuntimeCall(state, [state, timeout] {
    const auto ready =
        RequireRuntime(state).Poll(static_cast<std::int32_t>(timeout));
    lua_createtable(state, static_cast<int>(ready.size()), 0);
    lua_Integer index = 1;
    for (const auto id : ready) {
      lua_pushinteger(state, id);
      lua_rawseti(state, -2, index++);
    }
    return 1;
  });
}

int Close(lua_State* state) {
  return RuntimeCall(state, [state] {
    auto& storage = GetStorage(state);
    if (storage.runtime_owner == nullptr) {
      return 0;
    }
    if (storage.runtime_owner != MainThread(state)) {
      throw std::runtime_error("async runtime belongs to another VM");
    }
    storage.runtime.reset();
    storage.runtime_owner = nullptr;
    return 0;
  });
}

int FinalizeRuntime(lua_State* state) {
  auto& storage = GetStorage(state);
  if (storage.runtime_owner == MainThread(state)) {
    storage.runtime.reset();
    storage.runtime_owner = nullptr;
  }
  return 0;
}

void RegisterRuntimeFinalizer(lua_State* state) {
  lua_getfield(state, LUA_REGISTRYINDEX, kRuntimeFinalizerRegistryKey);
  const bool registered = !lua_isnil(state, -1);
  lua_pop(state, 1);
  if (registered) {
    return;
  }
  lua_newuserdatauv(state, 1, 0);
  lua_newtable(state);
  lua_pushcfunction(state, FinalizeRuntime);
  lua_setfield(state, -2, "__gc");
  lua_setmetatable(state, -2);
  lua_setfield(state, LUA_REGISTRYINDEX, kRuntimeFinalizerRegistryKey);
}

}  // namespace

void InitializeRuntime(lua_State* state) {
  RegisterRuntimeFinalizer(state);
  lua_pushcfunction(state, Attach);
  lua_call(state, 0, 0);
  const luaL_Reg kFunctions[] = {
      {"Runtime_sleep", Sleep}, {"Runtime_poll", Poll},
      {"Runtime_close", Close}, {"Runtime_open", Open},
      {nullptr, nullptr},
  };
  luaL_setfuncs(state, kFunctions, 0);
}

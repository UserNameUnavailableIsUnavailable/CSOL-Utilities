#include <executor/native/input.hpp>
#include <new>

namespace input {
namespace {

void PushInput(lua_State* state, const Input& value) {
  if (const auto* number = std::get_if<lua_Integer>(&value)) {
    lua_pushinteger(state, *number);
  } else {
    const auto& name = std::get<std::string>(value);
    lua_pushlstring(state, name.data(), name.size());
  }
}

void AddPressed(lua_State* state, PressedInputs& inputs, int index,
                Identifier identifier) {
  const lua_Integer id = identifier(state, index);
  if (inputs.contains(id)) {
    return;
  }
  bool allocated = true;
  try {
    if (lua_type(state, index) == LUA_TSTRING) {
      size_t length;
      const char* name = lua_tolstring(state, index, &length);
      inputs.emplace(id, std::string(name, length));
    } else {
      inputs.emplace(id, lua_tointeger(state, index));
    }
  } catch (const std::bad_alloc&) {
    allocated = false;
  }
  if (!allocated) {
    luaL_error(state, "cannot allocate pressed input tracking");
  }
}

}  // namespace

void RequireFunction(lua_State* state, const char* name) {
  lua_getglobal(state, name);
  if (!lua_isfunction(state, -1)) {
    luaL_error(state, "Logitech function %s is unavailable", name);
  }
  lua_pop(state, 1);
}

int CallGlobal(lua_State* state, const char* name) {
  const int count = lua_gettop(state);
  RequireFunction(state, name);
  lua_getglobal(state, name);
  lua_insert(state, 1);
  lua_call(state, count, LUA_MULTRET);
  return lua_gettop(state);
}

int ChangePressed(lua_State* state, PressedInputs& inputs,
                  Identifier identifier, const char* function_name,
                  bool pressed) {
  const int count = lua_gettop(state);
  luaL_argcheck(state, count > 0, 1, "at least one input is required");
  for (int i = 1; i <= count; ++i) {
    identifier(state, i);
  }
  luaL_checkstack(state, count + 2, "too many input arguments");
  RequireFunction(state, function_name);
  lua_getglobal(state, function_name);
  for (int i = 1; i <= count; ++i) {
    lua_pushvalue(state, i);
  }
  lua_call(state, count, LUA_MULTRET);
  const int results = lua_gettop(state) - count;
  for (int i = 1; i <= count; ++i) {
    if (pressed) {
      AddPressed(state, inputs, i, identifier);
    } else {
      inputs.erase(identifier(state, i));
    }
  }
  return results;
}

int ListPressed(lua_State* state, const PressedInputs& inputs) {
  lua_newtable(state);
  lua_Integer index = 1;
  for (const auto& entry : inputs) {
    PushInput(state, entry.second);
    lua_rawseti(state, -2, index++);
  }
  return 1;
}

int Reset(lua_State* state, PressedInputs& inputs, const char* function_name) {
  while (!inputs.empty()) {
    const lua_Integer id = inputs.begin()->first;
    RequireFunction(state, function_name);
    lua_getglobal(state, function_name);
    PushInput(state, inputs.begin()->second);
    if (lua_pcall(state, 1, 0, 0) != LUA_OK) {
      const char* message = lua_tostring(state, -1);
      luaL_error(state, "Native reset: %s for tracked input %I failed: %s",
                 function_name, id,
                 message != nullptr ? message : "(non-string host error)");
    }
    inputs.erase(id);
  }
  return 0;
}

}  // namespace input

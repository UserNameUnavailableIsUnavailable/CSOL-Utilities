#pragma once

#include <executor/native/dll.hpp>
#include <string>
#include <unordered_map>
#include <variant>

namespace input {

using Input = std::variant<lua_Integer, std::string>;
using PressedInputs = std::unordered_map<lua_Integer, Input>;
using Identifier = lua_Integer (*)(lua_State*, int);

void RequireFunction(lua_State* state, const char* name);
int CallGlobal(lua_State* state, const char* name);
int ChangePressed(lua_State* state, PressedInputs& inputs,
                  Identifier identifier, const char* function_name,
                  bool pressed);
int ListPressed(lua_State* state, const PressedInputs& inputs);
int Reset(lua_State* state, PressedInputs& inputs, const char* function_name);

}  // namespace input

#pragma once

#include <executor/native/input.hpp>

class Keyboard {
 public:
  int Press(lua_State* state);
  int Release(lua_State* state);
  int Click(lua_State* state);
  int Reset(lua_State* state);
  int ListPressed(lua_State* state) const;
  int IsPressed(lua_State* state) const;
  int IsLocked(lua_State* state) const;

 private:
  input::PressedInputs pressed_inputs_;
};

#include <executor/native/global_storage.hpp>

namespace {

lua_Integer ButtonIdentifier(lua_State* state, int index) {
  int is_integer = 0;
  const lua_Integer button = lua_type(state, index) == LUA_TNUMBER
                                 ? lua_tointegerx(state, index, &is_integer)
                                 : 0;
  if (!is_integer || button < 1 || button > 5) {
    return luaL_argerror(state, index,
                         "expected a mouse button number from 1 to 5");
  }
  return button;
}

int PressMouseButton(lua_State* state) {
  return GetStorage(state).mouse.Press(state);
}

int ReleaseMouseButton(lua_State* state) {
  return GetStorage(state).mouse.Release(state);
}

int ClickMouseButton(lua_State* state) {
  return GetStorage(state).mouse.Click(state);
}

int ResetMouse(lua_State* state) {
  return GetStorage(state).mouse.Reset(state);
}

int ListPressedButtons(lua_State* state) {
  return GetStorage(state).mouse.ListPressed(state);
}

int IsPressed(lua_State* state) {
  luaL_argcheck(state, lua_gettop(state) == 1, 1,
                "exactly one mouse button is required");
  ButtonIdentifier(state, 1);
  return input::CallGlobal(state, "IsMouseButtonPressed");
}

int GetPosition(lua_State* state) {
  return input::CallGlobal(state, "GetMousePosition");
}

int SetPosition(lua_State* state) {
  return input::CallGlobal(state, "MoveMouseToVirtual");
}

int Move(lua_State* state) {
  return input::CallGlobal(state, "MoveMouseRelative");
}

int Scroll(lua_State* state) {
  return input::CallGlobal(state, "MoveMouseWheel");
}

}  // namespace

int Mouse::Press(lua_State* state) {
  luaL_argcheck(state, lua_gettop(state) == 1, 1,
                "exactly one mouse button is required");
  return input::ChangePressed(state, pressed_inputs_, ButtonIdentifier,
                              "PressMouseButton", true);
}

int Mouse::Release(lua_State* state) {
  luaL_argcheck(state, lua_gettop(state) == 1, 1,
                "exactly one mouse button is required");
  return input::ChangePressed(state, pressed_inputs_, ButtonIdentifier,
                              "ReleaseMouseButton", false);
}

int Mouse::Reset(lua_State* state) {
  return input::Reset(state, pressed_inputs_, "ReleaseMouseButton");
}

int Mouse::Click(lua_State* state) {
  luaL_argcheck(state, lua_gettop(state) == 1, 1,
                "exactly one mouse button is required");
  return input::ChangePressed(state, pressed_inputs_, ButtonIdentifier,
                              "PressAndReleaseMouseButton", false);
}

int Mouse::ListPressed(lua_State* state) const {
  return input::ListPressed(state, pressed_inputs_);
}

void InitializeMouse(lua_State* state) {
  input::RequireFunction(state, "PressMouseButton");
  input::RequireFunction(state, "ReleaseMouseButton");
  GetStorage(state).mouse.Reset(state);
  const luaL_Reg kFunctions[] = {
      {"Mouse_Press", PressMouseButton},
      {"Mouse_Release", ReleaseMouseButton},
      {"Mouse_Click", ClickMouseButton},
      {"Mouse_Reset", ResetMouse},
      {"Mouse_ListPressed", ListPressedButtons},
      {"Mouse_IsPressed", IsPressed},
      {"Mouse_GetPosition", GetPosition},
      {"Mouse_SetPosition", SetPosition},
      {"Mouse_Move", Move},
      {"Mouse_Scroll", Scroll},
      {nullptr, nullptr},
  };
  luaL_setfuncs(state, kFunctions, 0);
}

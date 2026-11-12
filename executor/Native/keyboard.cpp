#include <Windows.h>

#include <cstring>
#include <executor/native/global_storage.hpp>

namespace {

struct Key {
  const char* name;
  lua_Integer code;
};

constexpr Key kKeys[] = {
    {"escape", 0x01},     {"f1", 0x3b},
    {"f2", 0x3c},         {"f3", 0x3d},
    {"f4", 0x3e},         {"f5", 0x3f},
    {"f6", 0x40},         {"f7", 0x41},
    {"f8", 0x42},         {"f9", 0x43},
    {"f10", 0x44},        {"f11", 0x57},
    {"f12", 0x58},        {"f13", 0x64},
    {"f14", 0x65},        {"f15", 0x66},
    {"f16", 0x67},        {"f17", 0x68},
    {"f18", 0x69},        {"f19", 0x6a},
    {"f20", 0x6b},        {"f21", 0x6c},
    {"f22", 0x6d},        {"f23", 0x6e},
    {"f24", 0x76},        {"tilde", 0x29},
    {"1", 0x02},          {"2", 0x03},
    {"3", 0x04},          {"4", 0x05},
    {"5", 0x06},          {"6", 0x07},
    {"7", 0x08},          {"8", 0x09},
    {"9", 0x0a},          {"0", 0x0b},
    {"minus", 0x0c},      {"equal", 0x0d},
    {"backspace", 0x0e},  {"tab", 0x0f},
    {"q", 0x10},          {"w", 0x11},
    {"e", 0x12},          {"r", 0x13},
    {"t", 0x14},          {"y", 0x15},
    {"u", 0x16},          {"i", 0x17},
    {"o", 0x18},          {"p", 0x19},
    {"lbracket", 0x1a},   {"rbracket", 0x1b},
    {"backslash", 0x2b},  {"capslock", 0x3a},
    {"a", 0x1e},          {"s", 0x1f},
    {"d", 0x20},          {"f", 0x21},
    {"g", 0x22},          {"h", 0x23},
    {"j", 0x24},          {"k", 0x25},
    {"l", 0x26},          {"semicolon", 0x27},
    {"quote", 0x28},      {"enter", 0x1c},
    {"lshift", 0x2a},     {"non_us_slash", 0x56},
    {"z", 0x2c},          {"x", 0x2d},
    {"c", 0x2e},          {"v", 0x2f},
    {"b", 0x30},          {"n", 0x31},
    {"m", 0x32},          {"comma", 0x33},
    {"period", 0x34},     {"slash", 0x35},
    {"rshift", 0x36},     {"lctrl", 0x1d},
    {"lgui", 0x15b},      {"lalt", 0x38},
    {"spacebar", 0x39},   {"ralt", 0x138},
    {"rgui", 0x15c},      {"appkey", 0x15d},
    {"rctrl", 0x11d},     {"printscreen", 0x137},
    {"scrolllock", 0x46}, {"pause", 0x146},
    {"insert", 0x152},    {"home", 0x147},
    {"pageup", 0x149},    {"delete", 0x153},
    {"end", 0x14f},       {"pagedown", 0x151},
    {"up", 0x148},        {"left", 0x14b},
    {"down", 0x150},      {"right", 0x14d},
    {"numlock", 0x45},    {"numslash", 0x135},
    {"numminus", 0x4a},   {"numstar", 0x37},
    {"num7", 0x47},       {"num8", 0x48},
    {"num9", 0x49},       {"numplus", 0x4e},
    {"num4", 0x4b},       {"num5", 0x4c},
    {"num6", 0x4d},       {"num1", 0x4f},
    {"num2", 0x50},       {"num3", 0x51},
    {"num0", 0x52},       {"numperiod", 0x53},
    {"numenter", 0x11c},
};

lua_Integer KeyIdentifier(lua_State* state, int index) {
  if (lua_type(state, index) == LUA_TSTRING) {
    size_t length;
    const char* name = lua_tolstring(state, index, &length);
    for (const auto& key : kKeys) {
      if (length == std::strlen(key.name) &&
          std::memcmp(name, key.name, length) == 0) {
        return key.code;
      }
    }
  } else if (lua_type(state, index) == LUA_TNUMBER) {
    int is_integer;
    const lua_Integer code = lua_tointegerx(state, index, &is_integer);
    if (is_integer) {
      for (const auto& key : kKeys) {
        if (code == key.code) {
          return code;
        }
      }
    }
  }
  return luaL_argerror(state, index,
                       "expected a supported Logitech key name or scan code");
}

int PressKeyboardKey(lua_State* state) {
  return GetStorage(state).keyboard.Press(state);
}

int ReleaseKeyboardKey(lua_State* state) {
  return GetStorage(state).keyboard.Release(state);
}

int ClickKeyboardKey(lua_State* state) {
  return GetStorage(state).keyboard.Click(state);
}

int ResetKeyboard(lua_State* state) {
  return GetStorage(state).keyboard.Reset(state);
}

int ListPressedKeyboardKeys(lua_State* state) {
  return GetStorage(state).keyboard.ListPressed(state);
}

int IsPressed(lua_State* state) {
  return GetStorage(state).keyboard.IsPressed(state);
}

int IsLocked(lua_State* state) {
  return GetStorage(state).keyboard.IsLocked(state);
}

int VirtualKey(lua_State* state, lua_Integer code) {
  // Preserve keypad identity and handle special scan codes before Win32 mapping.
  switch (code) {
    case 0x146:
      return VK_PAUSE;
    case 0x137:
      return VK_SNAPSHOT;
    case 0x45:
      return VK_NUMLOCK;
    case 0x47:
      return VK_NUMPAD7;
    case 0x48:
      return VK_NUMPAD8;
    case 0x49:
      return VK_NUMPAD9;
    case 0x4b:
      return VK_NUMPAD4;
    case 0x4c:
      return VK_NUMPAD5;
    case 0x4d:
      return VK_NUMPAD6;
    case 0x4f:
      return VK_NUMPAD1;
    case 0x50:
      return VK_NUMPAD2;
    case 0x51:
      return VK_NUMPAD3;
    case 0x52:
      return VK_NUMPAD0;
    case 0x53:
      return VK_DECIMAL;
  }
  const UINT scan_code =
      static_cast<UINT>(code & 0xff) | ((code & 0x100) != 0 ? 0xe000 : 0);
  const UINT key = MapVirtualKeyW(scan_code, MAPVK_VSC_TO_VK_EX);
  if (key == 0) {
    return luaL_error(state, "cannot map scan code %I to a Win32 virtual key",
                      code);
  }
  return static_cast<int>(key);
}

}  // namespace

int Keyboard::Press(lua_State* state) {
  return input::ChangePressed(state, pressed_inputs_, KeyIdentifier, "PressKey",
                              true);
}

int Keyboard::Release(lua_State* state) {
  return input::ChangePressed(state, pressed_inputs_, KeyIdentifier,
                              "ReleaseKey", false);
}

int Keyboard::Reset(lua_State* state) {
  return input::Reset(state, pressed_inputs_, "ReleaseKey");
}

int Keyboard::Click(lua_State* state) {
  return input::ChangePressed(state, pressed_inputs_, KeyIdentifier,
                              "PressAndReleaseKey", false);
}

int Keyboard::ListPressed(lua_State* state) const {
  return input::ListPressed(state, pressed_inputs_);
}

int Keyboard::IsPressed(lua_State* state) const {
  luaL_argcheck(state, lua_gettop(state) == 1, 1,
                "exactly one key is required");
  int key = 0;
  if (lua_type(state, 1) == LUA_TSTRING) {
    size_t length;
    const char* name = lua_tolstring(state, 1, &length);
    if (length == 4 && std::memcmp(name, "ctrl", 4) == 0) {
      key = VK_CONTROL;
    } else if (length == 5 && std::memcmp(name, "shift", 5) == 0) {
      key = VK_SHIFT;
    } else if (length == 3 && std::memcmp(name, "alt", 3) == 0) {
      key = VK_MENU;
    }
  }
  if (key == 0) {
    key = VirtualKey(state, KeyIdentifier(state, 1));
  }
  lua_pushboolean(state, (GetAsyncKeyState(key) & 0x8000) != 0);
  return 1;
}

int Keyboard::IsLocked(lua_State* state) const {
  luaL_argcheck(state, lua_gettop(state) == 1, 1,
                "exactly one lock key is required");
  const lua_Integer code = KeyIdentifier(state, 1);
  int key = 0;
  switch (code) {
    case 0x3a:
      key = VK_CAPITAL;
      break;
    case 0x45:
      key = VK_NUMLOCK;
      break;
    case 0x46:
      key = VK_SCROLL;
      break;
    default:
      return luaL_argerror(state, 1,
                           "expected capslock, numlock, or scrolllock");
  }
  lua_pushboolean(state, (GetKeyState(key) & 1) != 0);
  return 1;
}

void InitializeKeyboard(lua_State* state) {
  input::RequireFunction(state, "PressKey");
  input::RequireFunction(state, "ReleaseKey");
  GetStorage(state).keyboard.Reset(state);
  const luaL_Reg kFunctions[] = {
      {"Keyboard_Press", PressKeyboardKey},
      {"Keyboard_Release", ReleaseKeyboardKey},
      {"Keyboard_Click", ClickKeyboardKey},
      {"Keyboard_Reset", ResetKeyboard},
      {"Keyboard_ListPressed", ListPressedKeyboardKeys},
      {"Keyboard_IsPressed", IsPressed},
      {"Keyboard_IsLocked", IsLocked},
      {nullptr, nullptr},
  };
  luaL_setfuncs(state, kFunctions, 0);
}

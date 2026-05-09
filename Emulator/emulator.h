#pragma once

#include <inttypes.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <Windows.h>

__declspec(dllexport) int luaopen_LGHUB_Emulator(lua_State* L);
__declspec(dllexport) int emu_IsEmulating(lua_State* L);
__declspec(dllexport) int emu_Sleep(lua_State* L);
__declspec(dllexport) int emu_InitializeRunningTime(lua_State* L);
__declspec(dllexport) int emu_GetRunningTime(lua_State* L);
__declspec(dllexport) int emu_IsKeyLockOn(lua_State* L);
__declspec(dllexport) int emu_GetMousePosition(lua_State* L);
__declspec(dllexport) int emu_SetMousePosition(lua_State* L);
__declspec(dllexport) int emu_PressKey(lua_State* L);
__declspec(dllexport) int emu_ReleaseKey(lua_State* L);
__declspec(dllexport) int emu_PressAndReleaseKey(lua_State* L);
__declspec(dllexport) int emu_PressMouseButton(lua_State* L);
__declspec(dllexport) int emu_ReleaseMouseButton(lua_State* L);
__declspec(dllexport) int emu_PressAndReleaseMouseButton(lua_State* L);
__declspec(dllexport) int emu_MoveMouseRelative(lua_State* L);
__declspec(dllexport) int emu_MoveMouseWheel(lua_State* L);
__declspec(dllexport) int emu_ClearLog(lua_State* L);
__declspec(dllexport) int emu_OutputDebugMessage(lua_State* L);
__declspec(dllexport) int emu_GetModifierState(lua_State* L);

static const struct luaL_Reg Exports[] = {
    {"Sleep", (lua_CFunction)emu_Sleep},
    {"IsEmulating", (lua_CFunction)emu_IsEmulating},
    {"GetRunningTime", (lua_CFunction)emu_GetRunningTime},
    {"InitializeRunningTime", (lua_CFunction)emu_InitializeRunningTime},
    {"IsKeyLockOn", (lua_CFunction)emu_IsKeyLockOn},
    {"GetMousePosition", (lua_CFunction)emu_GetMousePosition},
    {"SetMousePosition", (lua_CFunction)emu_SetMousePosition},
    {"PressKey", (lua_CFunction)emu_PressKey},
    {"ReleaseKey", (lua_CFunction)emu_ReleaseKey},
    {"PressAndReleaseKey", (lua_CFunction)emu_PressAndReleaseKey},
    {"PressMouseButton", (lua_CFunction)emu_PressMouseButton},
    {"ReleaseMouseButton", (lua_CFunction)emu_ReleaseMouseButton},
    {"PressAndReleaseMouseButton", (lua_CFunction)emu_PressAndReleaseMouseButton},
    {"MoveMouseRelative", (lua_CFunction)emu_MoveMouseRelative},
    {"MoveMouseWheel", (lua_CFunction)emu_MoveMouseWheel},
    {"ClearLog", (lua_CFunction)emu_ClearLog},
    {"OutputDebugMessage", (lua_CFunction)emu_OutputDebugMessage},
    {"GetModifierState", (lua_CFunction)emu_GetModifierState},
    {NULL, NULL}
};

static void __impl_emu_Sleep(uint32_t milliseconds);
static void __impl_emu_InitializeRunningTime(void);
static uint64_t __impl_emu_GetRunningTime(void);
static int32_t __impl_emu_IsKeyLockOn(const char* key);
static void __impl_emu_GetMousePosition(LPPOINT pt);
static void __impl_emu_SetMousePosition(LPPOINT pt);
static void __impl_emu_PressKey(WORD wKey);
static void __impl_emu_ReleaseKey(WORD wKey);
static void __impl_emu_PressMouseButton(DWORD dwMouseEventFlags);
static void __impl_emu_ReleaseMouseButton(DWORD dwMouseEventFlags);
static void __impl_emu_MoveMouseRelative(long lVirtualX, long iVirtualY);
static void __impl_emu_MoveMouseWheel(long lCount);
static void __impl_emu_ClearLog(void);
static void __impl_emu_OutputDebugMessage(const char* message);
static uint32_t __impl_emu_GetModifierState(void);
static BOOL WINAPI __impl_emu_CtrlHandler(DWORD dwCtrlType); // 此函数用于在按下 Ctrl + C 等热键后正确退出

#define SC_ESCAPE (0x01)
#define SC_F1 (0x3b)
#define SC_F2 (0x3c)
#define SC_F3 (0x3d)
#define SC_F4 (0x3e)
#define SC_F5 (0x3f)
#define SC_F6 (0x40)
#define SC_F7 (0x41)
#define SC_F8 (0x42)
#define SC_F9 (0x43)
#define SC_F10 (0x44)
#define SC_F11 (0x57)
#define SC_F12 (0x58)
#define SC_F13 (0x64)
#define SC_F14 (0x65)
#define SC_F15 (0x66)
#define SC_F16 (0x67)
#define SC_F17 (0x68)
#define SC_F18 (0x69)
#define SC_F19 (0x6a)
#define SC_F20 (0x6b)
#define SC_F21 (0x6c)
#define SC_F22 (0x6d)
#define SC_F23 (0x6e)
#define SC_F24 (0x76)

#define SC_PRINT_SCREEN (0x137)
#define SC_SCROLL_LOCK (0x46)
#define SC_PAUSE_BREAK (0x146)

#define SC_INSERT (0x152)
#define SC_DELETE (0x153)
#define SC_HOME (0x147)
#define SC_END (0x14f)
#define SC_PAGE_UP (0x149)
#define SC_PAGE_DOWN (0x151)

#define SC_UP (0x148)
#define SC_LEFT (0x14b)
#define SC_DOWN (0x150)
#define SC_RIGHT (0x14d)

#define SC_BACKQUOTE (0x29)
#define SC_ONE (0x02)
#define SC_TWO (0x03)
#define SC_THREE (0x04)
#define SC_FOUR (0x05)
#define SC_FIVE (0x06)
#define SC_SIX (0x07)
#define SC_SEVEN (0x08)
#define SC_EIGHT (0x09)
#define SC_NINE (0x0a)
#define SC_ZERO (0x0b)
#define SC_MINUS (0x0c)
#define SC_EQUAL (0x0d)
#define SC_BACKSPACE (0x0e)

#define SC_TAB (0x0f)
#define SC_Q (0x10)
#define SC_W (0x11)
#define SC_E (0x12)
#define SC_R (0x13)
#define SC_T (0x14)
#define SC_Y (0x15)
#define SC_U (0x16)
#define SC_I (0x17)
#define SC_O (0x18)
#define SC_P (0x19)
#define SC_LEFT_BRACKET (0x1a)
#define SC_RIGHT_BRACKET (0x1b)
#define SC_BACKSLASH (0x2b)

#define SC_CAPS_LOCK (0x3a)
#define SC_A (0x1e)
#define SC_S (0x1f)
#define SC_D (0x20)
#define SC_F (0x21)
#define SC_G (0x22)
#define SC_H (0x23)
#define SC_J (0x24)
#define SC_K (0x25)
#define SC_L (0x26)
#define SC_SEMICOLON (0x27)
#define SC_QUOTE (0x28)
#define SC_ENTER (0x1c)

#define LEFT_SHIFT (0x2a)
#define SC_NON_US_SLASH (0x56)
#define SC_Z (0x2c)
#define SC_X (0x2d)
#define SC_C (0x2e)
#define SC_V (0x2f)
#define SC_B (0x30)
#define SC_N (0x31)
#define SC_M (0x32)
#define SC_COMMA (0x33)
#define SC_PERIOD (0x34)
#define SC_SLASH (0x35)
#define SC_RIGHT_SHIFT (0x36)

#define SC_LEFT_CTRL (0x1d)
#define SC_LEFT_WIN (0x15b)
#define SC_LEFT_ALT (0x38)
#define SC_SPACEBAR (0x39)
#define SC_RIGHT_ALT (0x138)
#define SC_RIGHT_WIN (0x15c)
#define SC_APPS (0x15d)
#define SC_RIGHT_CTRL (0x11d)

#define SC_NUM_LOCK (0x45)
#define SC_NUM_DIVIDE (0x135)
#define SC_NUM_MULTIPLY (0x37)
#define SC_NUM_MINUS (0x4a)
#define SC_NUM_SEVEN (0x47)
#define SC_NUM_EIGHT (0x48)
#define SC_NUM_NINE (0x49)
#define SC_NUM_PLUS (0x4e)
#define SC_NUM_FOUR (0x4b)
#define SC_NUM_FIVE (0x4c)
#define SC_NUM_SIX (0x4d)
#define SC_NUM_ONE (0x4f)
#define SC_NUM_TWO (0x50)
#define SC_NUM_THREE (0x51)
#define SC_NUM_ZERO (0x52)
#define SC_NUM_DECIMAL (0x53)
#define SC_NUM_ENTER (0x11c)

#define KEYBOARD_OPERATION(operation) \
int operation(lua_State* L) \
{\
    const int32_t scancode = luaL_checkinteger(L, 1); \
    switch (scancode) \
    {\
    case SC_ESCAPE: __impl_ ##operation(VK_ESCAPE); break; \
    case SC_F1: __impl_ ##operation(VK_F1); break; \
    case SC_F2: __impl_ ##operation(VK_F2); break; \
    case SC_F3: __impl_ ##operation(VK_F3); break; \
    case SC_F4: __impl_ ##operation(VK_F4); break; \
    case SC_F5: __impl_ ##operation(VK_F5); break; \
    case SC_F6: __impl_ ##operation(VK_F6); break; \
    case SC_F7: __impl_ ##operation(VK_F7); break; \
    case SC_F8: __impl_ ##operation(VK_F8); break; \
    case SC_F9: __impl_ ##operation(VK_F9); break; \
    case SC_F10: __impl_ ##operation(VK_F10); break; \
    case SC_F11: __impl_ ##operation(VK_F11); break; \
    case SC_F12: __impl_ ##operation(VK_F12); break; \
    case SC_F13: __impl_ ##operation(VK_F13); break; \
    case SC_F14: __impl_ ##operation(VK_F14); break; \
    case SC_F15: __impl_ ##operation(VK_F15); break; \
    case SC_F16: __impl_ ##operation(VK_F16); break; \
    case SC_F17: __impl_ ##operation(VK_F17); break; \
    case SC_F18: __impl_ ##operation(VK_F18); break; \
    case SC_F19: __impl_ ##operation(VK_F19); break; \
    case SC_F20: __impl_ ##operation(VK_F20); break; \
    case SC_F21: __impl_ ##operation(VK_F21); break; \
    case SC_F22: __impl_ ##operation(VK_F22); break; \
    case SC_F23: __impl_ ##operation(VK_F23); break; \
    case SC_F24: __impl_ ##operation(VK_F24); break; \
    case SC_PRINT_SCREEN: __impl_ ##operation(VK_SNAPSHOT); break; \
    case SC_SCROLL_LOCK: __impl_ ##operation(VK_SCROLL); break; \
    case SC_PAUSE_BREAK: __impl_ ##operation(VK_PAUSE); break; \
    case SC_INSERT: __impl_ ##operation(VK_INSERT); break; \
    case SC_DELETE: __impl_ ##operation(VK_DELETE); break; \
    case SC_HOME: __impl_ ##operation(VK_HOME); break; \
    case SC_END: __impl_ ##operation(VK_END); break; \
    case SC_PAGE_UP: __impl_ ##operation(VK_PRIOR); break; \
    case SC_PAGE_DOWN: __impl_ ##operation(VK_NEXT); break; \
    case SC_UP: __impl_ ##operation(VK_UP); break; \
    case SC_LEFT: __impl_ ##operation(VK_LEFT); break; \
    case SC_DOWN: __impl_ ##operation(VK_DOWN); break; \
    case SC_RIGHT: __impl_ ##operation(VK_RIGHT); break; \
    case SC_BACKQUOTE: __impl_ ##operation(VK_OEM_3); break; \
    case SC_ONE: __impl_ ##operation(0x31); break; \
    case SC_TWO: __impl_ ##operation(0x32); break; \
    case SC_THREE: __impl_ ##operation(0x33); break; \
    case SC_FOUR: __impl_ ##operation(0x34); break; \
    case SC_FIVE: __impl_ ##operation(0x35); break; \
    case SC_SIX: __impl_ ##operation(0x36); break; \
    case SC_SEVEN: __impl_ ##operation(0x37); break; \
    case SC_EIGHT: __impl_ ##operation(0x38); break; \
    case SC_NINE: __impl_ ##operation(0x39); break; \
    case SC_ZERO: __impl_ ##operation(0x30); break; \
    case SC_MINUS: __impl_ ##operation(VK_SUBTRACT); break; \
    case SC_EQUAL: __impl_ ##operation(VK_ADD); break; \
    case SC_BACKSPACE: __impl_ ##operation(VK_BACK); break; \
    case SC_TAB: __impl_ ##operation(VK_TAB); break; \
    case SC_Q: __impl_ ##operation(0x51); break; \
    case SC_W: __impl_ ##operation(0x57); break; \
    case SC_E: __impl_ ##operation(0x45); break; \
    case SC_R: __impl_ ##operation(0x52); break; \
    case SC_T: __impl_ ##operation(0x54); break; \
    case SC_Y: __impl_ ##operation(0x59); break; \
    case SC_U: __impl_ ##operation(0x55); break; \
    case SC_I: __impl_ ##operation(0x49); break; \
    case SC_O: __impl_ ##operation(0x4f); break; \
    case SC_P: __impl_ ##operation(0x50); break; \
    case SC_LEFT_BRACKET: __impl_ ##operation(VK_OEM_4); break; \
    case SC_RIGHT_BRACKET: __impl_ ##operation(VK_OEM_6); break; \
    case SC_BACKSLASH: __impl_ ##operation(VK_OEM_5); break; \
    case SC_CAPS_LOCK: __impl_ ##operation(VK_CAPITAL); break; \
    case SC_A: __impl_ ##operation(0x41); break; \
    case SC_S: __impl_ ##operation(0x53); break; \
    case SC_D: __impl_ ##operation(0x44); break; \
    case SC_F: __impl_ ##operation(0x46); break; \
    case SC_G: __impl_ ##operation(0x47); break; \
    case SC_H: __impl_ ##operation(0x48); break; \
    case SC_J: __impl_ ##operation(0x4a); break; \
    case SC_K: __impl_ ##operation(0x4b); break; \
    case SC_L: __impl_ ##operation(0x4c); break; \
    case SC_SEMICOLON: __impl_ ##operation(VK_OEM_1); break; \
    case SC_QUOTE: __impl_ ##operation(VK_OEM_7); break; \
    case SC_ENTER: __impl_ ##operation(VK_RETURN); break; \
    case LEFT_SHIFT: __impl_ ##operation(VK_LSHIFT); break; \
    case SC_NON_US_SLASH: __impl_ ##operation(VK_OEM_102); break; \
    case SC_Z: __impl_ ##operation(0x5a); break; \
    case SC_X: __impl_ ##operation(0x58); break; \
    case SC_C: __impl_ ##operation(0x43); break; \
    case SC_V: __impl_ ##operation(0x56); break; \
    case SC_B: __impl_ ##operation(0x42); break; \
    case SC_N: __impl_ ##operation(0x4e); break; \
    case SC_M: __impl_ ##operation(0x4d); break; \
    case SC_COMMA: __impl_ ##operation(VK_OEM_COMMA); break; \
    case SC_PERIOD: __impl_ ##operation(VK_OEM_PERIOD); break; \
    case SC_SLASH: __impl_ ##operation(VK_OEM_2); break; \
    case SC_RIGHT_SHIFT: __impl_ ##operation(VK_RSHIFT); break; \
    case SC_LEFT_CTRL: __impl_ ##operation(VK_LCONTROL); break; \
    case SC_LEFT_WIN: __impl_ ##operation(VK_LWIN); break; \
    case SC_LEFT_ALT: __impl_ ##operation(VK_LMENU); break; \
    case SC_SPACEBAR: __impl_ ##operation(VK_SPACE); break; \
    case SC_RIGHT_ALT: __impl_ ##operation(VK_RMENU); break; \
    case SC_RIGHT_WIN: __impl_ ##operation(VK_RWIN); break; \
    case SC_APPS: __impl_ ##operation(VK_APPS); break; \
    case SC_RIGHT_CTRL: __impl_ ##operation(VK_RCONTROL); break; \
    case SC_NUM_LOCK: __impl_ ##operation(VK_NUMLOCK); break; \
    case SC_NUM_DIVIDE: __impl_ ##operation(VK_DIVIDE); break; \
    case SC_NUM_MULTIPLY: __impl_ ##operation(VK_MULTIPLY); break; \
    case SC_NUM_MINUS: __impl_ ##operation(VK_SUBTRACT); break; \
    case SC_NUM_SEVEN: __impl_ ##operation(VK_NUMPAD7); break; \
    case SC_NUM_EIGHT: __impl_ ##operation(VK_NUMPAD8); break; \
    case SC_NUM_NINE: __impl_ ##operation(VK_NUMPAD9); break; \
    case SC_NUM_PLUS: __impl_ ##operation(VK_ADD); break; \
    case SC_NUM_FOUR: __impl_ ##operation(VK_NUMPAD4); break; \
    case SC_NUM_FIVE: __impl_ ##operation(VK_NUMPAD5); break; \
    case SC_NUM_SIX: __impl_ ##operation(VK_NUMPAD6); break; \
    case SC_NUM_ONE: __impl_ ##operation(VK_NUMPAD1); break; \
    case SC_NUM_TWO: __impl_ ##operation(VK_NUMPAD2); break; \
    case SC_NUM_THREE: __impl_ ##operation(VK_NUMPAD3); break; \
    case SC_NUM_ZERO: __impl_ ##operation(VK_NUMPAD0); break; \
    case SC_NUM_DECIMAL: __impl_ ##operation(VK_DECIMAL); break; \
    case SC_NUM_ENTER: __impl_ ##operation(VK_RETURN); break; \
    default: \
        luaL_error(L, "Invalid key: %d", scancode); \
    } \
    return 0; \
}

#define MOUSE_OPERATION(operation, type) \
int operation(lua_State* L) \
{\
    const int32_t button = luaL_checkinteger(L, 1); \
    switch (button) \
    {\
    case 1: __impl_ ##operation(MOUSEEVENTF_LEFT##type); break; \
    case 2: __impl_ ##operation(MOUSEEVENTF_RIGHT##type); break; \
    case 3: __impl_ ##operation(MOUSEEVENTF_MIDDLE##type); break; \
    case 4: __impl_ ##operation(MOUSEEVENTF_X##type); break; \
    case 5: __impl_ ##operation(MOUSEEVENTF_LEFT##type); break; \
    default: \
        luaL_error(L, "Invalid button: %d", button); \
    } \
    return 0; \
}
#include <Windows.h>
#include <consoleapi.h>
#include <stdio.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <inttypes.h>
#include "LGHUB_Emulator.h"

static uint64_t s_ScriptLoadTime = 0;
static BOOL s_bEmulating = TRUE;
static const char* s_lpszCtrlEventCallbackName = NULL;

int luaopen_LGHUB_Emulator(lua_State* L)
{
    luaL_newlib(L, Exports);
    SetConsoleCtrlHandler(__impl_emu_CtrlHandler, TRUE);
    return 1;
}

int emu_IsEmulating(lua_State* L)
{
    lua_pushboolean(L, s_bEmulating);
    return 1;
}

int emu_Sleep(lua_State* L)
{
    uint32_t milliseconds = (uint32_t)luaL_checkinteger(L, 1);
    __impl_emu_Sleep(milliseconds);
    return 0;
}

int emu_InitializeRunningTime(lua_State* L)
{
    __impl_emu_InitializeRunningTime();
    return 0;
}

int emu_GetRunningTime(lua_State* L)
{
    uint64_t running_time = __impl_emu_GetRunningTime();
    lua_pushinteger(L, running_time);
    return 1;
}

int emu_IsKeyLockOn(lua_State* L)
{
    const char* key = luaL_checkstring(L, 1);
    int32_t result = __impl_emu_IsKeyLockOn(key);
    lua_pushinteger(L, result);
    return 1;
}

int emu_GetMousePosition(lua_State* L)
{
    POINT pt;
    __impl_emu_GetMousePosition(&pt);
    lua_pushinteger(L, pt.x);
    lua_pushinteger(L, pt.y);
    return 2;
}

int emu_SetMousePosition(lua_State* L)
{
    LONG lX = luaL_checkinteger(L, 1);
    LONG lY = luaL_checkinteger(L, 2);
    if (
        lX < 0 || lX > 65535 ||
        lY < 0 || lY > 65535)
    {
        luaL_error(L, "Invalid mouse position: (%d, %d). Coordinates are expected in range [0, 65535].", lX, lY);
    }
    POINT pt;
    pt.x = lX;
    pt.y = lY;
    __impl_emu_SetMousePosition(&pt);
    return 0;
}
KEYBOARD_OPERATION(emu_PressKey)
KEYBOARD_OPERATION(emu_ReleaseKey)

int emu_PressAndReleaseKey(lua_State *L)
{
    /* 通过 luaL_error 抛出错误，若发生错误，则后续的函数调用不会执行 */
    emu_PressKey(L);
    emu_ReleaseKey(L);
    return 0;
}

MOUSE_OPERATION(emu_PressMouseButton, DOWN)
MOUSE_OPERATION(emu_ReleaseMouseButton, UP)
int emu_PressAndReleaseMouseButton(lua_State* L)
{
    const int32_t button = luaL_checkinteger(L, 1);
    switch (button)
    {
    case 1: __impl_emu_PressMouseButton(MOUSEEVENTF_LEFTDOWN); __impl_emu_ReleaseMouseButton(MOUSEEVENTF_LEFTUP); break;
    case 2: __impl_emu_PressMouseButton(MOUSEEVENTF_RIGHTDOWN); __impl_emu_ReleaseMouseButton(MOUSEEVENTF_RIGHTUP); break;
    case 3: __impl_emu_PressMouseButton(MOUSEEVENTF_MIDDLEDOWN); __impl_emu_ReleaseMouseButton(MOUSEEVENTF_MIDDLEUP); break;
    case 4: __impl_emu_PressMouseButton(MOUSEEVENTF_XDOWN); __impl_emu_ReleaseMouseButton(MOUSEEVENTF_XUP); break;
    case 5: __impl_emu_PressMouseButton(MOUSEEVENTF_XDOWN | MOUSEEVENTF_XUP); break;
    default:
        luaL_error(L, "Invalid button : %d", button);
    }
    return 0;
}

int emu_MoveMouseRelative(lua_State *L)
{
    int32_t lVirtualX = luaL_checkinteger(L, 1);
    int32_t lVirtualY = luaL_checkinteger(L, 2);
    __impl_emu_MoveMouseRelative(lVirtualX, lVirtualY);
    return 0;
}

int emu_MoveMouseWheel(lua_State *L)
{
    long lCount = luaL_checkinteger(L, 1);
    __impl_emu_MoveMouseWheel(lCount);
    return 0;
}

int emu_ClearLog(lua_State *L)
{
    __impl_emu_ClearLog();
    return 0;
}

int emu_OutputDebugMessage(lua_State *L)
{
    const char* message = luaL_checkstring(L, 1);
    __impl_emu_OutputDebugMessage(message);
    return 0;
}

int emu_GetModifierState(lua_State *L)
{
    uint32_t state = __impl_emu_GetModifierState();
    lua_pushinteger(L, state);
    return 1;
}

/* impl_emu_emu_ementations*/

void __impl_emu_Sleep(uint32_t milliseconds)
{
    Sleep(milliseconds);
}

void __impl_emu_InitializeRunningTime()
{
    s_ScriptLoadTime = GetTickCount64();
}

uint64_t __impl_emu_GetRunningTime()
{
    return GetTickCount64() - s_ScriptLoadTime;
}

int32_t __impl_emu_IsKeyLockOn(const char* key)
{
    if (strcmp(key, "capslock") == 0)
    {
        return (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
    }
    else if (strcmp(key, "numlock") == 0)
    {
        return (GetKeyState(VK_NUMLOCK) & 0x0001) != 0;
    }
    else if (strcmp(key, "scrolllock") == 0)
    {
        return (GetKeyState(VK_SCROLL) & 0x0001) != 0;
    }
    return -1;
}

void __impl_emu_GetMousePosition(LPPOINT pt)
{
    int screen_width = GetSystemMetrics(SM_CXSCREEN);
    int screen_height = GetSystemMetrics(SM_CYSCREEN);
    POINT p;
    GetCursorPos(&p);
    uint32_t x = (p.x * 65535 + (screen_width - 1 + 1) / 2) / (screen_width - 1);
    uint32_t y = (p.y * 65535 + (screen_width - 1 + 1) / 2) / (screen_height - 1);
    pt->x = x;
    pt->y = y;
}

void __impl_emu_SetMousePosition(LPPOINT pt)
{
    int screen_width = GetSystemMetrics(SM_CXSCREEN);
    int screen_height = GetSystemMetrics(SM_CYSCREEN);
    uint32_t x = (pt->x * (screen_width - 1) + (65535 + 1) / 2) / 65535;
    uint32_t y = (pt->y * (screen_height - 1) + (65535 + 1) / 2) / 65535;
    SetCursorPos(x, y);
}

void __impl_emu_PressKey(WORD wKey)
{
    INPUT input = { 0 };
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = wKey;
    SendInput(1, &input, sizeof(INPUT));
}

void __impl_emu_ReleaseKey(WORD wKey)
{
    INPUT input = { 0 };
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = wKey;
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));
}

void __impl_emu_PressAndReleaseKey(WORD wKey)
{
    INPUT input = { 0 };
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = wKey;
    SendInput(1, &input, sizeof(INPUT));
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));
}

void __impl_emu_PressMouseButton(DWORD dwMouseEventFlags)
{
    INPUT input = {
        .type = INPUT_MOUSE,
        .mi = {
            .dx = 0,
            .dy = 0,
            .mouseData = 0,
            .dwFlags = dwMouseEventFlags,
            .time = 0,
            .dwExtraInfo = 0
        }
    };
    SendInput(1, &input, sizeof(INPUT));
}

void __impl_emu_ReleaseMouseButton(DWORD dwMouseEventFlags)
{
    INPUT input = {
        .type = INPUT_MOUSE,
        .mi = {
            .dx = 0,
            .dy = 0,
            .mouseData = 0,
            .dwFlags = dwMouseEventFlags,
            .time = 0,
            .dwExtraInfo = 0
        }
    };
    SendInput(1, &input, sizeof(INPUT));
}

void __impl_emu_MoveMouseRelative(long lVirtualX, long lVirtualY)
{
    long screen_width = GetSystemMetrics(SM_CXSCREEN);
    long screen_height = GetSystemMetrics(SM_CYSCREEN);
    long directionX = lVirtualX < 0 ? -1 :
                     lVirtualX == 0 ? 0 :
                     1;
    long directionY = lVirtualY < 0 ? -1 :
                     lVirtualY == 0 ? 0 :
                     1;
    long dX = ((screen_width - 1) * lVirtualX + directionX * 65534) / 65535;
    long dY = ((screen_width - 1) * lVirtualY + directionY * 65534) / 65535;
    INPUT input = {
        .type = INPUT_MOUSE,
        .mi = {
            .dx = dX,
            .dy = dY,
            .mouseData = 0,
            .dwFlags = MOUSEEVENTF_MOVE,
            .time = 0,
            .dwExtraInfo = 0
        }
    };
    SendInput(1, &input, sizeof(INPUT));
}

void __impl_emu_MoveMouseWheel(long lCount)
{
    INPUT input = {
        .type = INPUT_MOUSE,
        .mi = {
            .dx = 0,
            .dy = 0,
            .mouseData = (DWORD)lCount * WHEEL_DELTA, /* 一次滚动需要乘以 WHEEL_DELTA */
            .dwFlags = MOUSEEVENTF_WHEEL,
            .time = 0,
            .dwExtraInfo = 0
        }
    };
    SendInput(1, &input, sizeof(INPUT));
}

void __impl_emu_ClearLog()
{
    // reference: https://learn.microsoft.com/en-us/windows/console/clearing-the-screen
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    SMALL_RECT scrollRect;
    COORD scrollTarget;
    CHAR_INFO fill;

    // Get the number of character cells in the current buffer.
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
    {
        return;
    }

    // Scroll the rectangle of the entire buffer.
    scrollRect.Left = 0;
    scrollRect.Top = 0;
    scrollRect.Right = csbi.dwSize.X;
    scrollRect.Bottom = csbi.dwSize.Y;

    // Scroll it upwards off the top of the buffer with a magnitude of the entire height.
    scrollTarget.X = 0;
    scrollTarget.Y = (SHORT)(0 - csbi.dwSize.Y);

    // Fill with empty spaces with the buffer's default text attribute.
    fill.Char.UnicodeChar = TEXT(' ');
    fill.Attributes = csbi.wAttributes;

    // Do the scroll
    ScrollConsoleScreenBuffer(hConsole, &scrollRect, NULL, scrollTarget, &fill);

    // Move the cursor to the top left corner too.
    csbi.dwCursorPosition.X = 0;
    csbi.dwCursorPosition.Y = 0;

    SetConsoleCursorPosition(hConsole, csbi.dwCursorPosition);
}

static void __impl_emu_OutputDebugMessage(const char* message)
{
    // 与罗技 G HUB 行为保持一致，使用 OutputDebugStringA 输出调试信息，而不是 OutputDebugStringW
    OutputDebugStringA(message);
}

uint32_t __impl_emu_GetModifierState()
{
    uint32_t uState = 0;
    if (GetAsyncKeyState(VK_LMENU) & 0x8000) // left alt
    {
        uState += (1 << 0);
    }
    if (GetAsyncKeyState(VK_RMENU) & 0x8000) // right alt
    {
        uState += (1 << 1);
    }
    if (GetAsyncKeyState(VK_LSHIFT) & 0x8000) // left shift
    {
        uState += (1 << 2);
    }
    if (GetAsyncKeyState(VK_RSHIFT) & 0x8000) // right shift
    {
        uState += (1 << 3);
    }
    if (GetAsyncKeyState(VK_LCONTROL) & 0x8000) // left ctrl
    {
        uState += (1 << 4);
    }
    if (GetAsyncKeyState(VK_RCONTROL) & 0x8000) // right ctrl
    {
        uState += (1 << 5);
    }
    return uState;
}

BOOL WINAPI __impl_emu_CtrlHandler(DWORD dwCtrlType)
{
    const char* lpszEvent;
    switch (dwCtrlType)
    {
    case CTRL_C_EVENT:
        lpszEvent = "CTRL_C_EVENT";
        break;
    case CTRL_CLOSE_EVENT:
        lpszEvent = "CTRL_CLOSE_EVENT";
        break;
    case CTRL_BREAK_EVENT:
        lpszEvent = "CTRL_BREAK_EVENT";
        break;
    case CTRL_LOGOFF_EVENT:
        lpszEvent = "CTRL_LOGOFF_EVENT";
        break;
    case CTRL_SHUTDOWN_EVENT:
        lpszEvent = "CTRL_SHUTDOWN_EVENT";
        break;
    default:
        lpszEvent = "UNKNOWN";
    }
    printf("Control event %s captured. Emulation will stop.\r\n", lpszEvent);
    s_bEmulating = FALSE;
    SetConsoleCtrlHandler(__impl_emu_CtrlHandler, FALSE); // 移除控制信号处理函数
    return TRUE;
}
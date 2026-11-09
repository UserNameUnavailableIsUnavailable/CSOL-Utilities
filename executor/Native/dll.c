#include <executor/native/dll.h>
#define Win32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdint.h>

static luaL_Reg native_functions[] = {
    {"GetMonotonicTimepoint", Native_GetMonotonicTimepoint},
    {"GetSystemTimepoint", Native_GetSystemTimepoint},
    {"Sleep", Native_Sleep},
    {NULL, NULL}
};

extern uint64_t g_MonotonicOpenTimepoint;

EXPORT int luaopen_executor_Native(struct lua_State *L)
{
    g_MonotonicOpenTimepoint = GetTickCount64();
    luaL_newlib(L, native_functions);
    return 1;
}
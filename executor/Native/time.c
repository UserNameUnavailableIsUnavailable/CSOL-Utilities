#include <executor/native/dll.h>

#include <Windows.h>
#include <stdint.h>

uint64_t g_MonotonicOpenTimepoint = 0;

EXPORT int Native_GetMonotonicTimepoint(struct lua_State *L)
{
    uint64_t now = GetTickCount64(); // milliseconds precision
    uint64_t elapsed = now - g_MonotonicOpenTimepoint;
    lua_pushinteger(L, elapsed);
    return 1;
}

EXPORT int Native_GetSystemTimepoint(struct lua_State* L)
{
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft); // Get UTC time

    // Combine high and low bits into a 64-bit integer
    ULARGE_INTEGER uli;
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;

    // Convert to UNIX epoch: subtract the 1601-to-1970 offset, then convert to milliseconds (divide by 10000)
    // If you only need seconds, divide by 10000000
    uint64_t tp = (uint64_t)((uli.QuadPart - 116444736000000000ULL) / 10000);
    lua_pushinteger(L, tp);
    return 1;
}

EXPORT int Native_Sleep(struct lua_State* L)
{
    uint32_t milliseconds = (uint32_t)luaL_checkinteger(L, 1);
    Sleep(milliseconds);
    return 0;
}
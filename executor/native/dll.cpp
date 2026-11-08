#include <executor/native/dll.hpp>
#include <chrono>

const luaL_Reg g_Exports[] = {
    { "Native_GetMonotonicTimestamp", &Native_GetMonotonicTimestamp },
    { "Native_GetSystemTimestamp", Native_GetSystemTimestamp },
    { nullptr, nullptr }
};

NATIVE_API EXPORT
int luaopen_executor_Native(lua_State* L) {
    luaL_newlib(L, g_Exports);
    return 1;
}

NATIVE_API EXPORT
int Native_GetMonotonicTimestamp(lua_State* L) {
    auto now = std::chrono::steady_clock::now();
    std::size_t tp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    lua_pushinteger(L, tp);
    return 1;
}

NATIVE_API EXPORT
int Native_GetSystemTimestamp(lua_State* L) {
    auto now = std::chrono::system_clock::now();
    std::size_t tp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    lua_pushinteger(L, tp);
    return 1;
}
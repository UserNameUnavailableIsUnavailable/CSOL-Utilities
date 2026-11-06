#include <lauxlib.h>
#include <lghubex/dll/features.h>

#include <lua.h>

int l_print(struct lua_State *L) {
    int argc = lua_gettop(L);
    luaL_Buffer buf;
    luaL_buffinit(L, &buf);
    for (int i = 1; i <= argc; i++) {
        size_t len;
        const char *s = luaL_tolstring(L, i, &len);
        luaL_addlstring(&buf, s, len);
        lua_pop(L, 1); // remove the string returned by luaL_tolstring from the stack
        if (i != argc) {
            luaL_addchar(&buf, '\t');
        } else {
            luaL_addchar(&buf, '\n');
        }
    }
    luaL_pushresult(&buf); // push the final concatenated string onto the stack
    const char *msg = lua_tostring(L, -1);

    lua_getglobal(L, "OutputLogMessage"); // push the OutputLogMessage function onto the stack
    lua_pushvalue(L, -2); // push the concatenated string as the argument
    lua_call(L, 1, 0); // call OutputLogMessage with 1 argument and 0 return values
    lua_pop(L, 1); // remove the concatenated string from the stack

    return 0;
}

// Minimal implementation of loadfile
int l_loadfile(struct lua_State *L) {
    const char *filename = luaL_optstring(L, 1, NULL);
    int status = luaL_loadfilex(L, filename, NULL);
    if (status == LUA_OK) {
        return 1; // Returns the compiled function
    } else {
        lua_pushnil(L);
        lua_insert(L, -2); // nil, error message
        return 2;
    }
}

// Minimal implementation of dofile
int l_dofile(struct lua_State *L) {
    const char *filename = luaL_optstring(L, 1, NULL);
    if (luaL_loadfilex(L, filename, NULL) != LUA_OK) {
        return lua_error(L); // dofile throws errors directly
    }
    lua_call(L, 0, LUA_MULTRET);
    return lua_gettop(L); // Returns all results
}

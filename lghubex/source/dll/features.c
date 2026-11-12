#include <lghubex/dll/features.h>

#include <lualib.h>
#include <lauxlib.h>
#include <lua.h>

typedef struct {
    int (*function)(struct lua_State *L);
    const char *name;
} fn_t;

static int l_print(struct lua_State *L);
static int l_dofile(struct lua_State *L);
static int l_loadfile(struct lua_State *L);
static int l_rawget(struct lua_State *L);
static int l_rawset(struct lua_State *L);
static int l_rawequal(struct lua_State *L);
static int l_rawlen(struct lua_State *L);

static fn_t functions[] = {
    {l_print, "print"},
    {l_dofile, "dofile"},
    {l_loadfile, "loadfile"},
    {l_rawget, "rawget"},
    {l_rawset, "rawset"},
    {l_rawequal, "rawequal"},
    {l_rawlen, "rawlen"},
};

void RestoreBuiltinFunctions(struct lua_State *L)
{
    for (size_t i = 0; i < sizeof(functions) / sizeof(functions[0]); i++) {
        lua_pushcfunction(L, functions[i].function);
        lua_setglobal(L, functions[i].name);
    }
}

void RestoreBuiltinModules(struct lua_State *L)
{
    // `require`
    luaL_requiref(L, "package", luaopen_package, 1);
    lua_pop(L, 1);

    // `debug` module
    luaL_requiref(L, "debug", luaopen_debug, 1);
    lua_pop(L, 1);

    // `os` module
    luaL_requiref(L, "os", luaopen_os, 1);
    lua_pop(L, 1);
    // IMPORTANT: disable `os.exit` function to avoid terminating the host process
    lua_getglobal(L, "os");
    lua_pushnil(L);
    lua_setfield(L, -2, "exit");
    lua_pop(L, 1);

    // `io` module
    luaL_requiref(L, "io", luaopen_io, 1);
    lua_pop(L, 1);

    // `coroutine` module
    luaL_requiref(L, "coroutine", luaopen_coroutine, 1);
    lua_pop(L, 1);

    lua_gc(L, LUA_GCRESTART, 0);
}

static int l_print(struct lua_State *L)
{
    int argc = lua_gettop(L);
    luaL_Buffer buf;
    luaL_buffinit(L, &buf);
    for (int i = 1; i <= argc; i++)
    {
        size_t len;
        const char *s = luaL_tolstring(L, i, &len);
        luaL_addlstring(&buf, s, len);
        lua_pop(L, 1); // remove the string returned by luaL_tolstring from the stack
        if (i != argc) {
            luaL_addchar(&buf, '\t');
        }
    }
    luaL_addchar(&buf, '\n');
    luaL_pushresult(&buf);
    int message_index = lua_gettop(L);

    // call OutputLogMessage
    lua_getglobal(L, "OutputLogMessage");
    lua_pushvalue(L, message_index);
    lua_call(L, 1, 0);

    return 0;
}

// Minimal implementation of loadfile
static int l_loadfile(struct lua_State *L)
{
    const char *filename = luaL_optstring(L, 1, NULL);
    int status = luaL_loadfilex(L, filename, NULL);
    if (status == LUA_OK)
    {
        return 1; // Returns the compiled function
    } else
    {
        lua_pushnil(L);
        lua_insert(L, -2); // nil, error message
        return 2;
    }
}

// Minimal implementation of dofile
static int l_dofile(struct lua_State *L)
{
    const char *filename = luaL_optstring(L, 1, NULL);
    if (luaL_loadfilex(L, filename, NULL) != LUA_OK)
    {
        return lua_error(L); // dofile throws errors directly
    }
    lua_call(L, 0, LUA_MULTRET);
    return lua_gettop(L); // Returns all results
}

static int l_rawget(struct lua_State *L)
{
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_pushvalue(L, 2);          // push key
    lua_rawget(L, 1);             // pops key, pushes value
    return 1;
}

static int l_rawset(struct lua_State *L)
{
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_pushvalue(L, 2);          // push key
    lua_pushvalue(L, 3);          // push value
    lua_rawset(L, 1);             // pops key+value
    return 1;                     // return the table, like the stdlib
}

static int l_rawequal(struct lua_State *L)
{
    lua_pushboolean(L, lua_rawequal(L, 1, 2));
    return 1;
}

static int l_rawlen(struct lua_State *L)
{
    lua_pushinteger(L, (lua_Integer)lua_rawlen(L, 1));
    return 1;
}
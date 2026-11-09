#include <lua.h>

static const char *s_LogitechFunctions[] = {
    "PressAndReleaseMouseButton",
    "PressAndReleaseHidKey",
    "PressAndReleaseKey",
    "PlayMacro",
    "EnablePrimaryMouseButtonEvents",
    "SetMouseDPITableIndex",
    "SetMouseDPITable",
    "GetMouseSpeed",
    "ReleaseHidKey",
    "DecrementMouseSpeed",
    "IncrementMouseSpeed",
    "SetBacklightColor",
    "PressMouseButton",
    "ReleaseKey",
    "SetMKeyState",
    "IsModifierPressed",
    "GetMKeyState",
    "AbortMacro",
    "ReleaseMacro",
    "GetRunningTime",
    "GetDate",
    "Sleep",
    "MoveMouseTo",
    "OutputDebugMessage",
    "PressMacro",
    "GetMousePosition",
    "PressHidKey",
    "MoveMouseToVirtual",
    "MoveMouseRelative",
    "MoveMouseWheel",
    "IsMouseButtonPressed",
    "SetMouseSpeed",
    "ClearLog",
    "OutputLogMessage",
    "IsKeyLockOn",
    "ReleaseMouseButton",
    "PressKey",
};

// Must be called after the modules are initialized
void CreateLogitechNamespace(struct lua_State *L) {
    lua_createtable(L, 0, (int)(sizeof(s_LogitechFunctions) / sizeof(s_LogitechFunctions[0])));
    for (size_t i = 0; i < sizeof(s_LogitechFunctions) / sizeof(s_LogitechFunctions[0]); i++) {
        const char *name = s_LogitechFunctions[i];
        lua_getglobal(L, name);
        lua_setfield(L, -2, name);
        lua_pushnil(L);
        lua_setglobal(L, name);
    }
    // lua_setglobal(L, "Logitech");
    // Get the package.loaded table
    lua_getglobal(L, "package");      // Stack: table, package
    lua_getfield(L, -1, "loaded");    // Stack: table, package, loaded
    lua_remove(L, -2);                // Stack: table, loaded

    // Insert your table into package.loaded["mymodule"]
    lua_pushvalue(L, -2);             // Copy the table (now at -2)
    lua_setfield(L, -2, "executor.Logitech");  // loaded["mymodule"] = table

    // Clean up the stack, leaving only the original table
    lua_pop(L, 1);                    // Pop the loaded table, leaving the original table
}

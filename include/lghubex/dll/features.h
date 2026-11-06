#pragma once

#ifdef __cplusplus
extern "C" {
#endif
struct lua_State;
void RestoreBuiltinFunctions(struct lua_State *L);
void RestoreBuiltinModules(struct lua_State *L);
#ifdef __cplusplus
}
#endif
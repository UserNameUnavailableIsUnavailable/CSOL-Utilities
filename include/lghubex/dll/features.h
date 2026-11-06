#pragma once

#ifdef __cplusplus
extern "C" {
#endif
struct lua_State;
int l_print(struct lua_State *L);
int l_loadfile(struct lua_State *L);
int l_dofile(struct lua_State *L);
#ifdef __cplusplus
}
#endif
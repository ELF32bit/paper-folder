#ifndef LUA_MANAGER_H
#define LUA_MANAGER_H

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

void luaM_register_modules(lua_State* L);
void luaM_register_scripts(lua_State* L);
void luaM_register_arg(lua_State* L, int argc, char* argv[]);
int luaM_close(lua_State* L, int status);

#endif
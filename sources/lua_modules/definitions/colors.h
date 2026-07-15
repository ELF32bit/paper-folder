#ifndef LUA_MODULE_DEFINITIONS_COLORS_H
#define LUA_MODULE_DEFINITIONS_COLORS_H

#include <lua.h>
#include <lauxlib.h>

int l_definitions_colors_all(lua_State* L);

int l_definitions_colors_RESET(lua_State* L);
int l_definitions_colors_BOLD(lua_State* L);
int l_definitions_colors_FAINT(lua_State* L);

int l_definitions_colors_BLACK(lua_State* L);
int l_definitions_colors_MAROON(lua_State* L);
int l_definitions_colors_GREEN(lua_State* L);
int l_definitions_colors_OLIVE(lua_State* L);
int l_definitions_colors_NAVY(lua_State* L);
int l_definitions_colors_PURPLE(lua_State* L);
int l_definitions_colors_TEAL(lua_State* L);
int l_definitions_colors_SILVER(lua_State* L);

int l_definitions_colors_GRAY(lua_State* L);
int l_definitions_colors_RED(lua_State* L);
int l_definitions_colors_LIME(lua_State* L);
int l_definitions_colors_YELLOW(lua_State* L);
int l_definitions_colors_BLUE(lua_State* L);
int l_definitions_colors_FUCHSIA(lua_State* L);
int l_definitions_colors_AQUA(lua_State* L);
int l_definitions_colors_WHITE(lua_State* L);

#endif /* LUA_MODULE_DEFINITIONS_COLORS_H */
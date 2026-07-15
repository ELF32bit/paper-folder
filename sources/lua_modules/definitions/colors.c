#include "colors.h"

#include "definitions/colors.h"

int l_definitions_colors_all(lua_State* L) {
	lua_createtable(L, 0, 19);
	lua_pushliteral(L, ANSI_COLOR_RESET);
	lua_setfield(L, -2, "RESET");
	lua_pushliteral(L, ANSI_COLOR_BOLD);
	lua_setfield(L, -2, "BOLD");
	lua_pushliteral(L, ANSI_COLOR_FAINT);
	lua_setfield(L, -2, "FAINT");
	lua_pushliteral(L, ANSI_COLOR_BLACK);
	lua_setfield(L, -2, "BLACK");
	lua_pushliteral(L, ANSI_COLOR_MAROON);
	lua_setfield(L, -2, "MAROON");
	lua_pushliteral(L, ANSI_COLOR_GREEN);
	lua_setfield(L, -2, "GREEN");
	lua_pushliteral(L, ANSI_COLOR_OLIVE);
	lua_setfield(L, -2, "OLIVE");
	lua_pushliteral(L, ANSI_COLOR_NAVY);
	lua_setfield(L, -2, "NAVY");
	lua_pushliteral(L, ANSI_COLOR_PURPLE);
	lua_setfield(L, -2, "PURPLE");
	lua_pushliteral(L, ANSI_COLOR_TEAL);
	lua_setfield(L, -2, "TEAL");
	lua_pushliteral(L, ANSI_COLOR_SILVER);
	lua_setfield(L, -2, "SILVER");
	lua_pushliteral(L, ANSI_COLOR_GRAY);
	lua_setfield(L, -2, "GRAY");
	lua_pushliteral(L, ANSI_COLOR_RED);
	lua_setfield(L, -2, "RED");
	lua_pushliteral(L, ANSI_COLOR_LIME);
	lua_setfield(L, -2, "LIME");
	lua_pushliteral(L, ANSI_COLOR_YELLOW);
	lua_setfield(L, -2, "YELLOW");
	lua_pushliteral(L, ANSI_COLOR_BLUE);
	lua_setfield(L, -2, "BLUE");
	lua_pushliteral(L, ANSI_COLOR_FUCHSIA);
	lua_setfield(L, -2, "FUCHSIA");
	lua_pushliteral(L, ANSI_COLOR_AQUA);
	lua_setfield(L, -2, "AQUA");
	lua_pushliteral(L, ANSI_COLOR_WHITE);
	lua_setfield(L, -2, "WHITE");
	return 1;
}

#define GENERATE_FUNCTIONS(color) \
int l_definitions_colors_##color(lua_State* L) { \
	lua_pushstring(L, ANSI_COLOR_##color); \
	return 1; \
}

GENERATE_FUNCTIONS(RESET)
GENERATE_FUNCTIONS(BOLD)
GENERATE_FUNCTIONS(FAINT)

GENERATE_FUNCTIONS(BLACK)
GENERATE_FUNCTIONS(MAROON)
GENERATE_FUNCTIONS(GREEN)
GENERATE_FUNCTIONS(OLIVE)
GENERATE_FUNCTIONS(NAVY)
GENERATE_FUNCTIONS(PURPLE)
GENERATE_FUNCTIONS(TEAL)
GENERATE_FUNCTIONS(SILVER)

GENERATE_FUNCTIONS(GRAY)
GENERATE_FUNCTIONS(RED)
GENERATE_FUNCTIONS(LIME)
GENERATE_FUNCTIONS(YELLOW)
GENERATE_FUNCTIONS(BLUE)
GENERATE_FUNCTIONS(FUCHSIA)
GENERATE_FUNCTIONS(AQUA)
GENERATE_FUNCTIONS(WHITE)
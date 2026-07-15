#include <stdlib.h>
#include <stdio.h>

#include "lua_manager.h"
#include "definitions/colors.h"

int main(int argc, char* argv[]) {
	lua_State* L = luaL_newstate();
	if (L == NULL) {
		fprintf(stderr,
			ANSI_COLOR_BOLD ANSI_COLOR_AQUA
			"main.lua:0: failed to start Lua"
			ANSI_COLOR_RESET "\n");
		return luaM_close(L, EXIT_FAILURE);
	}

	luaL_openlibs(L);
	luaM_register_modules(L);
	luaM_register_scripts(L);
	luaM_register_arg(L, argc, argv);
	lua_getglobal(L, "require");
	lua_pushstring(L, "main");

	if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
		fprintf(stderr,
			ANSI_COLOR_BOLD ANSI_COLOR_AQUA
			"%s"
			ANSI_COLOR_RESET "\n",
			lua_tostring(L, -1));
		return luaM_close(L, EXIT_FAILURE);
	}

	return luaM_close(L, EXIT_SUCCESS);
}

#include "lua_manager.h"

#include "lua_modules.h"
#include "lua_scripts.h"

void luaM_register_modules(lua_State* L) {
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "preload");
	for (size_t i = 0; g_lua_modules[i].name != NULL; i++) {
		lua_pushcfunction(L, g_lua_modules[i].open_function);
		lua_setfield(L, -2, g_lua_modules[i].name);
	}
	lua_pop(L, 2);
}

static int luaM_script_loader(lua_State* L) {
	const LuaScript* script = lua_touserdata(L,
		lua_upvalueindex(1));
	lua_settop(L, 0);
	if (luaL_loadbuffer(L, script->data,
		script->data_size, script->name) != LUA_OK) {
		return lua_error(L);
	}
	if (lua_pcall(L, 0, LUA_MULTRET, 0) != LUA_OK) {
		return lua_error(L);
	}
	return lua_gettop(L);
}

void luaM_register_scripts(lua_State* L) {
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "preload");
	for (size_t i = 0; g_lua_scripts[i].name != NULL; i++) {
		lua_pushlightuserdata(L, (void*)&g_lua_scripts[i]);
		lua_pushcclosure(L, luaM_script_loader, 1);
		lua_setfield(L, -2, g_lua_scripts[i].name);
	}
	lua_pop(L, 2);
}

void luaM_register_arg(lua_State* L, int argc, char* argv[]) {
	lua_newtable(L);
	for (int i = 0; i < argc; i++) {
		lua_pushinteger(L, i + 0);
		lua_pushstring(L, argv[i]);
		lua_settable(L, -3);
	}
	lua_setglobal(L, "arg");
}

int luaM_close(lua_State* L, int status) {
	if (L != NULL) { lua_close(L); }
	return status;
}
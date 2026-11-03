#include "lua_modules/fold/graph.h"

#include "fold_graph/edges.h"

int l_fold_graph_EA_from_EF_meta(lua_State* L) {
	LuaFoldGraph* wrapper = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);

	Error result = fold_graph_EA_from_EF(wrapper->graph);
	if IS_ERROR(result) {
		lua_pushboolean(L, false);
		lua_pushstring(L, ERROR_STRING(result));
		return 2;
	}

	lua_pushboolean(L, true);
	lua_pushnil(L);
	return 2;
}

int l_fold_graph_EA_from_EFA_meta(lua_State* L) {
	LuaFoldGraph* wrapper = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);

	Error result = fold_graph_EA_from_EFA(wrapper->graph);
	if IS_ERROR(result) {
		lua_pushboolean(L, false);
		lua_pushstring(L, ERROR_STRING(result));
		return 2;
	}

	lua_pushboolean(L, true);
	lua_pushnil(L);
	return 2;
}

int l_fold_graph_EFA_from_EA_meta(lua_State* L) {
	LuaFoldGraph* wrapper = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);

	Error result = fold_graph_EFA_from_EA(wrapper->graph);
	if IS_ERROR(result) {
		lua_pushboolean(L, false);
		lua_pushstring(L, ERROR_STRING(result));
		return 2;
	}

	lua_pushboolean(L, true);
	lua_pushnil(L);
	return 2;
}

int l_fold_graph_EL_from_EVC_meta(lua_State* L) {
	LuaFoldGraph* wrapper = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);

	Error result = fold_graph_EL_from_EVC(wrapper->graph);
	if IS_ERROR(result) {
		lua_pushboolean(L, false);
		lua_pushstring(L, ERROR_STRING(result));
		return 2;
	}

	lua_pushboolean(L, true);
	lua_pushnil(L);
	return 2;
}

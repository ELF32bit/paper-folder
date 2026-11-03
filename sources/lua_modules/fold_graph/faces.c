#include "lua_modules/fold/graph.h"

#include "fold_graph/faces.h"

int l_fold_graph_FE_from_FV_meta(lua_State* L) {
	LuaFoldGraph* wrapper = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);

	Error result = fold_graph_FE_from_FV(wrapper->graph, NULL);
	if IS_ERROR(result) {
		lua_pushboolean(L, false);
		lua_pushstring(L, ERROR_STRING(result));
		return 2;
	}

	lua_pushboolean(L, true);
	lua_pushnil(L);
	return 2;
}

int l_fold_graph_FF_from_FV_meta(lua_State* L) {
	LuaFoldGraph* wrapper = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);

	Error result = fold_graph_FF_from_FV(wrapper->graph);
	if IS_ERROR(result) {
		lua_pushboolean(L, false);
		lua_pushstring(L, ERROR_STRING(result));
		return 2;
	}

	lua_pushboolean(L, true);
	lua_pushnil(L);
	return 2;
}

int l_fold_graph_FV_triangulate_meta(lua_State* L) {
	LuaFoldGraph* wrapper = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);

	Error result = fold_graph_FV_triangulate(wrapper->graph);
	if IS_ERROR(result) {
		lua_pushboolean(L, false);
		lua_pushstring(L, ERROR_STRING(result));
		return 2;
	}

	lua_pushboolean(L, true);
	lua_pushnil(L);
	return 2;
}

int l_fold_graph_FV_flip_meta(lua_State* L) {
	LuaFoldGraph* wrapper = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	fold_graph_FV_flip(wrapper->graph);
	return 0;
}
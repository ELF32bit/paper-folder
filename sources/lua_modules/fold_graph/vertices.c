#include "lua_modules/fold/graph.h"

#include "fold_graph/vertices.h"

int l_fold_graph_VV_from_EV_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	bool sorted = lua_toboolean(L, 2);

	Error result = fold_graph_VV_from_EV(graph, sorted);
	if IS_ERROR(result) {
		lua_pushboolean(L, false);
		lua_pushstring(L, ERROR_STRING(result));
		return 2;
	}

	lua_pushboolean(L, true);
	lua_pushnil(L);
	return 2;
}

int l_fold_graph_VV_from_FV_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	bool sorted = lua_toboolean(L, 2);

	Error result = fold_graph_VV_from_FV(graph, sorted);
		if IS_ERROR(result) {
		lua_pushboolean(L, false);
		lua_pushstring(L, ERROR_STRING(result));
		return 2;
	}

	lua_pushboolean(L, true);
	lua_pushnil(L);
	return 2;
}

int l_fold_graph_VE_from_VV_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);

	Error result = fold_graph_VE_from_VV(graph, NULL);
	if IS_ERROR(result) {
		lua_pushboolean(L, false);
		lua_pushstring(L, ERROR_STRING(result));
		return 2;
	}

	lua_pushboolean(L, true);
	lua_pushnil(L);
	return 2;
}

int l_fold_graph_VF_from_VV_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);

	Error result = fold_graph_VF_from_VV(graph);
	if IS_ERROR(result) {
		lua_pushboolean(L, false);
		lua_pushstring(L, ERROR_STRING(result));
		return 2;
	}

	lua_pushboolean(L, true);
	lua_pushnil(L);
	return 2;
}
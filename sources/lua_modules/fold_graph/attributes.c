#include "lua_modules/fold/graph.h"

#include "fold_graph/attributes.h"

int l_fold_graph_is_abstract_meta(lua_State* L) {
	LuaFoldGraph* wrapper = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	lua_pushboolean(L, fold_graph_is_abstract(wrapper->graph));
	return 1;
}

int l_fold_graph_is_2D_meta(lua_State* L) {
	LuaFoldGraph* wrapper = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	lua_pushboolean(L, fold_graph_is_2D(wrapper->graph));
	return 1;
}

int l_fold_graph_is_3D_meta(lua_State* L) {
	LuaFoldGraph* wrapper = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	lua_pushboolean(L, fold_graph_is_3D(wrapper->graph));
	return 1;
}

int l_fold_graph_is_manifold_meta(lua_State* L) {
	LuaFoldGraph* wrapper = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	lua_pushboolean(L, fold_graph_is_manifold(wrapper->graph));
	return 1;
}

int l_fold_graph_is_orientable_meta(lua_State* L) {
	LuaFoldGraph* wrapper = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);

	bool is;
	Error result = fold_graph_is_orientable(wrapper->graph, &is);
	if IS_ERROR(result) {
		lua_pushnil(L);
		lua_pushstring(L, ERROR_STRING(result));
		return 2;
	}

	lua_pushboolean(L, is);
	lua_pushnil(L);
	return 1;
}

int l_fold_graph_is_self_touching_meta(lua_State* L) {
	LuaFoldGraph* wrapper = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);

	bool is;
	Error result = fold_graph_is_self_touching(wrapper->graph, &is);
	if IS_ERROR(result) {
		lua_pushnil(L);
		lua_pushstring(L, ERROR_STRING(result));
		return 2;
	}

	lua_pushboolean(L, is);
	lua_pushnil(L);
	return 1;
}

int l_fold_graph_is_self_intersecting_meta(lua_State* L) {
	LuaFoldGraph* wrapper = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);

	bool is;
	Error result = fold_graph_is_self_intersecting(wrapper->graph, &is);
	if IS_ERROR(result) {
		lua_pushnil(L);
		lua_pushstring(L, ERROR_STRING(result));
		return 2;
	}

	lua_pushboolean(L, is);
	lua_pushnil(L);
	return 1;
}

int l_fold_graph_has_cuts_meta(lua_State* L) {
	LuaFoldGraph* wrapper = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	lua_pushboolean(L, fold_graph_has_cuts(wrapper->graph));
	return 1;
}

int l_fold_graph_has_joins_meta(lua_State* L) {
	LuaFoldGraph* wrapper = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	lua_pushboolean(L, fold_graph_has_joins(wrapper->graph));
	return 1;
}

int l_fold_graph_has_concave_faces_meta(lua_State* L) {
	LuaFoldGraph* wrapper = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	lua_pushboolean(L, fold_graph_has_concave_faces(wrapper->graph));
	return 1;
}
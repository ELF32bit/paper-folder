#include "graph.h"

int l_fold_graph_is_abstract_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	lua_pushboolean(L, fold_graph_is_abstract(graph));
	return 1;
}

int l_fold_graph_is_2D_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	lua_pushboolean(L, fold_graph_is_2D(graph));
	return 1;
}

int l_fold_graph_is_3D_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	lua_pushboolean(L, fold_graph_is_3D(graph));
	return 1;
}

int l_fold_graph_is_manifold_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	lua_pushboolean(L, fold_graph_is_manifold(graph));
	return 1;
}
int l_fold_graph_is_orientable_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	lua_pushboolean(L, fold_graph_is_orientable(graph));
	return 1;
}
int l_fold_graph_has_cuts_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	lua_pushboolean(L, fold_graph_has_cuts(graph));
	return 1;
}

int l_fold_graph_has_joins_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	lua_pushboolean(L, fold_graph_has_joins(graph));
	return 1;
}

int l_fold_graph_is_self_touching_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	lua_pushboolean(L, fold_graph_is_self_touching(graph));
	return 1;
}

int l_fold_graph_is_self_intersecting_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	lua_pushboolean(L, fold_graph_is_self_intersecting(graph));
	return 1;
}

int l_fold_graph_has_non_convex_faces_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	lua_pushboolean(L, fold_graph_has_non_convex_faces(graph));
	return 1;
}
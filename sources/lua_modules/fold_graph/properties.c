#include "lua_modules/fold/graph.h"

#include "fold_graph/properties.h"

int l_fold_graph_get_abstract_size_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	lua_pushinteger(L, fold_graph_get_abstract_size(graph));
	return 1;
}

int l_fold_graph_get_aabb2_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	AABB2 aabb = fold_graph_get_aabb2(graph);
	lua_pushnumber(L, aabb.min.x);
	lua_pushnumber(L, aabb.min.y);
	lua_pushnumber(L, aabb.max.x);
	lua_pushnumber(L, aabb.max.y);
	return 4;
}

int l_fold_graph_get_aabb3_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	AABB3 aabb = fold_graph_get_aabb3(graph);
	lua_pushnumber(L, aabb.min.x);
	lua_pushnumber(L, aabb.min.y);
	lua_pushnumber(L, aabb.min.z);
	lua_pushnumber(L, aabb.max.x);
	lua_pushnumber(L, aabb.max.y);
	lua_pushnumber(L, aabb.max.z);
	return 6;
}

int l_fold_graph_is_maekawa_compliant_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	lua_pushboolean(L, fold_graph_is_maekawa_compliant(graph));
	return 1;
}

int l_fold_graph_is_kawasaki_compliant_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	real error = luaL_optnumber(L, 2, 1.0);
	lua_pushboolean(L, fold_graph_is_kawasaki_compliant(graph, error));
	return 1;
}

int l_fold_graph_is_locally_flat_foldable_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	real error = luaL_optnumber(L, 2, 1.0);
	lua_pushboolean(L, fold_graph_is_locally_flat_foldable(graph, error));
	return 1;
}

int l_fold_graph_has_unassigned_vertices_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	lua_pushboolean(L, fold_graph_has_unassigned_vertices(graph));
	return 1;
}

int l_fold_graph_is_triangulated_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	lua_pushboolean(L, fold_graph_is_triangulated(graph));
	return 1;
}
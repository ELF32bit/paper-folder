#include "lua_modules/fold/graph.h"

#include "fold_graph/properties.h"

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

int l_fold_graph_is_kawasaki_compatible_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	real error = luaL_optnumber(L, 2, 1.0);
	lua_pushboolean(L, fold_graph_is_kawasaki_compatible(graph, error));
	return 1;
}
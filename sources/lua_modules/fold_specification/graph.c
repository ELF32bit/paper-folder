#include "lua_modules/fold/graph.h"

/* ========================================================================= */
/* Creation & Destruction                                                    */
/* ========================================================================= */

int l_fold_graph_new(lua_State* L) {
	FoldGraph* graph = lua_newuserdata(L, sizeof(FoldGraph));
	luaL_getmetatable(L, FOLD_GRAPH_METATABLE);
	lua_setmetatable(L, -2);
	fold_graph_create(graph);
	return 1;
}

int l_fold_graph___index_meta(lua_State* L) {
	luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	luaL_getmetatable(L, FOLD_GRAPH_METATABLE);
	lua_pushvalue(L, 2);
	lua_gettable(L, -2);
	return 1;
}

int l_fold_graph___gc_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	fold_graph_destroy(graph);
	return 0;
}
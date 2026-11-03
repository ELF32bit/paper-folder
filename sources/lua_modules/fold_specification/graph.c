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

/* ========================================================================= */
/* Getters                                                                   */
/* ========================================================================= */

#define GENERATE_GET_ARRAY_SIZE(array) \
int l_fold_graph_get_##array##_size_meta(lua_State* L) { \
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE); \
	lua_pushinteger(L, graph->array.size); \
	return 1; \
}

GENERATE_GET_ARRAY_SIZE(VC)
GENERATE_GET_ARRAY_SIZE(VV)
GENERATE_GET_ARRAY_SIZE(VE)
GENERATE_GET_ARRAY_SIZE(VF)
GENERATE_GET_ARRAY_SIZE(EV)
GENERATE_GET_ARRAY_SIZE(EF)
GENERATE_GET_ARRAY_SIZE(EA)
GENERATE_GET_ARRAY_SIZE(EFA)
GENERATE_GET_ARRAY_SIZE(EL)
GENERATE_GET_ARRAY_SIZE(EO)
GENERATE_GET_ARRAY_SIZE(FV)
GENERATE_GET_ARRAY_SIZE(FE)
GENERATE_GET_ARRAY_SIZE(FF)
GENERATE_GET_ARRAY_SIZE(FO)

GENERATE_GET_ARRAY_SIZE(PC)
GENERATE_GET_ARRAY_SIZE(TC)
GENERATE_GET_ARRAY_SIZE(T2C)
GENERATE_GET_ARRAY_SIZE(NC)
GENERATE_GET_ARRAY_SIZE(MN)
GENERATE_GET_ARRAY_SIZE(VP)
GENERATE_GET_ARRAY_SIZE(FT)
GENERATE_GET_ARRAY_SIZE(FT2)
GENERATE_GET_ARRAY_SIZE(FN)
GENERATE_GET_ARRAY_SIZE(FM)
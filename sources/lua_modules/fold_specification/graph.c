#include "lua_modules/fold/graph.h"

/* ========================================================================= */
/* Creation & Destruction                                                    */
/* ========================================================================= */

int l_fold_graph_new(lua_State* L) {
	LuaFoldGraph* wrapper = lua_newuserdata(L, sizeof(LuaFoldGraph));
	wrapper->graph = malloc(sizeof(FoldGraph));
	TRY_NEW_MEMORY_OR_ERROR(wrapper->graph,
		luaL_error(L, ERROR_STRING(ERROR_OUT_OF_MEMORY)));
	fold_graph_create(wrapper->graph);
	wrapper->is_owned = true;
	luaL_getmetatable(L, FOLD_GRAPH_METATABLE);
	lua_setmetatable(L, -2);
	return 1;
}

int l_fold_graph_is_owned_meta(lua_State* L) {
	LuaFoldGraph* wrapper = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	lua_pushboolean(L, NOT(wrapper->is_owned));
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
	LuaFoldGraph* wrapper = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	if (wrapper->is_owned && wrapper->graph != NULL) {
		fold_graph_destroy(wrapper->graph);
		free(wrapper->graph);
		wrapper->graph = NULL;
		wrapper->is_owned = false;
	}
	return 0;
}

/* ========================================================================= */
/* Getters                                                                   */
/* ========================================================================= */

#define GENERATE_GET_ARRAY_SIZE(array) \
int l_fold_graph_get_##array##_size_meta(lua_State* L) { \
	LuaFoldGraph* wrapper = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE); \
	lua_pushinteger(L, wrapper->graph->array.size); \
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

/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */

int l_fold_graph_copy_meta(lua_State* L) {
	LuaFoldGraph* wrapper = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	LuaFoldGraph* source_wrapper = luaL_checkudata(L, 2, FOLD_GRAPH_METATABLE);

	Error result = fold_graph_copy(wrapper->graph, source_wrapper->graph);
	if IS_ERROR(result) {
		lua_pushboolean(L, false);
		lua_pushstring(L, ERROR_STRING(result));
		return 2;
	}

	lua_pushboolean(L, true);
	lua_pushnil(L);
	return 2;
}

int l_fold_graph_inherit_meta(lua_State* L) {
	LuaFoldGraph* wrapper = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	LuaFoldGraph* source_wrapper = luaL_checkudata(L, 2, FOLD_GRAPH_METATABLE);
	fold_graph_inherit(wrapper->graph, source_wrapper->graph);
	return 0;
}

int l_fold_graph_is_inherited_meta(lua_State* L) {
	LuaFoldGraph* wrapper = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	lua_pushboolean(L, fold_graph_is_inherited(wrapper->graph));
	return 1;
}
#include "lua_modules/fold/frame.h"

/* ========================================================================= */
/* Creation & Destruction                                                    */
/* ========================================================================= */

int l_fold_frame_new(lua_State* L) {
	FoldFrame* frame = lua_newuserdata(L, sizeof(FoldFrame));
	luaL_getmetatable(L, FOLD_FRAME_METATABLE);
	lua_setmetatable(L, -2);
	fold_frame_create(frame);
	return 1;
}

int l_fold_frame___index_meta(lua_State* L) {
	luaL_checkudata(L, 1, FOLD_FRAME_METATABLE);
	luaL_getmetatable(L, FOLD_FRAME_METATABLE);
	lua_pushvalue(L, 2);
	lua_gettable(L, -2);
	return 1;
}

int l_fold_frame___gc_meta(lua_State* L) {
	FoldFrame* frame = luaL_checkudata(L, 1, FOLD_FRAME_METATABLE);
	fold_frame_destroy(frame);
	return 0;
}

/* ========================================================================= */
/* Getters                                                                   */
/* ========================================================================= */

#define GENERATE_GET_STRING(property) \
int l_fold_frame_get_##property##_meta(lua_State* L) { \
	FoldFrame* frame = luaL_checkudata(L, 1, FOLD_FRAME_METATABLE); \
	lua_pushlstring(L, frame->property.data, frame->property.length); \
	return 1; \
}

#define GENERATE_GET_STRING_ARRAY(property) \
int l_fold_frame_get_##property##_meta(lua_State* L) { \
	FoldFrame* frame = luaL_checkudata(L, 1, FOLD_FRAME_METATABLE); \
	lua_createtable(L, frame->property.size, 0); \
	ARRAY_FOR_EACH(&frame->property, index, char*, string) { \
		lua_pushstring(L, string); \
		lua_rawseti(L, -2, index + 1); \
	} \
	return 1; \
}

GENERATE_GET_STRING(author)
GENERATE_GET_STRING(title)
GENERATE_GET_STRING(description)
GENERATE_GET_STRING_ARRAY(classes)
GENERATE_GET_STRING_ARRAY(attributes)
GENERATE_GET_STRING(unit)

int l_fold_frame_get_graph_meta(lua_State* L) {
	FoldFrame* frame = luaL_checkudata(L, 1, FOLD_FRAME_METATABLE);
	lua_pushlightuserdata(L, &frame->graph);
	return 1;
}

int l_fold_frame_get_parent_meta(lua_State* L) {
	FoldFrame* frame = luaL_checkudata(L, 1, FOLD_FRAME_METATABLE);
	if (frame->parent != FOLD_FRAME_PARENT_NONE) {
		lua_pushinteger(L, frame->parent);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int l_fold_frame_get_inherit_meta(lua_State* L) {
	FoldFrame* frame = luaL_checkudata(L, 1, FOLD_FRAME_METATABLE);
	lua_pushboolean(L, frame->inherit);
	return 1;
}

/* ========================================================================= */
/* Graph Getters                                                             */
/* ========================================================================= */

#define GENERATE_GET_GRAPH_ARRAY_SIZE(array) \
int l_fold_frame_get_graph_##array##_size_meta(lua_State* L) { \
	FoldFrame* frame = luaL_checkudata(L, 1, FOLD_FRAME_METATABLE); \
	lua_pushinteger(L, frame->graph.array.size); \
	return 1; \
}

GENERATE_GET_GRAPH_ARRAY_SIZE(VC)
GENERATE_GET_GRAPH_ARRAY_SIZE(VV)
GENERATE_GET_GRAPH_ARRAY_SIZE(VE)
GENERATE_GET_GRAPH_ARRAY_SIZE(VF)
GENERATE_GET_GRAPH_ARRAY_SIZE(EV)
GENERATE_GET_GRAPH_ARRAY_SIZE(EF)
GENERATE_GET_GRAPH_ARRAY_SIZE(EA)
GENERATE_GET_GRAPH_ARRAY_SIZE(EFA)
GENERATE_GET_GRAPH_ARRAY_SIZE(EL)
GENERATE_GET_GRAPH_ARRAY_SIZE(EO)
GENERATE_GET_GRAPH_ARRAY_SIZE(FV)
GENERATE_GET_GRAPH_ARRAY_SIZE(FE)
GENERATE_GET_GRAPH_ARRAY_SIZE(FF)
GENERATE_GET_GRAPH_ARRAY_SIZE(FO)

GENERATE_GET_GRAPH_ARRAY_SIZE(PC)
GENERATE_GET_GRAPH_ARRAY_SIZE(TC)
GENERATE_GET_GRAPH_ARRAY_SIZE(T2C)
GENERATE_GET_GRAPH_ARRAY_SIZE(NC)
GENERATE_GET_GRAPH_ARRAY_SIZE(MN)
GENERATE_GET_GRAPH_ARRAY_SIZE(VP)
GENERATE_GET_GRAPH_ARRAY_SIZE(FT)
GENERATE_GET_GRAPH_ARRAY_SIZE(FT2)
GENERATE_GET_GRAPH_ARRAY_SIZE(FN)
GENERATE_GET_GRAPH_ARRAY_SIZE(FM)

/* ========================================================================= */
/* Attributes                                                                */
/* ========================================================================= */

int l_fold_frame_attributes_assign_meta(lua_State* L) {
	FoldFrame* frame = luaL_checkudata(L, 1, FOLD_FRAME_METATABLE);
	Error result = fold_frame_attributes_assign(frame);
	if IS_OK(result) {
		lua_pushboolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}
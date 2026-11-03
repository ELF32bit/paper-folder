#include "lua_modules/fold/frame.h"
#include "lua_modules/fold/graph.h"

/* ========================================================================= */
/* Creation & Destruction                                                    */
/* ========================================================================= */

int l_fold_frame_new(lua_State* L) {
	LuaFoldFrame* wrapper = lua_newuserdata(L, sizeof(LuaFoldFrame));
	wrapper->frame = malloc(sizeof(FoldFrame));
	TRY_NEW_MEMORY_OR_ERROR(wrapper->frame,
		luaL_error(L, ERROR_STRING(ERROR_OUT_OF_MEMORY)));
	fold_frame_create(wrapper->frame);
	wrapper->is_owned = true;
	luaL_getmetatable(L, FOLD_FRAME_METATABLE);
	lua_setmetatable(L, -2);
	return 1;
}

int l_fold_frame_is_owned_meta(lua_State* L) {
	LuaFoldFrame* wrapper = luaL_checkudata(L, 1, FOLD_FRAME_METATABLE);
	lua_pushboolean(L, NOT(wrapper->is_owned));
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
	LuaFoldFrame* wrapper = luaL_checkudata(L, 1, FOLD_FRAME_METATABLE);
	if (wrapper->is_owned && wrapper->frame != NULL) {
		fold_frame_destroy(wrapper->frame);
		free(wrapper->frame);
		wrapper->frame = NULL;
		wrapper->is_owned = false;
	}
	return 0;
}

/* ========================================================================= */
/* Getters                                                                   */
/* ========================================================================= */

#define GENERATE_GET_STRING(property) \
int l_fold_frame_get_##property##_meta(lua_State* L) { \
	LuaFoldFrame* wrapper = luaL_checkudata(L, 1, FOLD_FRAME_METATABLE); \
	lua_pushlstring(L, wrapper->frame->property.data, \
		wrapper->frame->property.length); \
	return 1; \
}

#define GENERATE_GET_STRING_ARRAY(property) \
int l_fold_frame_get_##property##_meta(lua_State* L) { \
	LuaFoldFrame* wrapper = luaL_checkudata(L, 1, FOLD_FRAME_METATABLE); \
	lua_createtable(L, wrapper->frame->property.size, 0); \
	ARRAY_FOR_EACH(&wrapper->frame->property, index, char*, string) { \
		lua_pushstring(L, string); \
		lua_rawseti(L, -2, index + 1); \
	} \
	return 1; \
} \

GENERATE_GET_STRING(author)
GENERATE_GET_STRING(title)
GENERATE_GET_STRING(description)
GENERATE_GET_STRING_ARRAY(classes)
GENERATE_GET_STRING_ARRAY(attributes)
GENERATE_GET_STRING(unit)

int l_fold_frame_get_graph_meta(lua_State* L) {
	LuaFoldFrame* wrapper = luaL_checkudata(L, 1, FOLD_FRAME_METATABLE);
	LuaFoldGraph* graph_wrapper = lua_newuserdata(L, sizeof(LuaFoldGraph));
	graph_wrapper->graph = &wrapper->frame->graph;
	graph_wrapper->is_owned = false;
	luaL_getmetatable(L, FOLD_GRAPH_METATABLE);
	lua_setmetatable(L, -2);
	return 1;
}

int l_fold_frame_get_parent_meta(lua_State* L) {
	LuaFoldFrame* wrapper = luaL_checkudata(L, 1, FOLD_FRAME_METATABLE);
	if (wrapper->frame->parent != FOLD_FRAME_PARENT_NONE) {
		lua_pushinteger(L, wrapper->frame->parent);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int l_fold_frame_get_inherit_meta(lua_State* L) {
	LuaFoldFrame* wrapper = luaL_checkudata(L, 1, FOLD_FRAME_METATABLE);
	lua_pushboolean(L, wrapper->frame->inherit);
	return 1;
}

/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */

int l_fold_frame_copy_meta(lua_State* L) {
	LuaFoldFrame* wrapper = luaL_checkudata(L, 1, FOLD_FRAME_METATABLE);
	LuaFoldFrame* source_wrapper = luaL_checkudata(L, 2, FOLD_FRAME_METATABLE);

	Error result = fold_frame_copy(wrapper->frame, source_wrapper->frame);
	if IS_ERROR(result) {
		lua_pushboolean(L, false);
		lua_pushstring(L, ERROR_STRING(result));
		return 2;
	}

	lua_pushboolean(L, true);
	lua_pushnil(L);
	return 2;
}

int l_fold_frame_inherit_meta(lua_State* L) {
	LuaFoldFrame* wrapper = luaL_checkudata(L, 1, FOLD_FRAME_METATABLE);
	LuaFoldFrame* source_wrapper = luaL_checkudata(L, 2, FOLD_FRAME_METATABLE);
	fold_frame_inherit(wrapper->frame, source_wrapper->frame);
	return 0;
}

int l_fold_frame_is_inherited_meta(lua_State* L) {
	LuaFoldFrame* wrapper = luaL_checkudata(L, 1, FOLD_FRAME_METATABLE);
	lua_pushboolean(L, fold_frame_is_inherited(wrapper->frame));
	return 1;
}

/* ========================================================================= */
/* Attributes                                                                */
/* ========================================================================= */

int l_fold_frame_attributes_assign_meta(lua_State* L) {
	LuaFoldFrame* wrapper = luaL_checkudata(L, 1, FOLD_FRAME_METATABLE);
	Error result = fold_frame_attributes_assign(wrapper->frame);
	lua_pushboolean(L, IS_OK(result));
	return 1;
}
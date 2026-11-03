#include "lua_modules/fold/file.h"

/* ========================================================================= */
/* Creation & Destruction                                                    */
/* ========================================================================= */

int l_fold_file_new(lua_State* L) {
	FoldFile* file = lua_newuserdata(L, sizeof(FoldFile));
	luaL_getmetatable(L, FOLD_FILE_METATABLE);
	lua_setmetatable(L, -2);
	TRY_OR_ELSE(fold_file_create(file),
		lua_pushnil(L));
	return 1;
}

int l_fold_file___index_meta(lua_State* L) {
	luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	luaL_getmetatable(L, FOLD_FILE_METATABLE);
	lua_pushvalue(L, 2);
	lua_gettable(L, -2);
	return 1;
}

int l_fold_file___gc_meta(lua_State* L) {
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	fold_file_destroy(file);
	return 0;
}

/* ========================================================================= */
/* Getters                                                                   */
/* ========================================================================= */

#define GENERATE_GET_STRING(property) \
int l_fold_file_get_##property##_meta(lua_State* L) { \
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE); \
	lua_pushlstring(L, file->property.data, file->property.length); \
	return 1; \
}

int l_fold_file_get_version_meta(lua_State* L) {
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	lua_pushnumber(L, file->version);
	return 1;
}

GENERATE_GET_STRING(creator)
GENERATE_GET_STRING(author)
GENERATE_GET_STRING(title)
GENERATE_GET_STRING(description)

int l_fold_file_get_classes_meta(lua_State* L) {
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	lua_createtable(L, file->classes.size, 0);
	ARRAY_FOR_EACH(&file->classes, index, char*, class) {
		lua_pushstring(L, class);
		lua_rawseti(L, -2, index + 1);
	}
	return 1;
}

int l_fold_file_get_frames_meta(lua_State* L) {
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	lua_createtable(L, file->frames.size, 0);
	ARRAY_FOR_EACH(&file->frames, index, FoldFrame*, frame) {
		lua_pushlightuserdata(L, frame);
		lua_rawseti(L, -2, index + 1);
	}
	return 1;
}

/* ========================================================================= */
/* Frame Getters                                                             */
/* ========================================================================= */

#define GENERATE_GET_FRAME_STRING(property) \
int l_fold_file_get_frame_##property##_meta(lua_State* L) { \
	luaL_checkudata(L, 1, FOLD_FILE_METATABLE); \
	luaL_checktype(L, 2, LUA_TLIGHTUSERDATA); \
	FoldFrame* frame = (FoldFrame*)lua_touserdata(L, 2); \
	lua_pushlstring(L, frame->property.data, frame->property.length); \
	return 1; \
}

#define GENERATE_GET_FRAME_STRING_ARRAY(property) \
int l_fold_file_get_frame_##property##_meta(lua_State* L) { \
	luaL_checkudata(L, 1, FOLD_FILE_METATABLE); \
	luaL_checktype(L, 2, LUA_TLIGHTUSERDATA); \
	FoldFrame* frame = (FoldFrame*)lua_touserdata(L, 2); \
	lua_createtable(L, frame->property.size, 0); \
	ARRAY_FOR_EACH(&frame->property, index, char*, string) { \
		lua_pushstring(L, string); \
		lua_rawseti(L, -2, index + 1); \
	} \
	return 1; \
}

GENERATE_GET_FRAME_STRING(author)
GENERATE_GET_FRAME_STRING(title)
GENERATE_GET_FRAME_STRING(description)
GENERATE_GET_FRAME_STRING_ARRAY(classes)
GENERATE_GET_FRAME_STRING_ARRAY(attributes)
GENERATE_GET_FRAME_STRING(unit)

int l_fold_file_get_frame_graph_meta(lua_State* L) {
	luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	luaL_checktype(L, 2, LUA_TLIGHTUSERDATA);
	FoldFrame* frame = (FoldFrame*)lua_touserdata(L, 2);
	lua_pushlightuserdata(L, &frame->graph);
	return 1;
}

int l_fold_file_get_frame_parent_meta(lua_State* L) {
	luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	luaL_checktype(L, 2, LUA_TLIGHTUSERDATA);
	FoldFrame* frame = (FoldFrame*)lua_touserdata(L, 2);
	if (frame->parent != FOLD_FRAME_PARENT_NONE) {
		lua_pushinteger(L, frame->parent);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int l_fold_file_get_frame_inherit_meta(lua_State* L) {
	luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	luaL_checktype(L, 2, LUA_TLIGHTUSERDATA);
	FoldFrame* frame = (FoldFrame*)lua_touserdata(L, 2);
	lua_pushboolean(L, frame->inherit);
	return 1;
}

/* ========================================================================= */
/* Graph Getters                                                             */
/* ========================================================================= */

#define GENERATE_GET_FRAME_GRAPH_ARRAY_SIZE(array) \
int l_fold_file_get_frame_graph_##array##_size_meta(lua_State* L) { \
	luaL_checkudata(L, 1, FOLD_FILE_METATABLE); \
	luaL_checktype(L, 2, LUA_TLIGHTUSERDATA); \
	FoldGraph* graph = (FoldGraph*)lua_touserdata(L, 2); \
	lua_pushinteger(L, graph->array.size); \
	return 1; \
}

GENERATE_GET_FRAME_GRAPH_ARRAY_SIZE(VC)
GENERATE_GET_FRAME_GRAPH_ARRAY_SIZE(VV)
GENERATE_GET_FRAME_GRAPH_ARRAY_SIZE(VE)
GENERATE_GET_FRAME_GRAPH_ARRAY_SIZE(VF)
GENERATE_GET_FRAME_GRAPH_ARRAY_SIZE(EV)
GENERATE_GET_FRAME_GRAPH_ARRAY_SIZE(EF)
GENERATE_GET_FRAME_GRAPH_ARRAY_SIZE(EA)
GENERATE_GET_FRAME_GRAPH_ARRAY_SIZE(EFA)
GENERATE_GET_FRAME_GRAPH_ARRAY_SIZE(EL)
GENERATE_GET_FRAME_GRAPH_ARRAY_SIZE(EO)
GENERATE_GET_FRAME_GRAPH_ARRAY_SIZE(FV)
GENERATE_GET_FRAME_GRAPH_ARRAY_SIZE(FE)
GENERATE_GET_FRAME_GRAPH_ARRAY_SIZE(FF)
GENERATE_GET_FRAME_GRAPH_ARRAY_SIZE(FO)

GENERATE_GET_FRAME_GRAPH_ARRAY_SIZE(PC)
GENERATE_GET_FRAME_GRAPH_ARRAY_SIZE(TC)
GENERATE_GET_FRAME_GRAPH_ARRAY_SIZE(T2C)
GENERATE_GET_FRAME_GRAPH_ARRAY_SIZE(NC)
GENERATE_GET_FRAME_GRAPH_ARRAY_SIZE(MN)
GENERATE_GET_FRAME_GRAPH_ARRAY_SIZE(VP)
GENERATE_GET_FRAME_GRAPH_ARRAY_SIZE(FT)
GENERATE_GET_FRAME_GRAPH_ARRAY_SIZE(FT2)
GENERATE_GET_FRAME_GRAPH_ARRAY_SIZE(FN)
GENERATE_GET_FRAME_GRAPH_ARRAY_SIZE(FM)
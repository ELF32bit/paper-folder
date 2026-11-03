#include "lua_modules/fold/file.h"
#include "lua_modules/fold/frame.h"

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
		LuaFoldFrame* wrapper = lua_newuserdata(L, sizeof(LuaFoldFrame));
		wrapper->frame = frame;
		wrapper->is_owned = false;
		luaL_getmetatable(L, FOLD_FRAME_METATABLE);
		lua_setmetatable(L, -2);
		lua_rawseti(L, -2, index + 1);
	}
	return 1;
}

int l_fold_file_get_frames_size_meta(lua_State* L) {
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	lua_pushinteger(L, file->frames.size);
	return 1;
}

int l_fold_file_get_key_frame_meta(lua_State* L) {
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	LuaFoldFrame* wrapper = lua_newuserdata(L, sizeof(LuaFoldFrame));
	wrapper->frame = fold_file_get_key_frame(file);
	wrapper->is_owned = false;
	luaL_getmetatable(L, FOLD_FRAME_METATABLE);
	lua_setmetatable(L, -2);
	return 1;
}

/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */

int l_fold_file_copy_meta(lua_State* L) {
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	FoldFile* source = luaL_checkudata(L, 2, FOLD_FILE_METATABLE);

	Error result = fold_file_copy(file, source);
	if IS_ERROR(result) {
		lua_pushboolean(L, false);
		lua_pushstring(L, ERROR_STRING(result));
		return 2;
	}

	lua_pushboolean(L, true);
	lua_pushnil(L);
	return 2;
}

int l_fold_file_frame_inherit_meta(lua_State* L) {
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	usize frame_index = luaL_checkinteger(L, 2);
	LuaFoldFrame* wrapper = lua_newuserdata(L, sizeof(LuaFoldFrame));
	wrapper->frame = fold_file_frame_inherit(file, frame_index);
	wrapper->is_owned = false;
	luaL_getmetatable(L, FOLD_FRAME_METATABLE);
	lua_setmetatable(L, -2);
	return 1;
}

int l_fold_file_frames_inherit_meta(lua_State* L) {
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	fold_file_frames_inherit(file);
	return 0;
}
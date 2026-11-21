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

int l_fold_file_get_frame_meta(lua_State* L) {
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	usize frame_index = USIZE(luaL_checkinteger(L, 2));
	if NOT(frame_index < file->frames.size) {
		lua_pushnil(L);
		return 1;
	}

	LuaFoldFrame* wrapper = lua_newuserdata(L, sizeof(LuaFoldFrame));
	wrapper->frame = array_get(&file->frames, frame_index);
	wrapper->is_owned = false;
	luaL_getmetatable(L, FOLD_FRAME_METATABLE);
	lua_setmetatable(L, -2);
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
/* Setters                                                                   */
/* ========================================================================= */

#define GENERATE_SET_STRING(property) \
int l_fold_file_set_##property##_meta(lua_State* L) { \
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE); \
	const char* property = luaL_checkstring(L, 2); \
\
	Error result = string_copy_raw(&file->property, property); \
	if IS_ERROR(result) { \
		lua_pushboolean(L, false); \
		lua_pushstring(L, ERROR_STRING(result)); \
		return 2; \
	} \
\
	lua_pushboolean(L, true); \
	lua_pushnil(L); \
	return 2; \
}

int l_fold_file_set_version_meta(lua_State* L) {
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	file->version = luaL_checknumber(L, 2);
	return 0;
}

GENERATE_SET_STRING(creator)
GENERATE_SET_STRING(author)
GENERATE_SET_STRING(title)
GENERATE_SET_STRING(description)

int l_fold_file_set_classes_meta(lua_State* L) {
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	luaL_checktype(L, 2, LUA_TTABLE);

	array_recreate(&file->classes);
	FOR_EACH(index, lua_rawlen(L, 2)) {
		lua_rawgeti(L, 2, TRY_SAFE(index + 1));
		const char* class = luaL_checkstring(L, -1);

		STRING_BUFFER_RAW(class, buffer,
			file->classes.element_size);

		Error result = array_append(&file->classes, &buffer);
		if IS_ERROR(result) {
			lua_pop(L, 1);
			lua_pushboolean(L, false);
			lua_pushstring(L, ERROR_STRING(result));
			return 2;
		}

		lua_pop(L, 1);
	}

	lua_pushboolean(L, true);
	lua_pushnil(L);
	return 2;
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
	usize frame_index = USIZE(luaL_checkinteger(L, 2));
	if NOT(frame_index < file->frames.size) {
		lua_pushnil(L);
		return 1;
	}

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
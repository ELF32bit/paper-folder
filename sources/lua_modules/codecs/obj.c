#include "lua_modules/fold/file.h"

#include "codecs/obj.h"

/* ========================================================================= */
/* FOLD Serialization & Deserialization                                      */
/* ========================================================================= */

#define OBJ_READ_ERROR "failed to read OBJ file"
#define OBJ_WRITE_ERROR "failed to write OBJ file"

int l_fold_file_from_obj_file_meta(lua_State* L) {
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	const char* input_file_path = luaL_checkstring(L, 2);

	Error result = fold_file_from_obj(file, (void*)input_file_path);
	if IS_ERROR(result) {
		lua_pushnil(L);
		lua_pushstring(L, ERROR_STRING_OR( \
			result, OBJ_READ_ERROR)); \
		return 2;
	}

	lua_pushvalue(L, 1);
	lua_pushnil(L);
	return 2;
}

int l_fold_file_to_obj_file_meta(lua_State* L) {
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	const char* output_file_path = luaL_optstring(L, 2, NULL);

	Error result = fold_file_to_obj(file, (void*)output_file_path);
	if IS_ERROR(result) {
		lua_pushnil(L);
		lua_pushstring(L, ERROR_STRING_OR( \
			result, OBJ_WRITE_ERROR)); \
		return 2;
	}

	lua_pushvalue(L, 1);
	lua_pushnil(L);
	return 2;
}
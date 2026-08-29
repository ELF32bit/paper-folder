#include "lua_modules/fold/file.h"

#include "codecs/svg.h"

/* ========================================================================= */
/* FOLD Serialization & Deserialization                                      */
/* ========================================================================= */

#define SVG_READ_ERROR "failed to read SVG file"
#define SVG_WRITE_ERROR "failed to write SVG file"

int l_fold_file_frame_from_svg_file_meta(lua_State* L) {
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	usize frame_index = luaL_checkinteger(L, 2);
	const char* input_file_path = luaL_checkstring(L, 3);

	Error result = fold_file_frame_from_svg(file, frame_index,
		(void*)input_file_path);
	if IS_ERROR(result) {
		lua_pushnil(L);
		lua_pushstring(L, ERROR_STRING_OR( \
			result, SVG_READ_ERROR)); \
		return 2;
	}

	lua_pushvalue(L, 1);
	lua_pushnil(L);
	return 2;
}

int l_fold_file_frame_to_svg_file_meta(lua_State* L) {
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	usize frame_index = luaL_checkinteger(L, 2);
	const char* output_file_path = luaL_optstring(L, 3, NULL);

	Error result = fold_file_frame_to_svg(file, frame_index,
		(void*)output_file_path);
	if IS_ERROR(result) {
		lua_pushnil(L);
		lua_pushstring(L, ERROR_STRING_OR( \
			result, SVG_WRITE_ERROR)); \
		return 2;
	}

	lua_pushvalue(L, 1);
	lua_pushnil(L);
	return 2;
}
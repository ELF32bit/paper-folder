#include "../file.h"
#include "../frame.h"

#include "codecs/svg.h"

/* ========================================================================= */
/* Serialization & Deserialization                                           */
/* ========================================================================= */

#define SVG_READ_ERROR "failed to read SVG file"
#define SVG_WRITE_ERROR "failed to write SVG file"

int l_fold_file_from_svg_file_meta(lua_State* L) {
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	const char* path = luaL_checkstring(L, 2);

	Error result = fold_file_from_svg(file, (void*)path);
	if IS_ERROR(result) {
		lua_pushnil(L);
		lua_pushstring(L, SVG_READ_ERROR);
		return 2;
	}

	lua_pushvalue(L, 1);
	lua_pushnil(L);
	return 2;
}

int l_fold_file_to_svg_file_meta(lua_State* L) {
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	const char* path = luaL_optstring(L, 2, NULL);

	Error result = fold_file_to_svg(file, (void*)path);
	if IS_ERROR(result) {
		lua_pushnil(L);
		lua_pushstring(L, SVG_WRITE_ERROR);
		return 2;
	}

	lua_pushvalue(L, 1);
	lua_pushnil(L);
	return 2;
}

int l_fold_frame_from_svg_file_meta(lua_State* L) {
	FoldFrame* frame = luaL_checkudata(L, 1, FOLD_FRAME_METATABLE);
	const char* path = luaL_checkstring(L, 2);

	Error result = fold_frame_from_svg(frame, (void*)path);
	if IS_ERROR(result) {
		lua_pushnil(L);
		lua_pushstring(L, SVG_READ_ERROR);
		return 2;
	}

	lua_pushvalue(L, 1);
	lua_pushnil(L);
	return 2;
}

int l_fold_frame_to_svg_file_meta(lua_State* L) {
	FoldFrame* frame = luaL_checkudata(L, 1, FOLD_FRAME_METATABLE);
	const char* path = luaL_optstring(L, 2, NULL);

	Error result = fold_frame_to_svg(frame, (void*)path);
	if IS_ERROR(result) {
		lua_pushnil(L);
		lua_pushstring(L, SVG_WRITE_ERROR);
		return 2;
	}

	lua_pushvalue(L, 1);
	lua_pushnil(L);
	return 2;
}
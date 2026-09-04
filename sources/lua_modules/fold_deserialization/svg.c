#include "lua_modules/fold/file.h"

#include "fold_deserialization/svg.h"

#define SVG_READ_ERROR "failed to read SVG file"

/* ========================================================================= */
/* FOLD File                                                                 */
/* ========================================================================= */

int l_fold_file_frame_from_svg_file_meta(lua_State* L) {
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	const char* input_file_path = luaL_checkstring(L, 2);
	usize frame_index = luaL_optinteger(L, 3, 0);

	Error result = fold_file_frame_from_svg(file, frame_index,
		(void*)input_file_path);
	if IS_ERROR(result) {
		lua_pushnil(L);
		lua_pushstring(L, ERROR_STRING_OR(
			result, SVG_READ_ERROR));
		return 2;
	}

	lua_pushvalue(L, 1);
	lua_pushnil(L);
	return 2;
}
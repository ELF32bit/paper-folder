#include "lua_modules/fold/file.h"

#include "fold_serialization/svg.h"

#define SVG_WRITE_ERROR "failed to write SVG file"

/* ========================================================================= */
/* FOLD File                                                                 */
/* ========================================================================= */

int l_fold_file_frame_to_svg_file_meta(lua_State* L) {
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	const char* output_file_path = luaL_optstring(L, 2, NULL);
	usize frame_index = luaL_optinteger(L, 3, 0);

	Error result = fold_file_frame_to_svg(file, frame_index,
		(void*)output_file_path);
	if IS_ERROR(result) {
		lua_pushboolean(L, false);
		lua_pushstring(L, ERROR_STRING_OR(
			result, SVG_WRITE_ERROR));
		return 2;
	}

	lua_pushboolean(L, true);
	lua_pushnil(L);
	return 2;
}
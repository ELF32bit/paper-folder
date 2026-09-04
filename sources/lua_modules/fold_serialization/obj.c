#include "lua_modules/fold/file.h"

#include "fold_serialization/obj.h"

#define OBJ_WRITE_ERROR "failed to write OBJ file"

/* ========================================================================= */
/* FOLD File                                                                 */
/* ========================================================================= */

int l_fold_file_to_obj_file_meta(lua_State* L) {
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	const char* output_file_path = luaL_optstring(L, 2, NULL);

	Error result = fold_file_to_obj(file,
		(void*)output_file_path);
	if IS_ERROR(result) {
		lua_pushboolean(L, false);
		lua_pushstring(L, ERROR_STRING_OR(
			result, OBJ_WRITE_ERROR));
		return 2;
	}

	lua_pushboolean(L, true);
	lua_pushnil(L);
	return 2;
}
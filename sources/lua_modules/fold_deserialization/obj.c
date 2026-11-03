#include "lua_modules/fold/file.h"

#include "fold_deserialization/obj.h"

#define OBJ_READ_ERROR "failed to read OBJ file"

/* ========================================================================= */
/* FOLD File                                                                 */
/* ========================================================================= */

int l_fold_file_from_obj_file_meta(lua_State* L) {
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	const char* input_file_path = luaL_checkstring(L, 2);

	struct FoldFileFromOBJParameters parameters;
	parameters.triangulate = lua_toboolean(L, 3);

	Error result = fold_file_from_obj(file,
		(void*)input_file_path, &parameters);

	if IS_ERROR(result) {
		lua_pushnil(L);
		lua_pushstring(L, ERROR_STRING_OR(
			result, OBJ_READ_ERROR));
		return 2;
	}

	lua_pushvalue(L, 1);
	lua_pushnil(L);
	return 2;
}
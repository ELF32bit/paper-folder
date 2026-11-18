#include "lua_modules/fold/file.h"

#include "fold_serialization/obj.h"

#define OBJ_WRITE_ERROR "failed to write OBJ file"

/* ========================================================================= */
/* FOLD File                                                                 */
/* ========================================================================= */

int l_fold_file_to_obj_file_meta(lua_State* L) {
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	const char* output_file_path = luaL_optstring(L, 2, NULL);

	struct FoldFileToOBJParameters parameters;
	parameters.scale = luaL_optnumber(L, 3, 1.0);
	parameters.abstract_seed = luaL_optinteger(L, 4, 0);
	parameters.enable_groups = lua_toboolean(L, 5);
	parameters.disable_colors = lua_toboolean(L, 6);
	parameters.enable_weights = lua_toboolean(L, 7);
	parameters.disable_textures = lua_toboolean(L, 8);
	parameters.disable_normals = lua_toboolean(L, 9);
	parameters.disable_materials = lua_toboolean(L, 10);
	parameters.disable_lines = lua_toboolean(L, 11);
	parameters.disable_faces = lua_toboolean(L, 12);

	Error result = fold_file_to_obj(file,
		(void*)output_file_path, &parameters);

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
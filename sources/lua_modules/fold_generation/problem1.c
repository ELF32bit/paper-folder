#include "lua_modules/fold/generation.h"

#include "fold_generation/problems.h"

#define GENERATION_ERROR "failed to generate FOLD file"

int l_fold_generation_locally_flat_foldable(lua_State* L) {
	struct LFFParameters parameters;
	parameters.min_boundary_vertices = (usize)luaL_optinteger(L, 1, 4);
	parameters.max_boundary_vertices = (usize)luaL_optinteger(L, 2, 4);
	parameters.min_interior_vertices = (usize)luaL_optinteger(L, 3, 0);
	parameters.max_interior_vertices = (usize)luaL_optinteger(L, 4, 0);
	parameters.boundary_offset = (real)luaL_optnumber(L, 5, 0.015625);
	parameters.cell_size = (real)luaL_optnumber(L, 6, 0.015625);
	parameters.kawasaki_error = (real)luaL_optnumber(L, 7, 1.0);
	parameters.seed = (usize)luaL_optinteger(L, 8, 0);

	FoldFile* file = lua_newuserdata(L, sizeof(FoldFile));
	luaL_getmetatable(L, FOLD_FILE_METATABLE);
	lua_setmetatable(L, -2);

	TRY_OR_ELSE(fold_file_create(file),
		lua_pushnil(L);
		lua_pushnil(L);
		return 2);

	Error result = fold_generate_locally_flat_foldable(file, &parameters);
	if IS_ERROR(result) {
		fold_file_destroy(file);
		lua_pushnil(L);
		lua_pushstring(L, ERROR_STRING_OR(
			result, GENERATION_ERROR));
		return 2;
	}

	lua_pushnil(L);
	return 2;
}
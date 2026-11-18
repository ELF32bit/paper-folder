#include "lua_modules/fold/file.h"

#include "fold_deserialization/obj.h"

#define OBJ_READ_ERROR "failed to read OBJ file"

/* ========================================================================= */
/* FOLD File                                                                 */
/* ========================================================================= */

static inline
void _fold_file_from_obj_parameters_create(lua_State* L,
	struct FoldFileFromOBJParameters* parameters)
{
	parameters->scale = luaL_optnumber(L, 3, 1.0);
	parameters->sort_objects = lua_toboolean(L, 4);
	parameters->disable_objects = lua_toboolean(L, 5);
	parameters->disable_groups = lua_toboolean(L, 6);
	parameters->disable_colors = lua_toboolean(L, 7);
	parameters->enable_weights = lua_toboolean(L, 8);
	parameters->disable_textures = lua_toboolean(L, 9);
	parameters->disable_normals = lua_toboolean(L, 10);
	parameters->disable_materials = lua_toboolean(L, 11);
	parameters->disable_lines = lua_toboolean(L, 12);
	parameters->disable_faces = lua_toboolean(L, 13);
}

int l_fold_file_from_obj_file_meta(lua_State* L) {
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	const char* input_file_path = luaL_checkstring(L, 2);

	String input_string;
	string_create(&input_string);
	Error result = string_from_file(
		&input_string, input_file_path);

	if IS_ERROR(result) {
		string_destroy(&input_string);
		lua_pushnil(L);
		lua_pushstring(L, ERROR_STRING_OR(
			result, OBJ_READ_ERROR));
		return 2;
	}

	struct FoldFileFromOBJParameters parameters;
	_fold_file_from_obj_parameters_create(L, &parameters);
	result = fold_file_from_obj(file, (void*)
		(&input_string), &parameters);
	string_destroy(&input_string);

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

int l_fold_file_from_obj_string_meta(lua_State* L) {
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE);

	String input_string;
	input_string.data = TRY_SAFE((char*)
		luaL_checklstring(L, 2, &input_string.length));

	struct FoldFileFromOBJParameters parameters;
	_fold_file_from_obj_parameters_create(L, &parameters);
	Error result = fold_file_from_obj(file, (void*)
		(&input_string), &parameters);

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
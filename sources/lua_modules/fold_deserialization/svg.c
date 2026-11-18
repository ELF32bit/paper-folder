#include "lua_modules/fold/file.h"

#include "fold_deserialization/svg.h"

#include <nanosvg.h>

#define SVG_READ_ERROR "failed to read SVG file"

/* ========================================================================= */
/* FOLD File                                                                 */
/* ========================================================================= */

static inline
void _fold_file_frame_from_svg_parameters_create(lua_State* L,
	struct FoldFileFrameFromSVGParameters* parameters)
{
	parameters->flatness = luaL_checknumber(L, 4);
	parameters->min_segment_length = luaL_checknumber(L, 5);
}

int l_fold_file_frame_from_svg_file_meta(lua_State* L) {
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	const char* input_file_path = luaL_checkstring(L, 2);
	usize frame_index = luaL_optinteger(L, 3, 0);

	NSVGimage* image =
		nsvgParseFromFile(input_file_path,
			"px", 96.0f);

	if (image == NULL) {
		lua_pushnil(L);
		lua_pushstring(L, SVG_READ_ERROR);
	}

	struct FoldFileFrameFromSVGParameters parameters;
	_fold_file_frame_from_svg_parameters_create(L, &parameters);
	Error result = fold_file_frame_from_svg(file, frame_index,
		(void*)image, &parameters);
	nsvgDelete(image);

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

int l_fold_file_frame_from_svg_string_meta(lua_State* L) {
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	usize frame_index = luaL_optinteger(L, 3, 0);

	String input_string;
	input_string.data = TRY_SAFE((char*)
		luaL_checklstring(L, 2, &input_string.length));

	String input_string_copy;
	string_create(&input_string_copy);
	Error result = string_copy(&input_string_copy,
		&input_string);

	if IS_ERROR(result) {
		string_destroy(&input_string_copy);
		lua_pushnil(L);
		lua_pushstring(L, ERROR_STRING_OR(
			result, SVG_READ_ERROR));
		return 2;
	}

	NSVGimage* image =
		nsvgParse(input_string_copy.data,
			"px", 96.0f);
	string_destroy(&input_string_copy);

	if (image == NULL) {
		lua_pushnil(L);
		lua_pushstring(L, SVG_READ_ERROR);
	}

	struct FoldFileFrameFromSVGParameters parameters;
	_fold_file_frame_from_svg_parameters_create(L, &parameters);
	result = fold_file_frame_from_svg(file, frame_index,
		(void*)image, &parameters);
	nsvgDelete(image);

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
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
	parameters->image_units =
		TRY_SAFE((char*)luaL_optstring(L, 4, parameters->image_units));
	parameters->image_dpi = luaL_optnumber(L, 5, parameters->image_dpi);

	parameters->flatness = luaL_checknumber(L, 6);
	parameters->min_segment_length = luaL_checknumber(L, 7);
}

int l_fold_file_frame_from_svg_file_meta(lua_State* L) {
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	const char* input_file_path = luaL_checkstring(L, 2);
	usize frame_index = USIZE(luaL_optinteger(L, 3, 0));
	if NOT(frame_index < file->frames.size) {
		lua_pushnil(L);
		lua_pushnil(L);
		return 2;
	}

	struct FoldFileFrameFromSVGParameters parameters;
	fold_file_frame_from_svg_parameters_default(&parameters);
	_fold_file_frame_from_svg_parameters_create(L, &parameters);

	NSVGimage* image =
		nsvgParseFromFile(input_file_path,
			parameters.image_units,
			parameters.image_dpi);

	if (image == NULL) {
		lua_pushnil(L);
		lua_pushstring(L, SVG_READ_ERROR);
	}

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
	usize frame_index = USIZE(luaL_optinteger(L, 3, 0));
	if NOT(frame_index < file->frames.size) {
		lua_pushnil(L);
		lua_pushnil(L);
		return 2;
	}

	String input_string;
	input_string.data = TRY_SAFE((char*)
		luaL_checklstring(L, 2, &input_string.length));

	String mutable_string;
	string_create(&mutable_string);
	Error result = string_copy(&mutable_string,
		&input_string);

	if IS_ERROR(result) {
		string_destroy(&mutable_string);
		lua_pushnil(L);
		lua_pushstring(L, ERROR_STRING_OR(
			result, SVG_READ_ERROR));
		return 2;
	}

	struct FoldFileFrameFromSVGParameters parameters;
	fold_file_frame_from_svg_parameters_default(&parameters);
	_fold_file_frame_from_svg_parameters_create(L, &parameters);

	NSVGimage* image =
		nsvgParse(mutable_string.data,
			parameters.image_units,
			parameters.image_dpi);

	string_destroy(&mutable_string);
	if (image == NULL) {
		lua_pushnil(L);
		lua_pushstring(L, SVG_READ_ERROR);
	}

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
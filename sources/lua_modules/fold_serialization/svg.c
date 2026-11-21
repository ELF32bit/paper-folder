#include "lua_modules/fold/file.h"

#include "fold_serialization/svg.h"

#define SVG_WRITE_ERROR "failed to write SVG file"

/* ========================================================================= */
/* FOLD File                                                                 */
/* ========================================================================= */

int l_fold_file_frame_to_svg_file_meta(lua_State* L) {
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	const char* output_file_path = luaL_optstring(L, 2, NULL);
	usize frame_index = USIZE(luaL_optinteger(L, 3, 0));
	if NOT(frame_index < file->frames.size) {
		lua_pushboolean(L, false);
		lua_pushnil(L);
		return 2;
	}

	struct FoldFileFrameToSVGParameters parameters;
	fold_file_frame_to_svg_parameters_default(&parameters);

	parameters.scale = luaL_optnumber(L, 4, parameters.scale);
	parameters.abstract_seed = luaL_optinteger(L, 5, parameters.abstract_seed);

	const char* B_color = luaL_optstring(L, 6, NULL);
	const char* M_color = luaL_optstring(L, 7, NULL);
	const char* V_color = luaL_optstring(L, 8, NULL);
	const char* F_color = luaL_optstring(L, 9, NULL);
	const char* U_color = luaL_optstring(L, 10, NULL);
	const char* C_color = luaL_optstring(L, 11, NULL);
	const char* J_color = luaL_optstring(L, 12, NULL);

	if (B_color != NULL) parameters.B_color = color_from_html(B_color);
	if (M_color != NULL) parameters.M_color = color_from_html(M_color);
	if (V_color != NULL) parameters.V_color = color_from_html(V_color);
	if (F_color != NULL) parameters.F_color = color_from_html(F_color);
	if (U_color != NULL) parameters.U_color = color_from_html(U_color);
	if (C_color != NULL) parameters.C_color = color_from_html(C_color);
	if (J_color != NULL) parameters.J_color = color_from_html(J_color);

	parameters.B_scale = luaL_optnumber(L, 13, parameters.B_scale);
	parameters.M_scale = luaL_optnumber(L, 14, parameters.M_scale);
	parameters.V_scale = luaL_optnumber(L, 15, parameters.V_scale);
	parameters.F_scale = luaL_optnumber(L, 16, parameters.F_scale);
	parameters.U_scale = luaL_optnumber(L, 17, parameters.U_scale);
	parameters.C_scale = luaL_optnumber(L, 18, parameters.C_scale);
	parameters.J_scale = luaL_optnumber(L, 19, parameters.J_scale);

	parameters.B_order = luaL_optinteger(L, 20, parameters.B_order);
	parameters.M_order = luaL_optinteger(L, 21, parameters.M_order);
	parameters.V_order = luaL_optinteger(L, 22, parameters.V_order);
	parameters.F_order = luaL_optinteger(L, 23, parameters.F_order);
	parameters.U_order = luaL_optinteger(L, 24, parameters.U_order);
	parameters.C_order = luaL_optinteger(L, 25, parameters.C_order);
	parameters.J_order = luaL_optinteger(L, 26, parameters.J_order);

	parameters.image_min_size = luaL_optnumber(L, 27, parameters.image_min_size);
	parameters.image_grow_size = luaL_optnumber(L, 28, parameters.image_grow_size);
	parameters.stroke_width = luaL_optnumber(L, 29, parameters.stroke_width);

	Error result = fold_file_frame_to_svg(file, frame_index,
		(void*)output_file_path, &parameters);

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
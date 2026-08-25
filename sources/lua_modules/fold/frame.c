#include "frame.h"

/* ========================================================================= */
/* Attributes                                                                */
/* ========================================================================= */

int l_fold_frame_attributes_compute_meta(lua_State* L) {
	FoldFrame* frame = luaL_checkudata(L, 1, FOLD_FRAME_METATABLE);
	Error result = fold_frame_attributes_compute(frame);
	if IS_OK(result) {
		lua_pushboolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}
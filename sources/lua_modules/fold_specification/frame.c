#include "lua_modules/fold/frame.h"

/* ========================================================================= */
/* Creation & Destruction                                                    */
/* ========================================================================= */

int l_fold_frame_new(lua_State* L) {
	FoldFrame* frame = lua_newuserdata(L, sizeof(FoldFrame));
	luaL_getmetatable(L, FOLD_FRAME_METATABLE);
	lua_setmetatable(L, -2);
	fold_frame_create(frame);
	return 1;
}

int l_fold_frame___index_meta(lua_State* L) {
	luaL_checkudata(L, 1, FOLD_FRAME_METATABLE);
	luaL_getmetatable(L, FOLD_FRAME_METATABLE);
	lua_pushvalue(L, 2);
	lua_gettable(L, -2);
	return 1;
}

int l_fold_frame___gc_meta(lua_State* L) {
	FoldFrame* frame = luaL_checkudata(L, 1, FOLD_FRAME_METATABLE);
	fold_frame_destroy(frame);
	return 0;
}

/* ========================================================================= */
/* Attributes                                                                */
/* ========================================================================= */

int l_fold_frame_attributes_assign_meta(lua_State* L) {
	FoldFrame* frame = luaL_checkudata(L, 1, FOLD_FRAME_METATABLE);
	Error result = fold_frame_attributes_assign(frame);
	if IS_OK(result) {
		lua_pushboolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}
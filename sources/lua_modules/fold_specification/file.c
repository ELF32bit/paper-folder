#include "lua_modules/fold/file.h"

/* ========================================================================= */
/* Creation & Destruction                                                    */
/* ========================================================================= */

int l_fold_file_new(lua_State* L) {
	FoldFile* file = lua_newuserdata(L, sizeof(FoldFile));
	luaL_getmetatable(L, FOLD_FILE_METATABLE);
	lua_setmetatable(L, -2);
	TRY_OR_ELSE(fold_file_create(file),
		lua_pushnil(L));
	return 1;
}

int l_fold_file___index_meta(lua_State* L) {
	luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	luaL_getmetatable(L, FOLD_FILE_METATABLE);
	lua_pushvalue(L, 2);
	lua_gettable(L, -2);
	return 1;
}

int l_fold_file___gc_meta(lua_State* L) {
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	fold_file_destroy(file);
	return 0;
}
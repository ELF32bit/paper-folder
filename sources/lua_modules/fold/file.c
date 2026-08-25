#include "file.h"

#include "codecs/obj.h"
#include "codecs/svg.h"

#define OBJ_READ_ERROR "failed to read OBJ file"
#define OBJ_WRITE_ERROR "failed to write OBJ file"

int l_fold_file_from_obj_file_meta(lua_State* L) {
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	const char* path = luaL_checkstring(L, 2);

	Error result = fold_file_from_obj(file, (void*)path);
	if IS_ERROR(result) {
		fold_file_destroy(file);
		lua_pushnil(L);
		lua_pushstring(L, OBJ_READ_ERROR);
		return 2;
	}

	lua_pushvalue(L, 1);
	lua_pushnil(L);
	return 2;
}

int l_fold_file_to_obj_file_meta(lua_State* L) {
	FoldFile* file = luaL_checkudata(L, 1, FOLD_FILE_METATABLE);
	const char* path = luaL_checkstring(L, 2);

	Error result = fold_file_to_obj(file, (void*)path);
	if IS_ERROR(result) {
		lua_pushnil(L);
		lua_pushstring(L, OBJ_WRITE_ERROR);
		return 2;
	}

	lua_pushvalue(L, 1);
	lua_pushnil(L);
	return 2;
}
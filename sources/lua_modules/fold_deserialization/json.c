#include "lua_modules/fold/file.h"
#include "lua_modules/fold/frame.h"
#include "lua_modules/fold/graph.h"

#include "fold_deserialization/json.h"

#include <yyjson.h>

#define JSON_READ_FLAGS \
	YYJSON_READ_ALLOW_COMMENTS | \
	YYJSON_READ_ALLOW_EXT_NUMBER | \
	YYJSON_READ_ALLOW_SINGLE_QUOTED_STR | \
	YYJSON_READ_ALLOW_TRAILING_COMMAS

#define JSON_READ_ERROR "failed to read FOLD file"

#define GENERATE_FUNCTIONS(Fold, fold, METATABLE) \
int l_fold_##fold##_from_json_file_meta(lua_State* L) { \
	Fold* fold = luaL_checkudata(L, 1, METATABLE); \
	const char* input_file_path = luaL_checkstring(L, 2); \
\
	yyjson_read_err json_errors; \
	yyjson_doc* json = yyjson_read_file(input_file_path, \
		JSON_READ_FLAGS, NULL, &json_errors); \
\
	if (json == NULL) { \
		lua_pushnil(L); \
		lua_pushstring(L, json_errors.msg); \
		return 2; \
	} \
\
	yyjson_val* object = yyjson_doc_get_root(json); \
	Error result = fold_##fold##_from_json(fold, json, object); \
	yyjson_doc_free(json); \
\
	if IS_ERROR(result) { \
		lua_pushnil(L); \
		lua_pushstring(L, ERROR_STRING_OR( \
			result, JSON_READ_ERROR)); \
		return 2; \
	} \
\
	lua_pushvalue(L, 1); \
	lua_pushnil(L); \
	return 2; \
} \
\
int l_fold_##fold##_from_json_string_meta(lua_State* L) { \
	size_t string_length; \
	Fold* fold = luaL_checkudata(L, 1, METATABLE); \
	const char* string = luaL_checklstring(L, 2, &string_length); \
\
	yyjson_read_err json_errors; \
	yyjson_doc* json = yyjson_read_opts((char*)string, string_length, \
		JSON_READ_FLAGS, NULL, &json_errors); \
\
	if (json == NULL) { \
		lua_pushnil(L); \
		lua_pushstring(L, json_errors.msg); \
		return 2; \
	} \
\
	yyjson_val* object = yyjson_doc_get_root(json); \
	Error result = fold_##fold##_from_json(fold, json, object); \
	yyjson_doc_free(json); \
\
	if IS_ERROR(result) { \
		lua_pushnil(L); \
		lua_pushstring(L, ERROR_STRING_OR( \
			result, JSON_READ_ERROR)); \
		return 2; \
	} \
\
	lua_pushvalue(L, 1); \
	lua_pushnil(L); \
	return 2; \
}

GENERATE_FUNCTIONS(FoldFile, file, FOLD_FILE_METATABLE)
GENERATE_FUNCTIONS(FoldFrame, frame, FOLD_FRAME_METATABLE)
GENERATE_FUNCTIONS(FoldGraph, graph, FOLD_GRAPH_METATABLE)
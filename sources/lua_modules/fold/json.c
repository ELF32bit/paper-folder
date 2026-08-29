#include "file.h"
#include "frame.h"
#include "graph.h"

#include <yyjson.h>

#include <stdlib.h>

/* ========================================================================= */
/* Serialization & Deserialization                                           */
/* ========================================================================= */

#define JSON_READ_FLAGS \
	YYJSON_READ_ALLOW_COMMENTS | \
	YYJSON_READ_ALLOW_EXT_NUMBER | \
	YYJSON_READ_ALLOW_SINGLE_QUOTED_STR | \
	YYJSON_READ_ALLOW_TRAILING_COMMAS

#define JSON_WRITE_FLAGS \
	YYJSON_WRITE_NEWLINE_AT_END

#define FOLD_READ_ERROR "failed to read FOLD file"
#define FOLD_WRITE_ERROR "failed to write FOLD file"

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
			result, FOLD_READ_ERROR)); \
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
			result, FOLD_READ_ERROR)); \
		return 2; \
	} \
\
	lua_pushvalue(L, 1); \
	lua_pushnil(L); \
	return 2; \
} \
\
int l_fold_##fold##_to_json_file_meta(lua_State* L) { \
	Fold* fold = luaL_checkudata(L, 1, METATABLE); \
	const char* output_file_path = luaL_checkstring(L, 2); \
	yyjson_write_flag flags = lua_toboolean(L, 3) \
		? JSON_WRITE_FLAGS | YYJSON_WRITE_PRETTY \
		: YYJSON_WRITE_NOFLAG; \
\
	yyjson_mut_doc* json = yyjson_mut_doc_new(NULL); \
	if (json == NULL) { \
		lua_pushboolean(L, false); \
		lua_pushstring(L, FOLD_WRITE_ERROR); \
		return 2; \
	} \
\
	yyjson_mut_val* object = yyjson_mut_obj(json); \
	if (object == NULL) { \
		yyjson_mut_doc_free(json); \
		lua_pushboolean(L, false); \
		lua_pushstring(L, FOLD_WRITE_ERROR); \
		return 2; \
	} \
\
	yyjson_mut_doc_set_root(json, object); \
	Error result = fold_##fold##_to_json(fold, json, object); \
\
	if IS_ERROR(result) { \
		yyjson_mut_doc_free(json); \
		lua_pushboolean(L, false); \
		lua_pushstring(L, ERROR_STRING_OR( \
			result, FOLD_WRITE_ERROR)); \
		return 2; \
	} \
\
	yyjson_write_err json_error; \
	bool success = yyjson_mut_write_file(output_file_path, json, \
		flags, NULL, &json_error); \
	yyjson_mut_doc_free(json); \
\
	if NOT(success) { \
		lua_pushboolean(L, false); \
		lua_pushstring(L, json_error.msg); \
		return 2; \
	} \
\
	lua_pushboolean(L, true); \
	lua_pushnil(L); \
	return 2; \
} \
\
int l_fold_##fold##_to_json_string_meta(lua_State* L) { \
	Fold* fold = luaL_checkudata(L, 1, METATABLE); \
	yyjson_write_flag flags = lua_toboolean(L, 2) \
		? JSON_WRITE_FLAGS | YYJSON_WRITE_PRETTY \
		: YYJSON_WRITE_NOFLAG; \
\
	yyjson_mut_doc* json = yyjson_mut_doc_new(NULL); \
	if (json == NULL) { \
		lua_pushnil(L); \
		lua_pushstring(L, FOLD_WRITE_ERROR); \
		return 2; \
	} \
\
	yyjson_mut_val* object = yyjson_mut_obj(json); \
	if (object == NULL) { \
		yyjson_mut_doc_free(json); \
		lua_pushnil(L); \
		lua_pushstring(L, FOLD_WRITE_ERROR); \
		return 2; \
	} \
\
	yyjson_mut_doc_set_root(json, object); \
	Error result = fold_##fold##_to_json(fold, json, object); \
\
	if IS_ERROR(result) { \
		yyjson_mut_doc_free(json); \
		lua_pushnil(L); \
		lua_pushstring(L, ERROR_STRING_OR( \
			result, FOLD_WRITE_ERROR)); \
		return 2; \
	} \
\
	size_t string_length; \
	yyjson_write_err json_error; \
	char* string = yyjson_mut_write_opts(json, \
		flags, NULL, &string_length, &json_error); \
	yyjson_mut_doc_free(json); \
\
	if (string == NULL) { \
		lua_pushnil(L); \
		lua_pushstring(L, json_error.msg); \
		return 2; \
	} \
\
	lua_pushlstring(L, string, string_length); \
	lua_pushnil(L); \
	free(string); \
	return 2; \
} \

GENERATE_FUNCTIONS(FoldFile, file, FOLD_FILE_METATABLE)
GENERATE_FUNCTIONS(FoldFrame, frame, FOLD_FRAME_METATABLE)
GENERATE_FUNCTIONS(FoldGraph, graph, FOLD_GRAPH_METATABLE)
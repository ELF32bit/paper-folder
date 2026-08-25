#include "file.h"
#include "frame.h"
#include "graph.h"

/* ========================================================================= */
/* Creation & Destruction                                                    */
/* ========================================================================= */

#define GENERATE_CONSTRUCTION_FUNCTIONS(Fold, fold, METATABLE) \
int l_fold_##fold##_new(lua_State* L) { \
	Fold* fold = lua_newuserdata(L, sizeof(Fold)); \
	fold_##fold##_create(fold); \
	luaL_getmetatable(L, METATABLE); \
	lua_setmetatable(L, -2); \
	return 1; \
} \
\
int l_fold_##fold##___index_meta(lua_State* L) { \
	luaL_checkudata(L, 1, METATABLE); \
	luaL_getmetatable(L, METATABLE); \
	lua_pushvalue(L, 2); \
	lua_gettable(L, -2); \
	return 1; \
} \
\
int l_fold_##fold##___gc_meta(lua_State* L) { \
	Fold* fold = luaL_checkudata(L, 1, METATABLE); \
	fold_##fold##_destroy(fold); \
	return 0; \
}

GENERATE_CONSTRUCTION_FUNCTIONS(FoldFile, file, FOLD_FILE_METATABLE)
GENERATE_CONSTRUCTION_FUNCTIONS(FoldFrame, frame, FOLD_FRAME_METATABLE)
GENERATE_CONSTRUCTION_FUNCTIONS(FoldGraph, graph, FOLD_GRAPH_METATABLE)

/* ========================================================================= */
/* Validation                                                                */
/* ========================================================================= */

#define FOLD_VALIDATE_ERROR "failed to validate"

#define GENERATE_VALIDATION_FUNCTIONS(Fold, fold, METATABLE, inherited) \
int l_fold_##fold##_validate##inherited##_meta(lua_State* L) { \
	Fold* fold = luaL_checkudata(L, 1, METATABLE); \
	int write_errors = !lua_toboolean(L, 2); \
\
	STRING_CREATE(errors); \
	bool is = true; \
\
	Error result = fold_##fold##_validate##inherited(fold, &is, \
		write_errors ? (&errors) : NULL); \
	if IS_ERROR(result) { \
		string_destroy(&errors); \
		lua_pushboolean(L, false); \
		lua_pushstring(L, FOLD_VALIDATE_ERROR); \
		return 2; \
	} \
\
	lua_pushboolean(L, is); \
	if (errors.length > 0) { \
		lua_pushlstring(L, errors.data, errors.length); \
	} else { lua_pushnil(L); } \
	string_destroy(&errors); \
	return 2; \
}

GENERATE_VALIDATION_FUNCTIONS(FoldFile, file, FOLD_FILE_METATABLE,)
GENERATE_VALIDATION_FUNCTIONS(FoldFrame, frame, FOLD_FRAME_METATABLE,)
GENERATE_VALIDATION_FUNCTIONS(FoldFrame, frame, FOLD_FRAME_METATABLE, _inherited)
GENERATE_VALIDATION_FUNCTIONS(FoldGraph, graph, FOLD_GRAPH_METATABLE,)
GENERATE_VALIDATION_FUNCTIONS(FoldGraph, graph, FOLD_GRAPH_METATABLE, _inherited)



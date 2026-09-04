#include "lua_modules/fold/graph.h"

#include "fold_validation/graph.h"

#define FOLD_VALIDATE_ERROR "failed to validate"

int l_fold_graph_validate_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	int write_errors = NOT(lua_toboolean(L, 2));

	STRING_CREATE(errors);
	bool is = true;

	Error result = fold_graph_validate(graph, &is,
		write_errors ? (&errors) : NULL);
	if IS_ERROR(result) {
		string_destroy(&errors);
		lua_pushboolean(L, false);
		lua_pushstring(L, ERROR_STRING_OR(
			result, FOLD_VALIDATE_ERROR));
		return 2;
	}

	lua_pushboolean(L, is);
	if (errors.length > 0) {
		lua_pushlstring(L, errors.data, errors.length);
	} else { lua_pushnil(L); }
	string_destroy(&errors);
	return 2;
}

int l_fold_graph_validate_inherited_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	int write_errors = NOT(lua_toboolean(L, 2));

	STRING_CREATE(errors);
	bool is = true;

	Error result = fold_graph_validate_inherited(graph, &is,
		write_errors ? (&errors) : NULL);
	if IS_ERROR(result) {
		string_destroy(&errors);
		lua_pushboolean(L, false);
		lua_pushstring(L, ERROR_STRING_OR(
			result, FOLD_VALIDATE_ERROR));
		return 2;
	}

	lua_pushboolean(L, is);
	if (errors.length > 0) {
		lua_pushlstring(L, errors.data, errors.length);
	} else { lua_pushnil(L); }
	string_destroy(&errors);
	return 2;
}
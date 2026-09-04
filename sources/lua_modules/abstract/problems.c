#include "problems.h"

#include "abstract/problems.h"

int l_abstract_problems_solve1(lua_State* L) {
	Error result = abstract_problems_solve1();
	lua_pushboolean(L, IS_OK(result));
	return 1;
}
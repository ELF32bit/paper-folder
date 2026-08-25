#include "graph.h"

/* ========================================================================= */
/* Attributes                                                                */
/* ========================================================================= */

int l_fold_graph_is_abstract_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	lua_pushboolean(L, fold_graph_is_abstract(graph));
	return 1;
}

int l_fold_graph_is_2D_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	lua_pushboolean(L, fold_graph_is_2D(graph));
	return 1;
}

int l_fold_graph_is_3D_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	lua_pushboolean(L, fold_graph_is_3D(graph));
	return 1;
}

int l_fold_graph_is_manifold_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	lua_pushboolean(L, fold_graph_is_manifold(graph));
	return 1;
}

int l_fold_graph_is_orientable_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	bool is;
	Error result = fold_graph_is_orientable(graph, &is);
	if IS_OK(result) {
		lua_pushboolean(L, is);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int l_fold_graph_is_self_touching_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	bool is;
	Error result = fold_graph_is_self_touching(graph, &is);
	if IS_OK(result) {
		lua_pushboolean(L, is);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int l_fold_graph_is_self_intersecting_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	bool is;
	Error result = fold_graph_is_self_intersecting(graph, &is);
	if IS_OK(result) {
		lua_pushboolean(L, is);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int l_fold_graph_has_cuts_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	lua_pushboolean(L, fold_graph_has_cuts(graph));
	return 1;
}

int l_fold_graph_has_joins_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	lua_pushboolean(L, fold_graph_has_joins(graph));
	return 1;
}

int l_fold_graph_has_concave_faces_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	lua_pushboolean(L, fold_graph_has_concave_faces(graph));
	return 1;
}

/* ========================================================================= */
/* Vertices Building                                                         */
/* ========================================================================= */

/* ========================================================================= */
/* Edges Building                                                            */
/* ========================================================================= */

int l_fold_graph_EA_from_EF_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	Error result = fold_graph_EA_from_EF(graph);
	if IS_OK(result) {
		lua_pushboolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int l_fold_graph_EA_from_EFA_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	Error result = fold_graph_EA_from_EFA(graph);
	if IS_OK(result) {
		lua_pushboolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int l_fold_graph_EFA_from_EA_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	Error result = fold_graph_EFA_from_EA(graph);
	if IS_OK(result) {
		lua_pushboolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int l_fold_graph_EL_from_EVC_meta(lua_State* L) {
	FoldGraph* graph = luaL_checkudata(L, 1, FOLD_GRAPH_METATABLE);
	Error result = fold_graph_EL_from_EVC(graph);
	if IS_OK(result) {
		lua_pushboolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

/* ========================================================================= */
/* Faces Building                                                            */
/* ========================================================================= */
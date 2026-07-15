#ifndef LUA_MODULE_FOLD_GRAPH_H
#define LUA_MODULE_FOLD_GRAPH_H

#include <lua.h>
#include <lauxlib.h>

#include "fold/graph.h"

#define FOLD_GRAPH_METATABLE "fold_graph_C.instance"

int l_fold_graph_new(lua_State* L);
int l_fold_graph___index_meta(lua_State* L);
int l_fold_graph___gc_meta(lua_State* L);

int l_fold_graph_from_json_file_meta(lua_State* L);
int l_fold_graph_from_json_string_meta(lua_State* L);
int l_fold_graph_to_json_file_meta(lua_State* L);
int l_fold_graph_to_json_string_meta(lua_State* L);

int l_fold_graph_validate_meta(lua_State* L);

int l_fold_graph_is_abstract_meta(lua_State* L);
int l_fold_graph_is_2D_meta(lua_State* L);
int l_fold_graph_is_3D_meta(lua_State* L);
int l_fold_graph_is_manifold_meta(lua_State* L);
int l_fold_graph_is_orientable_meta(lua_State* L);
int l_fold_graph_has_cuts_meta(lua_State* L);
int l_fold_graph_has_joins_meta(lua_State* L);
int l_fold_graph_is_self_touching_meta(lua_State* L);
int l_fold_graph_is_self_intersecting_meta(lua_State* L);
int l_fold_graph_has_non_convex_faces_meta(lua_State* L);

#endif /* LUA_MODULE_FOLD_GRAPH_H */
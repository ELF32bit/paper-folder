#ifndef LUA_MODULES_FOLD_GRAPH_H
#define LUA_MODULES_FOLD_GRAPH_H

#include <lua.h>
#include <lauxlib.h>

#include "fold_specification/graph.h"

#define FOLD_GRAPH_METATABLE "fold_graph_C.instance"

typedef struct LuaFoldGraph {
	FoldGraph* graph;
	int is_owned;
} LuaFoldGraph;

/* ========================================================================= */
/* Creation & Destruction                                                    */
/* ========================================================================= */

int l_fold_graph_new(lua_State* L);
int l_fold_graph_is_owned_meta(lua_State* L);
int l_fold_graph___index_meta(lua_State* L);
int l_fold_graph___gc_meta(lua_State* L);

/* ========================================================================= */
/* Serialization & Deserialization                                           */
/* ========================================================================= */

int l_fold_graph_from_json_file_meta(lua_State* L);
int l_fold_graph_from_json_string_meta(lua_State* L);
int l_fold_graph_to_json_file_meta(lua_State* L);
int l_fold_graph_to_json_string_meta(lua_State* L);

/* ========================================================================= */
/* Getters                                                                   */
/* ========================================================================= */

int l_fold_graph_get_VC_size_meta(lua_State* L);
int l_fold_graph_get_VV_size_meta(lua_State* L);
int l_fold_graph_get_VE_size_meta(lua_State* L);
int l_fold_graph_get_VF_size_meta(lua_State* L);
int l_fold_graph_get_EV_size_meta(lua_State* L);
int l_fold_graph_get_EF_size_meta(lua_State* L);
int l_fold_graph_get_EA_size_meta(lua_State* L);
int l_fold_graph_get_EFA_size_meta(lua_State* L);
int l_fold_graph_get_EL_size_meta(lua_State* L);
int l_fold_graph_get_EO_size_meta(lua_State* L);
int l_fold_graph_get_FV_size_meta(lua_State* L);
int l_fold_graph_get_FE_size_meta(lua_State* L);
int l_fold_graph_get_FF_size_meta(lua_State* L);
int l_fold_graph_get_FO_size_meta(lua_State* L);

int l_fold_graph_get_PC_size_meta(lua_State* L);
int l_fold_graph_get_TC_size_meta(lua_State* L);
int l_fold_graph_get_T2C_size_meta(lua_State* L);
int l_fold_graph_get_NC_size_meta(lua_State* L);
int l_fold_graph_get_MN_size_meta(lua_State* L);
int l_fold_graph_get_VP_size_meta(lua_State* L);
int l_fold_graph_get_FT_size_meta(lua_State* L);
int l_fold_graph_get_FT2_size_meta(lua_State* L);
int l_fold_graph_get_FN_size_meta(lua_State* L);
int l_fold_graph_get_FM_size_meta(lua_State* L);

/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */

int l_fold_graph_copy_meta(lua_State* L);
int l_fold_graph_inherit_meta(lua_State* L);
int l_fold_graph_is_inherited_meta(lua_State* L);

/* ========================================================================= */
/* Validation                                                                */
/* ========================================================================= */

int l_fold_graph_validate_meta(lua_State* L);
int l_fold_graph_validate_inherited_meta(lua_State* L);

/* ========================================================================= */
/* Attributes                                                                */
/* ========================================================================= */

int l_fold_graph_is_abstract_meta(lua_State* L);
int l_fold_graph_is_2D_meta(lua_State* L);
int l_fold_graph_is_3D_meta(lua_State* L);
int l_fold_graph_is_manifold_meta(lua_State* L);
int l_fold_graph_is_orientable_meta(lua_State* L);
int l_fold_graph_has_cuts_meta(lua_State* L);
int l_fold_graph_has_joins_meta(lua_State* L);
int l_fold_graph_is_self_touching_meta(lua_State* L);
int l_fold_graph_is_self_intersecting_meta(lua_State* L);
int l_fold_graph_has_concave_faces_meta(lua_State* L);

/* ========================================================================= */
/* Properties                                                                */
/* ========================================================================= */

int l_fold_graph_get_abstract_size_meta(lua_State* L);
int l_fold_graph_get_aabb2_meta(lua_State* L);
int l_fold_graph_get_aabb3_meta(lua_State* L);

int l_fold_graph_is_maekawa_compliant_meta(lua_State* L);
int l_fold_graph_is_kawasaki_compliant_meta(lua_State* L);
int l_fold_graph_is_locally_flat_foldable_meta(lua_State* L);

int l_fold_graph_has_unassigned_vertices_meta(lua_State* L);
int l_fold_graph_is_triangulated_meta(lua_State* L);

/* ========================================================================= */
/* Vertices Building                                                         */
/* ========================================================================= */

int l_fold_graph_VV_from_EV_meta(lua_State* L);
int l_fold_graph_VV_from_FV_meta(lua_State* L);
int l_fold_graph_VE_from_VV_meta(lua_State* L);
int l_fold_graph_VF_from_VV_meta(lua_State* L);

/* ========================================================================= */
/* Edges Building                                                            */
/* ========================================================================= */

int l_fold_graph_EA_from_EF_meta(lua_State* L);
int l_fold_graph_EA_from_EFA_meta(lua_State* L);
int l_fold_graph_EFA_from_EA_meta(lua_State* L);
int l_fold_graph_EL_from_EVC_meta(lua_State* L);

/* ========================================================================= */
/* Faces Building                                                            */
/* ========================================================================= */

int l_fold_graph_FE_from_FV_meta(lua_State* L);
int l_fold_graph_FF_from_FV_meta(lua_State* L);

int l_fold_graph_FV_triangulate_meta(lua_State* L);
int l_fold_graph_FV_flip_meta(lua_State* L);

#endif /* LUA_MODULES_FOLD_GRAPH_H */
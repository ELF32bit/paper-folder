#ifndef LUA_MODULE_FOLD_FILE_H
#define LUA_MODULE_FOLD_FILE_H

#include <lua.h>
#include <lauxlib.h>

#include "fold_specification/file.h"

#define FOLD_FILE_METATABLE "fold_file_C.instance"

/* ========================================================================= */
/* Creation & Destruction                                                    */
/* ========================================================================= */

int l_fold_file_new(lua_State* L);
int l_fold_file___index_meta(lua_State* L);
int l_fold_file___gc_meta(lua_State* L);

/* ========================================================================= */
/* Serialization & Deserialization                                           */
/* ========================================================================= */

int l_fold_file_from_json_file_meta(lua_State* L);
int l_fold_file_from_json_string_meta(lua_State* L);
int l_fold_file_to_json_file_meta(lua_State* L);
int l_fold_file_to_json_string_meta(lua_State* L);

int l_fold_file_frame_from_svg_file_meta(lua_State* L);
int l_fold_file_frame_to_svg_file_meta(lua_State* L);

int l_fold_file_from_obj_file_meta(lua_State* L);
int l_fold_file_to_obj_file_meta(lua_State* L);

/* ========================================================================= */
/* Getters                                                                   */
/* ========================================================================= */

int l_fold_file_get_version_meta(lua_State* L);
int l_fold_file_get_creator_meta(lua_State* L);
int l_fold_file_get_author_meta(lua_State* L);
int l_fold_file_get_title_meta(lua_State* L);
int l_fold_file_get_description_meta(lua_State* L);
int l_fold_file_get_classes_meta(lua_State* L);
int l_fold_file_get_frames_meta(lua_State* L);

/* ========================================================================= */
/* Frame Getters                                                             */
/* ========================================================================= */

int l_fold_file_get_frame_author_meta(lua_State* L);
int l_fold_file_get_frame_title_meta(lua_State* L);
int l_fold_file_get_frame_description_meta(lua_State* L);
int l_fold_file_get_frame_classes_meta(lua_State* L);
int l_fold_file_get_frame_attributes_meta(lua_State* L);
int l_fold_file_get_frame_unit_meta(lua_State* L);
int l_fold_file_get_frame_graph_meta(lua_State* L);
int l_fold_file_get_frame_parent_meta(lua_State* L);
int l_fold_file_get_frame_inherit_meta(lua_State* L);

/* ========================================================================= */
/* Graph Getters                                                             */
/* ========================================================================= */

int l_fold_file_get_frame_graph_VC_size_meta(lua_State* L);
int l_fold_file_get_frame_graph_VV_size_meta(lua_State* L);
int l_fold_file_get_frame_graph_VE_size_meta(lua_State* L);
int l_fold_file_get_frame_graph_VF_size_meta(lua_State* L);
int l_fold_file_get_frame_graph_EV_size_meta(lua_State* L);
int l_fold_file_get_frame_graph_EF_size_meta(lua_State* L);
int l_fold_file_get_frame_graph_EA_size_meta(lua_State* L);
int l_fold_file_get_frame_graph_EFA_size_meta(lua_State* L);
int l_fold_file_get_frame_graph_EL_size_meta(lua_State* L);
int l_fold_file_get_frame_graph_EO_size_meta(lua_State* L);
int l_fold_file_get_frame_graph_FV_size_meta(lua_State* L);
int l_fold_file_get_frame_graph_FE_size_meta(lua_State* L);
int l_fold_file_get_frame_graph_FF_size_meta(lua_State* L);
int l_fold_file_get_frame_graph_FO_size_meta(lua_State* L);

int l_fold_file_get_frame_graph_PC_size_meta(lua_State* L);
int l_fold_file_get_frame_graph_TC_size_meta(lua_State* L);
int l_fold_file_get_frame_graph_T2C_size_meta(lua_State* L);
int l_fold_file_get_frame_graph_NC_size_meta(lua_State* L);
int l_fold_file_get_frame_graph_MN_size_meta(lua_State* L);
int l_fold_file_get_frame_graph_VP_size_meta(lua_State* L);
int l_fold_file_get_frame_graph_FT_size_meta(lua_State* L);
int l_fold_file_get_frame_graph_FT2_size_meta(lua_State* L);
int l_fold_file_get_frame_graph_FN_size_meta(lua_State* L);
int l_fold_file_get_frame_graph_FM_size_meta(lua_State* L);

/* ========================================================================= */
/* Validation                                                                */
/* ========================================================================= */

int l_fold_file_validate_meta(lua_State* L);

#endif /* LUA_MODULE_FOLD_FILE_H */
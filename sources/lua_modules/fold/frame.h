#ifndef LUA_MODULES_FOLD_FRAME_H
#define LUA_MODULES_FOLD_FRAME_H

#include <lua.h>
#include <lauxlib.h>

#include "fold_specification/frame.h"

#define FOLD_FRAME_METATABLE "fold_frame_C.instance"

typedef struct LuaFoldFrame {
	FoldFrame* frame;
	int is_owned;
} LuaFoldFrame;

/* ========================================================================= */
/* Creation & Destruction                                                    */
/* ========================================================================= */

int l_fold_frame_new(lua_State* L);
int l_fold_frame_is_owned_meta(lua_State* L);
int l_fold_frame___index_meta(lua_State* L);
int l_fold_frame___gc_meta(lua_State* L);

/* ========================================================================= */
/* Serialization & Deserialization                                           */
/* ========================================================================= */

int l_fold_frame_from_json_file_meta(lua_State* L);
int l_fold_frame_from_json_string_meta(lua_State* L);
int l_fold_frame_to_json_file_meta(lua_State* L);
int l_fold_frame_to_json_string_meta(lua_State* L);

/* ========================================================================= */
/* Getters                                                                   */
/* ========================================================================= */

int l_fold_frame_get_author_meta(lua_State* L);
int l_fold_frame_get_title_meta(lua_State* L);
int l_fold_frame_get_description_meta(lua_State* L);
int l_fold_frame_get_classes_meta(lua_State* L);
int l_fold_frame_get_attributes_meta(lua_State* L);
int l_fold_frame_get_unit_meta(lua_State* L);
int l_fold_frame_get_graph_meta(lua_State* L);
int l_fold_frame_get_parent_meta(lua_State* L);
int l_fold_frame_get_inherit_meta(lua_State* L);

/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */

int l_fold_frame_copy_meta(lua_State* L);
int l_fold_frame_inherit_meta(lua_State* L);
int l_fold_frame_is_inherited_meta(lua_State* L);

/* ========================================================================= */
/* Validation                                                                */
/* ========================================================================= */

int l_fold_frame_validate_meta(lua_State* L);
int l_fold_frame_validate_inherited_meta(lua_State* L);

/* ========================================================================= */
/* Attributes                                                                */
/* ========================================================================= */

int l_fold_frame_attributes_assign_meta(lua_State* L);

#endif /* LUA_MODULES_FOLD_FRAME_H */
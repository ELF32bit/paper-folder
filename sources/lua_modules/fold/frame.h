#ifndef LUA_MODULE_FOLD_FRAME_H
#define LUA_MODULE_FOLD_FRAME_H

#include <lua.h>
#include <lauxlib.h>

#include "fold/frame.h"

#define FOLD_FRAME_METATABLE "fold_frame_C.instance"

/* ========================================================================= */
/* Creation & Destruction                                                    */
/* ========================================================================= */

int l_fold_frame_new(lua_State* L);
int l_fold_frame___index_meta(lua_State* L);
int l_fold_frame___gc_meta(lua_State* L);

/* ========================================================================= */
/* Serialization & Deserialization                                           */
/* ========================================================================= */

int l_fold_frame_from_json_file_meta(lua_State* L);
int l_fold_frame_from_json_string_meta(lua_State* L);
int l_fold_frame_to_json_file_meta(lua_State* L);
int l_fold_frame_to_json_string_meta(lua_State* L);

int l_fold_frame_from_svg_file_meta(lua_State* L);
int l_fold_frame_to_svg_file_meta(lua_State* L);

/* ========================================================================= */
/* Validation                                                                */
/* ========================================================================= */

int l_fold_frame_validate_meta(lua_State* L);
int l_fold_frame_validate_inherited_meta(lua_State* L);

/* ========================================================================= */
/* Attributes                                                                */
/* ========================================================================= */

int l_fold_frame_attributes_assign_meta(lua_State* L);

#endif /* LUA_MODULE_FOLD_FRAME_H */
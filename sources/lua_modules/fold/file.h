#ifndef LUA_MODULES_FOLD_FILE_H
#define LUA_MODULES_FOLD_FILE_H

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
int l_fold_file_get_frames_size_meta(lua_State* L);
int l_fold_file_get_key_frame_meta(lua_State* L);

/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */

int l_fold_file_copy_meta(lua_State* L);
int l_fold_file_frame_inherit_meta(lua_State* L);
int l_fold_file_frames_inherit_meta(lua_State* L);

/* ========================================================================= */
/* Validation                                                                */
/* ========================================================================= */

int l_fold_file_validate_inherit_meta(lua_State* L);
int l_fold_file_validate_meta(lua_State* L);

#endif /* LUA_MODULES_FOLD_FILE_H */
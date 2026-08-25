#ifndef LUA_MODULE_FOLD_FILE_H
#define LUA_MODULE_FOLD_FILE_H

#include <lua.h>
#include <lauxlib.h>

#include "fold/file.h"

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

int l_fold_file_from_obj_file_meta(lua_State* L);
int l_fold_file_to_obj_file_meta(lua_State* L);

/* ========================================================================= */
/* Validation                                                                */
/* ========================================================================= */

int l_fold_file_validate_meta(lua_State* L);

#endif /* LUA_MODULE_FOLD_FILE_H */
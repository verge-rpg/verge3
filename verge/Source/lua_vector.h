#ifndef _LUA_VECTOR_H
#define _LUA_VECTOR_H

#ifdef ENABLE_LUA

#include "lua_main.h"

class vec3;

//pushes a vec3 onto the lua stack as userdata
//returns a pointer to the new vec3
vec3 * lua_pushvec3(lua_State * L);

//pops a vec3 off the lua stack, after verifying
//that is the correct userdata type
//returns a pointer to the popped vec3
vec3 * lua_checkvec3(lua_State * L, int index);

//registers operations on Vector3 objects with lua
int luaopen_vector(lua_State * L);

#endif //ENABLE_LUA

#endif //_LUA_VECTOR_H
/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


/****************************************************************
	xerxes engine
	lua_vector.cpp
 ****************************************************************/

#include "xerxes.h"

#include <cassert>

#ifdef ENABLE_LUA

#include "lua_vector.h"
#include "algebra3.h"

using namespace std;

namespace
{
	int vector_new(lua_State * L)
	{
		// can't push the metatable on until we've checked for optional initialization args on the stack

		vec3 tmp(0, 0, 0);
		
		tmp.x = luaL_optnumber(L, 1, 0);
		tmp.y = luaL_optnumber(L, 2, 0);
		tmp.z = luaL_optnumber(L, 3, 0);

		vec3 * v = lua_pushvec3(L);

		assert(v);

		*v = tmp;

		return 1;
	}

	int vector_getfield(lua_State * L)
	{
		vec3 * v = lua_checkvec3(L, 1);
		const char * i = luaL_checkstring(L, 2);

		switch (*i)
		{
			case 'x': 
				lua_pushnumber(L, (v->x));
				break;
			case 'y': 
				lua_pushnumber(L, (v->y));
				break;
			case 'z': 
				lua_pushnumber(L, (v->z));
				break;
			default:
				// just try to fetch the value from the meta table
				luaL_getmetafield(L, -2, i);
		}

		return 1;
	}

	int vector_setfield(lua_State * L)
	{
		vec3 * v = lua_checkvec3(L, 1);
		const char * i = luaL_checkstring(L, 2);
		const double n = luaL_checknumber(L, 3);

		switch (*i)
		{
			case 'x': 
				v->x = n;
				break;
			case 'y': 
				v->y = n;
				break;
			case 'z': 
				v->z = n;
				break;
		}

		return 0;
	}

	int vector_add(lua_State * L)
	{
		vec3 * lhs = lua_checkvec3(L, 1);
		vec3 * rhs = lua_checkvec3(L, 2);

		vec3 * sum = lua_pushvec3(L);

		*sum = *lhs + *rhs;

		return 1;
	}

	int vector_subtract(lua_State * L)
	{
		vec3 * lhs = lua_checkvec3(L, 1);
		vec3 * rhs = lua_checkvec3(L, 2);

		vec3 * diff = lua_pushvec3(L);

		*diff = *lhs - *rhs;

		return 1;
	}

	int vector_dot(lua_State * L)
	{
		vec3 * lhs = lua_checkvec3(L, 1);
		vec3 * rhs = lua_checkvec3(L, 2);

		double ans = (*lhs)*(*rhs);

		lua_pushnumber(L, (ans));

		return 1;
	}

	int vector_scale(lua_State * L)
	{
		vec3 * lhs = lua_checkvec3(L, 1);
		const double n = luaL_checknumber(L, 2);

		vec3 * result = lua_pushvec3(L);

		*result = *lhs * n;

		return 1;
	}

	/*int vector_GetAngles(lua_State * L)
	{
		vec3 * v = lua_checkvec3(L, 1);

		Angle3 angles(v->angles());

		lua_pushnumber(L, (angles.pitch.toDegrees()));
		lua_pushnumber(L, (angles.yaw.toDegrees()));
		lua_pushnumber(L, (angles.roll.toDegrees()));

		return 3;
	}

	int vector_GetPitch(lua_State * L)
	{
		vec3 * v = lua_checkvec3(L, 1);
		
		Angle pitch(v->pitch());

		lua_pushnumber(L, (pitch.toDegrees()));

		return 1;
	}

	int vector_GetYaw(lua_State * L)
	{
		vec3 * v = lua_checkvec3(L, 1);
		
		Angle yaw(v->yaw());

		lua_pushnumber(L, (yaw.toDegrees()));

		return 1;
	}

	int vector_GetRoll(lua_State * L)
	{
		vec3 * v = lua_checkvec3(L, 1);

		Angle3 angles(v->angles());
		Angle roll(v->roll(angles.yaw, angles.pitch));

		lua_pushnumber(L, (roll.toDegrees()));
		
		return 1;
	}

	int vector_RotateX(lua_State * L)
	{
		vec3 * v = lua_checkvec3(L, 1);
		lua_Number val = luaL_checknumber(L, 2);
		
		float angle(float::noConvert(lua_number2fixed(val)));

		v->rotateX(Angle::fromDegrees(angle));
			
		return 0;
	}	

	int vector_RotateY(lua_State * L)
	{
		vec3 * v = lua_checkvec3(L, 1);
		lua_Number val = luaL_checknumber(L, 2);
	
		float angle(float::noConvert(lua_number2fixed(val)));

		v->rotateY(Angle::fromDegrees(angle));
			
		return 0;
	}	

	int vector_RotateZ(lua_State * L)
	{
		vec3 * v = lua_checkvec3(L, 1);
		lua_Number val = luaL_checknumber(L, 2);
		
		float angle(float::noConvert(lua_number2fixed(val)));
	
		v->rotateZ(Angle::fromDegrees(angle));
			
		return 0;
	}	*/

	int vector_Length(lua_State * L)
	{
		vec3 * v = lua_checkvec3(L, 1);

		double ans = v->length();

		lua_pushnumber(L, (ans));

		return 1;
	}	


	int vector_LengthSquared(lua_State * L)
	{
		vec3 * v = lua_checkvec3(L, 1);

		double ans = v->length2();

		lua_pushnumber(L, (ans));

		return 1;
	}	

	int vector_Normalize(lua_State * L)
	{
		vec3 * v = lua_checkvec3(L, 1);

		v->normalize();

		return 0;
	}	

	int vector_tostring(lua_State * L)
	{
		vec3 * v = lua_checkvec3(L, 1);

		lua_pushstring(L, v->toString().c_str());
		return 1;
	}

	const luaL_Reg vector_static[] =
	{
		{"new", vector_new},
		{NULL, NULL}
	};

	const luaL_Reg vector_members[] =
	{
		{"__index",		vector_getfield},
		{"__newindex",	vector_setfield},
		{"__tostring",	vector_tostring},
		{"__add",		vector_add},
		{"__sub",		vector_subtract},
		{"__mul",		vector_scale},
		{"dot",			vector_dot},
		//{"GetAngles",	vector_GetAngles},
		//{"GetPitch", vector_GetPitch},
		//{"GetYaw", vector_GetYaw},
		//{"GetRoll", vector_GetRoll},
		//{"RotateX", vector_RotateX},
		//{"RotateY", vector_RotateY},
		//{"RotateZ", vector_RotateZ},
		{"Length", vector_Length},
		{"LengthSquared", vector_LengthSquared},
		{"Normalize", vector_Normalize},
		{NULL, NULL}
	};
}

vec3 * lua_pushvec3(lua_State * L)
{
	vec3 * v = static_cast<vec3 *>(lua_newuserdata(L, sizeof(vec3)));

	luaL_getmetatable(L, "Verge3vec3");
	lua_setmetatable(L, -2);

	return v;
}

vec3 * lua_checkvec3(lua_State * L, int index)
{
	return static_cast<vec3 *>(luaL_checkudata(L, index, "Verge3vec3"));
}


int luaopen_vector(lua_State * L)
{
	luaL_newmetatable(L, "Verge3vec3");
	lua_pushvalue(L, -1); // dupe the metatable on the stack
	lua_setfield(L, -2, "__index"); // metatable.__index = metatable

	luaL_register(L, NULL, vector_members);
	lua_pop(L,1);

	// Push the v3 namespace
	lua_getglobal(L, "v3");

	//create vector statics in a vector table
	lua_newtable(L);
	luaL_register(L, NULL, vector_static);
	lua_setfield(L,-2,"vector");
	lua_pop(L,1);

	return 1;
}


#endif // ENABLE_LUA


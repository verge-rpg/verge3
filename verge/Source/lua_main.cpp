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
	lua_main.cpp
 ****************************************************************/

#include "xerxes.h"

#ifdef ENABLE_LUA

#include "lua_main.h"
#include "lua_vector.h"
#include <stdio.h>

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <memory>


int LUA::activeFunctionIndex;
int LUA::activeFunctionStackOffset;

LUA::LUA()
{
	L = lua_open();
	luaL_openlibs(L);
	bindApi();

}

LUA::~LUA()
{
	lua_close(L);
}

//http://www.codeproject.com/cpp/luaincpp.asp
bool LUA::CompileMap(const char *f) {
	VFILE *si = vopen(va("%s.lua", f));
	if (!si) ::err("unable to open %s.lua", f);
	int scriptlen = filesize(si);
	std::auto_ptr<char> temp(new char[scriptlen+1]);
	vread(temp.get(),scriptlen,si);
	vclose(si);

	VFILE *mi = vopen(va("%s.map", f));
	if (!mi) ::err("unable to open %s.map", f);
	vseek(mi, 10, 0);

	int mapcoresize;
	vread(&mapcoresize, 4, mi);
	log("DESTINATION POS: %d", mapcoresize);
	vseek(mi, 0, 0);
	byte* buf = new byte[mapcoresize];
	vread(buf, mapcoresize, mi);
	vclose(mi);

	FILE *mo = fopen(va("%s.map", f), "wb");
	if (!mo)
		::err("couldn't open map for writing");
	fwrite(buf, 1, mapcoresize, mo);
	delete[] buf;

	// we need to endian flip this so that it gets read properly
	flip(&scriptlen,4);
	
	fwrite(&scriptlen,4,1,mo);
	fwrite(temp.get(),1,scriptlen,mo);
	fclose(mo);

	return true;
}


void LUA::compileSystem() {
	loadFile("system.lua");
}

void LUA::LoadMapScript(VFILE *f, CStringRef filename) {
	int scriptsize;
	vread(&scriptsize,4,f);
	std::auto_ptr<char> temp(new char[scriptsize+1]);
	vread(temp.get(),scriptsize,f);
	temp.get()[scriptsize] = 0;

	if(luaL_loadbuffer(L, temp.get(), strlen(temp.get()), filename.c_str()))
		err("Error loading " + filename.str());
	if(lua_pcall(L, 0, 0, 0))
		err("Error compiling " + filename.str());
}

void LUA::loadFile(const char *fname) {

	boost::shared_array<byte> buf = vreadfile(fname);
	if(!buf.get())
		err("Error loading " + std::string(fname));

	if(luaL_loadbuffer(L, (char*)buf.get()+4, *(int*)buf.get(), fname))
		err("Error loading " + std::string(fname));

	if(lua_pcall(L, 0,0,0))
		err("Error compiling " + std::string(fname));
}

void LUA::ExecAutoexec() {
	ExecuteFunctionString("autoexec");
	//lua_getglobal(L, "autoexec");
	//if(lua_pcall(L, 0,0,0))
	//	err("Error calling autoexec");
}

std::string strtolower(std::string val) { std::transform(val.begin(), val.end(), val.begin(), tolower); return val; }

// Error handler. Don't shove this in a package, since this is important to the Lua environment.
int LUA::InitErrorHandler(lua_State *L)
{
	return luaL_dostring(L,"function __err(e)\n\
		local level = 1 \n\
		local s = \"\" \n\
		\n\
		local function explode(div, str) \n\
			if (div=='') then return false end \n\
			if (not div or not str) then return false end \n\
			local pos,arr = 0,{} \n\
			-- for each divider found \n\
			for st,sp in function() return string.find(str,div,pos,true) end do \n\
				table.insert(arr,string.sub(str,pos,st-1)) -- Attach chars left of current divider \n\
				pos = sp + 1 -- Jump past current divider \n\
			end \n\
			table.insert(arr,string.sub(str,pos)) -- Attach chars right of last divider \n\
			return arr \n\
		end \n\
		\n\
		-- Start printing all trace after the offending line. We don't want to trace the error handler or the error() call if there was level modifiers. \n\
		local match = false \n\
		local t = explode(\":\", e) \n\
		if not t then return \"Error was so bad the error handler failed too.\" end \n\
		local filename, line, message = unpack(t) \n\
		line = tonumber(line) \n\
		if not message then \n\
			filename = \"?\" \n\
			line = 0 \n\
			message = e \n\
			match = true \n\
		end \n\
		\n\
		s = s .. message .. \"\\n\" \n\
		s = s .. \"  Traceback (most recent call first):\\n\" \n\
		while true do \n\
			local info = debug.getinfo(level, \"Sln\") \n\
			if not info then \n\
				break \n\
			end \n\
			\n\
			s = s .. \"    File '\" .. info.short_src .. \"', line \" .. tostring(info.currentline) \n\
			if info.name or info.namewhat then \n\
				s = s .. \", in\" \n\
				if info.namewhat then \n\
					if info.namewhat ~= \"\" then \n\
						s = s .. \" \" .. info.namewhat \n\
					else \n\
						s = s .. \" (function)\" \n\
					end \n\
				end \n\
				if info.name and info.name ~= \"\" then \n\
					s = s .. \" \" .. info.name \n\
				end \n\
			end \n\
			s = s .. \"\\n\" \n\
			level = level + 1 \n\
		end \n\
		return s \n\
	end\n");
}

void LUA::LuaError(const char *text, ...)
{
	va_list argptr;
	char msg[4096];

	va_start(argptr, text);
	vsnprintf(msg, 4096, text, argptr);
	va_end(argptr);

	lua_pushstring(L, msg);
	lua_error(L);
}

void LUA::LuaError(lua_State *L, const char *text, ...)
{
	va_list argptr;
	char msg[4096];

	va_start(argptr, text);
	vsnprintf(msg, 4096, text, argptr);
	va_end(argptr);

	lua_pushstring(L, msg);
	lua_error(L);
}


// Ensures that the arguments passed to a function are correct.
void LUA::VerifyFunctionSignature(lua_State* L, int functionIndex)
{
	int providedArgumentCount = lua_gettop(L);
	int declaredArgumentCount = libfuncs[functionIndex].argumentTypes.size();
	int i = 0;
	bool varargs = false;

	// No args, don't verify
	if(declaredArgumentCount == 0)
	{
		return;
	}

	// Varargs functions, need to decrease "expected" args by one.
	if(libfuncs[functionIndex].argumentTypes[declaredArgumentCount - 1] == t_VARARG)
	{
		varargs = true;
		declaredArgumentCount--;
	}

	while(i < providedArgumentCount)
	{
		if (i < declaredArgumentCount)
		{
			// Ints. Need to check i + 1th stack index because Lua indices start at 1
			if(libfuncs[functionIndex].argumentTypes[i] == t_INT && !lua_isnumber(L, i + 1))
			{
				LuaError(L,"Problem invoking %s: Argument #%d to function must be a int.", libfuncs[functionIndex].name.c_str(), i + 1);
			}
			// Booleans.  Need to check i + 1th stack index because Lua indices start at 1
			else if(libfuncs[functionIndex].argumentTypes[i] == t_BOOL && !lua_isboolean(L, i + 1))
			{
				LuaError(L,"Problem invoking %s: Argument #%d to function must be a boolean (true/false value).", libfuncs[functionIndex].name.c_str(), i + 1);
			}
			// Strings.  Need to check i + 1th stack index because Lua indices start at 1
			else if(libfuncs[functionIndex].argumentTypes[i] == t_STRING && !lua_isstring(L, i + 1))
			{
				LuaError(L,"Problem invoking %s: Argument #%d to function must be a string.", libfuncs[functionIndex].name.c_str(), i + 1);
			}
		}
		// Now for varargs and stuff, we'll error if this is 'too many' args afterwards.
		else
		{
			// Varargs stacks only know how to deal with strings and ints
			// (For verification, checking if a string catches both).
			if(!lua_isstring(L, i + 1))
			{
				LuaError(L,"Problem invoking %s: Argument #%d to function must be a string or number.", libfuncs[functionIndex].name.c_str(), i + 1);
			}
		}
		i++;
	}

	if (i < declaredArgumentCount || (i > declaredArgumentCount && !varargs))
	{
		LuaError(L,"Problem invoking %s: Function expects %d arguments. (Got %d)", libfuncs[functionIndex].name.c_str(), declaredArgumentCount, i);
	}
}

// Careful, this needs to be static and return an int so it can be bound to Lua.
int LUA::InvokeBuiltinFunction(lua_State* L)
{
	// Grab the function index upvalue
	int functionIndex = (int)lua_tonumber(L, lua_upvalueindex(1));

	// Track the active function
	activeFunctionIndex = functionIndex;
	activeFunctionStackOffset = 0;

	LUA::VerifyFunctionSignature(L, functionIndex);

	// TODO:	Make t_BOOL signatures for functions/vars, that are the same as t_INT in Verge,
	//			but have proper coercion and validation in Lua.

	// Calls the function by looking up in the dispatch table.
	VcFunctionImpl ptr = dispatchTable[functionIndex];
	if (ptr) {
		ptr();
	}
	else {
		::err("Error calling function (index %d) '%s'", functionIndex, libfuncs[functionIndex].name.c_str());
	}

	// Push return values onto the stack.
	if(libfuncs[functionIndex].returnType == t_INT)
	{
		lua_pushnumber(L, se->vcreturn);
		return 1; // Single return value
	}
	else if(libfuncs[functionIndex].returnType == t_BOOL)
	{
		lua_pushboolean(L, se->vcreturn);
		return 1; // Single return value
	}
	else if(libfuncs[functionIndex].returnType == t_STRING)
	{
		lua_pushstring(L, se->vcretstr.c_str());
		return 1; // Single return value
	}
	else
	{
		return 0; // No return value
	}
}

void LUA::BindFunction(lua_State* L, int functionIndex)
{
	// Don't bind all those empty name functions.
	if(libfuncs[functionIndex].name.length() == 0)
	{
		return;
	}

	// Push the v3 namespace
	lua_getglobal(L, "v3");
	// Push the name associated with the function
	lua_pushstring (L, strtolower(libfuncs[functionIndex].name).c_str());
	// Push the function index as an upvalue so we can dispatch this function later.
	lua_pushnumber (L, (lua_Number) functionIndex);
	// Push a callback function. Pops the function index upvalue with it.
	lua_pushcclosure(L, LUA::InvokeBuiltinFunction, 1);
	// Pop the function value and then pop string name.
	// Translates to v3[func.name] = fptr
	lua_rawset (L, -3);
	// And after that assignment, the v3 namespace is popped off.
	lua_pop(L, -1);
}


void LUA::BindHvar(lua_State* L, int index)
{
	const char* type = libvars[index][0];
	const char* name = libvars[index][1];
	const char* dimlist = libvars[index][2];

	//transform dots in names to underscores
	char namebuf[256] = "get_";
	strcat(namebuf,name);
	char* tmp = namebuf;
	while(*tmp) *tmp++ = (*tmp=='.'?'_':tolower(*tmp));

	lua_getglobal(L, "v3");
	
	//setup the getter
	lua_pushstring(L, namebuf);
	lua_pushinteger (L, index);
	lua_pushlightuserdata (L, (void*)this);
	lua_pushcclosure(L, &LUA::Get_Hvar, 2);
	lua_rawset (L, -3); //v3.[xform] = LuaVerge_Get_Hvar

	//change name to set_
	namebuf[0] = 's';

	//setup the setter
	lua_pushstring(L, namebuf);
	lua_pushinteger (L, index);
	lua_pushlightuserdata (L, (void*)this);
	lua_pushcclosure(L, &LUA::Set_Hvar, 2);
	lua_rawset (L, -3); //v3.[xform] = LuaVerge_Set_Hvar
	
	lua_pop(L, -1); //pop v3 namespace
}


int LUA::Get_Hvar(lua_State* L)
{
	//Grab the upvalues
	int index = lua_tointeger(L, lua_upvalueindex(1));
	LUA* lua = (LUA*)lua_topointer(L, lua_upvalueindex(2));

	const char* type = libvars[index][0];
	const char* name = libvars[index][1];
	const char* dimlist = libvars[index][2];

	int args = lua_gettop(L);

	if(*dimlist == '1')
	{
		if(args < 1) lua->LuaError("getting v3 system variable `%s` requires a subscript",name);
		if(args > 1) lua->LuaError("getting v3 system variable `%s` with too many parameters",name);
		if(!lua_isnumber(L, 1)) lua->LuaError("subscript on v3 system variable `%s` is not an integer", name);
		int ofs = lua_tointeger(L,1);
		if(*type == '0' + t_INT)
		{
			int ret = lua->ReadHvar(intHVAR1, index, ofs);
			lua_pushinteger(L, ret);
		} 
		else if(*type == '0' + t_BOOL)
		{
			int ret = lua->ReadHvar(intHVAR1, index, ofs);
			lua_pushboolean(L, ret);
		} 
		else if(*type == '0' + t_STRING)
		{
			StringRef ret = lua->ReadHvar_str(strHSTR1, index, ofs);
			lua_pushstring(L, ret.c_str());
		} else lua->LuaError("Fatal Error Code Haberdasher");
		return 1;
	}
	else if(*dimlist == 0)
	{
		if(args != 0) lua->LuaError("v3 system variable `%s` accessed with unexpected parameter. None are necessary.",name);
		if(*type == '0' + t_INT)
		{
			int ret = lua->ReadHvar(intHVAR0, index, 0);
			lua_pushinteger(L, ret);
		}
		else if(*type == '0' + t_BOOL)
		{
			int ret = lua->ReadHvar(intHVAR0, index, 0);
			lua_pushboolean(L, ret);
		} 
		else if(*type == '0' + t_STRING)
		{
			StringRef ret = lua->ReadHvar_str(strHSTR0, index, 0);
			lua_pushstring(L, ret.c_str());
		} else lua->LuaError("Fatal Error Code Sandman");
		return 1;
	} else lua->LuaError("Fatal Error Code Spaniard");
	return -1;
}

int LUA::Set_Hvar(lua_State* L)
{
	//Grab the upvalues
	int index = lua_tointeger(L, lua_upvalueindex(1));
	LUA* lua = (LUA*)lua_topointer(L, lua_upvalueindex(2));

	const char* type = libvars[index][0];
	const char* name = libvars[index][1];
	const char* dimlist = libvars[index][2];

	//count and validate the arguments
	int args = lua_gettop(L);

	if(*dimlist == '1')
	{
		if(args < 2) lua->LuaError("setting v3 system variable `%s` requires parameters subscript and value",name);
		if(args > 2) lua->LuaError("setting v3 system variable `%s` has too many parameters", name);
		if(!lua_isnumber(L, 1)) lua->LuaError("subscript on v3 system variable `%s` is not an integer", name);
		int ofs = lua_tointeger(L,1);
		if(*type == '0'  + t_INT)
		{
			if(!lua_isnumber(L, 2)) lua->LuaError("value for v3 system variable `%s` must be an integer", name);
			int value = lua_tointeger(L, 2);
			lua->WriteHvar(intHVAR1, index, ofs, value);
		}
		else if(*type == '0' + t_BOOL)
		{
			if(!lua_isboolean(L, 2)) lua->LuaError("value for v3 system variable `%s` must be an boolean", name);
			int value = lua_toboolean(L, 2);
			lua->WriteHvar(intHVAR1, index, ofs, value);
		} 
		else if(*type == '0' + t_STRING)
		{
			if(!lua_isstring(L, 2)) lua->LuaError("value for v3 system variable `%s` must be a string", name);
			StringRef value = lua_tostring(L, 2);
			lua->WriteHvar_str(strHSTR1, index, ofs, value);
		}
	} 
	else if(*dimlist == 0)
	{
		if(args < 1) lua->LuaError("setting v3 system variable `%s` requires a value parameter",name);
		if(args > 1) lua->LuaError("setting v3 system variable `%s` has too many parameters",name);
		if(*type == '0' + t_INT)
		{
			if(!lua_isnumber(L, 1)) lua->LuaError("value for v3 system variable `%s` must be an integer", name);
			int value = lua_tointeger(L, 1);
			lua->WriteHvar(intHVAR0, index, 0, value);
		}
		else if(*type == '0' + t_BOOL)
		{
			if(!lua_isboolean(L, 1)) lua->LuaError("value for v3 system variable `%s` must be an boolean", name);
			int value = lua_toboolean(L, 1);
			lua->WriteHvar(intHVAR0, index, 0, value);
		} 
		else if(*type == '0' + t_STRING)
		{
			if(!lua_isstring(L, 1)) lua->LuaError("value for v3 system variable `%s` must be a string", name);
			StringRef value = lua_tostring(L, 1);
			lua->WriteHvar_str(strHSTR0, index, 0, value);
		}
	}
	else lua->LuaError("Fatal Error Code Antarctica");
	
	return 0;
}

void LUA::BindHdef(lua_State* L, int index)
{
	std::string name = strtolower(hdefs[index][0]);
	char* value = hdefs[index][1];

	lua_getglobal(L, "v3");

	lua_pushstring(L, "hdef");
	lua_gettable(L, -2);

	lua_pushstring(L, name.c_str());
	lua_pushstring(L, value); // Lua auto-coerces numeric strings to numbers, cool!
	lua_settable(L, -3); // v3.hdef[def] = value
	lua_pop(L, -1); // pop hdef

	lua_pop(L, -1); // pop v3 namespace
}

static const luaL_reg luaGCHandleMeta[] = {
    {"__gc", LUA::GCHandleDestruct},
	{"__tostring", LUA::GCHandleToString},
    {0, 0}
};

void LUA::InitGCHandleSystem(lua_State* L)
{
	// Load v3 namespace
	lua_getglobal(L, "v3");

	// Bind the public constructor.
	lua_pushstring(L, "GCHandle");
	lua_pushcclosure(L, LUA::GCHandleConstruct, 0);
	lua_settable(L, -3); // v3.GCHandle = value
	lua_pop(L, -1); // pop v3 namespace

	// Bind the metatable junk
	luaL_newmetatable(L, "GCHandleInternal");
	luaL_openlib(L, 0, luaGCHandleMeta, 0);
	lua_pop(L, 1);
}

int LUA::GCHandleConstruct(lua_State* L)
{
	int handle = (int) lua_tonumber(L, 1);
	const char* destructor_name = lua_tostring(L, 2);

	LUA::GCHandle* gch = (LUA::GCHandle*) lua_newuserdata(L, sizeof(LUA::GCHandle));
	luaL_getmetatable(L, "GCHandleInternal");
	lua_setmetatable(L, -2);

	gch->handle = handle;
	strcpy(gch->destructor_name, destructor_name);
	return 1;
}

int LUA::GCHandleDestruct(lua_State* L)
{
	LUA::GCHandle* gch = (LUA::GCHandle*) lua_touserdata(L, 1);
	
	// Push v3 namespace
	lua_getglobal(L, "v3");

	// Get the value of v3[destructor_name]
	lua_getfield(L, -1, gch->destructor_name);
	// remove v3 namespace from stack
	lua_remove(L, -2);	

	// Push the argument
	lua_pushinteger(L, gch->handle);
	// Call v3[destructor_name](handle)
	lua_call(L, 1, 0);

	return 0;
}

int LUA::GCHandleToString(lua_State *L)
{
	//LUA::GCHandle* gch = (LUA::GCHandle*) lua_touserdata(L, 1);
	lua_pushfstring(L, "GCHandle: %p", lua_touserdata(L, 1));
	//lua_pushfstring(L, "GCHandle: %p (handle %d, free with %s)", gch, gch->handle, gch->destructor_name);
	return 1;
}

void LUA::bindApi()
{
	int i;
	int status = 0;

	status = LUA::InitErrorHandler(L);
	if(status)
	{
		::err("Error function couldn't be initialized! The irony.");
	}

	// Create an empty table!
	lua_newtable(L);
	// Put the table we created into the global namespace as v3.
	lua_setglobal(L, "v3");

	// now that v3 namespace exists, it is a fine time to install the vector stuff
	luaopen_vector(L);
	
	// Metatable boilerplate that basically handles variable handling magic.
	status = luaL_dostring(L,
		"v3.get_hvar0 = {}\n"
		"v3.set_hvar0 = {}\n"
		"v3.hdef = {}\n"

		// Now we can pass a scoped variable, instead of having to lookup a nested table tons of times.
		"local getter = v3.get_hvar0\n"
		"local setter = v3.set_hvar0\n"
		"local hdef = v3.hdef\n"

		"local meta = {}\n"
		"setmetatable(v3, meta)\n"

		"function meta:__index(name)\n"
		"	name = string.lower(name)"
		"	local f = getter[name]\n"
		"	return (f and f()) or hdef[name] or rawget(self, name)\n"
		"end\n"

		"function meta:__newindex(name, value)\n"
		"	name = string.lower(name)"
		"	local f = setter[name]\n"
		"	if f then\n"
		"		f(value)\n"
		"	elseif hdef[name] then\n"
		"		error('Tried to write to v3.' .. name .. ', which is a readonly constant.')\n"
		"	else\n"
		"		rawset(self, name, value)\n"
		"	end\n"
		"end\n"

		// Create a table with getters and setters to v3's hvars on the backend.
		"function _builtin_struct()\n"
		"	local t = {}\n"
		"	\n"
		"	local getter = {}\n"
		"	local setter = {}\n"
		"	local meta = {}\n"
		"	\n"
		"	setmetatable(t, meta)\n"
		"	\n"
		"	t.get_hvar0 = getter\n"
		"	t.set_hvar0 = setter\n"
		"	\n"
		"	function meta:__index(name)\n"
		"		name = string.lower(name)"
		"		local f = getter[name]\n"
		"		return (f and f()) or rawget(self, name)\n"
		"	end\n"
		"	\n"
		"	function meta:__newindex(name, value)\n"
		"		name = string.lower(name)"
		"		local f = setter[name]\n"
		"		if f then\n"
		"			f(value)\n"
		"		else\n"
		"			rawset(self, name, value)\n"
		"		end\n"
		"	end\n"
		"	\n"
		"	return t\n"
		"end\n"

		// Create a builtin that is numerically indexed and calls the getter/setter bound to it.
		"function _builtin_array(getter, setter)\n"
		"	local t = {}\n"
		"	\n"
		"	local meta = {}\n"
		"	setmetatable(t, meta)\n"
		"	\n"
		"	function meta:__index(i)\n"
		"		if tonumber(i) then\n"
		"			return getter(i)\n"
		"		end\n"
		"		error('Attempt to index hvar array by non-numeric \\'' .. tostring(i) .. '\\' value.') \n"
		"	end\n"
		"	\n"
		"	function meta:__newindex(i, value)\n"
		"		if tonumber(i) then\n"
		"			setter(i, value)\n"
		"		end\n"
		"		error('Attempt to index hvar array by non-numeric \\'' .. tostring(i) .. '\\' value.') \n"
		"	end\n"
		"	return t\n"
		"end\n"

		// Create an numerically indexed table that caches its entries.
		"function _builtin_struct_collection()\n"
		"	local t = {}\n"
		"	\n"
		"	local getter = {}\n"
		"	local setter = {}\n"
		"	local cache = {}\n"
		"	\n"
		"	local meta = {}\n"
		"	setmetatable(t, meta)\n"
		"	\n"
		"	t.get_hvar1 = getter\n"
		"	t.set_hvar1 = setter\n"
		"	\n"
		"	local function get_entry(i)\n"
		"		local handle = { index = i } \n"
		"		\n"
		"		local handle_meta = {}\n"
		"		setmetatable(handle, handle_meta)\n"
		"		\n"
		"		function handle_meta:__index(name)\n"
		"			name = string.lower(name)\n"
		"			local f = getter[name]\n"
		"			return (f and f(i)) or rawget(self, name)\n"
		"		end\n"
		"		\n"
		"		function handle_meta:__newindex(name, value)\n"
		"			name = string.lower(name)"
		"			local f = setter[name]\n"
		"			if f then\n"
		"				f(i, value)\n"
		"			else\n"
		"				rawset(self, name, value)\n"
		"			end\n"
		"		end\n"
		"		cache[i] = handle\n"
		"		return handle\n"
		"	end\n"
		"	\n"
		"	\n"
		"	function meta:__index(i)\n"
		"		return cache[i]\n"
		"				or (tonumber(i) and get_entry(i))\n"
		"				or error('Attempt to index hvar array by non-numeric \\'' .. tostring(i) .. '\\' value.') \n"
		"	end\n"
		"	\n"
		"	function meta:__newindex(name, value)\n"
		"		error('Attempt to reassign an internal structured array to something else. Bad you.')\n"
		"	end\n"
		"	\n"
		"	return t\n"
		"end\n"
	);
	if(status) ::err("Failed to load LuaVerge's v3 metatables.");

	// Bind the functions to the v3 namespace.
	for(i = 0; i < NUM_LIBFUNCS; i++)
	{
		LUA::BindFunction(L, i);
	}

	// Bind hvars
	for(i = 0; i < NUM_HVARS; i++)
	{
		LUA::BindHvar(L, i);
	}

	// Bind hdefs
	for(i = 0; i < NUM_HDEFS; i++)
	{
		LUA::BindHdef(L, i);
	}

	// Boilerplate: Bind a bunch of HVARs
	status = luaL_dostring(L,
		"function bind_hvar(dim, name, ...)\n"
		"	local a = { ... }\n"
		"	local t = (name and v3[name]) or v3\n"
		"	local name = name and (name .. '_') or ''\n"
		"	for i, v in ipairs(a) do\n"
		"		t['get_hvar' .. dim][v] = v3['get_' .. name .. v]\n"
		"		t['set_hvar' .. dim][v] = v3['set_' .. name .. v]\n"
		"	end\n"
		"end\n"
		"\n"

		"function bind_array(name, ...)\n"
		"	local a = { ... }\n"
		"	local t = (name and v3[name]) or v3\n"
		"	local name = name and (name .. '_') or ''\n"
		"	for i, v in ipairs(a) do\n"
		"		t[v] = _builtin_array(v3['get_' .. name .. v], v3['set_' .. name .. v])\n"
		"	end\n"
		"end\n"

		// Bind the global builtins
		"bind_hvar(0, false,\n"
		"	'systemtime', 'timer', 'lastpressed', 'joystick', \n"
		"	'up', 'down', 'left', 'right', 'b1', 'b2', 'b3', 'b4',\n"
		"	'xwin', 'ywin', 'cameratracking', 'entities', 'cameratracker',\n"
		"	'transcolor', '_skewlines', 'gamewindow', 'lastkey',\n"
		"	'playerstep', 'playerdiagonals'\n"
		")\n"
		"bind_array(false, 'key')\n"

		// Bind the clipboard builtins.
		"v3.clipboard = _builtin_struct()\n"
		"bind_hvar(0, 'clipboard', 'text')\n"
		
		// Bind the mouse related builtins
		"v3.mouse = _builtin_struct()\n"
		"bind_hvar(0, 'mouse', 'x', 'y', 'l', 'r', 'm', 'w')\n"

		// Bind the system date and time builtins
		"v3.sysdate = _builtin_struct()\n"
		"v3.systime = _builtin_struct()\n"
		"bind_hvar(0, 'sysdate', 'year', 'month', 'day', 'dayofweek')\n"
		"bind_hvar(0, 'systime', 'hour', 'minute', 'second')\n"

		// Bind the event builtins
		"v3.event = _builtin_struct()\n"
		"bind_hvar(0, 'event', 'tx', 'ty', 'zone', 'entity', 'sprite', 'param', 'entity_hit')\n"

		// Bind the joy builtins
		"v3.joy = _builtin_struct()\n"
		"bind_hvar(0, 'joy', 'active', 'up', 'down', 'left', 'right', 'analogx', 'analogy')\n"
		"bind_array('joy', 'button')\n"

		// Bind the dma builtins that almost nobody will use.
		"v3.dma = _builtin_struct()\n"
		"bind_array('dma', 'byte', 'word', 'quad', 'sbyte', 'sword', 'squad')\n"

		// Bind the entity builtins
		"v3.entity = _builtin_struct_collection()\n"
		"bind_hvar(1, 'entity',\n"
		"	'x', 'y', 'specframe', 'frame', 'hotx', 'hoty', 'hotw', 'hoth',\n"
		"	'framew', 'frameh', 'lucent', 'visible', 'movecode', 'face', 'speed',\n"
		"	'script', 'obstruct', 'obstructable', 'chr', 'description'\n"
		")\n"

		// Bind the current map builtins
		"v3.curmap = _builtin_struct()\n"
		"bind_hvar(0, 'curmap',\n"
		"	'w', 'h', 'startx', 'starty', 'name', 'rstring', \n"
		"	'music', 'tileset', 'path', 'savevsp' \n"
		")\n"

		// Bind the layer builtins
		"v3.layer = _builtin_struct_collection()\n"
		"bind_hvar(1, 'layer',\n"
		"	'w', 'h', 'visible', 'lucent', 'parallaxx', 'parallaxy' \n"
		")\n"

		// Bind the zone builtins
		"v3.zone = _builtin_struct_collection()\n"
		"bind_hvar(1, 'zone', 'name', 'event')\n"

		// Bind the sprite builtins
		"v3.sprite = _builtin_struct_collection()\n"
		"bind_hvar(1, 'sprite',\n"
		"	'x', 'y', 'sc', 'image', 'lucent', 'addsub', 'alphamap', \n"
		"	'thinkrate', 'thinkproc', 'xflip', 'yflip', 'ybase', 'addsub', 'alphamap', \n"
		"	'ent', 'silhouette', 'color', 'wait', 'onmap', 'layer', 'timer'\n"
		")\n"

		// Bind trigger builtins
		"v3.trigger = _builtin_struct()\n"
		"bind_hvar(0, 'trigger',\n"
		"	'on_step', 'after_step', 'before_entityscript', \n"
		"	'after_entityscript', 'on_entitycollide', 'after_playermove' \n"
		")\n"
	);
	if(status) ::err("Failed to load the LuaVerge hvar boilerplates!");
	
	luaL_dostring(L,
		"function v3.vpkloader(modulename)\n"
			// Find the source in a vpk.
		"	local modulepath = string.gsub(modulename, '%.', '/')\n"
		"	for path in string.gmatch(package.path, '([^;]+)') do\n"
		"		local filename = string.gsub(path, '%?', modulepath)\n"
		"		local f = v3.FileOpen(filename, v3.FILE_READ)\n"
		"		if f ~= 0 then\n"
		"			local function reader()\n"
		"				return v3.FileEOF(f) and '' or v3.FileReadLn(f)\n"
		"			end\n"
		"			\n"
					// Read and compile the chunk.
		"			chunk = assert(load(reader, filename))\n"
		"			\n"
					// Success?
		"			return chunk\n"
		"		end\n"
		"	end\n"
			// Failed to open it.
		"	return '\\n\\tno vpk\\'d module \\'' .. modulename .. '\\''\n"
		"end\n"
		"\n"

		// Add a few more package path rules that agree with our loader a lot more.
		"package.path = package.path .. ';?.lua;?/init.lua;?\\\\init.lua'\n"
		// Install the loader so that it's called just before the DLL loader
		"table.insert(package.loaders, 3, v3.vpkloader)\n"
	);
	// Boilerplate: Add a packfile loader.
	if(status) ::err("Failed to load the LuaVerge packfile loader!");

	LUA::InitGCHandleSystem(L);
}

#endif

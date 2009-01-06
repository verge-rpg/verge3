#include "xerxes.h"

#ifdef ENABLE_LUA

#include "lua_main.h"
#include <stdio.h>

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <memory>


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
	if(lua_pcall(L, 0, LUA_MULTRET, 0))
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
int LuaVerge_InitErrorHandler(lua_State *L)
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

// Ensures that the arguments passed to a function are correct.
void LuaVerge_VerifySignature(lua_State* L, int functionIndex)
{
	int providedArgumentCount = lua_gettop(L);
	int declaredArgumentCount = libfuncs[functionIndex].argumentTypes.size();
	int i = 0;
	bool varargs = false;

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
				char errorMessage[4096];
				sprintf(errorMessage, "Problem invoking %s: Argument #%d to function must be a int.", libfuncs[functionIndex].name.c_str(), i + 1);
				lua_pushstring(L, errorMessage);
				lua_error(L);
			}
			// Strings.  Need to check i + 1th stack index because Lua indices start at 1
			else if(libfuncs[functionIndex].argumentTypes[i] == t_STRING && !lua_isstring(L, i + 1))
			{
				char errorMessage[4096];
				sprintf(errorMessage, "Problem invoking %s: Argument #%d to function must be a string.", libfuncs[functionIndex].name.c_str(), i + 1);
				lua_pushstring(L, errorMessage);
				lua_error(L);
			}
		}
		// Now for varargs and stuff, we'll error if this is 'too many' args afterwards.
		else
		{
			// Varargs stacks only know how to deal with strings and ints
			// (For verification, checking if a string catches both).
			if(!lua_isstring(L, i + 1))
			{
				char errorMessage[4096];
				sprintf(errorMessage, "Problem invoking %s: Argument #%d to function must be a string or number.", libfuncs[functionIndex].name.c_str(), i);
				lua_pushstring(L, errorMessage);
				lua_error(L);
			}
		}
		i++;
	}

	if (i < declaredArgumentCount || (i > declaredArgumentCount && !varargs))
	{
		char errorMessage[4096];
		sprintf(errorMessage, "Problem invoking %s: Function expects %d arguments. (Got %d)", libfuncs[functionIndex].name.c_str(), declaredArgumentCount, i);
		lua_pushstring(L, errorMessage);
		lua_error(L);
	}
}

int LuaVerge_InvokeBuiltinFunc(lua_State* L)
{
	// Grab the function index upvalue
	int functionIndex = lua_tonumber(L, lua_upvalueindex(1));

	LuaVerge_VerifySignature(L, functionIndex);

	// TODO: ResolveOperand/ResolveString copycats but for Lua.
	// TODO: Varariadic functions in Lua.
	// TODO:	Make t_BOOL signatures for functions/vars, that are the same as t_INT in Verge,
	//			but have proper coercion and validation in Lua.

	// Calls the function by looking up in the dispatch table.
	VcFunctionImpl ptr = dispatchTable[functionIndex];
	if (ptr) {
		ptr();
	}

	//err("Woo! Calling function (index %d) %s", functionIndex, libfuncs[functionIndex].name.c_str());

	// TODO: Push return values onto the stack.
	// TODO: Return number of arguments returned by the callback we're wrapping.
	return 0;
}

void LuaVerge_BindFunction(lua_State* L, int functionIndex)
{
	// Don't bind all those empty name functions.
	if(libfuncs[functionIndex].name.length() == 0)
	{
		return;
	}

	// Push the v3 namespace
	lua_getglobal(L, "v3");
	// Push the name associated with the function
	lua_pushstring (L, libfuncs[functionIndex].name.c_str());
	// Push the function index as an upvalue so we can dispatch this function later.
	lua_pushnumber (L, (lua_Number) functionIndex);
	// Push a callback function. Pops the function index upvalue with it.
	lua_pushcclosure(L, &LuaVerge_InvokeBuiltinFunc, 1);
	// Pop the function value and then pop string name.
	// Translates to v3[func.name] = fptr
	lua_settable (L, -3);
	// And after that assignment, the v3 namespace is popped off.
	lua_pop(L, -1);
}

void LUA::bindApi()
{
	int i;

	LuaVerge_InitErrorHandler(L);

	// Create an empty table!
	lua_newtable(L);
	// Put the table we created into the global namespace as v3.
	lua_setglobal(L, "v3");

	// Bind the functions to the v3 namespace.
	for(i = 0; i < NUM_LIBFUNCS; i++)
	{
		LuaVerge_BindFunction(L, i);
	}
}

#endif

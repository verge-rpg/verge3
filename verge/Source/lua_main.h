/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.

#ifndef _LUA_MAIN_H
#define _LUA_MAIN_H

#ifdef ENABLE_LUA

#include "xerxes.h"
#include "opcodes.h"

#include <stdarg.h>

#include "g_script.h"

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

class LUA : public ScriptEngine, public MapScriptCompiler
{
	private:
		lua_State *L;
	public:
		// Internal userdata pointer with a gc metamethod that calls v3[destructor_name](handle)
		struct GCHandle
		{
			char destructor_name[256];
			int handle;
		};

		static int activeFunctionStackOffset;
		static int activeFunctionIndex;
		static void VerifyFunctionSignature(lua_State* L, int functionIndex);
		static void BindFunction(lua_State* L, int functionIndex);
		static int InvokeBuiltinFunction(lua_State* L);
		static int InitErrorHandler(lua_State* L);

		// GC Handles
		static void InitGCHandleSystem(lua_State* L);
		static int GCHandleConstruct(lua_State* L);
		static int GCHandleDestruct(lua_State* L);
		static int GCHandleToString(lua_State* L);

		//hvars
		void BindHvar(lua_State* L, int index);
		static int Get_Hvar(lua_State* L);
		static int Set_Hvar(lua_State* L);

		// hdefs
		static void BindHdef(lua_State* L, int index);

		LUA();
		~LUA();

		static void LuaError(lua_State *L, const char *text, ...);
		void LuaError(const char *text, ...);

		void loadFile(const char *fname);
		void compileSystem();
		
		//MapScriptCompiler implementations
		bool CompileMap(const char *fname);
		
		//ScriptEngine implementations
		void DisplayError(const std::string& msg)
		{
			::err(msg.c_str());
		}
		
		virtual void LoadMapScript(VFILE *f, const std::string& filename);
		virtual void ExecAutoexec();

		virtual bool ExecuteFunctionString(const std::string& func)
		{
			//the gettop/settop is to recover the stack from the user having returned a value from the callback function (we dont want one)
			int temp = lua_gettop(L);

			// Lookup our error callback.
			lua_getglobal(L, "__err");
			int errhandler = lua_gettop(L);

			// Push the function.
			lua_getglobal(L, func.c_str());
			// Check for existance.
			bool exists = lua_isfunction(L, -1);
			// Push arguments onto the stack
			for(int i = 0; i < argument_pass_list.size(); i++)
			{
				lua_pushstring(L, argument_pass_list[i].string_value.c_str());
			}

			if(exists)
			{
                invc++;
				if(lua_pcall(L, argument_pass_list.size(), 0, errhandler))
				{
					err("Error when calling '" + func + "'");
				}
				ArgumentPassClear();
                invc--;
			}

			lua_settop(L, temp);
			return exists;
		}

		virtual bool FunctionExists(const std::string& func)
		{
			lua_getglobal(L, func.c_str());
			bool ret = lua_isfunction(L, -1);
			lua_remove(L, -1);
			return ret;
		}

		virtual int ResolveOperand()
		{
			if(activeFunctionStackOffset < lua_gettop(L))
			{
				activeFunctionStackOffset++;
				if(lua_isboolean(L, activeFunctionStackOffset))
				{
					return lua_toboolean(L, activeFunctionStackOffset);
				}
				else
				{
					return (int)lua_tonumber(L, activeFunctionStackOffset);
				}
			}
			else
			{
				dump("Function '" + libfuncs[activeFunctionIndex].name + "'"
					+ " is trying to get more operands than are on the stack.");
				return 0;
			}
		}

		virtual std::string ResolveString()
		{
			if(activeFunctionStackOffset < lua_gettop(L))
			{
				activeFunctionStackOffset++;
				return lua_tostring(L, activeFunctionStackOffset);
			}
			else
			{
				dump("Function '" + libfuncs[activeFunctionIndex].name + "'"
					+ " is trying to get more operands than are on the stack.");
				return 0;
			}
		}

		// Get callback from argument list (Lua allocates a reference).
		virtual VergeCallback ResolveCallback()
		{
			VergeCallback cb;
			if(activeFunctionStackOffset < lua_gettop(L))
			{
				activeFunctionStackOffset++;

				// Push the function onto the stack temporarily, so we can pass to the registry (which pops it)
				lua_pushvalue(L, activeFunctionStackOffset);
				cb.functionIndex = luaL_ref(L, LUA_REGISTRYINDEX);
			}
			else
			{
				dump("Function '" + libfuncs[activeFunctionIndex].name + "'"
					+ " is trying to get more operands than are on the stack.");
			}
			return cb;
		}

		// Free callback (or else, memory bloat in Lua.).
		virtual void ReleaseCallback(VergeCallback& cb)
		{
			luaL_unref(L, LUA_REGISTRYINDEX, cb.functionIndex);
		}

		// Invoke callback.
		virtual void ExecuteCallback(VergeCallback& cb, bool calling_from_library)
		{
			//the gettop/settop is to recover the stack from the user having returned a value from the callback function (we dont want one)
			int temp = lua_gettop(L);

			// Lookup our error callback.
			lua_getglobal(L, "__err");
			int errhandler = lua_gettop(L);

			lua_rawgeti(L, LUA_REGISTRYINDEX, cb.functionIndex);

            invc++;
			// Call global function by string name.
			if(lua_isstring(L, -1))
			{
				ExecuteFunctionString(lua_tostring(L, -1));
			}
			// Call function reference on stack.
			else if(lua_isfunction(L, -1))
			{
				// Push arguments onto the stack (if any.)
				for(int i = 0; i < argument_pass_list.size(); i++)
				{
					lua_pushstring(L, argument_pass_list[i].string_value.c_str());
				}				
				if(lua_pcall(L, argument_pass_list.size(), 0, errhandler))
				{
					err("Error when invoking callback function.");
				}
				ArgumentPassClear();
			}
            invc--;

			lua_settop(L, temp);
		}

		virtual bool CheckForVarargs()
		{
			return libfuncs[activeFunctionIndex].argumentTypes.size() <= lua_gettop(L);
		}

		virtual void ReadVararg(std::vector<argument_t>& vararg)
		{
			int start = libfuncs[activeFunctionIndex].argumentTypes.size();
			int argumentCount = lua_gettop(L);

			if (!CheckForVarargs())
			{
				return;
			}

			for(int i = start; i <= argumentCount; i++)
			{
				argument_t argument;
				argument.int_value = (int)lua_tonumber(L, i);
				argument.string_value = lua_tostring(L, i);

				// Whatever, doesn't really matter, coercion is 100% possible
				// between numeric strings and numbers. The signature verifier uses6
				// lua_isnumber() so we can really ignore this here.
				argument.type_id = t_STRING;

				// Put the argument in the list!
				vararg.push_back(argument);
			}
		}
	private:
		void err(const std::string &msg)
		{
			::err("%s: %s",msg.c_str(),lua_tostring(L, -1));
		}

		void dump(const std::string &msg)
		{
			::err("%s",msg.c_str());
			//todo - dump stacktrace or offending line or something
		}

		void bindApi();
};

#endif //ENABLE_LUA

#endif //_LUA_MAIN_H

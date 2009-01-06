#include "xerxes.h"
#include "opcodes.h"

#ifdef ENABLE_LUA

#include "g_script.h"

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

class LUA : public ScriptEngine, public MapScriptCompiler {
public:
	LUA() {
		L = lua_open();
		luaL_openlibs(L);
		bindApi();
	}
	~LUA() {
		lua_close(L);
	}

	void loadFile(const char *fname);
	void compileSystem();
	
	//MapScriptCompiler implementations
	bool CompileMap(const char *fname);
	
	//ScriptEngine implementations
	void DisplayError(const StringRef& msg) {
		::err(msg.c_str());
	}
	
	virtual void LoadMapScript(VFILE *f, CStringRef filename);
	virtual void ExecAutoexec();
	virtual bool ExecuteFunctionString(const StringRef& func) {
		//the gettop/settop is to recover the stack from the user having returned a value from the callback function (we dont want one)
		int temp = lua_gettop(L);

		lua_getglobal(L, "__err"); // Lookup our error callback.
		int errhandler = lua_gettop(L);

		lua_getglobal(L, func.c_str());
		bool ret = lua_isfunction(L,-1);

		if(ret) if(lua_pcall(L,0,0,errhandler)) err("Error when calling '" + func.str() + "'");

		lua_settop(L,temp);
		return ret;
	}
	virtual bool FunctionExists(const StringRef& func) {
		lua_getglobal(L, func.c_str());
		bool ret = lua_isfunction(L, -1);
		lua_remove(L, -1);
		return ret;
	}

private:
	lua_State *L;

	void err(const std::string &msg) {
		::err("%s: %s",msg.c_str(),lua_tostring(L, -1));
	}

	void dump(const std::string &msg) {
		::err("%s",msg.c_str());
		//todo - dump stacktrace or offending line or something
	}

	void bindApi();
};

#endif
#include "xerxes.h"
#include "g_script.h"
#include <luabind/luabind.hpp>

extern "C" {
#include <lualib.h>
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
	void DisplayError(const std::string &msg) {
		::err(msg.c_str());
	}
	
	virtual void LoadMapScript(VFILE *f);
	virtual void ExecAutoexec();
	virtual bool ExecuteFunctionString(const std::string &func) {
		//the gettop/settop is to recover the stack from the user having returned a value from the callback function (we dont want one)
		int temp = lua_gettop(L);
		lua_getglobal(L, func.c_str());
		bool ret = lua_isfunction(L,-1);
		if(ret) lua_call(L,0,0);
		lua_settop(L,temp);
		return ret;
	}
	virtual bool FunctionExists(const std::string &func) {
		lua_getglobal(L, func.c_str());
		bool ret = lua_isfunction(L, -1);
		lua_remove(L, -1);
		return ret;
	}

private:
	lua_State *L;

	void err(std::string msg) {
		::err("%s: %s",msg.c_str(),lua_tostring(L, -1));
	}

	void dump(std::string msg) {
		::err("%s",msg.c_str());
		//todo - dump stacktrace or offending line or something
	}

	void bindApi();

};
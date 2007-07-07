#include "xerxes.h"
#include "g_script.h"
#include <luabind/luabind.hpp>

extern "C" {
#include <lualib.h>
}

class LUA : public ScriptEngine {
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
	virtual void ExecAutoexec();

	//ScriptEngine implementations
	virtual bool ExecuteFunctionString(const char *s) {
		return false;
	}
	virtual bool FunctionExists(const char *str) {
		lua_getglobal(L, str);
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
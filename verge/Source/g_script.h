#ifndef _G_SCRIPT
#define _G_SCRIPT

void HookTimer();
void HookRetrace();
image *ImageForHandle(int handle);
void FreeImageHandle(int handle);
void SetHandleImage(int handle, image *img);
int HandleForImage(image *img);

class ScriptEngine;
extern ScriptEngine *se;

class MapScriptCompiler {
public:
	virtual bool CompileMap(const char *fname) = 0;
};

class ScriptEngine {
public:
	void Error(const char *s, ...);

	virtual bool ExecuteFunctionString(const std::string &script) = 0;
	virtual bool FunctionExists(const std::string &func) = 0;
	virtual void ExecAutoexec() = 0;
	virtual void LoadMapScript(VFILE *f) = 0;
	virtual void DisplayError(const std::string &msg) = 0;

	//script services
	static void Exit(std::string message);
	static void SetButtonJB(int b, int jb);
	static void HookButton(int b, std::string s);
	static void HookKey(int k, std::string s);
	static void HookTimer(std::string s);
	static void HookRetrace(std::string s);
	static void Log(std::string s);
	static void MessageBox(std::string msg);
	static int Random(int min, int max);
	static void SetAppName(std::string s);
	static void SetButtonKey(int b, int k);
	static void SetRandSeed(int seed);
	static void SetResolution(int v3_xres, int v3_yres);
	static void Unpress(int n);
	static void UpdateControls();

	//VI.d. Map Functions
	static void Map(const std::string &map);


};



#endif
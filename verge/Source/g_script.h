#ifndef _G_SCRIPT
#define _G_SCRIPT

#include "xerxes.h"

class ScriptEngine {
public:
	virtual bool ExecuteFunctionString(const char *s) = 0;
	virtual bool FunctionExists(const char *str) = 0;
	virtual void ExecAutoexec() = 0;
};

void HookTimer();
void HookRetrace();
image *ImageForHandle(int handle);
void FreeImageHandle(int handle);
void SetHandleImage(int handle, image *img);
int HandleForImage(image *img);

extern ScriptEngine *se;

#endif
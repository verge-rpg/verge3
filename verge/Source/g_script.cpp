#include "xerxes.h"
#include "g_script.h"

ScriptEngine *se;

void ScriptEngine::Error(const char *str, ...) { 
  	va_list argptr;
	char msg[256];

	va_start(argptr, str);
	vsprintf(msg, str, argptr);
	va_end(argptr);
	DisplayError(msg);
}

void HookTimer()
{
	if (timerfunc != "")
	{
		while (hooktimer)
		{
			se->ExecuteFunctionString(timerfunc);
			hooktimer--;
		}
	}
}

void HookRetrace()
{
	if (renderfunc != "")
		se->ExecuteFunctionString(renderfunc);
}

image *ImageForHandle(int handle)
{
	if (handle == 0)
		se->Error("ImageForHandle() - Null image reference, probably an uninitialized image handle");

	if (handle<0 || handle >= Handle::getHandleCount(HANDLE_TYPE_IMAGE) )
		se->Error("ImageForHandle() - Image reference is bogus! (%d)", handle);

	image* ptr = (image*) Handle::getPointer(HANDLE_TYPE_IMAGE,handle);

	if (ptr == NULL)
		se->Error("ImageForHandle() - Image reference is valid but no image is allocated for this handle. You may have mistakenly freed it and continued to use it.");
	return ptr;
}


void FreeImageHandle(int handle)
{
	Handle::free(HANDLE_TYPE_IMAGE,handle);
}

void SetHandleImage(int handle, image *img)
{
	Handle::setPointer(HANDLE_TYPE_IMAGE, handle, (void*)img);
}

int HandleForImage(image *img)
{
	return Handle::alloc(HANDLE_TYPE_IMAGE, img);
}

//-------------

void ScriptEngine::Exit(std::string message) { err("%s",message.c_str()); }

void ScriptEngine::SetButtonJB(int b, int jb) {
	switch (b)
	{
		case 1: j_b1 = jb; break;
		case 2: j_b2 = jb; break;
		case 3: j_b3 = jb; break;
		case 4: j_b4 = jb; break;
	}
}

void ScriptEngine::HookButton(int b, std::string s) {
	if (b<0 || b>3) return;
	bindbutton[b] = s;
}

void ScriptEngine::HookKey(int k, std::string s) {
	if (k<0 || k>127) return;
	bindarray[k] = s;
}

void ScriptEngine::HookTimer(std::string s) {
	hooktimer = 0;
	timerfunc = s;
}

void ScriptEngine::HookRetrace(std::string s) {
	renderfunc = s;
}

void ScriptEngine::Log(std::string s) { log(s.c_str()); }
void ScriptEngine::MessageBox(std::string msg) { showMessageBox(msg); }
int ScriptEngine::Random(int min, int max) { return rnd(min, max); }
void ScriptEngine::SetAppName(std::string s) { setWindowTitle(s.c_str()); }

void ScriptEngine::SetButtonKey(int b, int k) {
	switch (b)
	{
		case 1: k_b1 = k; break;
		case 2: k_b2 = k; break;
		case 3: k_b3 = k; break;
		case 4: k_b4 = k; break;
		// Overkill (2006-06-25): Can set the directionals as well, now.
		case 5: k_up = k; break;
		case 6: k_down = k; break;
		case 7: k_left = k; break;
		case 8: k_right = k; break;
	}
}

void ScriptEngine::SetRandSeed(int seed) { arandseed(seed); }
void ScriptEngine::SetResolution(int v3_xres, int v3_yres) { vid_SetMode(v3_xres, v3_yres, vid_bpp, vid_window, MODE_SOFTWARE); }

void ScriptEngine::Unpress(int n) {
	switch (n)
	{
		case 0: if (b1) UnB1(); if (b2) UnB2(); if (b3) UnB3(); if (b4) UnB4();	break;
		case 1: if (b1) UnB1(); break;
		case 2: if (b2) UnB2(); break;
		case 3: if (b3) UnB3(); break;
		case 4: if (b4) UnB4(); break;
		case 5: if (up) UnUp(); break;
		case 6: if (down) UnDown(); break;
		case 7: if (left) UnLeft(); break;
		case 8: if (right) UnRight(); break;
	}
}

void ScriptEngine::UpdateControls() { ::UpdateControls(); }

//VI.d. Map Functions
void ScriptEngine::Map(const std::string &map) {
	strcpy(mapname, map.c_str());
	die = 1;
	done = 1;
}
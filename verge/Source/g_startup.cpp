/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


/******************************************************************
 * verge3: g_startup.cpp                                          *
 * copyright (c) 2001 vecna                                       *
 ******************************************************************/

#include "xerxes.h"
#include "garlick.h"
#include "lua_main.h"
#include "opcodes.h"

#include <memory>
#include <functional>

/****************************** data ******************************/

int v3_xres=320, v3_yres=240, v3_bpp;
int v3_window_xres=0, v3_window_yres = 0;
// Overkill (2010-04-29): Aspect ratio enforcing.
ScaleFormat v3_scale_win = SCALE_FORMAT_ASPECT, v3_scale_full = SCALE_FORMAT_STRETCH;

bool windowmode = true;
bool sound = true;
bool cheats = false;
char mapname[255];
bool releasemode = false;
bool automax = true;
bool decompile = false;
bool editcode = false;
int gamerate = 100;
int soundengine = 0;
bool use_lua = false;
bool vc_oldstring = false;
bool vc_redefinelibfuncs = false;
bool showpage_auto_sleep = true;
int last_showpage = 0;

VCCore *vc;

#ifdef ALLOW_SCRIPT_COMPILATION
VCCompiler *vcc = 0;
#endif

/****************************** code ******************************/

void LoadConfig()
{
	cfg_Init("verge.cfg");
	cfg_SetDefaultKeyValue("startmap", "");
}

void ApplyConfig()
{
	if (cfg_KeyPresent("lua"))
		use_lua = cfg_GetIntKeyValue("lua") ? true : false;
	if (cfg_KeyPresent("xres"))
		v3_xres = cfg_GetIntKeyValue("xres");
	if (cfg_KeyPresent("yres"))
		v3_yres = cfg_GetIntKeyValue("yres");
    // Overkill (2010-04-29): Scaling policies.
    if (cfg_KeyPresent("scalewin"))
    {
        int value = cfg_GetIntKeyValue("scalewin");
        if(value >= 0 && value < SCALE_FORMAT_COUNT)
        {
            v3_scale_win = (ScaleFormat) value;
        }
    }
    if (cfg_KeyPresent("scalefull"))
    {
        int value = cfg_GetIntKeyValue("scalefull");
        if(value >= 0 && value < SCALE_FORMAT_COUNT)
        {
            v3_scale_full = (ScaleFormat) value;
        }
    }
	if (cfg_KeyPresent("bpp"))
		v3_bpp = cfg_GetIntKeyValue("bpp");

	if (cfg_KeyPresent("window_x_res"))
		v3_window_xres = cfg_GetIntKeyValue("window_x_res");
	if (cfg_KeyPresent("window_y_res"))
		v3_window_yres = cfg_GetIntKeyValue("window_y_res");

	if (cfg_KeyPresent("windowmode"))
		windowmode = cfg_GetIntKeyValue("windowmode") ? true : false;
	if (cfg_KeyPresent("nosound"))
		sound = cfg_GetIntKeyValue("nosound") ? false : true;
	if (cfg_KeyPresent("soundengine"))
		soundengine = cfg_GetIntKeyValue("soundengine");
	if (cfg_KeyPresent("automax"))
		automax = cfg_GetIntKeyValue("automax") ? true : false;
	if (cfg_KeyPresent("tilesize"))
		G_TILESIZE = cfg_GetIntKeyValue("tilesize");
	if (cfg_KeyPresent("startmap"))
		strcpy(mapname, cfg_GetKeyValue("startmap").c_str());
	if (cfg_KeyPresent("vcverbose"))
		verbose = cfg_GetIntKeyValue("vcverbose");
//	if (cfg_KeyPresent("paranoid"))                FIXME
//		vc_paranoid = cfg_GetIntKeyValue("paranoid");
	if (cfg_KeyPresent("arraycheck"))
		vc_arraycheck = cfg_GetIntKeyValue("arraycheck");
	if (cfg_KeyPresent("appname"))
		setWindowTitle(cfg_GetKeyValue("appname").c_str());
	if (cfg_KeyPresent("releasemode"))
		releasemode = cfg_GetIntKeyValue("releasemode") ? true : false;
	if (cfg_KeyPresent("gamerate"))
		gamerate = cfg_GetIntKeyValue("gamerate");
	if (cfg_KeyPresent("v3isuberlikethetens"))
		cheats = true;
	if (cfg_KeyPresent("decompile"))
		decompile = true;
	if (cfg_KeyPresent("editcode"))
		editcode = cfg_GetIntKeyValue("editcode") ? true : false;
	if (cfg_KeyPresent("logconsole"))
	{
		logconsole = true;
		initConsole();
	} else if (cfg_KeyPresent("logconsole-normalstdout")) {
		logconsole = true;
	}
    if (cfg_KeyPresent("oldstring"))
        vc_oldstring = true;
    if (cfg_KeyPresent("redefinelibfuncs"))
        vc_redefinelibfuncs = true;

	if (cfg_KeyPresent("mount1"))
		MountVFile(cfg_GetKeyValue("mount1").c_str());
	if (cfg_KeyPresent("mount2"))
		MountVFile(cfg_GetKeyValue("mount2").c_str());
	if (cfg_KeyPresent("mount3"))
		MountVFile(cfg_GetKeyValue("mount3").c_str());

    if (cfg_KeyPresent("autosleep"))
		showpage_auto_sleep = cfg_GetIntKeyValue("autosleep") ? true : false;

	void platform_ProcessConfig();
	platform_ProcessConfig();

	#ifndef ALLOW_SCRIPT_COMPILATION
	releasemode = true;
	editcode = false;
	#endif

	#ifndef ENABLE_LUA
	if(use_lua) err("User asked for lua, but build does not have lua enabled!");
	#endif
}

int getInitialWindowXres() {
	return v3_window_xres;
}

int getInitialWindowYres() {
	return v3_window_yres;
}

void InitVideo()
{
	//initialize the image handles
	//allocate one dummy, one for screen, and one for the active vsp
	Handle::forceAlloc(HANDLE_TYPE_IMAGE,3);

	if (!windowmode)
	{
		int result = vid_SetMode(v3_xres, v3_yres, v3_bpp, 0, MODE_SOFTWARE);
		if (!result)
			result = vid_SetMode(v3_xres, v3_yres, v3_bpp, 1, MODE_SOFTWARE);
		if (!result)
			err("Could not set video mode!");
		return;
	}
	if (windowmode)
	{
		int result = vid_SetMode(v3_xres, v3_yres, v3_bpp, 1, MODE_SOFTWARE);
		if (!result)
			err("Could not set video mode!");
		return;
	}
}

#ifdef __EMSCRIPTEN__
int wasm_scriptBusyWaitCounter;
int wasm_scriptTimeoutCallCounter;
double wasm_scriptTimeSinceLastFrame;

EM_JS(void, wasm_nextFrame_, (), 
{
    return Asyncify.handleSleep(requestAnimationFrame);
});

void wasm_nextFrame()
{
	wasm_nextFrame_();

	wasm_scriptBusyWaitCounter = 0;
	wasm_scriptTimeoutCallCounter = 0;	

	double time = EM_ASM_DOUBLE(
	{
		return performance.now();
	});	
	wasm_scriptTimeSinceLastFrame = time;
}

void wasm_detectScriptTimeout_()
{
	double time = EM_ASM_DOUBLE(
	{
		return performance.now();
	});

	double delta = time - wasm_scriptTimeSinceLastFrame;

	if (delta >= WASM_SCRIPT_TIMEOUT_TIME_LIMIT_MS)
	{
		log("wasm_detectScriptTimeout: script timeout hit, yielding until next frame", time, wasm_scriptTimeSinceLastFrame, delta);

		wasm_nextFrame();
	}
}
#endif

void ShowPage()
{
	HookTimer();
	TimedProcessSprites();
	RenderSprites();
	Flip();

#ifdef __EMSCRIPTEN__
    wasm_nextFrame();
#else
    if (systemtime - last_showpage <= 0 && showpage_auto_sleep)
    {
        Sleep(1);
    }
#endif

    last_showpage = systemtime;
}

#ifndef NOSPLASHSCREEN
#ifdef __APPLE__
#include "macsplash.h"
#else
#include "vcsplash.h"
#endif
#endif

#ifdef ALLOW_SCRIPT_COMPILATION
void DisplayCompileImage()
{
#ifndef NOSPLASHSCREEN
	FILE *f = fopen("__temp__img$$$.gif","wb");
	if (!f) return; // oh well, we tried
	fwrite(compileimg, 1, COMPILEIMG_LEN, f);
	fclose(f);
	image *splash = xLoadImage("__temp__img$$$.gif");
	remove("__temp__img$$$.gif");
	DrawRect(0, 0, screen->width, screen->height, 0, screen);
	Blit((screen->width/2)-(splash->width/2), (screen->height/2)-(splash->height/2), splash, screen);
	delete splash;
	ShowPage();
#endif
}
#endif

//---
//setup garlick to use vfile
void *Garlick_vf_open(const char *fname) { return vopen(fname); }
void Garlick_vf_close(void *handle) { vclose((VFILE*)handle); }
size_t Garlick_vf_read(void *ptr, size_t elemsize, size_t amt, void *handle) { return vread((char *)ptr, elemsize*amt,(VFILE*)handle); }
long Garlick_vf_tell(void *handle) { return vtell((VFILE*)handle); }
int Garlick_vf_seek(void *handle, long offset, int origin) { vseek((VFILE*)handle,offset,origin); return 0; }
void Garlick_error(const char *msg) { err(msg); }
void InitGarlick() {
	Garlick_cb_open = Garlick_vf_open;
	Garlick_cb_close = Garlick_vf_close;
	Garlick_cb_read = Garlick_vf_read;
	Garlick_cb_tell = Garlick_vf_tell;
	Garlick_cb_seek = Garlick_vf_seek;
	Garlick_cb_error = Garlick_error;
}

#ifdef ALLOW_SCRIPT_COMPILATION
bool CompileMaps(const char *ext, MapScriptCompiler *compiler, char *directory = NULL)
{
	if (!directory)
		directory = ".";

	std::string pattern = std::string(directory);
	pattern.append("/*");

	std::vector<std::string> filenames;
	listFilePattern(filenames, pattern.c_str());
	for(std::vector<std::string>::iterator i = filenames.begin();
		i != filenames.end();
		i++)
	{
		if (ExtensionIs(i->c_str(),"map"))
		{
			std::string fullpath(directory);
			fullpath.append("/");
			fullpath.append(*i);

			char *s = stripext(fullpath.c_str());
			if (Exist(va("%s.%s", s,ext))) 
				if(!compiler->CompileMap(s))
					return false;
		}
		else if (i->at(0) != '.')
		{
			// for now, if it's not a .map then try to use it as a directory
			std::string newpath(directory);
			newpath.append("/");
			newpath.append(*i);
			CompileMaps(ext,compiler,(char *)newpath.c_str());
		}
	}


	//log ("");
	return true;
}
#endif

void parseCommandlineConfigOption(const char* arg, size_t arglen, size_t start_offset, bool force_config)
{
	size_t equal_pos = SIZE_MAX;
	for (size_t i = start_offset; i < arglen; i++)
	{
		if (arg[i] == '=')
		{
			equal_pos = i;
		}
	}
	
	if (equal_pos != SIZE_MAX)
	{
		StringRef key(arg + start_offset, arg + equal_pos);
		StringRef value(arg + equal_pos + 1, arg + arglen);

		if (force_config)
		{
			cfg_SetKeyValue(key, value);
		}
		else
		{
			cfg_SetDefaultKeyValue(key, value);
		}
	}
	else
	{
		err("Argument %s is missing =value", arg);
	}
}

void _main(int argc, char** argv)
{
    vc_initBuiltins();
    vc_initLibrary();

    InitGarlick();
    Handle::init();

    strcpy(mapname, "");

    LoadConfig();
    
    for (size_t i = 1; i <= argc; i++)
    {
		char* arg = argv[i];
		size_t arglen = strlen(arg);

		constexpr size_t OPTION_ARG_PREFIX_LENGTH = 3;

		if (arglen >= OPTION_ARG_PREFIX_LENGTH && arg[0] == '-' && arg[2] == ':')
		{
			switch (arg[1])
			{
				case 'c': parseCommandlineConfigOption(arg, arglen, OPTION_ARG_PREFIX_LENGTH, false); break;
				case 'C': parseCommandlineConfigOption(arg, arglen, OPTION_ARG_PREFIX_LENGTH, true); break;
				default: err("Argument %s not supported", arg); break;
			}
		}
		else
		{
			if (arglen >= sizeof(mapname))
				err("Mapname argument too long!");
			strcpy(mapname, arg);
		}
    }

	ApplyConfig();

    InitVideo();

    mouse_Init();
    InitKeyboard();
    joy_Init();
    InitScriptEngine();

    //---cross-platform plugins initialization
    //	extern void p_datastructs();
    //p_datastructs();
    //---------

    gameWindow->setTitle(APPNAME);

    if (sound) snd_Init(soundengine);

    win_movie_init();
    ResetSprites();
    timer_Init(gamerate);

#ifdef ALLOW_SCRIPT_COMPILATION
    if (!releasemode)
        vcc = new VCCompiler();
#endif

#ifdef ALLOW_SCRIPT_COMPILATION
    if (editcode)
	{
        if (releasemode)
		{
            err("Cannot edit code in release mode.");
        }
        if (!windowmode)
		{
            err("Cannot edit code in full-screen mode.");
        }
        InitEditCode();
    }
#endif

#ifdef ENABLE_LUA
    LUA *lua;

    if (use_lua)
        se = lua = new LUA();
#endif
	
#ifdef ALLOW_SCRIPT_COMPILATION
    if (!releasemode)
    {
        DisplayCompileImage();
#ifdef ENABLE_LUA
        if (use_lua)
		{
            lua->compileSystem();
            CompileMaps("lua",lua);
        }
		else 
#endif
        {
            bool result = vcc->CompileAll();
            if (!result) err(vcc->errmsg);
            vcc->ExportSystemXVC();
            result = CompileMaps("vc",vcc);
            if (!result) err(vcc->errmsg);
        }
    }
#endif

    if (!use_lua)
	{
        se = vc = new VCCore();
        if (decompile)
            vc->Decompile();
    }
	
    se->ExecAutoexec();
}

//---
void xmain(int argc, char *argv[])
{
	_main(argc, argv);
	while (true && strlen(mapname)) // main game loop
	{
	  	Engine_Start(mapname);
	}
	err(""); // exit!
}

void xtestmain(int argc, char* argv[])
{
#ifdef __WIN32__
	// hMainInst = hCurrentInst;
	DesktopBPP = GetDeviceCaps(GetDC(nullptr), BITSPIXEL);
	v3_bpp = DesktopBPP;
	//dd_init();
	setWindowTitle("Test Verge3");

	srand(timeGetTime());
	log_Init(true);

	_main(argc, argv);
#endif
}

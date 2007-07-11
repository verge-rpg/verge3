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

#include <memory>
#include <functional>

/****************************** data ******************************/

int v3_xres=320, v3_yres=240;
bool windowmode=true;
bool sound=true;
bool cheats=false;
char mapname[255];
bool releasemode=false;
bool automax = true;
bool decompile = false;
bool editcode = false;
int gamerate = 100;
int soundengine = 0;
bool use_lua = false;

VCCompiler *vcc;
VCCore *vc;

/****************************** code ******************************/

void LoadConfig()
{
	cfg_Init("verge.cfg");
	cfg_SetDefaultKeyValue("startmap", "");

	if (cfg_KeyPresent("lua"))
		use_lua = cfg_GetIntKeyValue("lua") ? true : false;
	if (cfg_KeyPresent("xres"))
		v3_xres = cfg_GetIntKeyValue("xres");
	if (cfg_KeyPresent("yres"))
		v3_yres = cfg_GetIntKeyValue("yres");
	if (cfg_KeyPresent("windowmode"))
		windowmode = cfg_GetIntKeyValue("windowmode") ? true : false;
	if (cfg_KeyPresent("nosound"))
		sound = cfg_GetIntKeyValue("nosound") ? false : true;
	if (cfg_KeyPresent("soundengine"))
		soundengine = cfg_GetIntKeyValue("soundengine");
	if (cfg_KeyPresent("automax"))
		automax = cfg_GetIntKeyValue("automax") ? true : false;
	if (cfg_KeyPresent("startmap"))
		strcpy(mapname, cfg_GetKeyValue("startmap"));
	if (cfg_KeyPresent("vcverbose"))
		verbose = cfg_GetIntKeyValue("vcverbose");
//	if (cfg_KeyPresent("paranoid"))                FIXME
//		vc_paranoid = cfg_GetIntKeyValue("paranoid");
	if (cfg_KeyPresent("arraycheck"))
		vc_arraycheck = cfg_GetIntKeyValue("arraycheck");
	if (cfg_KeyPresent("appname"))
		setWindowTitle(cfg_GetKeyValue("appname"));
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
	}

	if (cfg_KeyPresent("mount1"))
		MountVFile(cfg_GetKeyValue("mount1"));
	if (cfg_KeyPresent("mount2"))
		MountVFile(cfg_GetKeyValue("mount2"));
	if (cfg_KeyPresent("mount3"))
		MountVFile(cfg_GetKeyValue("mount3"));
}

void InitVideo()
{
	//initialize the image handles
	//allocate one dummy, one for screen, and one for the active vsp
	Handle::forceAlloc(HANDLE_TYPE_IMAGE,3);


	if (!windowmode)
	{
		int result = vid_SetMode(v3_xres, v3_yres, 32, 0, MODE_SOFTWARE);
		if (!result)
			result = vid_SetMode(v3_xres, v3_yres, DesktopBPP, 1, MODE_SOFTWARE);
		if (!result)
			err("Could not set video mode!");
		return;
	}
	if (windowmode)
	{
		int result = vid_SetMode(v3_xres, v3_yres, DesktopBPP, 1, MODE_SOFTWARE);
		if (!result)
			err("Could not set video mode!");
		return;
	}
}

void ShowPage()
{
	HookTimer();
	TimedProcessSprites();
	RenderSprites();
	Flip();
}

#ifndef NOSPLASHSCREEN
#ifdef __APPLE__
#include "macsplash.h"
#endif
#ifdef __WIN32__
#include "vcsplash.h"
#endif
#endif

void DisplayCompileImage()
{
#ifndef NOSPLASHSCREEN
	FILE *f = fopen("__temp__img$$$.gif","wb");
	if (!f) return; // oh well, we tried
	fwrite(compileimg, 1, COMPILEIMG_LEN, f);
	fclose(f);
	image *splash = xLoadImage("__temp__img$$$.gif");
	remove("__temp__img$$$.gif");
	Rect(0, 0, screen->width, screen->height, 0, screen);
	Blit((screen->width/2)-(splash->width/2), (screen->height/2)-(splash->height/2), splash, screen);
	delete splash;
	ShowPage();
#endif
}

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


bool CompileMaps(const char *ext, MapScriptCompiler *compiler)
{
	std::vector<std::string> filenames = listFilePattern("*.map");
	for(std::vector<std::string>::iterator i = filenames.begin();
		i != filenames.end();
		i++)
	{
		char *s = stripext(i->c_str());
		if (Exist(va("%s.%s", s,ext))) {
			if(!compiler->CompileMap(s))
				return false;
			}
	}
	log ("");
	return true;
}



//---
void xmain(int argc, char *argv[])
{
	InitGarlick();
	Handle::init();
	LoadConfig();
	InitVideo();

	mouse_Init();
	InitKeyboard();
	joy_Init();
	InitScriptEngine();

	//---cross-platform plugins initialization
	extern void p_datastructs();
	p_datastructs();
	//---------

	gameWindow->setTitle(APPNAME);

	if (sound) snd_Init(soundengine);

	win_movie_init();
	ResetSprites();
	timer_Init(gamerate);

	strcpy(mapname,"");
	if (argc == 2)
	{
		if (strlen(argv[1]) > 254)
			err("Mapname arguement too long!");
		strcpy(mapname, argv[1]);
	}

	// always make a compiler, even in release mode,
	// just so we don't need to check for its
	// presence everywhere
	vcc = new VCCompiler();

	if(editcode) {
		if(releasemode) {
			err("Cannot edit code in release mode.");
		}
		if(!windowmode) {
			err("Cannot edit code in full-screen mode.");
		}
		InitEditCode();
	}

	LUA *lua;

	if(use_lua)
		se = lua = new LUA();
	
	if (!releasemode)
	{
		DisplayCompileImage();
		if(use_lua) {
			lua->compileSystem();
			CompileMaps("lua",lua);
		} else {
			bool result = vcc->CompileAll();
			if (!result) err(vcc->errmsg);
			vcc->ExportSystemXVC();
			result = CompileMaps("vc",vcc);
			if (!result) err(vcc->errmsg);
		}
	}

	if(!use_lua) {
		se = vc = new VCCore();
		if (decompile)
			vc->Decompile();
	}

	se->ExecAutoexec();

	while (true && strlen(mapname))
		Engine_Start(mapname);
	err("");
}

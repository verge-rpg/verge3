#include "xerxes.h"
#include <psptypes.h>
#include <psprtc.h>
#include <pspdebug.h>
#include <pspgu.h>
#include <pspdisplay.h>
#include <malloc.h>

bool AppIsForeground = true;
int DesktopBPP = 61;
int psp_tyres;

void psp_VidClose(void) {}
AuxWindow *psp_createAuxWindow(void) { return 0; }
AuxWindow *psp_findAuxWindow(int handle) { return 0; }

class PspWindow : public AuxWindow
{
public:
	bool bMouseInside;
	PspWindow() { bMouseInside = true; }
public:
	virtual void dispose() { err("Disposing main pspwindow?"); }
	virtual int getHandle() { return 0; };
	virtual int getImageHandle() { return 1; }
	virtual image* getImage() { return screen; }
	virtual int getXres() { return vid_xres; }
	virtual int getYres() { return vid_yres; }
	virtual int getWidth() { return vid_xres; }
	virtual int getHeight() { return vid_yres; }
	virtual void setPosition(int x, int y) { log("setPosition called on psp main window?"); }
	virtual void setResolution(int w, int h) { log("setResolution called on psp main window?"); }
	virtual void setSize(int w, int h) { log("setSize called on psp main window?"); }
	virtual void setVisibility(bool vis) { log("setVisability called on psp main window?"); }
	virtual void setTitle(char *title) { log("setTitle called on psp main window?"); }
	virtual void positionCommand(int command, int arg1, int arg2) { log("positionCOmmand called on psp main window?"); }
};

//we have hacked a reference to timer code
//we need to update the timer somewhere; showpage seems like a good enough place perhaps
void timer_update(); 
void psp_VidFlip(void) {
	timer_update();
	void *backBuffer = psp_showPage(screen,psp_tyres);
	screen->data = (void *)(0x04000000 | (int)backBuffer);
}


void dd_RegisterBlitters(); //in vid_ddblit.cpp
int psp_SetMode(int xres, int yres, int bpp, bool windowflag) {

	if(bpp != 61) return 0;

	//for now, no mode is allowed except for 320x240x16
	xres = 320;
	yres = 240;
	int txres = 512;
	int tyres = 256;
	psp_tyres = tyres;


	image *img = new image();
	img->shell = true;
	SetHandleImage(1,img);
	screen = img;
	//img->data = memalign(16,txres*tyres*2);
	img->data = (void *)0x04000000;
	img->bpp = bpp;
	img->width = xres;
	img->height = yres;
	img->pitch = txres;
	img->cx1 = 0;
	img->cx2 = xres-1;
	img->cy1 = 0;
	img->cy2 = yres-1;

	vid_xres = xres;
	vid_yres = yres;
	vid_window = false;
	vid_bpp = bpp;

	if(vid_bpp==15 || vid_bpp==16 || vid_bpp==61)
		vid_bytesperpixel = 2;
	else if(vid_bpp==32)
		vid_bytesperpixel = 4;
    
	dd_RegisterBlitters();

	return 1;
}

void LoadConfig();
int main(int argc, char **argv) {

	log_Init(true);

	psp_init();
	gameWindow = new PspWindow();

	LoadConfig();

	//init video hooks
	vid_Close = psp_VidClose;
	Flip = psp_VidFlip;
	vid_createAuxWindow = psp_createAuxWindow;
	vid_findAuxWindow = psp_findAuxWindow;

	xmain(argc,argv);
	return 0;
}

void initConsole() {}
void writeToConsole(char *text) {
}

unsigned int timeGetTime() {
	u64 tcurr;
	sceRtcGetCurrentTick(&tcurr);
	return (int)(tcurr/10000);
}

void Sleep(unsigned int msec) {
	//do nothing for now.
	//i object to the use of this
	//function in verge
	//even though it was likely myself
	//that put it there to begin with
}

char *clipboard_getText() { return ""; }
void clipboard_setText(char *text) {}
image *clipboard_getImage() { return 0; }
void clipboard_putImage(image *img) {}

//declared in xerxes.h
void err(char *s, ...) {

	va_list argptr;
	char msg[256];

	va_start(argptr, s);
	vsprintf(msg, s, argptr);
	va_end(argptr);

	pspDebugScreenInit();
	for(;;)
	{
		pspDebugScreenPrintf(msg);
		pspDebugScreenPrintf("\n");
		sceDisplayWaitVblankStart();
		sceGuSwapBuffers();
	}
}

void HandleMessages() {
	//etc.
}

int getYear() { return 0; }
int getMonth() { return 0; }
int getDay() { return 0; }
int getDayOfWeek() { return 0; }
int getHour() { return 0; }
int getMinute() { return 0; }
int getSecond() { return 0; }

void setWindowTitle(char *str) {}
void InitEditCode() {}
void AddSourceFile(string s) {}
std::vector<string> listFilePattern(string pattern) {
	return std::vector<string>();
}

void showMessageBox(string message) {}

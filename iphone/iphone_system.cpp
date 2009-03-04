#include "xerxes.h"


#include <sys/types.h>
#include <sys/time.h>
#include <dirent.h>
#include <glob.h>

// internal use, in mac_cocoa_util.mm
void doMessageBox(std::string msg);


quad systemtime = 0, timer = 0, vctimer = 0, hooktimer = 0;
quad key_timer, key_repeater, repeatedkey;

char *clipboard_getText() { return ""; }
void clipboard_setText(const char *text) {}
image *clipboard_getImage() { return 0; }
void clipboard_putImage(image *img) {}


int getYear() { return 0; }
int getMonth() { return 0; }
int getDay() { return 0; }
int getDayOfWeek() { return 0; }
int getHour() { return 0; }
int getMinute() { return 0; }
int getSecond() { return 0; }


void setWindowTitle(const char *str) {}
void InitEditCode() {}
void AddSourceFile(string s) {}
std::vector<string> listFilePattern(string pattern) {
	return std::vector<string>();
}

std::string GetSystemSaveDirectory(std::string name) { return ""; }

void timer_Init(int) {}


/* returns time in s since 1970 */
/* not suitable for movie timing */
unsigned int timeGetTime()
{
	return time(NULL);
}

// Returns a vector of filenames that match the given pattern.
// As you can see, it uses glob to get them, so this will now
// match any pattern intelligently.
void listFilePattern(std::vector<std::string> &res, CStringRef pattern)
{
	glob_t pglob;
	
	glob(pattern.c_str(),0,0,&pglob);
	
	int i;
	for (i = 0; i < pglob.gl_pathc; i++)
	{
		std::string s;
		s.append(pglob.gl_pathv[i]);
		res.push_back(s.substr(s.find_last_of("/\\")+1));
	}
	
	globfree(&pglob);
}

// replacement for windows string functions
void strupr(char *c)
{
	while(*c)
	{
		*c = toupper(*c);
		c++;
	}
}
void strlwr(char *c)
{
	while(*c)
	{
		*c = tolower(*c);
		c++;
	}
}
void initConsole()
{
	/* nothing to do, using stdio */
}


// show a message box to the user. Handles
// mouse showing (and re-hiding if you ask for it)
// and turns full screen off if it's on.
void showMessageBox(CStringRef msg)
{
	doMessageBox(msg.c_str());
}

int DesktopBPP = 16;

void Sleep(unsigned int msec)
{
	(void)msec;
	//todo
}

void platform_ProcessConfig()
{
}

bool AppIsForeground = true;

void err(const char *str, ...)
{
	va_list argptr;
	char msg[4096];

	va_start(argptr, str);
	vsprintf(msg, str, argptr);
	va_end(argptr);

	if(vid_Close)
 	{
 		vid_Close();
 	}

	if (strlen(msg))
	{
		showMessageBox(msg);
		log("Exiting: %s", msg);
	}

	exit(strlen(msg)==0?0:-1);
}

static long last_timestamp = 0;
static bool has_timestamp = false;
void runloop();

void tick() {
	win_movie_update();
	systemtime++;
	if(engine_paused) return;
	timer++;
	vctimer++;
	hooktimer++;
}

void HandleMessages(void)
{
	runloop();

	//for now, also update the timer
	timeval time;
	gettimeofday(&time,NULL);
	long millis = (time.tv_sec * 1000) + (time.tv_usec/1000);
	
	if(!has_timestamp) {
		has_timestamp = true;
		last_timestamp = millis;
		return;
	}
	
	int new_timer = (millis-last_timestamp)/10;
	
	for(int i=systemtime;i<new_timer;i++)
		tick();
}

int iphone_SetMode(int xres, int yres, int bpp, bool windowflag) {

	delete screen;
	image *img = new image();
	img->shell = true;
	SetHandleImage(1,img);
	screen = img;
	img->data = (quad*)malloc(xres*yres*2);
	img->width = xres;
	img->height = yres;
	img->pitch = xres;
	img->cx1 = 0;
	img->cx2 = xres-1;
	img->cy1 = 0;
	img->cy2 = yres-1;

	vid_xres = xres;
	vid_yres = yres;
	vid_window = false;
	vid_bpp = bpp;

	dd_RegisterBlitters();

	return 1;
}

void iphone_m_init();

void iphone_VidClose(void) {}
AuxWindow *iphone_createAuxWindow(void) { return 0; }
AuxWindow *iphone_findAuxWindow(int handle) { return 0; }

class IphoneWindow : public AuxWindow
{
public:
	bool bMouseInside;
	IphoneWindow() { bMouseInside = true; }
public:
	virtual void dispose() { err("Disposing main IphoneWindow?"); }
	virtual int getHandle() { return 0; };
	virtual int getImageHandle() { return 1; }
	virtual image* getImage() { return screen; }
	virtual int getXres() { return vid_xres; }
	virtual int getYres() { return vid_yres; }
	virtual int getWidth() { return vid_xres; }
	virtual int getHeight() { return vid_yres; }
	virtual void setPosition(int x, int y) { log("setPosition called on IphoneWindow?"); }
	virtual void setResolution(int w, int h) { log("setResolution called on IphoneWindow?"); }
	virtual void setSize(int w, int h) { log("setSize called on IphoneWindow?"); }
	virtual void setVisibility(bool vis) { log("setVisability called on IphoneWindow?"); }
	virtual void setTitle(const char *title) { log("setTitle called on IphoneWindow?"); }
	virtual void positionCommand(int command, int arg1, int arg2) { log("positionCommand called on IphoneWindow?"); }
};

void iphone_m_flip(unsigned int* data);

void iphone_VidFlip(void) {
	iphone_m_flip((unsigned int*)screen->data);
}

static void initvideo() {

}

void iphone_m_main(int argc, char *argv[]);
extern "C" int main(int argc, char * argv[]) 
{
	iphone_m_main(argc,argv);
	return 0;
}

void iphone_c_main()
{
	srand(timeGetTime());
	log_Init(true);


	initvideo();

	//init video hooks
	gameWindow = new IphoneWindow();
	vid_Close = iphone_VidClose;
	Flip = iphone_VidFlip;
	vid_createAuxWindow = iphone_createAuxWindow;
	vid_findAuxWindow = iphone_findAuxWindow;


	xmain(0,0);
	
	err("");
}
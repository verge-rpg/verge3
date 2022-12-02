/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


/****************************************************************
	xerxes engine
	mac_system.cpp
 ****************************************************************/

#include <sys/types.h>
#include <dirent.h>
#include <glob.h>
#include <time.h>

#include "xerxes.h"

#include "SDL_main.h"    

/***************************** data *****************************/


/****************************************************************/

bool AppIsForeground = true;
int DesktopBPP;
bool IgnoreEvents = false;
/***************************** code *****************************/


#if defined(__LINUX__) || defined(__EMSCRIPTEN__)
void InitEditCode()
{
	err("Edit code mode is not currently supported");
}

void AddSourceFile(std::string s)
{
	//log("AddSourceFile() is not currently supported");
}

StringRef GetSystemSaveDirectory(CStringRef name)
{
	static const StringRef dotSlash = "./";
	return dotSlash;
}
#endif

#ifdef __LINUX__
void doMessageBox(std::string msg)
{
	GtkWidget* w = GTK_WIDGET(gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s", msg.c_str()));
	gtk_window_set_title(GTK_WINDOW(w), "verge3");
	gtk_dialog_set_default_response(GTK_DIALOG(w), GTK_RESPONSE_CLOSE);

	gtk_dialog_run(GTK_DIALOG(w));

	gtk_widget_destroy(w);
}
#elif defined(__EMSCRIPTEN__)
void doMessageBox(std::string msg)
{
	EM_ASM({ alert(UTF8ToString($0)); }, msg.c_str());
}
#endif



void platform_ProcessConfig()
{
}

// internal use, in mac_cocoa_util.mm
void doMessageBox(std::string msg);

// internal use
int getCurrentBpp();

int main(int argc, char **argv)
{
#ifdef __LINUX__
	gtk_init(&argc, &argv);
#endif
    
	srand(timeGetTime());
	log_Init(true);

    unsigned long sdlFlags = SDL_INIT_VIDEO | SDL_INIT_JOYSTICK;
#ifndef __IPHONE__
    sdlFlags |= SDL_INIT_TIMER;
#endif
    
	// we must init SDL before any other sdl stuff
	if(SDL_Init(sdlFlags) != 0)
		err("Couldn't start up SDL.");

	// create video window
	sdl_video_init();
	SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING,0);

	// hide the cursor while we're open -
	// SDL shows it when it moves outside of window
	SDL_ShowCursor(SDL_DISABLE);

	DesktopBPP = getCurrentBpp(); // needs video inited already

#ifdef __APPLE__
    ChangeToRootDirectory();
#endif
    
	xmain(argc,argv);
    err("");
    
	return 0;
}

/* returns time in s since 1970 */
/* not suitable for movie timing */
unsigned int timeGetTime()
{
	return time(NULL);
}

/* Get the bits per pixel of the screen currently */
int getCurrentBpp()
{
	const SDL_VideoInfo *info = SDL_GetVideoInfo();
	return info->vfmt->BitsPerPixel;
}

// clipboard stuff unimplemented
char *clipboard_getText()
{
	return "";
}

void clipboard_setText(const char *text)
{
}

image *clipboard_getImage()
{
	return 0;
}

void clipboard_putImage(image *img)
{
}

// called to update state variables based
// on a mouse event
void handleMouseButton(SDL_MouseButtonEvent e)
{
	bool state = (e.type == SDL_MOUSEBUTTONDOWN ? true : false);
	switch(e.button)
	{
		case SDL_BUTTON_LEFT:
			mouse_l = state;
			break;
		case SDL_BUTTON_RIGHT:
			mouse_r = state;
			break;
		case SDL_BUTTON_MIDDLE:
			mouse_m = state;
			break;
			/* XXX These are changed by 120 each time
			because that seems to be what happens
			in Windows. */
		case SDL_BUTTON_WHEELUP:
			mwheel += 120;
			break;
		case SDL_BUTTON_WHEELDOWN:
			mwheel -= 120;
			break;
	}
}

void handleActive(SDL_ActiveEvent e)
{
  if(e.state & SDL_APPINPUTFOCUS)
  {
    AppIsForeground = e.gain;
  }
}

/* Run the SDL event loop to get waiting messages */
void HandleMessages(void)
{
#ifdef __LINUX__
	while (gtk_events_pending())
	{
		gtk_main_iteration();
	}
#endif
	SDL_Event event;
	while ( SDL_PollEvent(&event) ) {
		if(IgnoreEvents) {
			// ignore everything but quit
			if(event.type == SDL_QUIT)
				err("");
			continue;
		}
		switch (event.type) {
			case SDL_VIDEORESIZE:
				if(!vid_window)
					break; // not in windowed mode; what's a resize?
				handleResize(event.resize);
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				handleMouseButton(event.button);
				break;
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				ParseKeyEvent(event.key);
				break;
			case SDL_QUIT:
				err("");
				break;
      case SDL_ACTIVEEVENT:
        handleActive(event.active);
        break;
			default:
				break;
		}
	}
}

void writeToConsole(char *str)
{
	printf("%s", str);
}

void initConsole()
{
	/* nothing to do, using stdio */
}

// helper function for get* below
struct tm *getTime()
{
	time_t theTime = time(NULL);
	return localtime(&theTime);
}

int getYear()
{
	return getTime()->tm_year + 1900;
}

int getMonth()
{
	return getTime()->tm_mon;
}

int getDay()
{
	return getTime()->tm_yday;
}

int getDayOfWeek()
{
	return getTime()->tm_wday;
}

int getHour()
{
	return getTime()->tm_hour;
}

int getMinute()
{
	return getTime()->tm_min;
}

int getSecond()
{
	return getTime()->tm_sec;
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
char* strupr(char *s)
{
	char *p = s;
	while(*p)
	{
		*p = toupper(*p);
		p++;
	}
	return s;
}

char* strlwr(char *s)
{
	char *p = s;
	while(*p)
	{
		*p = tolower(*p);
		p++;
	}
	return s;
}

// set main window title, if we have one
void setWindowTitle(const char *str)
{
	if(gameWindow) {
		gameWindow->setTitle(str);
	}
}

void Sleep(unsigned int msec)
{
	SDL_Delay(msec);
}

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

// show a message box to the user. Handles
// mouse showing (and re-hiding if you ask for it)
// and turns full screen off if it's on.
void showMessageBox(CStringRef msg)
{
	if(!vid_window)
		sdl_toggleFullscreen();

	// see what the cursor is now
	int cursorState = SDL_ShowCursor(SDL_QUERY);

	// need to show cursor so they can click on it
	SDL_ShowCursor(SDL_ENABLE);

	doMessageBox(msg.c_str());

	// now restore it back to the old state
	SDL_ShowCursor(cursorState);
}


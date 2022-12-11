/// The v3wasm 3 Project is originally by Ben Eirich and is made available via
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

void doMessageBox(std::string msg)
{
#ifdef __LINUX__
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "verge3", msg.c_str(), NULL);
#elif defined(__EMSCRIPTEN__)
	EM_ASM({ alert(UTF8ToString($0)); }, msg.c_str());
#endif
}
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten/fetch.h>

/*namespace v3wasm {
    std::string gameRoot = "timeless/";
    std::vector<std::string> manifest;
    std::string saveGameRoot;

    void preload(std::string_view);

    using DownloadCB = void(*)(char* filename, size_t size, char* data);

    EM_JS(void, downloadAll, (const char** manifest, DownloadCB putFile), {
        return Asyncify.handleSleep(resume => {
            let promises = [];
            let count = 0;

            function download(pathPtr) {
                const path = UTF8ToString(pathPtr);
                return fetch(path).then(response => {
                    if (!response.ok) {
                        console.error('fetchSync failed', path);
                        HEAP32[size >> 2] = 0;
                        HEAP32[data >> 2] = 0;
                        throw 'fetchSync failed';
                    }
                    return response.blob();
                }).then(blob =>
                    blob.arrayBuffer()
                ).then(array => {
                    const bytes = new Uint8Array(array);
                    const dataPtr = _malloc(bytes.length);
                    HEAP8.set(bytes, dataPtr);
                    Module.dynCall_viii(putFile, pathPtr, bytes.length, dataPtr);

                    ++count;
                    v3wasm.setLoadingProgress((100 * count / promises.length) | 0)
                });
            }

            while (true) {
                let pathPtr = HEAPU32[manifest >> 2];
                if (pathPtr == 0) {
                    break;
                }
                manifest += 4;
                promises.push(download(pathPtr));
            }

            Promise.all(promises).then(() => { resume(); });
        });
    });

    EM_JS(void, fetchSync, (const char* pathPtr, size_t* size, char** data), {
        return Asyncify.handleSleep(resume => {
            const path = UTF8ToString(pathPtr);
            // console.log('fetchSync', path);
            return fetch(path).then(response => {
                if (!response.ok) {
                    console.error('fetchSync failed', path);
                    HEAP32[size >> 2] = 0;
                    HEAP32[data >> 2] = 0;
                    resume();
                    return;
                }
                return response.blob();
            }).then(blob =>
                blob.arrayBuffer()
            ).then(array => {
                const bytes = new Uint8Array(array);
                HEAP32[size >> 2] = bytes.length;
                const dataPtr = _malloc(bytes.length);
                HEAP32[data >> 2] = dataPtr;
                HEAP8.set(bytes, dataPtr);
                resume();
            });
        });
    });

    struct FreeDelete { void operator()(char* p) { free(p); } };
    using Deleter = std::unique_ptr<char, FreeDelete>;

    void downloadGame() {
        std::string manifestPath = gameRoot + "manifest.txt";
        char* manifestPtr;
        size_t manifestLength;
        fetchSync(manifestPath.c_str(), &manifestLength, &manifestPtr);
        Deleter hello{ manifestPtr };

        std::string_view manifest{ manifestPtr, manifestLength };

        std::vector<std::string> files;
        auto append = [&](std::string_view name) {
            if (name.empty())
                return;

            if (name[name.size() - 1] == '\r')
                name.remove_suffix(1);

            files.push_back(gameRoot + std::string{ name });
        };

        while (!manifest.empty()) {
            auto pos = manifest.find('\n');
            if (pos == std::string::npos) {
                append(std::string{ manifest });
                break;
            }
            append(std::string{ manifest.substr(0, pos) });
            manifest.remove_prefix(pos + 1);
        }

        char** stuff = new char*[files.size() + 1];
        for (int i = 0; i < files.size(); ++i) {
            stuff[i] = (char*)files[i].c_str();
        }
        stuff[files.size()] = nullptr;

        downloadAll((const char**)stuff, [](char* filename, size_t size, char* data) {
            v3wasm::DataVec vec(data, data + size);
            // filename always has gameRoot prefix.  Strip it off.
            v3wasm::vset(std::string{ filename + gameRoot.size() }, std::move(vec));
        });

        delete[] stuff;

        EM_ASM({
            window.v3wasm.setLoadingProgress(100);
        });
    }

    void preload(std::string_view path) {
        std::string filename = gameRoot;
        filename.append(path.begin(), path.end());

        size_t contentLength;
        char* content;
        fetchSync(filename.c_str(), &contentLength, &content);
        Deleter hello{ content };

        v3wasm::DataVec vec(content, content + contentLength);

        v3wasm::vset(std::string{ path }, std::move(vec));
    }

    EM_JS(void, wasm_initFileSystem, (const char* c), {
        let sgr = UTF8ToString(c);
        if (sgr.endsWith('/'))
            sgr = sgr.substr(0, sgr.length - 1);
        FS.mkdir("/persist");
        FS.mkdir(sgr);
        // Then mount with IDBFS type
        FS.mount(IDBFS, {}, sgr);

        // Then sync
        FS.syncfs(true, function (err) {
            // Error
            if (err)
                console.error('wasm_initFileSystem failed!', err);
        });
    });

    void initFileSystem() {
        saveGameRoot = "/persist/" + gameRoot;
        wasm_initFileSystem(saveGameRoot.c_str());
    }

    EM_JS(void, setBuildDate, (const char* date), {
        if (v3wasm.setBuildDate)
            v3wasm.setBuildDate(UTF8ToString(date));
    });
}
*/
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
#if defined(__EMSCRIPTEN__)
    /*v3wasm::setBuildDate(__DATE__);
    v3wasm::downloadGame();
    v3wasm::initFileSystem();*/
#endif
    
	srand(timeGetTime());
	log_Init(true);

    unsigned long sdlFlags = SDL_INIT_VIDEO | SDL_INIT_JOYSTICK;
#ifndef __IPHONE__
    sdlFlags |= SDL_INIT_TIMER;
#endif
    
	// we must init SDL before any other sdl stuff
	if(SDL_Init(sdlFlags) < 0)
		err("Couldn't start up SDL: %s", SDL_GetError());

	// create video window
	sdl_video_init();

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
#if __EMSCRIPTEN__
	return 32;
#else	
	const SDL_VideoInfo *info = SDL_GetVideoInfo();
	return info->vfmt->BitsPerPixel;
#endif	
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
void handleMouseButton(const SDL_MouseButtonEvent& e)
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
	}
}

void handleMouseWheel(const SDL_MouseWheelEvent& e)
{
    /* XXX These are changed by 120 each time
    because that seems to be what happens
    in Windows. */    
    if (e.y > 0)
    {
        mwheel += 120;
    }
    else if (e.y < 0)
    {
        mwheel -= 120;
    }
}

/* Run the SDL event loop to get waiting messages */
void HandleMessages(void)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
    {
		if (IgnoreEvents)
        {
			// ignore everything but quit
			if(event.type == SDL_QUIT)
				err("");
			continue;
		}
		switch (event.type) {
			case SDL_WINDOWEVENT_RESIZED:
				if(!vid_window)
					break; // not in windowed mode; what's a resize?
				handleResize(event.window);
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				handleMouseButton(event.button);
				break;
			case SDL_MOUSEWHEEL:
				handleMouseWheel(event.wheel);
				break;
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				ParseKeyEvent(event.key);
				break;
            case SDL_JOYDEVICEADDED:
                joy_Add(event.jdevice.which);
                break;
            case SDL_JOYDEVICEREMOVED:
                joy_Remove(event.jdevice.which);
                break;
			case SDL_QUIT:
				err("");
				break;
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                AppIsForeground = true;
                break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
                AppIsForeground = false;
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


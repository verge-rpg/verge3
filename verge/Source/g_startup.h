/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


extern int v3_xres, v3_yres, v3_bpp;
enum ScaleFormat
{
    SCALE_FORMAT_LETTERBOX = 0,    // The old Verge style of letter box.
    SCALE_FORMAT_ASPECT = 1,       // Get the largest integer factor of screen size that fits in the render area.
    SCALE_FORMAT_STRETCH = 2,      // Stretch screen to take up entire render area.
    // Total scale formats.
    SCALE_FORMAT_COUNT = 3,
};

extern ScaleFormat v3_scale_win, v3_scale_full;
extern bool windowmode;
extern bool sound;
extern bool use_lua;
extern bool cheats;
extern bool automax;
extern bool releasemode;
extern char mapname[255];
extern int gamerate;
extern bool vc_oldstring;
void ShowPage();

void xtestmain(int argc, char* argv[]); // for doctest init
void xmain(int argc, char* argv[]);

#ifdef __EMSCRIPTEN__
constexpr int WASM_SCRIPT_TIMEOUT_CALL_CHECK_THRESHOLD = 300;
constexpr double WASM_SCRIPT_TIMEOUT_TIME_LIMIT_MS = 30.0;
constexpr int WASM_SCRIPT_BUSY_WAIT_COUNTER_LIMIT = 500;

extern int wasm_scriptBusyWaitCounter;
extern int wasm_scriptTimeoutCallCounter;
extern double wasm_scriptTimeSinceLastFrame;

extern "C" double wasm_nextFrame_();

void wasm_nextFrame();
void wasm_detectScriptBusyWait();
void wasm_detectScriptTimeout();
#endif

#ifdef ALLOW_SCRIPT_COMPILATION
extern VCCompiler *vcc;
#endif

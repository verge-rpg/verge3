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
extern bool cheats;
extern bool automax;
extern char mapname[255];
extern int gamerate;

extern int cf_r1, cf_g1, cf_b1;
extern int cf_r2, cf_g2, cf_b2;
extern int cf_rr, cf_gr, cf_br;

void ShowPage();

#ifdef ALLOW_SCRIPT_COMPILATION
//extern VCCompiler *vcc;
#endif

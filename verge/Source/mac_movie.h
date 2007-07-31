/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


void win_movie_init();
void win_movie_shutdown();

void win_movie_update();
int win_movie_load(const char *fname, bool bPlayAudio);
void win_movie_close(int handle);

int win_movie_playSimple(const char *fname);



void win_movie_play(int m, int loop);
int win_movie_getImage(int m);
void win_movie_render(int m);
int win_movie_getCurrFrame(int m);
int win_movie_getFramerate(int m);
void win_movie_nextFrame(int m);
void win_movie_setFrame(int m, int f);
void win_movie_abortSimple(); // add stub - JR Dec 11/05
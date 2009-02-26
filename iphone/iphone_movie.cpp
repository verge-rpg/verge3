#include "xerxes.h"

// unimplemented -- just return errors

void win_movie_init() {}
void win_movie_shutdown() {}

void win_movie_update() {}
int win_movie_load(const char *fname, bool bPlayAudio) {return 0;}
void win_movie_close(int handle) {}

int win_movie_playSimple(const char *fname) {return 0;}
void win_movie_abortSimple() {}



void win_movie_play(int m, int loop) {}
int win_movie_getImage(int m) {return 0;}
void win_movie_render(int m) {}
int win_movie_getCurrFrame(int m) {return 0;}
int win_movie_getFramerate(int m) {return 0;}
void win_movie_nextFrame(int m) {}
void win_movie_setFrame(int m, int f){}

/*
garlick 1.0.0 7/12/06
see http://www.pi-r-squared.com/code/garlick for details
*/

#ifndef _GARLICK_H_
#define _GARLICK_H_

//garlick
//-------------------------

#ifndef _GARLICK_C_
extern void *(*Garlick_cb_open)(char *fname);
extern void (*Garlick_cb_close)(void *handle);
extern size_t (*Garlick_cb_read)(void *ptr, size_t elemsize, size_t amt, void *handle);
extern long (*Garlick_cb_tell)(void *handle);
extern int (*Garlick_cb_seek)(void *handle, long offset, int origin);
extern void *(*Garlick_cb_malloc)(int amt);
extern void (*Garlick_cb_free)(void *ptr);
#endif

//please make sure this is 64bits
typedef long long GARLICK_LONG;

#define GARLICK_MODE_FLAC 1
#define GARLICK_MODE_STEREO 2
#define GARLICK_MODE_16BITS 4
#define GARLICK_MODE_STEREOPLANAR_BIT 8
#define GARLICK_MODE_STEREOPLANAR 10
#define GARLICK_MODE_UNSIGNED 16


struct GarlickFile {
	void *inf, *lib;
	GARLICK_LONG position; 
	GARLICK_LONG length;
	int chunkleft, mode, passthrough;
	unsigned char encode_mode;

	int flac_in_counter;
	
	unsigned char *buf;
	int buf_counter;
	int buflen;
};

size_t GarlickRead(void *buf, size_t elemsize, size_t amt, GarlickFile *gf);
GARLICK_LONG GarlickTell(GarlickFile *gf);
int GarlickSeek(GarlickFile *gf, GARLICK_LONG offset, int origin);
GarlickFile *GarlickOpen(char *fname, char *fnamelib);
void GarlickClose(GarlickFile *gf);

//------------------------

#endif
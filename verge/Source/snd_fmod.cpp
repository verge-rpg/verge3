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
 * verge3: win_sound.cpp                                          *
 * copyright (c) 2004 vecna                                       *
 ******************************************************************/

//not supporting multiple instances
//because this was rapidly ported from a nonmodular C software

#include "xerxes.h"
#include "snd_fmod.h"
#include "../fmod/fmod.h"
#include "garlick.h"

#ifdef SND_USE_FMOD

/****************************** data ******************************/

#define MAX_SONGS	10

char playingsng[80] = {0};
int  numsamples     = 0;
bool UseSound       = false;

FMUSIC_MODULE *curmod;
FSOUND_STREAM *curstream;
int curstreamchan;

//-------------------------------- 
//DUMB-specific code
//-------------------------------- 
#ifdef USE_DUMB
#include "dumb.h"
static bool useDumb = false;
class DumbWrapper {
public:
	FSOUND_STREAM *stream;
	bool load(char *fname) {
		int len = strlen(fname);
		if(toupper(fname[len-1]) == 'T')
			duh = dumb_load_it_quick(fname);
		else if(toupper(fname[len-1]) == 'D')
			duh = dumb_load_mod_quick(fname);
		else if(toupper(fname[len-2]) == '3')
			duh = dumb_load_s3m_quick(fname);
		else if(toupper(fname[len-2]) == 'X')
			duh = dumb_load_xm_quick(fname);		
		if(duh)
			sr = duh_start_sigrenderer(duh, 0, 2, 0);
		else return false;
		return true;
	}
	DUH *duh;
	DUH_SIGRENDERER *sr;
	float volume;

	DumbWrapper() {
		duh = 0;
		sr = 0;
		stream = 0;
		volume = 1.0f;
	}

	~DumbWrapper() {
		if(duh) unload_duh(duh);
		if(sr) duh_end_sigrenderer(sr);
		if(stream) FSOUND_Stream_Close(stream);
	}
};

static void *dumb_open(const char *filename) {
	return GarlickOpen((char*)filename,"library");
}
static int dumb_skip(void *f, long n) {
	return GarlickSeek((GarlickFile*)f,n,SEEK_CUR);
}
static int dumb_getc(void *f) {
	unsigned char buf;
	if(GarlickRead(&buf,1,1,(GarlickFile*)f) != 1)
		return -1;
	else return buf;
}
static long dumb_getnc(char *ptr, long n, void *f) {
	return GarlickRead(ptr,1,n,(GarlickFile*)f);
}
static void dumb_close(void *f) {
	GarlickClose((GarlickFile*)f);
}

static DUMBFILE_SYSTEM dumb_filesystem = {
	dumb_open,dumb_skip,dumb_getc,
		dumb_getnc,dumb_close };

static void DUMB_init() {
	register_dumbfile_system(&dumb_filesystem);
}

#define DUH_SAMPLES (4400)
#define DUH_BYTES (DUH_SAMPLES*4)
signed char F_CALLBACKAPI dumb_fmod_streamCallback(FSOUND_STREAM *stream, void *buff, int len, int param)
{
	const float renderRate = 65536.0f/44100;
	DumbWrapper *wrapper = (DumbWrapper *)param;
	int samples = len >> 2;
	duh_render(wrapper->sr,16,0,wrapper->volume,renderRate, samples, buff);
	return 1;
}

static DumbWrapper *curDumb;
#endif
//-------------------------------- 

struct advsong
{
	FMUSIC_MODULE *mod;
	FSOUND_STREAM *str;
	int active;
	int chan;
	#ifdef USE_DUMB
	DumbWrapper *dumb;
	#endif
};

struct advsong songs[MAX_SONGS];


/****************************** code ******************************/

unsigned int F_CALLBACKAPI myopen(const char *name)
{
	return (unsigned int)GarlickOpen((char *)name,"library");
}

void F_CALLBACKAPI myclose(unsigned int handle)
{
	GarlickClose((GarlickFile *)handle);
}

int F_CALLBACKAPI myread(void *buffer, int size, unsigned int handle)
{
	return GarlickRead(buffer,1,size,(GarlickFile*)handle);
}

int F_CALLBACKAPI myseek(unsigned int handle, int pos, signed char mode)
{
	return GarlickSeek((GarlickFile*)handle,pos,mode);
}

int F_CALLBACKAPI mytell(unsigned int handle)
{
	return (int)GarlickTell((GarlickFile*)handle);
}

bool SoundEngine_Fmod::init()
{
    FSOUND_File_SetCallbacks((FSOUND_OPENCALLBACK) myopen, (FSOUND_CLOSECALLBACK) myclose, (FSOUND_READCALLBACK) myread, (FSOUND_SEEKCALLBACK) myseek, (FSOUND_TELLCALLBACK) mytell);
	FSOUND_SetMixer(FSOUND_MIXER_QUALITY_FPU);
	FSOUND_Init(44100, 64, FSOUND_INIT_USEDEFAULTMIDISYNTH);
	memset(songs, 0, sizeof songs);
	curmod = 0;
	curstream = 0;
	UseSound = true;
	#ifdef USE_DUMB
	curDumb = 0;
	useDumb = cfg_GetIntKeyValue("dumb")==1;
	if(useDumb)
		DUMB_init();
	#endif

	return true;
}

void SoundEngine_Fmod::shutdown() {
}

void killmodule()
{
	if (curmod)
	{
		FMUSIC_StopSong(curmod);
		FMUSIC_FreeSong(curmod);
		curmod = 0;
	}

	if (curstream)
	{
		FSOUND_Stream_Stop(curstream);
		FSOUND_Stream_Close(curstream);
		curstream = 0;
	}

	#ifdef USE_DUMB
	if(curDumb) {
		delete curDumb;
		curDumb = 0;
	}
	#endif
}

void SoundEngine_Fmod::PlayMusic(char *sng)
{
	// Check all possible fail conditions. We do this so that if it does fail, we don't
	// unnessarily stop whatever is presently playing.
	if (!UseSound) return;
	if (!strlen(sng)) return;
	if (!strcasecmp(sng, playingsng)) return;
	VFILE *f = vopen(sng);
	if (!f) return;
	vclose(f);

	killmodule();
    strcpy(playingsng, sng);

	#ifdef USE_DUMB
	//first, if using DUMB, check for allowed extensions
	if(useDumb)
		if(ExtensionIs(sng, "it") || ExtensionIs(sng, "mod") || ExtensionIs(sng,"s3m") || ExtensionIs(sng,"it")) {
			int numBytes = DUH_BYTES;
			DumbWrapper *wrapper = new DumbWrapper();
			if(wrapper->load(sng)) {
				FSOUND_STREAM *stream = FSOUND_Stream_Create(dumb_fmod_streamCallback,numBytes,FSOUND_STEREO | FSOUND_16BITS,44100,(int)wrapper);
				wrapper->stream = stream;
				curstream = stream;
				FSOUND_Stream_Play(FSOUND_FREE,stream);
				return;
			}
			else err("DUMB Couldnt load specified song: %s",sng);
		}
	#endif

	// Fail checks pass, now lets check the file extension to see if its a midi or a mp3/ogg
	if (ExtensionIs(sng, "mid") || ExtensionIs(sng, "midi") || ExtensionIs(sng, "rmi"))
	{
		// MIDI wont load through the File I/O callbacks, so we need to load it into memory ourselves.
		f = vopen(sng);
		int len = filesize(f);
		char *buf = (char *) malloc(len);
		vread(buf, len, f);
		vclose(f);
		curmod = FMUSIC_LoadSongEx(buf, 0,  len, FSOUND_LOADMEMORY, 0, 0);
		free(buf);
		if (curmod)
			FMUSIC_PlaySong(curmod);
		FMUSIC_SetLooping(curmod, true);
		return;
	}

	if (ExtensionIs(sng, "mp3") || ExtensionIs(sng, "mpeg3") || ExtensionIs(sng, "ogg"))
	{
		// MP3s and OGGs will use FSOUND_STREAM, not FMUSIC_MODULE
		curstream = FSOUND_Stream_Open(sng, FSOUND_LOOP_NORMAL, 0, 0);
		if (curstream)
			curstreamchan = FSOUND_Stream_Play(FSOUND_FREE, curstream);
		return;
	}

	curmod = FMUSIC_LoadSong(sng);

	if (curmod)
		FMUSIC_PlaySong(curmod);
	FMUSIC_SetLooping(curmod, true);
}

void SoundEngine_Fmod::StopMusic()
{
	if (!UseSound) return;
    playingsng[0]=0;
	killmodule();
}

void SoundEngine_Fmod::SetMusicVolume(int v)
{
	if (curmod)
		FMUSIC_SetMasterVolume(curmod, v*256/100);
	if (curstream)
		FSOUND_SetVolume(curstreamchan, v*256/100);
}

/******************** Sound effects interface *********************/

void* SoundEngine_Fmod::LoadSample(char *fn)
{
	FSOUND_SAMPLE *s = FSOUND_Sample_Load(FSOUND_UNMANAGED, fn, 0, 0, 0);
	FSOUND_Sample_SetMode(s, FSOUND_LOOP_OFF);
	return s;
}

void SoundEngine_Fmod::PlaySample(void *s, int vol)
{
	int chan = FSOUND_PlaySound(FSOUND_FREE, (FSOUND_SAMPLE*)s);
	FSOUND_SetVolume(chan, vol);
	vc->vcreturn = chan;
}

void SoundEngine_Fmod::StopSound(int chan)
{
	FSOUND_StopSound(chan);
}

// Overkill (2006-11-20)
int SoundEngine_Fmod::SoundIsPlaying(int chan)
{
	return FSOUND_IsPlaying(chan);
}

void SoundEngine_Fmod::FreeSample(void *s)
{
	FSOUND_Sample_Free((FSOUND_SAMPLE*)s);
}

/******************** Advanced Music Interface ********************/

int SoundEngine_Fmod::LoadSong(char *fn)
{
	int si = -1;
	for (int i=0; i<MAX_SONGS; i++)
	{
		if (!songs[i].active)
		{
			si = i;
			break;
		}
	}

	// Check all possible fail conditions.

	if (si == -1)
		return -1;
	if (!UseSound)
		return -1;
	if (!strlen(fn))
		return -1;
	VFILE *f = vopen(fn);
	if (!f)
		return -1;
	vclose(f);

	songs[si].mod = 0;
	songs[si].str = 0;

	#ifdef USE_DUMB
	songs[si].dumb = 0;
	//first, if using DUMB, check for allowed extensions
	if(useDumb)
		if(ExtensionIs(fn, "it") || ExtensionIs(fn, "mod") || ExtensionIs(fn,"s3m") || ExtensionIs(fn,"it")) {
			int numBytes = DUH_BYTES;
			DumbWrapper *wrapper = new DumbWrapper();
			if(wrapper->load(fn)) {
				wrapper->stream = FSOUND_Stream_Create(dumb_fmod_streamCallback,numBytes,FSOUND_STEREO | FSOUND_16BITS,44100,(int)wrapper);
				songs[si].str = wrapper->stream;
				songs[si].dumb = wrapper;
				songs[si].active = true;
				return si;
			}
			else err("DUMB Couldnt load specified song: %s",fn);
		}
	#endif

	// Fail checks pass, now lets check the file extension to see if its a midi or a mp3/ogg
	if (ExtensionIs(fn, "mid") || ExtensionIs(fn, "midi") || ExtensionIs(fn, "rmi"))
	{
		// MIDI wont load through the File I/O callbacks, so we need to load it into memory ourselves.
		f = vopen(fn);
		int len = filesize(f);
		char *buf = (char *) malloc(len);
		vread(buf, len, f);
		vclose(f);
		songs[si].mod = FMUSIC_LoadSongEx(buf, 0,  len, FSOUND_LOADMEMORY, 0, 0);
		free(buf);
		if (songs[si].mod)
		{
			songs[si].active = true;
			return si;
		}
		return -1;
	}

	if (ExtensionIs(fn, "mp3") || ExtensionIs(fn, "mpeg3") || ExtensionIs(fn, "ogg"))
	{
		// MP3s and OGGs will use FSOUND_STREAM, not FMUSIC_MODULE
		songs[si].str = FSOUND_Stream_Open(fn, FSOUND_LOOP_NORMAL, 0, 0);
		if (songs[si].str)
		{
			songs[si].active = true;
			return si;
		}
		return -1;
	}

	songs[si].mod = FMUSIC_LoadSong(fn);

	if (songs[si].mod)
	{
		songs[si].active = true;
		FMUSIC_SetLooping(songs[si].mod, true);
		return si;
	}
	return -1;
}

void SoundEngine_Fmod::PlaySong(int h)
{
	if (h == -1 || h >= MAX_SONGS) return;
	if (songs[h].mod)
		FMUSIC_PlaySong(songs[h].mod);
	if (songs[h].str)
		songs[h].chan = FSOUND_Stream_Play(FSOUND_FREE, songs[h].str);
	//dumb uses stream logic
}

void SoundEngine_Fmod::StopSong(int h)
{
	if (h == -1 || h >= MAX_SONGS) return;
	if (songs[h].mod)
		FMUSIC_StopSong(songs[h].mod);
	if (songs[h].str)
		FSOUND_Stream_Stop(songs[h].str);
	//dumb uses stream logic
}

void SoundEngine_Fmod::SetPaused(int h, int p)
{
	if (h == -1 || h >= MAX_SONGS) return;
	if (songs[h].mod)
		FMUSIC_SetPaused(songs[h].mod, p);
	if (songs[h].str)
		FSOUND_SetPaused(songs[h].chan, p);
	//dumb uses stream logic
}

int SoundEngine_Fmod::GetSongPos(int h)
{
	if (h == -1 || h >= MAX_SONGS) return 0;
	#ifdef USE_DUMB
	if(songs[h].dumb)
		return duh_sigrenderer_get_position(songs[h].dumb->sr);
	#endif
	if (songs[h].mod)
		return FMUSIC_GetOrder(songs[h].mod);
	if (songs[h].str)
		return FSOUND_Stream_GetPosition(songs[h].str);
	return 0;
}

void SoundEngine_Fmod::SetSongPos(int h, int p)
{
	if (h == -1 || h >= MAX_SONGS) return;
	#ifdef USE_DUMB
	if(songs[h].dumb) {
		//ouch! not supported?
		return;
	}
	#endif
	if (songs[h].mod)
		FMUSIC_SetOrder(songs[h].mod, p);
	if (songs[h].str)
		FSOUND_Stream_SetPosition(songs[h].str, p);
}

void SoundEngine_Fmod::SetSongVol(int h, int v)
{
	if (h == -1 || h >= MAX_SONGS) return;
	v = v * 256 / 100;
	if (songs[h].mod)
		FMUSIC_SetMasterVolume(songs[h].mod, v);
	if (songs[h].str)
		FSOUND_SetVolume(songs[h].chan, v);
	//dumb uses stream logic
}

int SoundEngine_Fmod::GetSongVol(int h)
{
	if (h == -1 || h >= MAX_SONGS) return 0;
	#ifdef USE_DUMB
	if(songs[h].dumb) {
		return (int)(100*songs[h].dumb->volume);
	}
	#endif
	if (songs[h].mod)
		return FMUSIC_GetMasterVolume(songs[h].mod) * 100 / 256;
	if (songs[h].str)
		return FSOUND_GetVolume(songs[h].chan) * 100 / 256;
	return 0;
}

void SoundEngine_Fmod::FreeSong(int h)
{
	if (h == -1 || h >= MAX_SONGS) return;
	#ifdef USE_DUMB
	if(songs[h].dumb) {
		delete songs[h].dumb;
		songs[h].dumb = 0;
		songs[h].str = 0;
		songs[h].active = false;
		return;
	}
	#endif
	if (songs[h].mod)
	{
		FMUSIC_FreeSong(songs[h].mod);
		songs[h].mod = 0;
		songs[h].active = false;
	}
	if (songs[h].str)
	{
		FSOUND_Stream_Close(songs[h].str);
		songs[h].str = 0;
		songs[h].active = false;
	}
}

#endif
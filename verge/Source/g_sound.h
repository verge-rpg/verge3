/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


#ifndef _G_SOUND_H

class SoundEngine {
public:
	
	virtual ~SoundEngine() {}
	
	virtual bool init()= 0;
	virtual void shutdown()= 0;

	virtual void PlayMusic(const char *sng) = 0;
	virtual void StopMusic() = 0;
	virtual void SetMusicVolume(int v) = 0;

	virtual void *LoadSample(const char *fn) = 0;
	virtual void PlaySample(void *sample, int vol) = 0;
	virtual void StopSound(int chan) = 0;
	virtual int SoundIsPlaying(int chan) = 0;
	virtual void FreeSample(void *sample) = 0;


	virtual int LoadSong(const char *fn) = 0;
	virtual void PlaySong(int h) = 0;
	virtual void StopSong(int h) = 0;
	virtual void SetPaused(int h, int p) = 0;
	virtual int GetSongPos(int h) = 0;
	virtual void SetSongPos(int h, int p) = 0;
	virtual void SetSongVol(int h, int v) = 0;
	virtual int GetSongVol(int h) = 0;
	virtual void FreeSong(int h) = 0;
};

bool snd_Init(int soundEngine);

//////////////////////////////////////
//global sound functions--link to active sound engine
// Dummy music interface
void PlayMusic(const char *sng);
void StopMusic();
int SoundIsPlaying(int chan); // Overkill (2006-11-20)
void SetMusicVolume(int v);

// Sound effect interface
void *LoadSample(const char *fn);
void PlaySample(void *s, int vol);
void StopSound(int chan);
void FreeSample(void *s);

int LoadSong(const char *fn);
void PlaySong(int h);
void StopSong(int h);
void SetPaused(int h, int p);
int  GetSongPos(int h);
void SetSongPos(int h, int p);
void SetSongVol(int h, int v);
int  GetSongVol(int h);
void FreeSong(int h);

void snd_Shutdown();

#ifndef _G_SOUND_CPP
extern SoundEngine *snd_engine;
#endif

#endif
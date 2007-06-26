/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


#define _G_SOUND_CPP
#include "xerxes.h"

#ifdef SND_USE_FMOD
#include "snd_fmod.h"
#endif

#ifdef SND_USE_OAKRA
#include "snd_oakra.h"
#endif

SoundEngine *snd_engine;

bool snd_Init(int soundEngine) {
	snd_engine = 0;

#ifdef SND_USE_FMOD
	if(soundEngine == 0)
		snd_engine = new SoundEngine_Fmod();
#endif

#ifdef SND_USE_OAKRA
	if(soundEngine == 1)
		snd_engine = new SoundEngine_Oakra();
#endif

	if(snd_engine)
		return snd_engine->init();
	else return false;
}


//////////////////////////////////////
//global sound functions--link to active sound engine
// Dummy music interface
// Added checks for snd_engine - Jesse 19-10-2005
void PlayMusic(char *sng) { if(snd_engine) snd_engine->PlayMusic(sng); }
void StopMusic() { if(snd_engine) snd_engine->StopMusic(); }
void SetMusicVolume(int v) { if(snd_engine) snd_engine->SetMusicVolume(v); }

// Sound effect interface
void *LoadSample(char *fn) { if(snd_engine) return snd_engine->LoadSample(fn); else return 0; }
void PlaySample(void *s, int vol) { if(snd_engine) snd_engine->PlaySample(s, vol); }
void StopSound(int chan) { if(snd_engine) snd_engine->StopSound(chan); }
// Overkill (2006-11-20)
int SoundIsPlaying(int chan) { if(snd_engine) return snd_engine->SoundIsPlaying(chan); else return 0; } 
void FreeSample(void *s) { if(snd_engine) snd_engine->FreeSample(s); }

int LoadSong(char *fn) { if(snd_engine) return snd_engine->LoadSong(fn); else return -1; }
void PlaySong(int h) { if(snd_engine) snd_engine->PlaySong(h); }
void StopSong(int h) { if(snd_engine) snd_engine->StopSong(h); }
void SetPaused(int h, int p) { if(snd_engine) snd_engine->SetPaused(h,p); }
int  GetSongPos(int h) { if(snd_engine) return snd_engine->GetSongPos(h); else return -1;}
void SetSongPos(int h, int p) { if(snd_engine) snd_engine->SetSongPos(h,p); }
void SetSongVol(int h, int v) { if(snd_engine) snd_engine->SetSongVol(h,v); }
int  GetSongVol(int h) { if(snd_engine) return snd_engine->GetSongVol(h); else return -1; }
void FreeSong(int h) { if(snd_engine) snd_engine->FreeSong(h); }

void snd_Shutdown() { if(snd_engine) snd_engine->shutdown(); }

/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.

#include "xerxes.h"

#define _G_SOUND_CPP
#include "garlick.h"
#ifdef __WIN32__
#define DELAYIMP_INSECURE_WRITABLE_HOOKS 
#include <delayimp.h>

PfnDliHook __pfnDliNotifyHook2;
PfnDliHook __pfnDliFailureHook2;
#endif

#ifdef SND_USE_AUDIERE
#include "snd_audiere.h"
#endif

#ifdef SND_USE_FMOD
#include "snd_fmod.h"
#endif

#ifdef SND_USE_OAKRA
#include "snd_oakra.h"
#endif

#ifdef SND_USE_WASM
#include "snd_wasm.h"
#endif

SoundEngine *snd_engine;
char playingsng[80] = {0};

#ifdef _WIN32
FARPROC WINAPI FmodFailHook(unsigned /* dliNotify */, PDelayLoadInfo  /* pdli */) {
	err("Failure initializing fmod. FMOD.dll is probably missing");
	return 0;
}
#endif


bool snd_Init(int soundEngine) {
	SoundEngine* engine = nullptr;

#ifdef SND_USE_FMOD
	if(soundEngine == 0)
		engine = new SoundEngine_Fmod();
#endif

#ifdef SND_USE_OAKRA
	if(soundEngine == 1)
		engine = new SoundEngine_Oakra();
#endif

#ifdef SND_USE_AUDIERE
	if(soundEngine == 2)
		engine = new SoundEngine_Audiere();
#endif

#ifdef SND_USE_WASM
    engine = new SoundEngine_Wasm();
#endif

	bool result;

	#ifdef _WIN32
	if(soundEngine == 0)
    {
        HMODULE lib = LoadLibrary(TEXT("fmod.dll"));

        if (lib == nullptr)
        {
#ifdef SND_USE_AUDIERE
            if (snd_Init(2))
            {
                return true;
            }
#endif
        }

		__pfnDliFailureHook2 = FmodFailHook;
    }
	#endif
	
	if (engine)
	{
		result = engine->init();
		if (result)
		{
			snd_engine = engine;
		}
	}
	else
	{
		result = false;
	}
	
	#ifdef _WIN32
	if (engine == nullptr)
		__pfnDliFailureHook2 = nullptr;
	#endif

	return result;
}


//////////////////////////////////////
//global sound functions--link to active sound engine
// Dummy music interface
void PlayMusic(const std::string &ssng) { 
	// Check all possible fail conditions. We do this so that if it does fail, we don't
	// unnessarily stop whatever is presently playing.
	if(!snd_engine)
    {
        log("PlayMusic: no sound engine");
        return;
    }

	const char *sng = ssng.c_str();
    log("PlayMusic: playing \"%s\"", sng);

	if (!strlen(sng))
    {
        log("PlayMusic: no song, doing nothing");
        return;
    }
	if (!strcasecmp(sng, playingsng))
    {
        log("PlayMusic: song already playing");
        return;
    }

	VFILE *f = vopen(sng);
	if (!f)
    {
		GarlickFile *gf = GarlickOpen(sng, "library");
		if(!gf) return;
		GarlickClose(gf);
	}
	else
    {
        vclose(f);
    }

	snd_engine->PlayMusic(sng);
}
void StopMusic() { if(snd_engine) snd_engine->StopMusic(); }
void SetMusicVolume(int v) { if(snd_engine) snd_engine->SetMusicVolume(v); }

// Sound effect interface
void *LoadSample(const char *fn) { if(snd_engine) return snd_engine->LoadSample(fn); else return 0; }
int PlaySample(void *s, int vol) { if(snd_engine) return snd_engine->PlaySample(s, vol); else return 0;}
void StopSound(int chan) { if(snd_engine) snd_engine->StopSound(chan); }
// Overkill (2006-11-20)
bool SoundIsPlaying(int chan) { if(snd_engine) return snd_engine->SoundIsPlaying(chan); else return 0; } 
void FreeSample(void *s) { if(snd_engine) snd_engine->FreeSample(s); }

int LoadSong(const std::string &fn) { if(snd_engine) return snd_engine->LoadSong(fn); else return -1; }
void PlaySong(int h) { if(snd_engine) snd_engine->PlaySong(h); }
void StopSong(int h) { if(snd_engine) snd_engine->StopSong(h); }
void SetPaused(int h, int p) { if(snd_engine) snd_engine->SetPaused(h,p); }
int  GetSongPos(int h) { if(snd_engine) return snd_engine->GetSongPos(h); else return -1;}
void SetSongPos(int h, int p) { if(snd_engine) snd_engine->SetSongPos(h,p); }
void SetSongVol(int h, int v) { if(snd_engine) snd_engine->SetSongVol(h,v); }
int  GetSongVol(int h) { if(snd_engine) return snd_engine->GetSongVol(h); else return -1; }
void FreeSong(int h) { if(snd_engine) snd_engine->FreeSong(h); }

void snd_Shutdown() { if(snd_engine) snd_engine->shutdown(); }

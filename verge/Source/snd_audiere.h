#include "audiere.h"
#include "threads.h"
#include "a_handle.h"

using namespace audiere;

//TODO: isolate an individual handleset from Handle and manage instances along with each instance of soundengine_audiere

class SoundEngine_Audiere : public SoundEngine, public audiere::Mutex {
public:
	AudioDevicePtr device;
	OutputStream *music;

	SoundEngine_Audiere() {
		music = 0;
		myStopCallback.eng = this;
		myStopCallback.ref();
		Handle::forceAlloc(HANDLE_TYPE_AUDCHN,1);
	}

	~SoundEngine_Audiere() {
		shutdown();
		myStopCallback.unref();
	}

	std::map<OutputStream *, int> channels;
	int registerChannel(OutputStream *os) { 
		ScopedLock lock(this);
		int h = Handle::alloc(HANDLE_TYPE_AUDCHN,(void*)os);
		channels[os] = h;
		return h;
	}
	void unregisterChannel(OutputStream *os) { 
		ScopedLock lock(this);
		int h = channels[os];
		Handle::free(HANDLE_TYPE_AUDCHN,h);
		channels.erase(os);
	}
	
	class MyStopCallback : public RefImplementation<audiere::StopCallback> {
	public:
		SoundEngine_Audiere *eng;
		void __stdcall streamStopped(StopEvent* event) {
			eng->unregisterChannel(event->getOutputStream());
		}
	};

	MyStopCallback myStopCallback;

	
	
	bool init() {
		device = AudioDevicePtr(OpenDevice());
		device->registerCallback(&myStopCallback);
		if (!device) return false;
		return true;
	}

	void shutdown() {
		StopMusic();
		device = 0;
	}


	void PlayMusic(const char *sng) {
		StopMusic();
		music = OpenSound(device,sng,true);
		if(!music) err("Could not open specified music file: %s",sng);
		music->ref();
		music->setRepeat(true);
		music->play();
	}
	void StopMusic() { 
		if(music) music->unref(); music = 0;
	}
	void SetMusicVolume(int v) {
		if(music) music->setVolume(v/100.0f);
	}

	void *LoadSample(const char *fn) {
		SampleSourcePtr ss = audiere::OpenSampleSource(fn);
		if(!ss) return 0;
		SampleBuffer *sb = audiere::CreateSampleBuffer(ss);
		return (void*)sb;
	}
	int PlaySample(void *sample, int vol) {
		SampleBuffer *sb = (SampleBuffer *)sample;
		SampleSource *ss = sb->openStream();
		OutputStream *os = device->openStream(ss);
		os->ref();
		os->setVolume(vol/100.0f);
		int h = registerChannel(os);
		os->play();
		return h;
	}
	void StopSound(int chan) {
		if(!Handle::isValid(HANDLE_TYPE_AUDCHN, chan)) return;
		OutputStream *os = (OutputStream*)Handle::getPointer(HANDLE_TYPE_AUDCHN,chan);
		if(!os) return; //maybe this sound is already gone
		os->unref();
		//os->stop(); //necessary?
	}
	int SoundIsPlaying(int chan) {
		if(!Handle::isValid(HANDLE_TYPE_AUDCHN, chan)) return 0;
		OutputStream *os = (OutputStream*)Handle::getPointer(HANDLE_TYPE_AUDCHN,chan);
		if(!os) return 0;
		return os->isPlaying()?1:0;
	}
	void FreeSample(void *sample) {
		SampleBuffer *sb = (SampleBuffer *)sample;
		sb->unref();
	}

	int LoadSong(const char *fn){return 0;}
	void PlaySong(int h){}
	void StopSong(int h){}
	void SetPaused(int h, int p){}
	int GetSongPos(int h){return 0;}
	void SetSongPos(int h, int p){}
	void SetSongVol(int h, int v){}
	int GetSongVol(int h){return 0;}
	void FreeSong(int h){}

};
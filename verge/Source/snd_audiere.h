#include "audiere.h"
#include "threads.h"
#include "a_handle.h"
#include "garlick.h"

#include <string>

using namespace audiere;

//TODO: isolate an individual handleset from Handle and manage instances along with each instance of soundengine_audiere
//not that we ever have multiple instances. but handle could use that flexibility anyway

class SoundEngine_Audiere : public SoundEngine, public audiere::Mutex {
public:
	AudioDevicePtr device;
	OutputStream *music;
	Handle::HandleSet<OutputStream*> songHandles;

	class AudiereFile : public RefImplementation<File> {
	private:
		GarlickFile *gf;
		AudiereFile(GarlickFile *gf) { this->gf = gf; }
		~AudiereFile() { 
			GarlickClose(gf);
		}
	public:
		static AudiereFile *tryOpen(const std::string &fname) {
			GarlickFile *gf = GarlickOpen(fname.c_str(),"library");
			if(!gf) return 0;
			return new AudiereFile(gf);
		}
		virtual int ADR_CALL read (void *buffer, int size) {
			return GarlickRead(buffer,1,size,gf);
		}
		virtual bool ADR_CALL seek (int position, SeekMode mode) {
			return GarlickSeek(gf,position,mode)==0;
		}
		virtual int ADR_CALL tell () {
			return (int)GarlickTell(gf);
		}
	};

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
		if(h==0) return; //wasnt a sound effect
		Handle::free(HANDLE_TYPE_AUDCHN,h);
		channels.erase(os);
		os->unref();
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
		device = AudioDevicePtr(OpenDevice("winmm"));
		device->registerCallback(&myStopCallback);
		if (!device) return false;
		return true;
	}

	void shutdown() {
		StopMusic();
		device = 0;
	}


	void PlayMusic(const std::string &sng) {
		AudiereFile *af = AudiereFile::tryOpen(sng);
		if(!af) err("Could not open specified music file: %s",sng);
		
		StopMusic();
		music = OpenSound(device,FilePtr(af),true);
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
		os->stop(); //wait for it to naturally stop, then unref it
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

	int LoadSong(const std::string &sng) {
		AudiereFile *af = AudiereFile::tryOpen(sng);
		if(!af) err("Could not load specified music file: %s",sng);
		
		StopMusic();
		OutputStream *os = OpenSound(device,FilePtr(af),true);
		if(!os) err("Could not load specified music file: %s",sng);
		os->ref();
		os->setRepeat(true);
		int h = songHandles.alloc(os);
		return h;
	}

	void PlaySong(int h) {
		songHandles[h]->play();
	}
	void StopSong(int h) {
		songHandles[h]->stop();
		songHandles[h]->reset();
		OutputStream *os;
	}
	void SetPaused(int h, int p) {
		if(p) songHandles[h]->stop();
		else if(p) songHandles[h]->play();
	}
	int GetSongPos(int h) { err("Not implemented yet, sorry"); return 0; }
	void SetSongPos(int h, int p){ err("Not implemented yet, sorry"); }
	void SetSongVol(int h, int v) {
		songHandles[h]->setVolume(v/100.0f);
	}
	int GetSongVol(int h) {
		return (int)songHandles[h]->getVolume()*100;
	}
	
	void FreeSong(int h) {
		StopSong(h);
		songHandles[h]->unref();
	}

};
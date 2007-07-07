/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


#include "../oakra/oakra.h"

//this should be defined in DUMB.
extern "C" {
	extern int dumb_resampling_quality;
}

class SoundEngine_Oakra_vfile : public OAKRA_FileSystem {
public:
	void *fopen(const char *fname, char *mode) { return ::vopen((char *)fname); }
	size_t fread(void *buf, int size, int count, void *stream) {
		int read = ::vread((char *)buf,size*count,(VFILE *)stream);
		if(size==1) return read;
		else if(size==2) return read>>1;
		else if(size==4) return read>>2;
		else if(size==8) return read>>3;
		else return read/size;
	}
	int fclose(void *stream) { vclose((VFILE *)stream); return 0; }
	int fseek(void *stream, long offset, int origin) { ::vseek((VFILE *)stream,offset,origin); return 0; }
	long ftell(void *stream) { return ::vtell((VFILE *)stream); }
	int feof(void *stream) { return ::veof((VFILE *)stream); }
};

class SoundEngine_Oakra : public SoundEngine {
protected:

	friend OAKRA_InputModule *getInputModuleCallback(void *param, int filetype);
	class MusicPlayer {
	public:
		OAKRA_Module_MusicPlayer *mp;
		SoundEngine_Oakra *engine;
		OAKRA_Voice *voice;

		MusicPlayer(SoundEngine_Oakra *engine) {
			voice = 0;
			this->engine = engine;
			mp = new OAKRA_Module_MusicPlayer();
			mp->getInputModuleCallback.func = ::getInputModuleCallback;
			mp->getInputModuleCallback.param = this;
		}

		OAKRA_InputModule *getInputModuleCallback(int filetype) {
			switch(filetype) {
				case OAKRA_FILETYPE_OGG: return new OAKRA_Module_InputOGG();
				case OAKRA_FILETYPE_MP3: return new OAKRA_Module_InputMP3();
				case OAKRA_FILETYPE_MOD: return new OAKRA_Module_InputDUMB(engine->params.dumbFormat);
				case OAKRA_FILETYPE_WAV: return new OAKRA_Module_InputWAV();
				default: throw "unexpected filetype ";
			}
		}
	

		~MusicPlayer() {
			if(voice) delete voice;
			delete mp;
		}

		bool load(bool loop, char *fname) {
			close();
			if(!mp->load(loop,fname)) return false;
			voice = engine->out->getVoice(mp->queryFormat());
			voice->setSource(mp);
			return true;
		}

		void close() {
			if(voice) {
				delete voice;
				voice = 0;
			}
			mp->close();
		}

	};

	class DummyMusicPlayer {
	public:
		std::string currMusic;
		SoundEngine_Oakra *engine;
		MusicPlayer *mp1, *mp2;
		MusicPlayer *mpa, *mpb;
		DummyMusicPlayer(SoundEngine_Oakra *engine) {
			mpa = mpb = 0;
			currMusic = "";
			this->engine = engine;
			mp1 = new MusicPlayer(engine);
			mp2 = new MusicPlayer(engine);
		}
		~DummyMusicPlayer() {
			delete mp1;
			delete mp2;
		}

		void PlayMusic(char *sng) {
			if(currMusic == sng) return;
			if(!mpa) {
				if(mp1->load(true,sng)) {
					currMusic = sng;
					mpa = mp1;
					mpb = mp2;
				}
			} else {
				if(mpb->load(true,sng)) {
					mpa->mp->close();
					currMusic = sng;
					mpa = mpb;
					if(mpa == mp1) mpb = mp2; else mpb = mp1;
				}
			}
		}
		void StopMusic() {
			if(mpa) mpa->close();
			currMusic = "";
			mpa = mpb = 0;
		}
		void SetMusicVolume(int v) {
			if(mpa) mpa->voice->setVol(v*255/100);
		}
	} *dummyPlayer;


	OAKRA_Module_OutputDS *dsout;
	//OAKRA_Module_OutputWinMM *mmout;
	OAKRA_OutputDriver *out;

	struct {
		OAKRA_Format dumbFormat;
		int driver;
		int voices;
	} params;

public:

	bool init() {

		OAKRA_init();
		OAKRA_setFilesystem(new SoundEngine_Oakra_vfile());
		//mmout = 0;
		dsout = 0;

		//default parameters
		params.dumbFormat.rate = 44100;
		params.dumbFormat.channels = 2;
		params.dumbFormat.format = OAKRA_S16;
		params.driver = 0;
		params.voices = 8;

		//scan for overriden parameters
		if(cfg_KeyPresent("soundparams")) {
			std::vector<std::string> ps = cfg_Tokenize("soundparams",';');
			int i = 0;
			while(i < ps.size()) {
				std::string &cmd = ps[i++];
				if(cmd == "modrate")
					params.dumbFormat.rate = atoi(ps[i++].c_str());
				else if(cmd == "modbits")
					params.dumbFormat.format = atoi(ps[i++].c_str())==16?OAKRA_S16:OAKRA_U8;
				else if(cmd == "modstereo")
					params.dumbFormat.format = atoi(ps[i++].c_str())==1?2:1;
				else if(cmd == "modquality")
					dumb_resampling_quality = atoi(ps[i++].c_str());
				else if(cmd == "driver")
					params.driver = atoi(ps[i++].c_str());
				else if(cmd == "voices")
					params.voices = atoi(ps[i++].c_str());
			}
		}

		//postprocess parameters
		params.dumbFormat.size = OAKRA_Module::calcSize(params.dumbFormat);

		//init driver
		if(params.driver==0) {
			dsout = new OAKRA_Module_OutputDS();
			dsout->start(0);
			dsout->beginThread();
			out = dsout;
		} else if(params.driver==1) {
			//just use the mod output format as the winmm output format
			//cant see any reason this will be problematic
			//mmout = new OAKRA_Module_OutputWinMM(params.voices,params.dumbFormat);
			//mmout->start();
			//mmout->beginThread();
			//out = mmout;
		}

		//the dummy interface musicplayer
		dummyPlayer = new DummyMusicPlayer(this);

		return true;
	}

	void shutdown() {
		delete this;
	}

	~SoundEngine_Oakra() {
		if(dsout != 0) delete dsout;
		//todo
	}

	void PlayMusic(char *sng) { dummyPlayer->PlayMusic(sng); }
	void StopMusic() { dummyPlayer->StopMusic(); }
	void SetMusicVolume(int v) { dummyPlayer->SetMusicVolume(v); }

	void *LoadSample(const char *fn) { return 0; }
	void PlaySample(void *sample, int vol) {}
	void StopSound(int chan) {}
	int SoundIsPlaying(int chan) { return 0; }
	void FreeSample(void *sample) {}


	int LoadSong(const char *fn) { return -1; }
	void PlaySong(int h) {}
	void StopSong(int h) {}
	void SetPaused(int h, int p) {}
	int GetSongPos(int h) { return -1; }
	void SetSongPos(int h, int p) {}
	void SetSongVol(int h, int v) {}
	int GetSongVol(int h) { return -1; }
	void FreeSong(int h) {}
};

OAKRA_InputModule *getInputModuleCallback(void *param, int filetype) {
	return ((SoundEngine_Oakra::MusicPlayer *)param)->getInputModuleCallback(filetype);
}
//TODO: 
//modify oggvorbis and dumb to provide malloc hooks.
//
//create the sound-effects driver stuff
//e.g. 
//ogg -> sfxdriver -> sfxmanager -> mixer
//the sfx driver will detect when the sound effect is done and
//set some flags.
//the sfxmanager will then free the voice
//there are some interesting possibilities, then
//for example, using mods as sound effects.
//
//see the 16S resampler module for notes on how 
//one might use it to make a heavyweight sfx controller
//
//
//---consider adding a comment to the malloc system
//that way, we can get a log of all the active mallocs and who/why theyre there.
//
//consider what we can use to fade out and pause songs.
//the same code might be useful for crossfading

//LAWS OF OAKRA
//I. when a module returns less samples to a driver than it asks for, then the voice that is playing it
//   shall be considered DEAD.

//configure
#define OAKRA_USE_DUMB
#define OAKRA_USE_OGG
#define OAKRA_USE_MP3
#define OAKRA_MAX_PATH (511)
#define OAKRA_USE_SWDSOUND
#define OAKRA_USE_DSOUND
#define OAKRA_USE_WINMM
//#define OAKRA_BIG_ENDIAN
///////


#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <map>
#include <stack>
#include <ctype.h>
#include <queue>


//nothing in here right now
#include "queue.h"

//set up your mixing/generation buffers
//to be able to handle this
//you may be requested for less;
//you will never be requested for more.
//const int OAKRA_MAX_SAMPLES = 32;
const int OAKRA_MAX_SAMPLES = 2000;

//try to use this min_samples when doing anything that will introduce latency
//small values will result large overhead due to function calls
//and loop setups from synthesizers.
//so this isnt a strict minimum as much as it is a latency target.
//later we should probably give each module its own latency control
//it would be rad to be able to have tight control over where
//the latency was coming from.
//for example, pulling small chunks from a wave is not going to incur overhead;
//but pulling small chunks from mods and mp3s might.
const int OAKRA_MIN_SAMPLES = 32;

const int OAKRA_U8 = 0;
const int OAKRA_S16 = 1;
const int OAKRA_S32 = 2;

const int OAKRA_STATUS_STOPPED = 0;
const int OAKRA_STATUS_PLAYING = 1;

const int OAKRA_FILETYPE_OGG = 1;
const int OAKRA_FILETYPE_MP3 = 2;
const int OAKRA_FILETYPE_MOD = 3;
const int OAKRA_FILETYPE_WAV = 4;

struct OAKRA_Format {
	int channels, format, rate, size;
};

//implements an easy to use, bland callback
template <typename T> struct OAKRA_Callback {
	OAKRA_Callback() { func = 0; param = 0; }
	T operator()() { if(func) return func(param); }
	T (*func)(void *param);
	void *param;
};
template <typename T, typename TARG> struct OAKRA_ArgumentCallback {
	OAKRA_ArgumentCallback() { func = 0; param = 0; }
	T operator()(TARG arg) { if(func) return func(param,arg); }
	T (*func)(void *param, TARG arg);
	void *param;
};


#undef feof
class OAKRA_FileSystem {
public:
	virtual void *fopen(const char *fname, char *mode) = 0;
	virtual size_t fread(void *buf, int size, int count, void *stream) = 0;
	virtual int fclose(void *stream) = 0;
	virtual int fseek(void *stream, long offset, int origin) = 0;
	virtual long ftell(void *stream) = 0;
	virtual int feof(void *stream) = 0;
};

class OAKRA_FileSystem_Stdio : public OAKRA_FileSystem {
public:
	void *fopen(const char *fname, char *mode) { return ::fopen(fname,mode); }
	size_t fread(void *buf, int size, int count, void *stream) { return ::fread(buf,size,count,(FILE *)stream); }
	int fclose(void *stream) { return ::fclose((FILE *)stream); }
	int fseek(void *stream, long offset, int origin) { return ::fseek((FILE *)stream,offset,origin); }
	long ftell(void *stream) { return ::ftell((FILE *)stream); }
	int feof(void *stream) { return ::feof((FILE *)stream); }
};



#ifdef _OAKRA_CPP_
OAKRA_Format OAKRA_FORMAT_NULL = {0,0,0,0};
OAKRA_FileSystem *_OAKRA_fs = new OAKRA_FileSystem_Stdio();
OAKRA_ArgumentCallback<void,char*> _OAKRA_error;
#else
extern OAKRA_Format OAKRA_FORMAT_NULL;
extern OAKRA_FileSystem *_OAKRA_fs;
extern OAKRA_ArgumentCallback<void,char*> _OAKRA_error;
#endif

void OAKRA_init();
void OAKRA_setFilesystem(OAKRA_FileSystem *fs);


class OAKRA_Manager {
private:
	static int mem_usage;
	static std::map<void *, int> allocs;

public:

	////////WTF
	/////VC++ STL STD::MAP CRASHES FOR NO REASON
	static void reportMalloc(void *ptr, int amt) {
//		mem_usage += amt;
		//allocs[ptr] = amt;
	}
	static void reportFree(void *ptr) {
		//mem_usage -= allocs[ptr];
		//allocs.erase(ptr);
	}

	//this only reports memory allocated with the OAKRA_Module's base class
	//malloc method.
	//it won't report memory used by class member variables
	//theres no easy way to get that from c++.
	static int getMemoryUsage() {
		return mem_usage;
	}
};

class OAKRA_Module {
public:
	virtual int generate(int samples, void *buf) { return 0; }

	int adapt_to_2S16(int samples, void *buf, OAKRA_Format &sourceFormat) {
		short *sbuf = (short *)buf;
		unsigned char *bbuf = (unsigned char *)buf;
		if(sourceFormat.format == OAKRA_S16) {
			if(sourceFormat.channels == 2) return samples;
			for(int i=samples-1,j=samples*2-2;i>=0;i--,j-=2)
				sbuf[j] = sbuf[j+1] = sbuf[i];
		} else {
			if(sourceFormat.channels == 1)
				for(int i=samples-1,j=samples*2-2;i>=0;i--,j-=2)
					sbuf[j] = sbuf[j+1] = ((int)bbuf[i]-128)<<8;
			else
				for(int j=samples*2-2;j>=0;j--)
					sbuf[j] = sbuf[j+1] = ((int)bbuf[j]-128)<<8;
		}
		return samples;
	}

	//trashes some samples using the buffer provided
	void trash(int samples, void *buf, int bufsamples) {
		while(samples) {
			int todo = std::min(samples,bufsamples);
			generate(todo,buf);
			samples -= todo;
		}
	}

	static int calcSize(OAKRA_Format &format) {
		int size = format.channels;
		if(format.format == OAKRA_S16) size *= 2;
		return size;
	}

	static int getFormatShift(OAKRA_Format &format) {
		if(format.size==1) return 0;
		else if(format.size==2) return 1;
		else if(format.size==4) return 2;
		return -1; //try and crash!
	}

	static void *malloc(int len) { 
		void *ptr = ::malloc(len);
		OAKRA_Manager::reportMalloc(ptr,len);
		return ptr;
	}
	void *malloc(int sampsize, int len) { return malloc(sampsize*len); }
	void *malloc(int sampsize, int channels, int len) { return malloc(sampsize*len*channels); }
	void *realloc(void *ptr, int len) {
		OAKRA_Manager::reportFree(ptr);
		ptr = ::realloc(ptr,len);
		OAKRA_Manager::reportMalloc(ptr,len);
		return ptr;
	}
	void free(void *ptr) {
		::free(ptr);
		OAKRA_Manager::reportFree(ptr);
	}
	typedef void FILE;
	static FILE *fopen(const char *fname, char *mode) { return _OAKRA_fs->fopen(fname,mode); }
	static size_t fread(void *buf, int size, int count, FILE *stream) { return _OAKRA_fs->fread(buf,size,count,stream); }
	static int fclose(FILE *stream) { return _OAKRA_fs->fclose(stream); }
	static int fseek(FILE *stream, long offset, int origin) { return _OAKRA_fs->fseek(stream,offset,origin); }
	static long ftell(FILE *stream) { return _OAKRA_fs->ftell(stream); }
	static int feof(FILE *stream) { return _OAKRA_fs->feof(stream); }

	static int generateNoise(int samples, void *buf, int sampleSize) {
		for(int i=0;i<samples;i++)
			for(int j=0;j<sampleSize;j++)
				((char *)buf)[i*sampleSize+j] = rand();
		return samples;
	}

	static int generateSilence(int samples, void *buf, int sampleSize) {
		char *cbuf = (char *)buf;
		int n = sampleSize*samples;
		memset(buf,0,n);
		return samples;
	}

	static int generateSilence(int samples, void *buf, OAKRA_Format &format) {
		char *cbuf = (char *)buf;
		int n = format.size*samples;
		memset(buf,0,n);
		return samples;
	}


	template<typename T>
	T *malloc() {
		return (T *)malloc(sizeof(T));
	}
	template<typename T>
	T *malloc(int amt) {
		return (T *)malloc(sizeof(T)*amt);
	}
	template<typename T>
	T **nmalloc(int number) {
		return (T **)malloc(sizeof(T)*number);
	}
};

class OAKRA_IQueryFormat {
public:
	virtual OAKRA_Format &queryFormat() = 0;
};

//this is basically a filter class
//one source, one sink
class OAKRA_BasicModule : public OAKRA_Module {
public:
	OAKRA_Module *source, *sink;
	OAKRA_BasicModule() { source = sink = 0; }
};

class OAKRA_Voice : public OAKRA_BasicModule {
public:
	OAKRA_Voice() { dead = false; }
	virtual ~OAKRA_Voice() { }
	virtual void setPan(int pan) = 0;
	virtual int getPan() =0;
	virtual void setVol(int vol) = 0;
	virtual int getVol()=0;
	virtual void setSource(OAKRA_Module *source) = 0;
	
	virtual void volFade(int start, int end, int ms)=0;

	//call this when youre in the middle of rendering the voice, but have decided to quit
	//the driver will then trash the voice as soon as it gets the chance.
	//you dont have to have the driver locked to call it. that would be illogical,
	//as the driver is currently locked while it is rendering!
	virtual void die() {
		dead = true;
	}

	//indicates whether a voice is dead
	bool dead;

	//callback fired when a voice dies
	OAKRA_Callback<void> callbackDied;
};

//class OAKRA_VoiceManager : public OAKRA_Module {
//public:
//    virtual OAKRA_Voice *getVoice(OAKRA_Format &format) = 0;
//	virtual void freeVoice(OAKRA_Voice *voice) = 0;
//};

class OAKRA_OutputDriver : public OAKRA_BasicModule {
public:
	virtual ~OAKRA_OutputDriver() {} ;
    virtual OAKRA_Voice *getVoice(OAKRA_Format &format) = 0;
	virtual OAKRA_Voice *getVoice(OAKRA_Format &format, OAKRA_Module *source) = 0;

	//this should be safe to call from within a driver callback. in general, nothing else will be.
	//even if you dont delete the voice (if youre recycling it) you should clear out the source asap
	virtual void freeVoice(OAKRA_Voice *voice) = 0;

	virtual void lock() = 0;
	virtual void unlock() = 0;
};

//this is a N:1 class. many sources and only one sink.
class OAKRA_ReductionModule : public OAKRA_Module {
	std::stack<int> sourceHandles;
public:
	OAKRA_Module **sources;
	int sourceCount;
	OAKRA_ReductionModule() { sources = 0; sourceCount = 0; }
	void setSourceCount(int count) { 
		sourceCount = count;
		if(sources) free(sources); 
		sources = nmalloc<OAKRA_Module>(count);
		while(!sourceHandles.empty()) sourceHandles.pop();
		for(int i=0;i<sourceCount;i++) {
			sources[i] = 0;
			sourceHandles.push(i);
		}
	}
	void setSource(int index, OAKRA_Module *source) {
		sources[index] = source;
	}
	int allocateSource() {
		int ret = sourceHandles.top();
		sourceHandles.pop();
		return ret;
	}
	void freeSource(int handle) {
		sourceHandles.push(handle);
	}
	
};


class OAKRA_InputModule : public OAKRA_BasicModule, public OAKRA_IQueryFormat {
protected:
	bool bPaused;
public:
	virtual bool load(char *fname) = 0;
	virtual void close() = 0;
	virtual int getStatus() = 0;
	virtual bool setPosition(int pos) { return false; }
	virtual int getLength() { return -1; }

	virtual void pause(bool paused) {
		bPaused = paused;
	}
	virtual void play() {
		bPaused = false;
	}
};


//this is a null output driver
//this might be useful to pump a sound source but throw away the samples
//we have a choice in here.. we could generate samples one at a time, or
//make a buffer and generate into it.
//one at a time would be inefficient, but thats what we're doing for now!
class OAKRA_Module_OutputNull : public OAKRA_ReductionModule {
public:
	void dump(int samples) {
		long long temp;
		for(int i=0;i<sourceCount;i++) {
			for(int j=0;j<samples;j++)
				sources[i]->generate(1, &temp);
		}
	}
	void start() {}
};



//this is the base class for integer resampling
//classes, i.e. high-performance fast-forwarding
//it should resample integerwise according to a skip variable
class OAKRA_Module_ResInteger_Base : public OAKRA_BasicModule {
protected:
	int skip;
	int skipping;
public:
	OAKRA_Module_ResInteger_Base() { skip = 0; }
	void setSkip(int skip) {
		this->skip = skip;
		skipping = 0;
	}
};

//16S version of the integer resampler fast-fowarder
//works on mono or stereo. 
//as usual, the SAMPLEBUFSIZE is a tradeoff between perf. and mem
template<int _channels, int SAMPLEBUFSIZE>
class OAKRA_Module_ResInteger_16S : public OAKRA_Module_ResInteger_Base {
private:
	int cursor;
	short *inbuf;
	int buflen;
public:
	OAKRA_Module_ResInteger_16S() { 
		skipping = 0;
		cursor = SAMPLEBUFSIZE;
		inbuf = (short *)malloc(2,_channels,SAMPLEBUFSIZE);
		buflen = SAMPLEBUFSIZE;
	}
	~OAKRA_Module_ResInteger_16S() { free(inbuf); }

	int generate(int samples, void *buf) {
		int done;

		//quickish bypass for non-skipping state
		if(!skip) {
			//if there is no buffer left to pick from 1:1, then
			//just pass the buck to the source generate and bail out
			if(cursor==buflen)
				return source->generate(samples,buf);
			
			
			//use some samples from our buffer
			short *dbuf = (short *)buf;
			int todo = samples;
			if(buflen-cursor < todo)
				todo = buflen-cursor;
			done = source->generate(todo,dbuf);
			cursor += done;
			dbuf += done;

			//if we've done all our work, bail out
			if(cursor==buflen) return done;

			//otherwise we have one last thing to do--generate more samples
			//from the source, without buffering them
			done += source->generate(samples-done,dbuf);
			return done;
		}

		//otherwise, we need to skip. thats a bit more work, with looping and all
		short *dbuf = (short *)buf;
		done = 0;
		for(;;) {
			//if we're done, bail
			if(done == samples) return samples;
			//if we need more input data, grab it
			if(cursor==buflen) {
				cursor = 0;

				buflen = source->generate(SAMPLEBUFSIZE,inbuf);
				//if the source quit generating, then just bail out
				if(buflen==0) 
					return done;
			}
			//if it is time to output a sample
			if(skipping == 0) {
				for(int i=0;i<_channels;i++)
					*dbuf++ = inbuf[(cursor<<1)+i];
				samples--;
				skipping = skip;
			} else skipping--;
			cursor++;
		}

		return samples;
	}
};

//the main 16S interpolating resampler.						 
//template argument is, of course, the number of channels
//as usual, the SAMPLEBUFSIZE is a tradeoff between perf. and mem
//
//you should use this whenever some library gives you
//an output in a samplerate you don't want (probably ogg or mp3)
//
//this might also be used for a heavyweight sound effect module
//with a changeable pitch.
//for example, you might generate interpolated samples
//and then in the sfx module, stereoize and voladjust it.
//that would not be especially slow.
//
//this module will generate a few extra zeros beyond the end of
//the input, until its resampling batch is done.
//right now, those zeros are actually resampled.
//this could be changed somehow in the future if needed to shave cycles
template<int _channels, int SAMPLEBUFSIZE>
class OAKRA_Module_ResInterp_S16 : public OAKRA_BasicModule {
public:
	int tcursor, incr;
	short *inbuf;
	short *first, *second, *last;
	bool dead;
	int scheduledIncr;

	//setting this in the midst of a resampling session will
	//break the speed at which the SAMPLEBUFSIZE's worth of samples
	//will be played
	void set(int from, int to) {
		incr = (from<<15)/to;
	}

	//ok this is a weird function
	//at the moment this function is called, it is assumed that
	//the input waveform is changing. therefore, the next time 
	//that the resampler asks its source for samples, it will
	//apply the new resampling settings
	void scheduleSet(int from, int to) {
		scheduledIncr = (from<<15)/to;
	}


	//resets the buffering and sample cursor
	//for use when the input waveform changes sources
	//and you need to basically flush the buffer
	void reset() {
		scheduledIncr = -1;
		tcursor = SAMPLEBUFSIZE<<15;
	}

	OAKRA_Module_ResInterp_S16(int from, int to) {
		init();
		set(from,to);
	}

	void init() {
		dead = false;
		scheduledIncr = -1;
		tcursor = SAMPLEBUFSIZE<<15;

		//allocate an extra sample which will contain the sample immediately
		//prior to the beginning of the current buffer.
		//the current buffer begins at `second`.
		inbuf = (short *)malloc(2*_channels*(SAMPLEBUFSIZE+1));
		first = inbuf;
		second = inbuf+_channels;
		last = inbuf+SAMPLEBUFSIZE*_channels;

		//initialize the last-samples to zero
		//this seems reasonable, but it will have one of two side effects
		//(1) if the input waveform starts at zero, then the first interpolated
		//sample will still be zero, and a latency of one sample may be introduced
		//(2) if the input waveform starts at non-zero, then the first interpolated
		//sample will soften the click a little bit since it will be averaged with
		//zero, perhaps, and the latency may be added.
		//this is all irrelevant if we are downsampling, since the first interpolated
		//output sample will be something like srcidx 1.4, as opposed to 0.4
		for(int i=0;i<_channels;i++) 
			last[i] = 0;
	}

	OAKRA_Module_ResInterp_S16() { init(); }
	~OAKRA_Module_ResInterp_S16() { free(inbuf); }
	int generate(int samples, void *buf) {
		short *dbuf = (short *)buf;
		int done = 0;
		for(;;) {

			//return if we're done
			if(dead) return done;

			//if we need more input samples, read them
			if((tcursor>>15) >= SAMPLEBUFSIZE) {
				//copy the last sample in the buffer to its reserved spot at the
				//beginning of the next buffer
				for(int i=0;i<_channels;i++) first[i] = last[i];
				int generated = source->generate(SAMPLEBUFSIZE,second);
				tcursor -= (SAMPLEBUFSIZE<<15);
				//if we didnt generate as many as we wanted, this is
				//a good place to introduce zeros until we're finished
				if(generated != SAMPLEBUFSIZE) {
					//second += generated*_channels; ????? WTF
					short *temp = second + generated*_channels;
					generateSilence(SAMPLEBUFSIZE-generated,temp,_channels*2);
					dead = true;
				}

				//see scheduleSet()
				if(scheduledIncr != -1) {
					incr = scheduledIncr;
					scheduledIncr = -1;
				}
			}

			for(;;) {
				//if we are finished, bail out
				if(done == samples) return samples;
				//figure out what sample we're on
				int sampidx = (tcursor>>15);
				//if that exceeds the size of our sample buffer, break out and refill it
				if(sampidx >= SAMPLEBUFSIZE) break;
				//shortidx is the offset into the sample buffer
				int shortidx = sampidx*_channels;

				//mix a sample for each channel
				for(int i=0;i<_channels;i++) {
					int sample = second[shortidx-_channels+i];
					sample += (((second[shortidx+i]-sample))*(tcursor&0x7FFF))>>15;
					*dbuf++ = sample;
				}

				tcursor += incr;
				done++;
			}
		}
		return samples;
	}
};
	



////////////////////////////////////////////////////////////////////////////

class OAKRA_Module_2S16_Mixer : public OAKRA_ReductionModule {
	int *mixbuf;
public:
	OAKRA_Module_2S16_Mixer() {
		mixbuf = (int *)malloc(4,2,OAKRA_MIN_SAMPLES);
	}
	~OAKRA_Module_2S16_Mixer() {
		free(mixbuf);
	}
	int shift;
	int generate(int samples, void *buf) {
		int done = 0;
		short *sbuf = (short *)buf;
		while(done != samples) {
	
			int count = 0;
			int todo = std::min(samples-done,OAKRA_MIN_SAMPLES);
			done += todo;
			int i=0;
			for(;i<sourceCount;i++) {
				if(!sources[i]) continue;
				int generated = sources[i]->generate(todo,sbuf);
				if(!generated) continue;
				count++;
				for(int j=0;j<generated;j++) {
					mixbuf[j*2] = sbuf[j*2];
					mixbuf[j*2+1] = sbuf[j*2+1];
				}
				for(int j=generated;j<todo;j++) {
					mixbuf[j*2] = 0;
					mixbuf[j*2+1] = 0;
				}
				i++;
				break;
			}
			for(;i<sourceCount;i++) {
				if(!sources[i]) continue;
				int generated = sources[i]->generate(todo,sbuf);
				if(!generated) continue;
				for(int j=0;j<generated;j++) {
					mixbuf[j*2] += sbuf[j*2];
					mixbuf[j*2+1] += sbuf[j*2+1];
				}
				count++;
			}
			if(!count) {
				memset(sbuf,0,4*todo);
			} else {
				for(int j=0;j<todo;j++) {
					int temp = mixbuf[j*2]>>shift;
					if(temp<-32768) temp = -32768;
					if(temp>32767) temp = 32767;
					sbuf[j*2] = temp;
					temp = mixbuf[j*2+1]>>shift;
					if(temp<-32768) temp = -32768;
					if(temp>32767) temp = 32767;
					sbuf[j*2+1] = temp;
				}
			}
			sbuf += todo*2;
		}
		
		return samples;
	}
};


namespace _OAKRA_INT {
	extern "C" {
		struct DUH;
		struct DUH_SIGRENDERER;
		void unload_duh(DUH *duh);
		DUH *dumb_load_it_quick(const char *filename);
		DUH *dumb_load_xm_quick(const char *filename);
		DUH *dumb_load_s3m_quick(const char *filename);
		DUH *dumb_load_mod_quick(const char *filename);
		long duh_render(DUH_SIGRENDERER *sigrenderer,
						int bits, int unsign,
						float volume, float delta,
						long size, void *sptr);
		DUH_SIGRENDERER *duh_start_sigrenderer(DUH *duh, int sig, int n_channels, long pos);
	}
}

class OAKRA_Module_InputDUMB : public OAKRA_InputModule {
	_OAKRA_INT::DUH *duh;
	_OAKRA_INT::DUH_SIGRENDERER *sr;
	OAKRA_Format format;
	int status;
	float dumb_renderRate;
	int dumb_bits, dumb_unsign;

public:

	OAKRA_Module_InputDUMB(OAKRA_Format &format) {
		this->format = format;
		status = OAKRA_STATUS_STOPPED;
		dumb_renderRate = 65536.0f/format.rate;
		if(format.format == OAKRA_S16) {
			dumb_bits = 16;
			dumb_unsign = 0;
		} else if(format.format == OAKRA_U8) {
			dumb_bits = 8;
			dumb_unsign = 1;
		}
		duh = 0;
	}

	OAKRA_Format &queryFormat() { return format; }
	int getStatus() { return OAKRA_STATUS_PLAYING; }

	void close() { 
		if(duh) _OAKRA_INT::unload_duh(duh);
		duh = 0;
		status = OAKRA_STATUS_STOPPED;
	}

	bool load(char *fname) {
		close();
		int len = (int)strlen(fname);
		if(toupper(fname[len-1]) == 'T')
			duh = _OAKRA_INT::dumb_load_it_quick(fname);
		else if(toupper(fname[len-1]) == 'D')
			duh = _OAKRA_INT::dumb_load_mod_quick(fname);
		else if(toupper(fname[len-2]) == '3')
			duh = _OAKRA_INT::dumb_load_s3m_quick(fname);
		else if(toupper(fname[len-2]) == 'X')
			duh = _OAKRA_INT::dumb_load_xm_quick(fname);		
		
		if(duh) {
			sr = _OAKRA_INT::duh_start_sigrenderer(duh, 0, format.channels, 0);
			play();
			status = OAKRA_STATUS_PLAYING;
			return true;
		}
		return false;
	}

	int generate(int samples, void *buf) {
		if(status == OAKRA_STATUS_STOPPED || bPaused) return 0;
		int done = (int)_OAKRA_INT::duh_render(sr, dumb_bits, dumb_unsign, 1.0, dumb_renderRate, samples, buf);
		if(done != samples)
			status = OAKRA_STATUS_STOPPED;
		return done;
	}
};

#if defined(OAKRA_USE_OGG)

class OAKRA_Module_InputOGG : public OAKRA_InputModule {
	int status;
	OAKRA_Format format;
	void *ovf;
	int byteshift;
	bool bFileLoaded;
public:
	OAKRA_Module_InputOGG();
	~OAKRA_Module_InputOGG();
	void close();
	bool load(char *fname);
	int generate(int samples, void *buf);
	OAKRA_Format &queryFormat() { return format; }
	int getStatus() { return status; }

};
#endif

//only outputs in mono S16
class OAKRA_Module_InputSine : public OAKRA_BasicModule {
public:
	float phase,delta;
	OAKRA_Module_InputSine(int rate, float freq) {
		phase = 0.0f;
		delta = 2.0f*(float)M_PI/(float)rate*freq;
	}
	int generate(int samples, void *buf) {
		for(int i=0;i<samples;i++) {
			((short *)buf)[i] = (short)(sin(phase)*32767.0);
			phase += delta;
			phase += delta;
			phase += delta;
		}
		return samples;
	}
};


class OAKRA_Module_2S32_to_2S16 : public OAKRA_BasicModule {
public:
	int ibuf[OAKRA_MAX_SAMPLES*2];

	int generate(int samples, void *buf) {
		int done = source->generate(samples,ibuf);
		short *sbuf = (short *)buf;
		for(int i=done-1;i>=0;i--) {
			sbuf[i*2] = ibuf[i*2];
			sbuf[i*2+1] = ibuf[i*2+1];
			int xxx=9;
		}
		return done;
		
	}
};

class OAKRA_Module_2S16_to_1S16 : public OAKRA_BasicModule {
public:
	int generate(int samples, void *buf) {
		short *sbuf = (short *)buf;
		if(!source) return 0;
		int done = source->generate(samples,buf);
		for(int i=0;i<done;i++)
			sbuf[i] = ((int)sbuf[i*2] + (int)sbuf[i*2+1])>>1;
		return done;
	}
};

class OAKRA_Module_Volpanner_Base : public OAKRA_BasicModule {
protected:
	int vf_ctr,vf_cur,vf_incr;

public:
	int pan;
	int vol;

	//programs a volume fade.
	void volFade(int start, int end, int ms, int freq) {
		vf_ctr = ms * freq / 1000;
		int diff = end-start;
		vf_incr = (diff*65536*128)/vf_ctr;
		vf_cur = start * 65536*128+32768*128;
		vol = start;
	}
};

template<int _channels, int format>
class OAKRA_Module_VolPanner_X_to_2S16 : public OAKRA_Module_Volpanner_Base {
public:
	OAKRA_Module_VolPanner_X_to_2S16() {
		vf_ctr = 0;
	}

	template<bool bRamp>
	void adjust(int samples, void *buf, int xlvol, int xrvol) {
		short *sbuf = (short *)buf;
		short *lsbuf = (short*)buf, *rsbuf = (short *)buf+1;
		unsigned char *lbuf = (unsigned char *)buf, *rbuf = (unsigned char *)buf+1;

		//if we're not ramping, go ahead and multiply the vol and pan factors to yield 16 bits of factor in lvol,rvol
		if(!bRamp) {
			xlvol *= vol;
			xrvol *= vol;
		}

		int lvol=xlvol,rvol=xrvol;
		for(int i=samples-1,j=samples*2-2;j>=0;i--,j-=2) {
			//if we're ramping, calculate the current volume factors
			if(bRamp) {
				lvol = xlvol * (vf_cur>>23);
				rvol = xrvol * (vf_cur>>23);
			} 

			if(format==OAKRA_S16) {
				if(_channels==2) {
					rsbuf[j] = (rsbuf[j]*lvol)>>16;
					lsbuf[j] = (lsbuf[j]*rvol)>>16;
				} else {
					short s = sbuf[i];
					rsbuf[j] = (s * rvol) >> 16;
					lsbuf[j] = (s * lvol) >> 16;
				}
			} else {
				if(_channels==2) {
					//r first! otherwise r gets overwritten!
					rsbuf[j] = (((int)rbuf[j] - 128)*rvol)>>8;
					lsbuf[j] = (((int)lbuf[j] - 128)*lvol)>>8;			
				} else {
					int c = (int)lbuf[i] - 128;
					//r first! otherwise r gets overwritten!
					rsbuf[j] = (c*rvol)>>8;
					lsbuf[j] = (c*lvol)>>8;
				}
			}
			//if we're ramping, increment our volume counter
			if(bRamp) vf_cur += vf_incr;
		}

		//if ramping, set our final volume
		if(bRamp) vol = vf_cur>>23;
	}

	int generate(int samples, void *buf) {
		int done = source->generate(samples,buf);
		int lvol,rvol;
		if(pan<0) {
			lvol = 255;
			rvol = 255 + pan;
		} else {
			lvol = 255 - pan;
			rvol = 255;
		}

		int vf_todo = std::min(vf_ctr,done);
		if(vf_todo) {
			adjust<true>(vf_todo,buf,lvol,rvol);
			int shift = (format==OAKRA_S16?(_channels==2?2:1):(_channels==2?1:0));
			adjust<false>(done-vf_todo,(char *)buf+(vf_todo<<shift),lvol,rvol);
		} else {
			adjust<false>(done,buf,lvol,rvol);
		}

		vf_ctr -= vf_todo;

		return done;
	}
};


class OAKRA_Module_InputWAV : public OAKRA_InputModule {

	OAKRA_Format format;
	FILE *inf;
	int length; //in samples
	int pos;
	int status;

public:

	OAKRA_Module_InputWAV() {
		status = OAKRA_STATUS_STOPPED;
		inf = 0;
	}

	OAKRA_Format &queryFormat() { return format; }
	int getStatus() { return status; }
	bool setPosition(int pos) {
		fseek(inf,pos*format.size+44,SEEK_SET);
		return true;
	}
	int getLength() { return length; }

	int generate(int samples, void *buf) {
		if(status == OAKRA_STATUS_STOPPED || bPaused) return 0;
		int todo = std::min(samples,length-pos);
		fread(buf,1,todo*format.size,inf);
		pos += todo;
		if(pos == length) status = OAKRA_STATUS_STOPPED;
		return todo;
	}

	void close() {
		if(inf) fclose(inf);
		inf = 0;
		status = OAKRA_STATUS_STOPPED;
	}

	//this just opens up a wave file and skips to the pcm data
	//you had better have set the format yourself first.
	//i'll read the length for you
	void loadQuick(char *fname) {
		close();
		inf = fopen(fname,"rb");
		fseek(inf,0x28,SEEK_SET);
		fread(&length,1,4,inf);
		length /= format.size;
		pos = 0;
		status = OAKRA_STATUS_PLAYING;
		play();
	}

	//in this version, i won't even read the length for you
	void loadQuick(char *fname, int length) {
		close();
		this->length = length;
		inf = fopen(fname,"rb");
		fseek(inf,0x2C,SEEK_SET);
		pos = 0;
		status = OAKRA_STATUS_PLAYING;
		play();
	}

	//we'll be doing some trivial checks
	//and setting our format properly.
	//but we expect to get a regular PCM wave of course.
	bool load(char *fname) {
		close();
		inf = fopen(fname,"rb");
		pos = 0;
		
		//packme
		struct {
			int riff,len1,wave;
			
			int fmt,len2;
			unsigned short wFormatTag, wChannels;
			unsigned int dwSamplesPerSec, dwAvgBytesPerSec;
			unsigned short wBlockAlign, wBitsPerSample;

			int data, len3;
		} hdr;
 
		if( fread(&hdr,1,44,inf) != 44) {
				fclose(inf); inf=0;
				return false;
		}
		
		if( hdr.len1 == 0  || hdr.riff != 0x46464952 || hdr.wave != 0x45564157
			|| hdr.len2 != 16 || hdr.fmt != 0x20746D66 || hdr.wFormatTag != 1 
			|| (hdr.wChannels !=1 && hdr.wChannels !=2) 
			|| (hdr.wBitsPerSample !=8 && hdr.wBitsPerSample !=16)
			|| hdr.data != 0x61746164 ) {

			fclose(inf); inf=0;
			return false;
		}

		format.channels = hdr.wChannels;
		format.rate = hdr.dwSamplesPerSec;
		format.format = (hdr.wBitsPerSample == 8)?OAKRA_U8:OAKRA_S16;
		format.size = hdr.wBlockAlign;
		length = hdr.len3/format.size;

		status = OAKRA_STATUS_PLAYING;
		play();

		return true;

	}
};

#if defined OAKRA_USE_MP3
#include "mpegsound/mpegsound.h"

class OAKRA_Module_InputMP3 : public OAKRA_InputModule  {
	void *sisapi, *spapi, *decoder;
	int status;
	int pos;
public:
	OAKRA_Format &queryFormat();
	int getStatus();
	OAKRA_Module_InputMP3();
	~OAKRA_Module_InputMP3();
	void close();
	bool load(char *fname);
	int generate(int samples, void *buf);
};
#endif

//note that for now it is impossible to disable looping of modfiles
//thats because it is a pain in the ass to do in DUMB
//starts out PAUSED. can never rewind. if it ever stops, it can't be restarted.
class OAKRA_Module_MusicPlayer : public OAKRA_Module {
	
	OAKRA_InputModule *imodule;
	char *filename;
	
	bool isPaused; //is it paused?
	bool isLooped; //does it loop?
	int loopStart; //what sample does looping begin at?
	int loopEnd; //what sample does looping end at?
	int status,pos;

public:

	//callback which you should set up so that the MusicPlayer can ask you to construct
	//an inputmodule of the appropriate type. it will be deleted for you.
	struct {
		OAKRA_InputModule *call(int filetype) { return func(param,filetype); }
		OAKRA_InputModule *(*func)(void *param, int filetype);
		void *param;
	} getInputModuleCallback;

	OAKRA_Module_MusicPlayer() {
		filename = (char *)malloc(OAKRA_MAX_PATH+1);
		imodule = 0;
	}
	~OAKRA_Module_MusicPlayer() {
		free(filename);
		close();
	}

	OAKRA_Format &queryFormat() {
		if(!imodule) return OAKRA_FORMAT_NULL;
		else return imodule->queryFormat();
	}

	void close() { 
		if(imodule)	imodule->close();
		imodule = 0;
	}
	void stop() { status = OAKRA_STATUS_STOPPED; }
	void pause() { isPaused = true; }
	void play() { isPaused = false;	}
	int getLength() { return imodule->getLength(); }

	bool load(char *fname) { 
		return load(false,fname);
	}
	bool load(bool isLooped, char *fname) {
		int len = (int)strlen(fname);
		if(toupper(fname[len-1]) == 'V')
			imodule = getInputModuleCallback.call(OAKRA_FILETYPE_WAV);
		else if(toupper(fname[len-1]) == 'G')
			imodule = getInputModuleCallback.call(OAKRA_FILETYPE_OGG);
		else if(fname[len-1] == '3')
			imodule = getInputModuleCallback.call(OAKRA_FILETYPE_MP3);
		else imodule = getInputModuleCallback.call(OAKRA_FILETYPE_MOD);

		if(!imodule) return false;

		if(!imodule->load(fname)) return false;
		strcpy(filename,fname);

		status = OAKRA_STATUS_PLAYING;
		this->isLooped = isLooped;
		isPaused = true;
		pos = 0;
		return true;
	}

	//just generates samples from the input module
	int generate(int samples, void *buf) {
		if(status == OAKRA_STATUS_STOPPED) return 0;
		if(isPaused) {
			generateSilence(samples,buf,imodule->queryFormat());
			return samples;
		}
		int done = 0;
		for(;;) {

			//if we have looping positions, make use of that now
			if(isLooped) {
				if(pos == loopEnd) {
					//try to rewind...
					if(!imodule->setPosition(loopStart)) {
						//if we can't, reload the file and trash the beginning samples
						//(try never to have to do this?)
						//weird error; we opened it once, why not again?
						if(!imodule->load(filename)) {
							_OAKRA_error("Weird error: could not rewind music file for looping");
							return done;
						}
					
						imodule->trash(loopStart,buf,samples);
						pos = loopStart;
					}
				}
			}
			
			//if the input module stopped, and we are looping, start it over
			//i hope this isnt too slow
			if(imodule->getStatus() == OAKRA_STATUS_STOPPED) {
				if(isLooped) {
					//weird error; we opened it once, why not again?
					if(!imodule->load(filename)) {
						_OAKRA_error("Weird error: could not rewind music file for looping");
						return done;
					}
				} else {
					status = OAKRA_STATUS_STOPPED;
					break;
				}
			}

			int todo = samples-done;
			if(isLooped)
				if(loopEnd-pos < todo) todo = loopEnd-pos;
			done += imodule->generate(todo,buf);
			pos += done;
			if(done == samples) break;
		}
		
		return done;
	}
};

class OAKRA_CachedSound : public OAKRA_Module {
protected:
	void sizebuf(int size) {
		if(size>buflen)
			if(buf) buf = (char *)realloc(buf,size);
			else buf = (char *)malloc(size);
		buflen = size;
	}

public:
	char *buf;
	int buflen;
	int len,sampSize;
	int status;
	int loopStart; //start sample of the loop
	bool loop; //does this sound loop?

	OAKRA_Module_MusicPlayer *mp;

	OAKRA_CachedSound() {
		buf = 0;
		buflen = 0;
		loop = false;
		mp = new OAKRA_Module_MusicPlayer();
	}
	~OAKRA_CachedSound() {
		if(buf) free(buf);
		delete mp;
	}

	virtual bool load(char *fname) {
		
		if(!mp->load(fname))
			return false;
		mp->play();
		sampSize = mp->queryFormat().size;
		len = mp->getLength();
		//if we found out the length, allocate enough for that
		if(len != -1) {
			sizebuf(len*sampSize);
			mp->generate(len,buf);
			return true;
		} else {
			//otherwise, we might have to keep resizing
			if(buflen == 0) sizebuf(1024);
			int sofar = 0;
			len = 0;
			for(;;) {
				int chunk = buflen/sampSize;
				chunk -= sofar;
				int chunkbytes = chunk*sampSize;
				int done = mp->generate(chunk,buf + sofar*sampSize);
				sofar += chunk;
				len += done;
				if(done != chunk) break;
				sizebuf(((buflen*3)>>1)&~3);
			}
			return true;
		}
	}

	OAKRA_Format &queryFormat() { 
		//if(buf) 
		return mp->queryFormat();
		//else return OAKRA_FORMAT_NULL;
	}
};

//starts out PAUSED. can never rewind. if it ever stops, it can't be restarted.
//if you set the source sound to loop, then the player will loop until you stop it
class OAKRA_CachedSoundPlayer : public OAKRA_BasicModule {
	int status,pos;
	OAKRA_CachedSound *cache;
	bool isPaused;
	
	bool loop; //does it loop?
	int loopStart; //what sample does looping begin at?

public:	

	OAKRA_CachedSoundPlayer(OAKRA_CachedSound *cache) {
		this->cache = cache;
		isPaused = true;
		status = OAKRA_STATUS_PLAYING;
		pos = 0;
		loop = cache->loop;
		if(loop) loopStart = cache->loopStart;
		else loopStart = -1;	
	}

	void stop() { status = OAKRA_STATUS_STOPPED; pos = 0;}
	void pause() { isPaused = true; }
	void play() { isPaused = false;	}

	int generate(int samples, void *buf) {
		int done = 0;
		//if we're paused, just generate silence and bail
		if(isPaused) {
			generateSilence(samples,buf,cache->queryFormat());
			return samples;
		}
		//otherwise keep generating
		while(samples) {
			if(status == OAKRA_STATUS_STOPPED) break;
			int todo = std::min(samples,cache->len-pos);
			memcpy((char *)buf+done*cache->sampSize,cache->buf+pos*cache->sampSize,todo*cache->sampSize);
			pos += todo;
			done += todo;
			samples -= todo;
			if(pos == cache->len)
				if(loop)
					pos = loopStart;
				else
					status = OAKRA_STATUS_STOPPED;
		}
		return done;
	}
};



class OAKRA_Module_OutputSW : public OAKRA_OutputDriver {
	void *data;
	void dump(int samples, void *buf);

public:
    OAKRA_Voice *getVoice(OAKRA_Format &format);
	OAKRA_Voice *getVoice(OAKRA_Format &format, OAKRA_Module *source);
	void freeVoice(OAKRA_Voice *voice);
	void freeVoiceInternal(OAKRA_Voice *voice, bool internal);
	OAKRA_Module_OutputSW(OAKRA_OutputDriver *driver, int channels, OAKRA_Format &outputFormat);
	virtual ~OAKRA_Module_OutputSW();
	void update();
	void start();
	void lock();
	void unlock();
	virtual int generate(int samples, void *buf);
};

//#if defined(OAKRA_USE_WINMM)
//class OAKRA_Module_OutputWinMM : public OAKRA_OutputDriver {
//	void *threadData;
//	void *data;
//	void dump(int samples, void *buf);
//
//public:
//    OAKRA_Voice *getVoice(OAKRA_Format &format);
//	void freeVoice(OAKRA_Voice *voice);
//	void freeVoiceInternal(OAKRA_Voice *voice, bool internal);
//	OAKRA_Module_OutputWinMM(int channels, OAKRA_Format &format);
//	virtual ~OAKRA_Module_OutputWinMM();
//	void beginThread();
//	void endThread();
//	void update();
//	void start();
//	void lock();
//	void unlock();
//};
//#endif

#if defined(OAKRA_USE_DSOUND)
class OAKRA_Module_OutputDS : public OAKRA_OutputDriver {
	
	void *threadData;
	void *data;

public:

	OAKRA_Module_OutputDS();
	~OAKRA_Module_OutputDS();
	OAKRA_Voice *getVoice(OAKRA_Format &format);
	OAKRA_Voice *getVoice(OAKRA_Format &format, OAKRA_Module *source);
	void freeVoice(OAKRA_Voice *voice);
	void freeVoiceInternal(OAKRA_Voice *voice, bool internal);
	void start(void *hwnd);
	void update();
	void beginThread();
	void endThread();
	void lock();
	void unlock();
};
#endif
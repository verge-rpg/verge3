////"OAKRA WinMM driver only supports 16bit stereo audio";
////yeah. to be improved later
//
//#include "oakra.h"
//
//#if defined(OAKRA_USE_WINMM)
//
//#define NOMINMAX
//#include <windows.h>
//#include <mmsystem.h>
//#include <vector>
//
//#define BUFFER_COUNT (16)
//#define BUFFER_LENGTH (1000)
//
//class MMVoice : public OAKRA_Voice {
//	OAKRA_Module_OutputWinMM *driver;
//	OAKRA_Format voiceformat;
//	OAKRA_Format mixformat;
//	OAKRA_BasicModule *res;
//	bool resampleFirst;
//	OAKRA_Module_Volpanner_Base *volpanner;
//	int mixerHandle;
//
//public:
//	virtual ~MMVoice() {
//		delete res;
//		delete volpanner;
//		driver->freeVoiceInternal(this,true);
//	}
//	MMVoice(OAKRA_Module_OutputWinMM *driver, int mixerHandle, OAKRA_Format &voiceformat, OAKRA_Format &mixformat) {
//		this->driver = driver;
//		this->mixerHandle = mixerHandle;
//		this->voiceformat = voiceformat;
//		this->mixformat = mixformat;
//		//if(voiceformat.rate*voiceformat.channels < mixformat.rate*mixformat.channels) resampleFirst = false;
//		//else resampleFirst = true;
//		resampleFirst = true;
//		if(resampleFirst || voiceformat.channels == 2)
//			res = new OAKRA_Module_ResInterp_S16<2,OAKRA_MIN_SAMPLES>(voiceformat.rate,mixformat.rate);
//		else
//			res = new OAKRA_Module_ResInterp_S16<1,OAKRA_MIN_SAMPLES>(voiceformat.rate,mixformat.rate);
//		if(voiceformat.channels == 1)
//			volpanner = new OAKRA_Module_VolPanner_S16_to_2S16<1>();
//		else 
//			volpanner = new OAKRA_Module_VolPanner_S16_to_2S16<2>();
//
//		source = 0;
//		vol = volpanner->vol = 255;
//		pan = volpanner->pan = 0;
//	}
//	void setPan(int pan) { this->pan = pan; volpanner->pan = pan; }
//	void setVol(int vol) { this->vol = vol; volpanner->vol = vol; }
//	void setSource(OAKRA_Module *source) {
//		//if the samplerates match, then cut out the resampler
//		if(voiceformat.rate == mixformat.rate) {
//			volpanner->source = source;
//			this->source = volpanner;
//		} else {
//			if(resampleFirst) {
//				res->source = source;
//				volpanner->source = res;
//				this->source = volpanner;
//			} else {
//				volpanner->source = source;
//				res->source = volpanner;
//				this->source = res;
//			}
//		}
//	}
//
//	int generate(int samples, void *buf) {
//		if(!source) return 0;
//		return source->generate(samples,buf);
//	}
//};
//
//class MMData {
//public:
//    WAVEHDR *m_buffers;
//    char *m_samples;
//	HWAVEOUT handle;
//	OAKRA_Format format;
//	std::vector<MMVoice *> voices;
//	OAKRA_Module_2S16_Mixer *mixer;
//	CRITICAL_SECTION criticalSection;
//};
//
//class ThreadData {
//public:
//	ThreadData() { kill = false; dead = false; }
//	OAKRA_Module_OutputWinMM *mm;
//	bool kill, dead;
//};
//
//
//DWORD WINAPI updateproc(LPVOID lpParameter) {
//	ThreadData *data = (ThreadData *)lpParameter;
//	for(;;) {
//		if(data->kill) break;
//		data->mm->update();
//		Sleep(2);
//	}
//	data->dead = true;
//	return 0;
//}
//
//
//OAKRA_Module_OutputWinMM::OAKRA_Module_OutputWinMM(int channels, OAKRA_Format &format) {
//
//	if(format.format != OAKRA_S16) throw "OAKRA WinMM driver only supports 16bit stereo audio";
//	if(format.channels != 2) throw "OAKRA WinMM driver only supports 16bit stereo audio";
//
//	data = new MMData();
//	MMData *data = (MMData *)this->data;
//	data->format = format;
//	data->mixer = new OAKRA_Module_2S16_Mixer();
//	int shift = 0;
//	if(channels>2) shift++;
//	if(channels>8) shift++;
//	if(channels>32) shift++;
//	if(channels>128) shift++;
//	data->mixer->shift = shift;
//	data->mixer->setSourceCount(channels);
//	InitializeCriticalSection(&data->criticalSection);
//}
//
//OAKRA_Module_OutputWinMM::~OAKRA_Module_OutputWinMM() {
//	//ask the driver to shutdown, and wait for it to do so
//	((ThreadData *)threadData)->kill = true;
//	while(!((ThreadData *)threadData)->dead) Sleep(1);
//
//	//delete the mixer and voices
//	MMData *data = (MMData*)this->data;
//	delete data->mixer;
//	for(int i=0;i<(int)data->voices.size();i++)
//		delete data->voices[i];
//
//	//free buffers and other resources
//	free(data->m_samples);
//	free(data->m_buffers);
//	DeleteCriticalSection(&data->criticalSection);
//	delete data;
//}
//
//
//void OAKRA_Module_OutputWinMM::dump(int samples, void *buf) {
//	lock();
//	MMData *data = (MMData *)this->data;
//	data->mixer->generate(samples,buf);
//	unlock();
//}
//
//void OAKRA_Module_OutputWinMM::update() {
//	MMData *data = (MMData *)this->data;
//
//	// if a buffer is done playing, add it to the queue again
//	for (int i = 0; i < BUFFER_COUNT; ++i) {
//		WAVEHDR& wh = data->m_buffers[i];
//		if (wh.dwFlags & WHDR_DONE) {
//
//			// unprepare
//			MMRESULT result = waveOutUnprepareHeader(data->handle, &wh, sizeof(wh));
//			if (result != MMSYSERR_NOERROR) {
//				int xxx=9;
//			}
//
//			// fill with new samples
//			//read(BUFFER_LENGTH / 4, wh.lpData);
//			dump(BUFFER_LENGTH,wh.lpData);
//			wh.dwFlags = 0;
//
//			// prepare
//			result = waveOutPrepareHeader(data->handle, &wh, sizeof(wh));
//			if (result != MMSYSERR_NOERROR) {
//				int xxx=9;
//			}
//
//			// write
//			result = waveOutWrite(data->handle, &wh, sizeof(wh));
//			if (result != MMSYSERR_NOERROR) {
//				int xxx=9;
//
//			}
//		}
//	}
//}
//
//void OAKRA_Module_OutputWinMM::beginThread() {
//	DWORD updateThreadId;
//	threadData = new ThreadData();
//	((ThreadData *)threadData)->mm = this;
//	HANDLE updateThread = CreateThread(0,0,updateproc,threadData,0,&updateThreadId);
//	SetThreadPriority(updateThread,THREAD_PRIORITY_TIME_CRITICAL);
//}
//
//void OAKRA_Module_OutputWinMM::endThread() {
//	((ThreadData *)threadData)->kill = true;
//}
//
//OAKRA_Voice *OAKRA_Module_OutputWinMM::getVoice(OAKRA_Format &format) {
//	MMData *data = (MMData *)this->data;
//	int handle = data->mixer->allocateSource();
//	MMVoice *mmv = new MMVoice(this,handle,format,data->format);
//	data->voices.push_back(mmv);
//	data->mixer->setSource(handle,mmv);
//	return mmv;
//}
//
//void OAKRA_Module_OutputWinMM::freeVoice(OAKRA_Voice *voice) {
//	freeVoiceInternal(voice,false);
//}
//
//void OAKRA_Module_OutputWinMM::freeVoiceInternal(OAKRA_Voice *voice, bool internal) {
//}
//
//void OAKRA_Module_OutputWinMM::start() {
//
//	MMData *data = (MMData *)this->data;
//	OAKRA_Format &format = data->format;
//	data->m_buffers = malloc<WAVEHDR>(BUFFER_COUNT);
//	data->m_samples = (char *)malloc(format.size,BUFFER_COUNT*BUFFER_LENGTH);
//
//	WAVEFORMATEX wfx;
//	memset(&wfx, 0, sizeof(wfx));
//	wfx.wFormatTag      = WAVE_FORMAT_PCM;
//	wfx.nChannels       = format.channels;
//	wfx.nSamplesPerSec  = format.rate;
//	wfx.nAvgBytesPerSec = format.size * format.rate;
//	wfx.nBlockAlign     = format.size;
//	wfx.wBitsPerSample  = (format.format == OAKRA_S16)?16:8;
//	wfx.cbSize          = sizeof(WAVEFORMATEX);
//
//	MMRESULT result = waveOutOpen(&data->handle, WAVE_MAPPER, &wfx, 0, 0, 0);
//	if (result != MMSYSERR_NOERROR) {
//		throw "couldnt make winmm driver";
//	}  
//
//	// fill each buffer with samples and prepare it for output
//	for (int i = 0; i < BUFFER_COUNT; ++i) {
//		WAVEHDR& wh = data->m_buffers[i];
//		memset(&wh, 0, sizeof(wh));
//		wh.lpData         = (char*)data->m_samples + i * BUFFER_LENGTH*format.size;
//		wh.dwBufferLength = BUFFER_LENGTH*format.size;
//
//		//read(BUFFER_LENGTH / 4, wh.lpData);
//
//		MMRESULT result = waveOutPrepareHeader(data->handle, &wh, sizeof(wh));
//		wh.dwFlags |= WHDR_DONE;
//		if (result != MMSYSERR_NOERROR) {
//			throw "prepareheader failed!";
//		}
//	}
//
//}
//
//void OAKRA_Module_OutputWinMM::lock() {
//	EnterCriticalSection(  &((MMData *)this->data)->criticalSection );
//}
//
//void OAKRA_Module_OutputWinMM::unlock() {
//	LeaveCriticalSection(  &((MMData *)this->data)->criticalSection );
//}
//
//#endif
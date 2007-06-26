#include "oakra.h"

#if defined OAKRA_USE_MP3
#include "mpegsound/mpegsound.h"

class _sisapi : public Soundinputstream, public OAKRA_Module {
public:
	FILE *inf;
	_sisapi() { 
		inf = 0;
	}

	virtual int getbytedirect() {
		unsigned char b;
		if(fread(&b,1,1,inf) != 1) {
			seterrorcode(SOUND_ERROR_FILEREADFAIL);
			return -1;
		} else return b;
	}
	virtual bool _readbuffer(char *buffer,int size) {
		if(fread(buffer,1,size,inf) != size) {
			seterrorcode(SOUND_ERROR_FILEREADFAIL);
			return false;
		} else return true;
	}
	virtual bool eof() { return feof(inf)!=0; }

};

class _spapi : public Soundplayer, public OAKRA_Module {
public:
	OAKRA_Format format;
	Mpegtoraw *decoder;
	short *buf;
	int buflen, bufsamples;
	bool bNeedsType;

	_spapi() { bufsamples = 0; buflen = 0; buf = 0; bNeedsType = false; }

	virtual bool setsoundtype(int stereo,int samplesize,int speed) {
		format.format = OAKRA_S16; //??
		format.channels = (stereo?2:1);
		format.size = format.channels*2; //??
		format.rate = speed;
		bNeedsType = false;
		return true;
	}

	
	void reset() {
		bNeedsType = true;
		update();
	}

	bool update() {
		for(;;) {
			decoder->run(bNeedsType?-1:1);
			if(decoder->geterrorcode() == SOUND_ERROR_FINISH)
				return false;
			if(decoder->geterrorcode() == SOUND_ERROR_BAD)
				continue;
			if(bNeedsType) continue;
			break;
		}
		return true;
	}

	//mpegaudio never calls this more than once per decode
	//cycle, (as long as we only decode one frame at a time)
	virtual bool putblock(void *buffer,int size) {
		if(buflen == size) return true;
		buflen = size;
		bufsamples = buflen / format.size;
		return true;
	}

};

OAKRA_Format &OAKRA_Module_InputMP3::queryFormat() { return ((_spapi*)spapi)->format; }
int OAKRA_Module_InputMP3::getStatus() { return status; }

OAKRA_Module_InputMP3::OAKRA_Module_InputMP3() { 
	sisapi = new _sisapi();
	spapi = new _spapi();
	decoder = new Mpegtoraw((Soundinputstream *)sisapi, (Soundplayer *)spapi);
	status = OAKRA_STATUS_STOPPED;
	((_spapi*)spapi)->buf = (short *)malloc(((Mpegtoraw*)decoder)->getRawBufferSize()*2);
	((Mpegtoraw*)decoder)->setRawBuffer(((_spapi*)spapi)->buf);
	((_spapi*)spapi)->decoder = (Mpegtoraw *)decoder;
}
OAKRA_Module_InputMP3::~OAKRA_Module_InputMP3() {
	free(((_spapi*)spapi)->buf);
	delete decoder;
	delete sisapi;
	delete spapi;
}

void OAKRA_Module_InputMP3::close() {
	if(((_sisapi*)sisapi)->inf) fclose(((_sisapi*)sisapi)->inf);
    status = OAKRA_STATUS_STOPPED;	
}

bool OAKRA_Module_InputMP3::load(char *fname) {
	close();
	FILE *inf = fopen(fname,"rb");
	if(!inf) return false;
	((_sisapi*)sisapi)->inf = inf;
	((Mpegtoraw *)decoder)->initialize();
	((_spapi*)spapi)->reset();
	pos=0;
	if(((_spapi*)spapi)->geterrorcode() != SOUND_ERROR_OK) {
		close();
		return false;
	}
	play();
	status = OAKRA_STATUS_PLAYING;
	return true;
}

int OAKRA_Module_InputMP3::generate(int samples, void *buf) {
	if(status == OAKRA_STATUS_STOPPED || bPaused) return 0;
	int done = 0;
	short *sbuf = (short *)buf;
	while(samples) {
		int bufptr = pos * ((_spapi*)spapi)->format.channels;
		int todo = ((_spapi*)spapi)->bufsamples-pos;
		if(samples<todo) todo = samples;
		memcpy(sbuf,((_spapi*)spapi)->buf + bufptr,todo*((_spapi*)spapi)->format.size);
		pos += todo;
		done += todo;
		samples -= todo;
		sbuf += todo*((_spapi*)spapi)->format.channels;
		if(pos == ((_spapi*)spapi)->bufsamples) {
			if(!((_spapi*)spapi)->update()) {
				status = OAKRA_STATUS_STOPPED;
				return done;
			}
			pos = 0;
		}
	}
	return done;
}



#endif

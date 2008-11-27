/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.
//


#include "xerxes.h"

#include <windowsx.h>
#include <vfw.h>
#include <stdio.h>

//todo

//1. tie audio playback to play() and pause()
//  do this by killing the voice as soon as pause is hit
//  then starting the voice again on the correct sample when play() is called

//2. remove gimping when fmod is missing!

#ifdef SND_USE_FMOD
signed char F_CALLBACKAPI win_movie_fmod_streamCallback(FSOUND_STREAM *stream, void *buff, int len, int param);
#endif

class win_movie_Rec
{
public:
	PAVIFILE avi;
	PAVISTREAM stream;
	HIC hIC;
	int start;

	int scanWidth;
	BITMAPINFOHEADER bihEncoded;
	BITMAPINFOHEADER bihDecoded;

	LONG lastSample;
	LONG numSamples;
	int framerate;

	bool bDecompStarted;

	void *indata,*outdata;

	image *img;
	int imgHandle;
	int handle;

	bool bPaused;
	bool bDirty;

	int timestamp;
	int timestampBegin;

	//audio stuff
	#ifdef SND_USE_FMOD
	FSOUND_STREAM *fmod_stream;
	#endif

	bool bAudio;
	bool bPlayAudio;

	char *audioEncoded;
	int audioSampleCount;
	PAVISTREAM audioStream;
	WAVEFORMATEX wfxDecoded;
	DWORD decodeBufferSize;
	int decodeBufferSamples;
	int audioChunkSize;

	HACMSTREAM acmStream;
	ACMSTREAMHEADER audioStreamHeader;
	char *audioDecoded;

	int playBufferSize;
	int playBufferSamples;

	bool bAudioStarted;

	int audioBufferLastSample;
	int encodeCursor;
	int bAudioPaused;
	int audioDecodedCursor;
	int sampleSize;
	int lastDecodedSize;
	//----

	win_movie_Rec()
	{
		stream = 0;
		img = 0;
		lastSample = 0;
		avi = 0;
		hIC = 0;
		bDecompStarted = true;
		indata = outdata = 0;

		timestamp = 0;
		handle = 0;

		bAudio = false;
		acmStream = 0;
		audioStream = 0;
		audioDecoded = audioEncoded = 0;
		bAudioStarted = false;
		encodeCursor = 0;
		bAudioPaused = false;
		audioDecodedCursor = 0;
		lastDecodedSize = -1;

		bPaused = true;
		bDirty = true;
	}

	~win_movie_Rec()
	{
		if(bAudio)
		{
			#ifdef SND_USE_FMOD
			if(fmod_stream)
				FSOUND_Stream_Close(fmod_stream);
			#endif

			acmStreamUnprepareHeader(acmStream,&audioStreamHeader,0);
			acmStreamClose(acmStream,0);

			delete[] audioDecoded;
			delete[] audioEncoded;

			AVIStreamRelease(audioStream);
		}

		if(bDecompStarted)
			ICDecompressEnd(hIC);
		if(hIC)
			ICClose(hIC);
		if(outdata) GlobalFreePtr(outdata);
		if(indata) GlobalFreePtr(indata);
		if(stream)
			AVIStreamRelease(stream);
		if(avi)
			AVIFileRelease(avi);
		if(img)
		{
			FreeImageHandle(imgHandle);
			delete img;
		}

		if(handle != 0)
			Handle::free(HANDLE_TYPE_MOVIE,handle);
	}

	void pause()
	{
		bPaused = true;
		timestamp = timeGetTime() - timestampBegin;
	}

	void play()
	{
		timestampBegin = timeGetTime();
		bPaused = false;
	}

	void decodeFrame()
	{
		LONG a,b;
		HRESULT hr;

		hr = AVIStreamRead(stream,lastSample,1,indata,bihEncoded.biSizeImage,&a,&b);
		if(hr != 0)
			err("error in AVIStreamRead: hresult: %d",hr);
		if(b != 1)
			err("error in AVIStreamRead: did not read 1 frame, as instructed");
		hr = ICDecompress(hIC,0,&bihEncoded,indata,&bihDecoded,outdata);
		if(hr != 0)
			err("error in ICDecompress: hresult: %d",hr);

		bDirty = true;
	}

	void nextFrame()
	{
		lastSample++;
		if(lastSample >= numSamples)
			lastSample = numSamples - 1;
	}

	void setFrame(int frame)
	{
		if(frame >= numSamples)
			frame = numSamples - 1;

		lastSample = AVIStreamFindSample(stream,(LONG)frame,FIND_KEY | FIND_PREV);
		while((int)lastSample != frame)
		{
			decodeFrame();
			lastSample++;
		}
	}

	void update()
	{
		if(bPaused)
			return;

		int time = timeGetTime() - timestampBegin + timestamp;

		LONG currSample = AVIStreamTimeToSample(stream,time);

		if(currSample >= numSamples)
			currSample = numSamples-1;
		for(LONG l = lastSample+1; l<= currSample; l++)
		{
			nextFrame();
			decodeFrame();
			if(lastSample == numSamples-1)
			{
				pause();
				//err("movie finished playing");
				break;
			}
		}

		lastSample = currSample;

		//render();
	}

	void render()
	{
		if(!bDirty)
			return;

		bDirty = false;

		int w = img->width;
		int h = img->height;
		int *dest = (int *)img->data;
		int *src = (int *)outdata + (scanWidth/4) * (img->height-1);
		int destadd = img->pitch - img->width;
		int srcadd = (scanWidth/4 - img->width) - (scanWidth/4)*2;


		srcadd*=4;
		destadd*=4;
		__asm
		{
			mov esi,src;
			mov edi,dest;
			mov ebx,h;
			loopy:
			mov ecx,w;
			rep movsd;
			add esi,srcadd;
			add edi,destadd;
			dec ebx;
			jnz loopy;
		}
		return;

		//for(int y=h;y>0;y--)
		//{
		//	for(int x=w;x>0;x--)
		//		*dest++ = *src++;
		//	dest += destadd;
		//	src += srcadd;
		//}
	}

	void audioFeed(char *buf, int len)
	{
		int work = len;
		int chunk;

		if(!work)
			return;

		char *bufPtr = buf;

		for(;;)
		{
			if(lastDecodedSize != -1)
			{
				chunk = lastDecodedSize-audioDecodedCursor;
				if(chunk > work)
					chunk = work;
				memcpy(bufPtr,audioDecoded + audioDecodedCursor * sampleSize,chunk*sampleSize);
				bufPtr += chunk*sampleSize;
				work -= chunk;
				audioDecodedCursor += chunk;
			}

			if(!work)
				break;

			//if we are at the end of our audiostream, we can't read any more!
			if(encodeCursor == audioSampleCount)
			{
				//write silence
				memset(bufPtr,0,work*sampleSize);
				bAudioPaused = true;
				return;
			}

			audioReadChunk();
			acmStreamConvert(acmStream,&audioStreamHeader,0);
			lastDecodedSize = audioStreamHeader.cbDstLengthUsed / sampleSize;
			audioDecodedCursor = 0;
		}


	}


	void audioReadChunk()
	{
		LONG a,b;
		AVIStreamRead(audioStream,encodeCursor,audioChunkSize,audioEncoded,audioChunkSize,&a,&b);
		encodeCursor += b;
	}


	bool setupAudio()
	{
		int ret;


		//read audio stream info; specifically, we need the encoded chunksize
		AVISTREAMINFO audioStreamInfo;
		AVIStreamInfo(audioStream,&audioStreamInfo,sizeof(AVISTREAMINFO));
		audioChunkSize = audioStreamInfo.dwSuggestedBufferSize;
		audioSampleCount = audioStreamInfo.dwLength;

		audioEncoded = new char[audioChunkSize];

		//read the audio streamformat info
		LONG formatSize;
		AVIStreamReadFormat(audioStream,AVIStreamStart(audioStream),0,&formatSize);
		char *format = (char *)malloc(formatSize);
		AVIStreamReadFormat(audioStream,AVIStreamStart(audioStream),format,&formatSize);
		WAVEFORMATEX *wfxEncoded = (WAVEFORMATEX *)format;

		//construct a descriptor for the format we desire to get out of the decoder
		//note that we have to use the same samplerate as the encoded format indicates
		//since acm can't change the samplerate in one fell swoop
		wfxDecoded.cbSize = sizeof(WAVEFORMATEX);
		wfxDecoded.nChannels = wfxEncoded->nChannels;
		wfxDecoded.wFormatTag = WAVE_FORMAT_PCM;
		wfxDecoded.nSamplesPerSec = wfxEncoded->nSamplesPerSec;
		wfxDecoded.wBitsPerSample = 16;
		wfxDecoded.nBlockAlign = wfxDecoded.wBitsPerSample/8 * wfxEncoded->nChannels;
		wfxDecoded.nAvgBytesPerSec = wfxDecoded.nBlockAlign * wfxDecoded.nSamplesPerSec;

		sampleSize = wfxDecoded.nBlockAlign;

		//try to get a converter from the encoded data to the decoded data
		ret = acmStreamOpen(&acmStream,0,wfxEncoded,&wfxDecoded,0,0,0,0);

		//now we're done with wfxEncoded
		free(format);

		if(ret)
		{
			delete[] audioEncoded;
			AVIStreamClose(audioStream);
			return false;
		}


		//decide on a playback buffer size
		//make each buffer 1/2sec
		playBufferSamples = wfxDecoded.nSamplesPerSec / 2;
		playBufferSize = playBufferSamples * sampleSize;


		//hurry and try to create the output stream.
		//if we can't do that, then everything that follows is pointless.
		int mode = 0;
		if(wfxDecoded.wBitsPerSample == 8)
			mode |= FSOUND_8BITS;
		else if(wfxDecoded.wBitsPerSample == 16)
			mode |= FSOUND_16BITS;
		if(wfxDecoded.nChannels == 1)
			mode |= FSOUND_MONO;
		else
			mode |= FSOUND_STEREO;

		#ifdef SND_USE_FMOD
		fmod_stream = FSOUND_Stream_Create(win_movie_fmod_streamCallback,playBufferSize,mode,wfxDecoded.nSamplesPerSec,(int)this);
		if(!fmod_stream)
		{
			acmStreamClose(acmStream,0);
			delete[] audioEncoded;
			AVIStreamClose(audioStream);
			err("Error creating fmod stream for movieplayback.  Please report this case so we can improve the robustness of the movie player!");
			return false;
		}
		#endif


		//find out how large a decode buffer we need for the encode buffer chunksize
		acmStreamSize(acmStream,audioChunkSize,&decodeBufferSize,ACM_STREAMSIZEF_SOURCE);
		decodeBufferSamples = decodeBufferSize / sampleSize;


		//allocate the decode buffer
		audioDecoded = new char[decodeBufferSize];

		//prep the decode operation
		audioStreamHeader.cbStruct = sizeof(ACMSTREAMHEADER);
		audioStreamHeader.fdwStatus = 0;
		audioStreamHeader.pbSrc = (LPBYTE)audioEncoded;
		audioStreamHeader.cbSrcLength = audioChunkSize;
		audioStreamHeader.pbDst = (LPBYTE)audioDecoded;
		audioStreamHeader.cbDstLength = decodeBufferSize;
		ret = acmStreamPrepareHeader(acmStream,&audioStreamHeader,0);
		if(ret)
		{
			delete[] audioDecoded;
			acmStreamClose(acmStream,0);
			delete[] audioEncoded;
			AVIStreamClose(audioStream);
			return false;
		}

		#ifdef SND_USE_FMOD
		//finally we're ready to start the audio stream
		FSOUND_Stream_Play(FSOUND_FREE,fmod_stream);
		#endif

		return true;
	}

};

#ifdef SND_USE_FMOD
signed char F_CALLBACKAPI win_movie_fmod_streamCallback(FSOUND_STREAM *stream, void *buff, int len, int param)
{
	win_movie_Rec *m = (win_movie_Rec *)param;
	m->audioFeed((char *)buff,len / m->sampleSize);
	return 1;
}
#endif SND_USE_FMOD

void win_movie_update()
{
	for(Handle::iterator it(Handle::begin(HANDLE_TYPE_MOVIE)); it != Handle::end(HANDLE_TYPE_MOVIE); ++it)
	{
		win_movie_Rec *m = (win_movie_Rec *)*it;
		m->update();
	}
}

void win_movie_close(int handle)
{
	delete ((win_movie_Rec *)Handle::getPointer(HANDLE_TYPE_MOVIE,handle));
	Handle::free(HANDLE_TYPE_MOVIE,handle);
}

int win_movie_load(const char *fname, bool bPlayAudio)
{
	win_movie_Rec *m = new win_movie_Rec();
	m->bPlayAudio = bPlayAudio;

	HRESULT hr;

	hr = AVIFileOpen(&m->avi, fname, OF_SHARE_DENY_WRITE, 0);
	if(hr != 0)
	{
		delete m;
		return 0;
	}

	AVIFileGetStream(m->avi,&m->stream,streamtypeVIDEO,0);

	AVISTREAMINFO streamInfo;
	AVIStreamInfo(m->stream,&streamInfo,sizeof(AVISTREAMINFO));
	m->numSamples = streamInfo.dwLength;

	//save the framerate in 16.16 fixed point ticks
	m->framerate = streamInfo.dwRate * 65536 / streamInfo.dwScale;

	///todo: dwStart ??????

	LONG bihSize = sizeof(BITMAPINFOHEADER);
	AVIStreamReadFormat(m->stream,0,&m->bihEncoded,&bihSize);

	m->bihDecoded = m->bihEncoded;
	m->bihDecoded.biCompression = BI_RGB;
	m->bihDecoded.biBitCount = 32;
	m->bihDecoded.biSizeImage = 0; //mbg 12/11/05 re id#28200


	m->hIC = ICLocate(ICTYPE_VIDEO,0, &m->bihEncoded, &m->bihDecoded, ICMODE_DECOMPRESS);
	if(!m->hIC)
	{
		delete m;
		return 0;
	}

	m->scanWidth = ((m->bihDecoded.biWidth * m->bihDecoded.biBitCount + 31)&~31)/8;
	m->indata = GlobalAllocPtr(GMEM_MOVEABLE,streamInfo.dwSuggestedBufferSize);
	m->outdata = GlobalAllocPtr(GMEM_MOVEABLE,m->scanWidth * m->bihDecoded.biHeight);

	if(ICDecompressBegin(m->hIC,&m->bihEncoded,&m->bihDecoded) != ICERR_OK)
	{
		delete m;
		return 0;
	}

	//---------------------
	//audio
	//todo-------------- what about preload interval?? augh


	//get the audio stream
	if(sound)
		if(m->bPlayAudio)
		{
			hr = AVIFileGetStream(m->avi,&m->audioStream,streamtypeAUDIO,0);

			if(hr != AVIERR_NODATA)
				m->bAudio = m->setupAudio();
		}

	//--------------------------

	m->bDecompStarted = true;

	m->img = new image(m->bihDecoded.biWidth,m->bihDecoded.biHeight);
	m->imgHandle = HandleForImage(m->img);

	m->start = timeGetTime();

	int handle = Handle::alloc(HANDLE_TYPE_MOVIE,m);
	m->handle = handle;

	m->decodeFrame();

	return handle;
}

static bool win_movie_is_initialized = false;
void win_movie_init()
{
	AVIFileInit();
	win_movie_is_initialized = true;

	//the following code jars the movie system so that it wont take so long to load a movie later
	//but i do not know how necessary it really is
	//BITMAPINFOHEADER bih;
	//memset(&bih,0,sizeof(BITMAPINFOHEADER));
	//bih.biCompression = 0x30355649; //indeo
	//ICLocate (ICTYPE_VIDEO,0, &bih, 0, ICMODE_DECOMPRESS );
}

void win_movie_shutdown()
{
	if(!win_movie_is_initialized) return;
	int count = Handle::getHandleCount(HANDLE_TYPE_MOVIE);
	int *todo = new int[count];
	int i = 0, j=0;
	for(int i=0;i<count;i++) {
		if(Handle::getPointer(HANDLE_TYPE_MOVIE,i))
			todo[j++] = i;
	}

	for(int i=0;i<j;i++) {
		((win_movie_Rec *)Handle::getPointer(HANDLE_TYPE_MOVIE,todo[i]))->pause();
		win_movie_close(todo[i]);
	}

	delete[] todo;

	AVIFileExit();
}

void win_movie_play(int movie, int loop)
{
	//loop is currently ignored
	((win_movie_Rec *)Handle::getPointer(HANDLE_TYPE_MOVIE,movie))->play();
}

int win_movie_getImage(int movie)
{
	return ((win_movie_Rec *)Handle::getPointer(HANDLE_TYPE_MOVIE,movie))->imgHandle;
}

void win_movie_render(int movie)
{
	((win_movie_Rec *)Handle::getPointer(HANDLE_TYPE_MOVIE,movie))->render();
}

int win_movie_getCurrFrame(int movie)
{
	return ((win_movie_Rec *)Handle::getPointer(HANDLE_TYPE_MOVIE,movie))->lastSample;
}

int win_movie_getFramerate(int movie)
{
	return ((win_movie_Rec *)Handle::getPointer(HANDLE_TYPE_MOVIE,movie))->framerate;
}

void win_movie_nextFrame(int movie)
{
	win_movie_Rec *m = (win_movie_Rec *)Handle::getPointer(HANDLE_TYPE_MOVIE,movie);
	m->nextFrame();

	m->decodeFrame();
}

void win_movie_setFrame(int movie, int frame)
{
	win_movie_Rec *m = (win_movie_Rec *)Handle::getPointer(HANDLE_TYPE_MOVIE,movie);
	m->setFrame(frame);

	m->decodeFrame();
}

//this is the function that plays a simple fmv
bool bAbortSimpleMovie = false;
int win_movie_playSimple(const char *fname)
{
	int movie = win_movie_load(fname,true);
	if(movie == 0)
		return 0;

	win_movie_Rec *m = (win_movie_Rec *)Handle::getPointer(HANDLE_TYPE_MOVIE,movie);

	int xres = gameWindow->getXres();
	int yres = gameWindow->getYres();
	bool resetResolution = (xres != m->img->width || yres != m->img->height);
	if(resetResolution) gameWindow->setResolution(m->img->width,m->img->height);

	m->play();
	while(!m->bPaused && !bAbortSimpleMovie)
	{
		m->render();
		::Clear(::MakeColor(255,0,0),screen);
		::Blit(0,0,m->img,screen);
		ShowPage();

		UpdateControls();
		while(!m->bDirty && !m->bPaused)
			Sleep(0);
	}
	bAbortSimpleMovie = false;

	win_movie_close(movie);

	if(resetResolution) gameWindow->setResolution(xres,yres);

	return 1;
}

//mbg 12/11/05 - used to abort a call to win_movie_playSimple
void win_movie_abortSimple() {
	bAbortSimpleMovie = true;
}
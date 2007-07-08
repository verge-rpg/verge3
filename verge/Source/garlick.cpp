//see accompanying garlick.h

#define _GARLICK_C_
#include "xerxes.h"
#include "garlick.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *Garlick_std_open(const char *fname) { return fopen(fname,"rb"); }
void Garlick_std_close(void *handle) { fclose((FILE*)handle); }
size_t Garlick_std_read(void *ptr, size_t elemsize, size_t amt, void *handle) { return fread(ptr,elemsize,amt,(FILE*)handle); }
long Garlick_std_tell(void *handle) { return ftell((FILE *)handle); }
int Garlick_std_seek(void *handle, long offset, int origin) { return fseek((FILE*)handle,offset,origin); }
void *Garlick_std_malloc(int amt) { return malloc(amt); }
void Garlick_std_free(void *ptr) { return free(ptr); }
void Garlick_std_error(const char *msg) {
	printf("%s\n",msg);
	exit(0);
}

void *(*Garlick_cb_open)(const char *fname) = Garlick_std_open;
void (*Garlick_cb_close)(void *handle) = Garlick_std_close;
size_t (*Garlick_cb_read)(void *ptr, size_t elemsize, size_t amt, void *handle) = Garlick_std_read;
long (*Garlick_cb_tell)(void *handle) = Garlick_std_tell;
int (*Garlick_cb_seek)(void *handle, long offset, int origin) = Garlick_std_seek;
void *(*Garlick_cb_malloc)(int amt) = Garlick_std_malloc;
void (*Garlick_cb_free)(void *ptr) = Garlick_std_free;
void (*Garlick_cb_error)(const char *ptr) = Garlick_std_error;

//ensures that the temp buffer has the required space
void GarlickEnsureBuffer(GarlickFile *gf, int size) {
	if(gf->buflen < size) {
		if(gf->buflen != 0) Garlick_cb_free(gf->buf);
		gf->buf = (unsigned char *)Garlick_cb_malloc(gf->buflen = size);
	}
}

#ifdef GARLICK_USE_FLAC
#define FLAC__NO_DLL
#include "FLAC/all.h"

FLAC__StreamDecoderReadStatus GarlickDecodeFlac_Read(const FLAC__StreamDecoder *decoder, FLAC__byte buffer[], unsigned *bytes, void *client_data) {
	GarlickFile *gf = (GarlickFile *)client_data;
	int wants = (int)*bytes;
	int todo = gf->flac_in_counter<wants?gf->flac_in_counter:wants;
	if(todo == 0) {
		*bytes = 0;
		return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
	}
	int done = Garlick_cb_read(buffer,1,todo,gf->lib);
	*bytes = done;
	gf->flac_in_counter -= done;
	if(done != todo)
		return FLAC__STREAM_DECODER_READ_STATUS_ABORT;
	else return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
}

FLAC__StreamDecoderWriteStatus GarlickDecodeFlac_Write(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 * const buffer[], void *client_data)   {
	GarlickFile *gf = (GarlickFile *)client_data;
	int buf_counter = gf->buf_counter;
	unsigned char *buf = gf->buf;
	
	int todo = frame->header.blocksize;
	unsigned char *dest = buf+buf_counter;
	short *sdest = (short *)(buf+buf_counter);

	//oh christ so many permutations
	const int *src = buffer[0];
	if(! (gf->encode_mode & GARLICK_MODE_STEREO) ) {
		//mono
		if(gf->encode_mode == GARLICK_MODE_FLAC) { //8bit mono
			for(int i=0;i<todo;i++)
				dest[i] = src[i];
			gf->buf_counter += todo;
		} else if(gf->encode_mode == (GARLICK_MODE_FLAC | GARLICK_MODE_16BITS)) { //16bit mono
			for(int i=0;i<todo;i++)
				sdest[i] = src[i];
			gf->buf_counter += todo*2;
		} else if(gf->encode_mode == (GARLICK_MODE_FLAC | GARLICK_MODE_16BITS | GARLICK_MODE_UNSIGNED)) { //16bit mono unsigned
			for(int i=0;i<todo;i++)
				sdest[i] = src[i] + 37268;
			gf->buf_counter += todo*2;
		} else if(gf->encode_mode == (GARLICK_MODE_FLAC | GARLICK_MODE_UNSIGNED)) { //8bit mono unsigned
			unsigned char *dest = buf+buf_counter;
			for(int i=0;i<todo;i++)
				dest[i] = src[i] + 128;
			gf->buf_counter += todo;
		} else { throw "eck"; }
	} else { //stereo 
		const int *src2 = buffer[1];
		if(! (gf->encode_mode & GARLICK_MODE_STEREOPLANAR_BIT) ){
			//interleaved stereo
			if(gf->encode_mode == (GARLICK_MODE_FLAC | GARLICK_MODE_STEREO)) { //8bit interleaved stereo
				for(int i=0;i<todo;i++) {
					dest[i*2] = src[i];
					dest[i*2+1] = src2[i];
				}
				gf->buf_counter += todo * 2;
			} else if(gf->encode_mode == (GARLICK_MODE_FLAC | GARLICK_MODE_16BITS | GARLICK_MODE_STEREO)) { //16bit interleaved stereo
				for(int i=0;i<todo;i++) {
					sdest[i*2] = src[i];
					sdest[i*2+1] = src2[i];
				}
				gf->buf_counter += todo * 4;
			} else if(gf->encode_mode == (GARLICK_MODE_FLAC | GARLICK_MODE_16BITS | GARLICK_MODE_STEREO | GARLICK_MODE_UNSIGNED)) { //16bit interleaved stereo unsigned
				throw "eck";
			} else if(gf->encode_mode == (GARLICK_MODE_FLAC | GARLICK_MODE_UNSIGNED | GARLICK_MODE_STEREO)) { //8bit interleaved stereo unsigned
				for(int i=0;i<todo;i++) {
					dest[i*2] = src[i] + 128;
					dest[i*2+1] = src2[i] + 128;
				}
				gf->buf_counter += todo*2;
			} else { throw "eck"; }
		} else {
			//planar stereo
				if(gf->encode_mode == (GARLICK_MODE_FLAC | GARLICK_MODE_STEREOPLANAR)) { //8bit planar stereo
				for(int i=0;i<todo;i++)
					dest[i] = src[i];
				for(int i=0;i<todo;i++)
					dest[i+todo] = src2[i];
				gf->buf_counter += todo*2;
			} else if(gf->encode_mode == (GARLICK_MODE_FLAC | GARLICK_MODE_16BITS | GARLICK_MODE_STEREOPLANAR)) { //16bit interleaved stereo
				for(int i=0;i<todo;i++)
					sdest[i] = src[i];
				for(int i=0;i<todo;i++)
					sdest[i+todo] = src2[i];
				gf->buf_counter += todo * 4;
			} else if(gf->encode_mode == (GARLICK_MODE_FLAC | GARLICK_MODE_16BITS | GARLICK_MODE_STEREOPLANAR | GARLICK_MODE_UNSIGNED)) { //16bit interleaved stereo unsigned
				throw "eck";
			} else if(gf->encode_mode == (GARLICK_MODE_FLAC | GARLICK_MODE_UNSIGNED | GARLICK_MODE_STEREOPLANAR)) { //8bit interleaved stereo unsigned
				for(int i=0;i<todo;i++)
					dest[i] = src[i] + 128;
				for(int i=0;i<todo;i++)
					dest[i+todo] = src2[i] + 128;
				gf->buf_counter += todo*2;
			} else { throw "eck"; }
		}
	}
	
	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void GarlickDecodeFlac_Metadata(const FLAC__StreamDecoder *decoder, const FLAC__StreamMetadata *metadata, void *client_data) {
}
 
void GarickDecodeFlac_Error(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data) {
}

void GarlickDecodeFlac(GarlickFile *gf) {
	const char * zz = FLAC__VERSION_STRING;

	//prepare flac
	FLAC__StreamDecoder *decoder = FLAC__stream_decoder_new();
	FLAC__stream_decoder_init_stream(decoder, GarlickDecodeFlac_Read, 0, 0, 0, 0, GarlickDecodeFlac_Write, GarlickDecodeFlac_Metadata, GarickDecodeFlac_Error, (void*)gf);

	//read the size of the flac stream
	Garlick_cb_read(&gf->flac_in_counter,1,4,gf->lib);
	//prep the output buffer
	GarlickEnsureBuffer(gf, gf->chunkleft);
	gf->buf_counter = 0;

	//decode and finish
	FLAC__stream_decoder_process_until_end_of_stream(decoder);
	FLAC__stream_decoder_finish(decoder);
	FLAC__stream_decoder_delete(decoder);
	printf("%d - %d - %d\n",gf->encode_mode, gf->buf_counter,gf->chunkleft);
	gf->buf_counter = 0;
}
#endif
//--if flac

size_t GarlickRead(void *buf, size_t elemsize, size_t amt, GarlickFile *gf) {

	if(gf->passthrough)
		return Garlick_cb_read(buf,elemsize,amt,gf->inf);

	size_t work = amt*elemsize;
	char *cbuf = (char *)buf;

	if(gf->position == gf->length) return 0;

	while(work>0) {

		//switch modes if necessary
		if(gf->chunkleft == 0) {

			//read chunksize and mode (raw or encoded)
			Garlick_cb_read(&gf->chunkleft,1,4,gf->inf);
			if(gf->chunkleft&0x80000000) gf->mode = 1;
			else gf->mode = 0;
			gf->chunkleft &= 0x7FFFFFFF;
			
			if(gf->mode == 0) {
				//raw mode: do nothing
			} else {
				//encoded mode:
				//read library cursor and seek to it in library file
				GARLICK_LONG libcursor;
				Garlick_cb_read(&libcursor,1,8,gf->inf);
				Garlick_cb_seek(gf->lib,(long)libcursor,SEEK_SET);
				//read encoding mode
				Garlick_cb_read(&gf->encode_mode,1,1,gf->lib);
				//act on encoding mode:
				if(gf->encode_mode & GARLICK_MODE_FLAC) {
					#ifdef GARLICK_USE_FLAC
					GarlickDecodeFlac(gf);
					#else
					Garlick_cb_error("PANIC! Garlick received a flacced file, but was not compiled with flac support!");
					#endif
					gf->mode = 2;
				}
			}
		}

		int chunkleft = gf->chunkleft;
		int todo = (long)chunkleft<(long)work?(long)chunkleft:(long)work;

		//do the reading or skipping
		if(buf)
			switch(gf->mode) {
				case 0: Garlick_cb_read(cbuf,1,todo,gf->inf); break; //raw mode
				case 1: Garlick_cb_read(cbuf,1,todo,gf->lib); break; //lib mode
				case 2: memcpy(cbuf,gf->buf + gf->buf_counter,todo); gf->buf_counter += todo; break; //lib mode
			}
		else //seek
			switch(gf->mode) {
				case 0: Garlick_cb_seek(gf->inf,todo,SEEK_CUR); break; //raw mode
				case 1: Garlick_cb_seek(gf->lib,todo,SEEK_CUR); break; //lib mode
				case 2: gf->buf_counter += todo; break; //lib mode
			}

		//increment cursors
		gf->chunkleft -= todo;
		gf->position += todo;
		work -= todo;
		cbuf += todo;

		if(gf->position == gf->length) 
			return amt-(work/elemsize);
	}

	return amt;
}

//trashes bytes
void GarlickTrash(GarlickFile *gf, GARLICK_LONG amt) {
	while(amt > 0) {
		int todo = (int)(amt<(GARLICK_LONG)2147483647?amt:2147483647);
		amt -= todo;
		GarlickRead(0,1,todo,gf);
	}
}

void GarlickRewind(GarlickFile *gf) {
	Garlick_cb_seek(gf->inf,8,SEEK_SET); //bypass file length
	gf->mode = 1;
	gf->position = 0;
	gf->chunkleft = 0;
}

GARLICK_LONG GarlickTell(GarlickFile *gf) { 
	if(gf->passthrough)
		return Garlick_cb_tell(gf->inf);

	return gf->position;
}

int GarlickSeek(GarlickFile *gf, GARLICK_LONG offset, int origin) {
	if(gf->passthrough)
		return Garlick_cb_seek(gf->inf,(long)offset,origin);

	switch(origin) {
		case SEEK_SET:
			//maybe we dont need to do anything
			if(offset == gf->position) return 0;
			//maybe we can just trash ahead
			else if(offset > gf->position) GarlickTrash(gf,offset-gf->position);
			else {
				//nope, we have to rewind and then skip
				GarlickRewind(gf);
				if(offset != 0) GarlickTrash(gf,offset);
			}
			return 0;
		case SEEK_CUR:
			return GarlickSeek(gf,gf->position + offset, SEEK_SET);
		case SEEK_END:
			return GarlickSeek(gf,gf->length + offset,SEEK_SET);
		default:
			return 1; //wtf
	}
}

//opens a garlicked file
GarlickFile *GarlickInternalOpen(char *fname, char *fnamelib) {

	//try to open the garlicked file and library
	void *inf = Garlick_cb_open(fname);
	if(!inf) return 0;
	void *lib = Garlick_cb_open(fnamelib);
	if(!lib) {
		Garlick_cb_close(inf);
		return 0;
	}

	//prep and return the GarlickFile
	GarlickFile *gf = (GarlickFile *)Garlick_cb_malloc(sizeof(GarlickFile));
	gf->inf = inf;
	gf->lib = lib;
	gf->passthrough = 0;
	Garlick_cb_read(&gf->length,1,8,gf->inf);
	GarlickRewind(gf);
	gf->buflen = 0;
	return gf;
}

GarlickFile *GarlickPassthroughOpen(const char *fname) {
	void *inf = Garlick_cb_open(fname);
	if(!inf) return 0;

	GarlickFile *gf = (GarlickFile *)Garlick_cb_malloc(sizeof(GarlickFile));
	gf->inf = inf;
	gf->passthrough = 1;
	return gf;
}

//opens a GarlickFile, possibly in passthrough mode if the garlicked file cannot be found or opened
GarlickFile *GarlickOpen(const char *fname, const char *fnamelib) {
	
	//build the _ fname
	int flen = strlen(fname);
	int fnameliblen = strlen(fnamelib);
	char *gfname = (char *)Garlick_std_malloc(flen+2);
	strcpy(gfname,fname);
	gfname[flen] = '_';
	gfname[flen+1] = 0;
	//build the lib fname
	int idx = flen;
	while(fname[idx] != '/' && fname[idx] != '\\' && idx != -1) idx--;
	char *libname = (char *)Garlick_std_malloc(idx+fnameliblen+2);
	strncpy(libname,fname,idx+1);
	strcpy(libname+idx+1,fnamelib);

	//try to open as a garlicked file first.
	GarlickFile *gf = GarlickInternalOpen(gfname,libname);
	if(!gf) {
		//try opening as passthrough
		printf("garlick opening %s (passthrough)\n",fname);
		gf = GarlickPassthroughOpen(fname);
	} else {
		printf("garlick opening %s (%s)\n",gfname,libname);
	}
	free(libname);
	free(gfname);
	return gf;
}

void GarlickClose(GarlickFile *gf) {
	if(gf->passthrough) {
		Garlick_cb_close(gf->inf);
		return;
	}
	Garlick_cb_close(gf->inf);
	Garlick_cb_close(gf->lib);
	if(gf->buflen != 0) Garlick_cb_free(gf->buf);
}
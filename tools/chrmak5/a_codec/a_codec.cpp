/****************************************************************
	xerxes engine
	a_codec.cpp
	MODIFIED VERSION for chrmak5.
	DO NOT USE THIS VERSION AS A BASE FOR UPDATES
 ****************************************************************/


#include "zlib.h"
#include <stdio.h>

/***************************** code *****************************/

int cfwrite(void *ptr, int size1, int size2, FILE *out)
{
	int size = size1 * size2;
	uLong comprLen = size+(size*20/100);
	if (comprLen<200) comprLen = 200;
    unsigned char *outbuf = new unsigned char[comprLen];
	unsigned char *inbuf = (unsigned char *) ptr;

    int myErr = compress(outbuf, &comprLen, inbuf, size);
	
    if (myErr != Z_OK)
    {
        if (myErr == Z_MEM_ERROR) return 0;
        if (myErr == Z_BUF_ERROR) return 0;
        if (myErr == Z_STREAM_ERROR) return 0;
    }    
    fwrite(&size, 1, 4, out);
    fwrite(&comprLen, 1, 4, out);
    fwrite(outbuf, 1, comprLen, out);
	delete[] outbuf;
	return 1;
}

int cfread(void *ptr, int size1, int size2, FILE *f)
{
	uLong size = size1 * size2, comprLen;
	int mysize;
	fread(&mysize, 1, 4, f);
	if (mysize != size) return 0;
	fread(&comprLen, 1, 4, f);
	unsigned char *inbuf = new unsigned char[comprLen];
	unsigned char *outbuf = (unsigned char *) ptr;
	fread(inbuf, 1, comprLen, f);

	int myErr = uncompress(outbuf, &size, inbuf, comprLen);

	if (myErr != Z_OK)
    {
        if (myErr == Z_MEM_ERROR) return 0;
        if (myErr == Z_BUF_ERROR) return 0;
        if (myErr == Z_STREAM_ERROR) return 0;
    }
	delete[] inbuf;
	return 1;
}

#ifdef VFILE_H

int cvread(void *ptr, int size1, VFILE *f)
{
	uLong size = size1, comprLen;
	int mysize;
	vread(&mysize, 4, f);
	if (mysize != size) return 0;
	vread(&comprLen, 4, f);
	unsigned char *inbuf = new byte[comprLen];
	unsigned char *outbuf = (unsigned char *) ptr;
	vread(inbuf, comprLen, f);

	int myErr = uncompress(outbuf, &size, inbuf, comprLen);

	if (myErr != Z_OK)
    {
        if (myErr == Z_MEM_ERROR) return 0;
        if (myErr == Z_BUF_ERROR) return 0;
        if (myErr == Z_STREAM_ERROR) return 0;
    }
	delete[] inbuf;
	return 1;
}

#endif

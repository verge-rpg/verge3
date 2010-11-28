/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


/****************************************************************
	xerxes engine
	a_vfile.cpp
 ****************************************************************/

#include "xerxes.h"
#include <boost/shared_array.hpp>

// ***************************** Code *****************************

bool Exist(const char *fname)
{
	/*FILE *tempf;

	tempf=fopen(fname,"rb");
	if (tempf)
	{
		fclose(tempf);
		return true;
	}
	return false;*/

	// Above stuff commented out because I thought faster detection of file existance could be nice.
	// However, not every OS supports access().
	// Leaving the above code in case your OS doesn't support POSIX standards.
#ifdef __WIN32__
	// Check if file exists and has read permissions.
	return _access(fname, 4) != -1;
#else
	// Check if file exists and has read permissions.
	return access(fname, R_OK) != -1;
#endif
}

VFILE *vopen(const char *fname)
{
	VFILE *tmp;
	char rf=0;
#ifdef __APPLE__
	// swap backslashes in path for forward slashes
	// (windows -> unix/max)
	string converted = fname;
	boost::algorithm::replace_all(converted, "\\", "/");
	fname = converted.c_str();
#endif
	// All files using V* are read-only. To write a file, use regular i/o.
	// First we'll see if a real file exists, then we'll check for one in VFiles,
	// if we don't find one in VFile or it's overridable then a real file will
	// be used. That's the general logic progression.

	if (Exist(fname)) rf=1;
	if (!rf) return 0;

	tmp = new VFILE;
	tmp->fp = fopen(fname,"rb");
    tmp->cachedSize = -1;
	return tmp;
}

int _vread(void *dest, int len, VFILE *f)
{
   // This is fairly simple.. Just make sure our filepointer is at the right
   // place, then do a straight fread.

   if (!len) return 0;
   return fread(dest, 1, len, f -> fp);
}

// The following functions are all identical.
// You might ask why we didn't just use a templated
// function, then. Well, this is because there's no
// verification of types in a templated version; we
// can't make sure someone doesn't do vread(&mystruct,...),
// which won't get flipped property in a templated version.

int vread(word *dest, int len, VFILE *f)
{
	if(!len) return 0; // don't read nothing

	int num = len/sizeof(*dest);
	if(num == 0) {
		// partial read, zero everything first
		*dest = 0;
		num = 1;
	}

	// do the read
	int ret = _vread(dest, len, f);

	// flip if needed
	for(int i = 0; i < num; i++)
	{
		flip(dest+i, sizeof(*dest));
	}
	return ret;
}

int vread(quad *dest, int len, VFILE *f)
{
	if(!len) return 0; // don't read nothing

	int num = len/sizeof(*dest);
	if(num == 0) {
		// partial read, zero everything first
		*dest = 0;
		num = 1;
	}

	// do the read
	int ret = _vread(dest, len, f);

	// flip if needed
	for(int i = 0; i < num; i++)
	{
		flip(dest+i, sizeof(*dest));
	}
	return ret;
}

int vread(byte *dest, int len, VFILE *f)
{
	int ret = _vread(dest, len, f);
	return ret;
}

int vread(double *dest, int len, VFILE *f)
{
	if(!len) return 0; // don't read nothing

	int num = len/sizeof(*dest);
	if(num == 0) {
		// partial read, zero everything first
		*dest = 0;
		num = 1;
	}

	// do the read
	int ret = _vread(dest, len, f);

	// flip if needed
	for(int i = 0; i < num; i++)
	{
		flip(dest+i, sizeof(*dest));
	}
	return ret;
}

int vread(float *dest, int len, VFILE *f)
{
	if(!len) return 0; // don't read nothing

	int num = len/sizeof(*dest);
	if(num == 0) {
		// partial read, zero everything first
		*dest = 0;
		num = 1;
	}

	// do the read
	int ret = _vread(dest, len, f);

	// flip if needed
	for(int i = 0; i < num; i++)
	{
		flip(dest+i, sizeof(*dest));
	}
	return ret;
}

int vread(unsigned long *dest, int len, VFILE *f)
{
	if(!len) return 0; // don't read nothing

	int num = len/sizeof(*dest);
	if(num == 0) {
		// partial read, zero everything first
		*dest = 0;
		num = 1;
	}

	// do the read
	int ret = _vread(dest, len, f);

	// flip if needed
	for(int i = 0; i < num; i++)
	{
		flip(dest+i, sizeof(*dest));
	}
	return ret;
}

int vread(long *dest, int len, VFILE *f)
{
	if(!len) return 0; // don't read nothing

	int num = len/sizeof(*dest);
	if(num == 0) {
		// partial read, zero everything first
		*dest = 0;
		num = 1;
	}

	// do the read
	int ret = _vread(dest, len, f);

	// flip if needed
	for(int i = 0; i < num; i++)
	{
		flip(dest+i, sizeof(*dest));
	}
	return ret;
}

int vread(short *dest, int len, VFILE *f)
{
	if(!len) return 0; // don't read nothing

	int num = len/sizeof(*dest);
	if(num == 0) {
		// partial read, zero everything first
		*dest = 0;
		num = 1;
	}

	// do the read
	int ret = _vread(dest, len, f);

	// flip if needed
	for(int i = 0; i < num; i++)
	{
		flip(dest+i, sizeof(*dest));
	}
	return ret;
}

int vread(int *dest, int len, VFILE *f)
{
	if(!len) return 0; // don't read nothing

	int num = len/sizeof(*dest);
	if(num == 0) {
		// partial read, zero everything first
		*dest = 0;
		num = 1;
	}

	// do the read
	int ret = _vread(dest, len, f);

	// flip if needed
	for(int i = 0; i < num; i++)
	{
		flip(dest+i, sizeof(*dest));
	}
	return ret;
}

int vread(char *dest, int len, VFILE *f)
{
	int ret = _vread(dest, len, f);
	return ret;
}

void flip(void *d, int size)
{
#ifdef __BIG_ENDIAN__
	byte *val = (byte *)d;
	for(int i = 0; i < size/2; i++)
	{
		byte temp = val[i];
		val[i] = val[size-i-1];
		val[size-i-1] = temp;
	}
#endif
}

void vclose(VFILE *f)
{
   if(!f) return;
   fclose(f->fp);
   f->fp = 0;
   delete f;
}

int _filesize(VFILE *f)
{
   int oldpos, tmp;

   // Size of external files.
   oldpos=ftell(f -> fp);
   fseek(f -> fp, 0, 2);
   tmp=ftell(f -> fp);
   fseek(f -> fp, oldpos, 0);
   return tmp;
}

int filesize(VFILE* f)
{
    if(f->cachedSize < 0)
        f->cachedSize = _filesize(f);
    return f->cachedSize;
}

void vseek(VFILE *f, int offset, int origin)
{
   fseek(f -> fp, offset, origin);
}

void _vscanf(VFILE *f, char *format, char *dest)
{
   fscanf(f -> fp, format, dest);
}

void _vscanf(VFILE *f, char *format, int *dest)
{
   fscanf(f -> fp, format, dest);
}

char vgetc(VFILE *f)
{
   char c;

   vread(&c, 1, f);
   return c;
}

word vgetw(VFILE *f)
{
   word c;

   vread((char *) &c, 2, f);
   return c;
}

void vgets(char *str, int len, VFILE *f)
{
   str[0] = '\0'; // return '' if fgets fails for consistency
   fgets(str, len, f->fp);
}

int vtell(VFILE* f)
{
	return ftell(f->fp);
}

int veof(VFILE *f)
{
	return vtell(f) >= filesize(f);
}

boost::shared_array<byte> vreadfile(const char *fname) {
	VFILE *f = vopen(fname);
	if(!f) return boost::shared_array<byte>(0);
	int len = filesize(f);
	boost::shared_array<byte> buf(new byte[len+4]);
	*((int*)buf.get()) = len;
	vread(buf.get()+4,len,f);
	vclose(f);
	return buf;
}
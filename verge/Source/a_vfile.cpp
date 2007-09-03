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

// ***************************** Data *****************************

mountstruct pack[10];			// packfile structs
int filesmounted=0;			// Number of VPK files to check.
char headertag[]={ 'V','3','P','A','K',0 };

// ***************************** Code *****************************

bool Exist(const char *fname)
{
	FILE *tempf;

	tempf=fopen(fname,"rb");
	if (tempf)
	{
		fclose(tempf);
		return true;
	}
	return false;
}

bool VExist(char *fname)
{
	VFILE *tempf;

	tempf=vopen(fname);
	if (tempf)
	{
		vclose(tempf);
		return true;
	}
	return false;
}

void DecryptHeader()
{
	char *ptr = (char *) pack[filesmounted].files;
	while (ptr < (char *) (int) pack[filesmounted].files + (int) pack[filesmounted].numfiles * sizeof (filestruct))
	{
		*ptr = ~*ptr;
		ptr++;
	}
}

void MountVFile(char *fname)
{
	char buffer[10];
#ifdef __APPLE__
	// swap backslashes in path for forward slashes
	// (windows -> unix/max)
	char * temp = fname;
	while(*temp) {
		if(*temp == '\\')
			*temp = '/';
		temp++;
	}

#endif

	if (filesmounted == 10)
		err("Too many packfiles mounted! You know, there's no limit to the number of files you can stick in one...");

	if (!(pack[filesmounted].vhandle = fopen(fname,"rb")))
		err("*error* Unable to mount %s; file not found. \n",fname);

	// Read pack header
	memset(&buffer, 0, 10);
	fread(&buffer, 1, 6, pack[filesmounted].vhandle);
	if (strcmp(buffer,headertag))
		err("*error* %s is not a valid packfile. \n",fname);

	fread(&buffer, 1, 1, pack[filesmounted].vhandle);
	if (buffer[0]!=1)
		err("*error* %s is an incompatible packfile version. (ver reported: %d) \n",fname,buffer[0]);

	fread(&pack[filesmounted].numfiles, 1, 4, pack[filesmounted].vhandle);
	flip(&pack[filesmounted].numfiles, sizeof(pack[filesmounted].numfiles));
	memcpy(pack[filesmounted].mountname, fname, strlen(fname)+1);

	// Allocate memory for headers and read them in.

	pack[filesmounted].files = (struct filestruct *) new byte[pack[filesmounted].numfiles*sizeof (filestruct)];
	for (int i=0; i<pack[filesmounted].numfiles; i++)
	{
		fread(pack[filesmounted].files[i].fname, 1, 256, pack[filesmounted].vhandle);
#ifdef __APPLE__
		// swap backslashes in path for forward slashes
		// (windows -> unix/max)
		unsigned char * temp = pack[filesmounted].files[i].fname;
		while(temp - (unsigned char *)pack[filesmounted].files[i].fname < 256) {
			// these are inverted to match inverted data
			// in header while being read in
			if(*temp == (unsigned char)(~'\\'))
				*temp = ~'/';
			temp++;
		}
#endif
		fread(&pack[filesmounted].files[i].size, 1, 4, pack[filesmounted].vhandle);
		flip(&pack[filesmounted].files[i].size, sizeof(pack[filesmounted].files[i].size));
		fread(&pack[filesmounted].files[i].packofs, 1, 4, pack[filesmounted].vhandle);
		flip(&pack[filesmounted].files[i].packofs, sizeof(pack[filesmounted].files[i].packofs));
		fread(&pack[filesmounted].files[i].extractable, 1, 1, pack[filesmounted].vhandle);
		fread(&pack[filesmounted].files[i].override, 1, 1, pack[filesmounted].vhandle);
	}
	DecryptHeader();
	int filestartofs = ftell(pack[filesmounted].vhandle);
	for (int i=0; i<pack[filesmounted].numfiles; i++)
		pack[filesmounted].files[i].packofs += filestartofs;
	filesmounted++;
}

VFILE *vopen(const char *fname)
{
	VFILE *tmp;
	char rf=0,vf=0;
	int i, j=0;
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

	// Search the VFiles.
	for (i=filesmounted-1; i>=0; i--)
	{
		for (j=0; j<pack[i].numfiles; j++)
			if (!fncmp(fname,(char *) pack[i].files[j].fname)) { vf=1; break; }
		if (vf) break;
	}

	if (!vf && !rf) return 0;

	tmp = new VFILE;

	if (vf && rf)
	{
		if (pack[i].files[j].override) vf=0;
		else rf=0;
	}

	if (vf)
	{
		tmp -> fp=pack[i].vhandle;
		tmp -> s=1;
		tmp -> v=i;
		tmp -> i=j;
		pack[i].files[j].curofs=0;
		fseek(tmp -> fp, pack[i].files[j].packofs, 0);
		pack[i].curofs=pack[i].files[j].packofs;
		return tmp;
	}

	tmp -> fp=fopen(fname,"rb");
	tmp -> s=0; tmp -> v=0; tmp -> i=0;
	return tmp;
}

int _vread(void *dest, int len, VFILE *f)
{
   // This is fairly simple.. Just make sure our filepointer is at the right
   // place, then do a straight fread.

   if (!len) return 0;
   if (f -> s)
   {
      if (pack[f -> v].curofs != (pack[f -> v].files[f -> i].packofs + pack[f -> v].files[f -> i].curofs))
         fseek(f -> fp, pack[f -> v].files[f -> i].curofs+pack[f -> v].files[f -> i].packofs, 0);
	  if (len + pack[f->v].files[f->i].curofs > pack[f->v].files[f->i].size)
		  len = pack[f->v].files[f->i].size - pack[f->v].files[f->i].curofs;
      pack[f -> v].files[f -> i].curofs += len;
      pack[f -> v].curofs+=len;
   }
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
#ifdef __APPLE__
#ifdef __BIG_ENDIAN__
	byte *val = (byte *)d;
	for(int i = 0; i < size/2; i++)
	{
		byte temp = val[i];
		val[i] = val[size-i-1];
		val[size-i-1] = temp;
	}
#endif
#endif
}

void vclose(VFILE *f)
{
   if (!f) return;
   if (!f -> s) fclose(f -> fp);
   f -> fp=0;
   delete f;
}

int filesize(VFILE *f)
{
   int oldpos, tmp;

   // Filesize for Vfiles is real simple.
   if (f -> s) return pack[f -> v].files[f -> i].size;

   // It's a bit more complex for external files.
   oldpos=ftell(f -> fp);
   fseek(f -> fp, 0, 2);
   tmp=ftell(f -> fp);
   fseek(f -> fp, oldpos, 0);
   return tmp;
}

void vseek(VFILE *f, int offset, int origin)
{
   if (!f->s)
   {
      fseek(f -> fp, offset, origin);
      return;
   }

   switch(origin)
   {
      case 0: pack[f->v].files[f->i].curofs=offset;
              fseek(f->fp, offset+pack[f->v].files[f->i].packofs, 0);
              return;
      case 1: pack[f->v].files[f->i].curofs+=offset;
              fseek(f->fp, offset, 1);
              return;
      // we seek to -abs(offset) so that negatives and positives from EOF both make sense
      case 2: pack[f->v].files[f->i].curofs = pack[f->v].files[f->i].size-abs(offset);
              fseek(f->fp, pack[f->v].files[f->i].curofs + pack[f->v].files[f->i].packofs, 0);
              return;
   }
}

void _vscanf(VFILE *f, char *format, char *dest)
{
   fscanf(f -> fp, format, dest);
   if (f -> s)
      pack[f -> v].files[f -> i].curofs = ftell(f -> fp) - pack[f -> v].files[f -> i].packofs;
}

void _vscanf(VFILE *f, char *format, int *dest)
{
   fscanf(f -> fp, format, dest);
   if (f -> s)
      pack[f -> v].files[f -> i].curofs = ftell(f -> fp) - pack[f -> v].files[f -> i].packofs;
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
   if (f->s)
   {
      if (pack[f->v].curofs != (pack[f->v].files[f->i].packofs + pack[f->v].files[f->i].curofs))
         fseek(f->fp, pack[f->v].files[f->i].curofs+pack[f->v].files[f->i].packofs, 0);
	  int myofs = ftell(f->fp);
	  fgets(str, len, f->fp);
	  int dif = ftell(f->fp) - myofs;
      pack[f->v].files[f->i].curofs += dif;
      pack[f->v].curofs+=dif;
	  return;
   }
   fgets(str, len, f->fp);
}

int vtell(VFILE* f)
{
	if (!f->s)
		return ftell(f->fp);
	return pack[f->v].files[f->i].curofs;
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
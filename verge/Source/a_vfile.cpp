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
    tmp->cachedSize = -1;
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
   if (!f) return;
   if (!f -> s) fclose(f -> fp);
   f -> fp=0;
   delete f;
}

int _filesize(VFILE *f)
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

int filesize(VFILE* f)
{
    if(f->cachedSize < 0)
        f->cachedSize = _filesize(f);
    return f->cachedSize;
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
	  // We need to check if we exceed EOF when we do these reads.
	  // (add 1 here because fgets takes a length that requires space for the final null-character.)
	  if (len + pack[f->v].files[f->i].curofs + 1 > pack[f->v].files[f->i].size)
	  {
		  len = pack[f->v].files[f->i].size - pack[f->v].files[f->i].curofs + 1;
	  }
      if (pack[f->v].curofs != (pack[f->v].files[f->i].packofs + pack[f->v].files[f->i].curofs))
         fseek(f->fp, pack[f->v].files[f->i].curofs+pack[f->v].files[f->i].packofs, 0);
	  int myofs = ftell(f->fp);

	  char* retval = fgets(str, len, f->fp);
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

int wildcmp(const char *wild, const char *string) {
  // http://www.codeproject.com/KB/string/wildcmp.aspx
  // Written by Jack Handy - jakkhandy@hotmail.com

  const char *cp = NULL, *mp = NULL;

  while ((*string) && (*wild != '*')) {
    if ((*wild != *string) && (*wild != '?')) {
      return 0;
    }
    wild++;
    string++;
  }

  while (*string) {
    if (*wild == '*') {
      if (!*++wild) {
        return 1;
      }
      mp = wild;
      cp = string+1;
    } else if ((*wild == *string) || (*wild == '?')) {
      wild++;
      string++;
    } else {
      wild = mp;
      string = cp++;
    }
  }

  while (*wild == '*') {
    wild++;
  }
  return !*wild;
}


/*
 * Copyright (c) 1989, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Guido van Rossum.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * From FreeBSD fnmatch.c 1.11
 * $Id: fnmatch.c,v 1.3 1997/08/19 02:34:30 jdp Exp $
 *
 * Function fnmatch() as specified in POSIX 1003.2-1992, section B.6.
 * Compares a filename or pathname to a pattern.
 *
 * NOTE: Contains several alterations by Overkill to better suit Verge's needs.
 *
 */

/* We #undef these before defining them because some losing systems
   (HP-UX A.08.07 for example) define these in <unistd.h>.  */
#undef	FNM_PATHNAME
#undef	FNM_NOESCAPE
#undef	FNM_PERIOD

#define	FNM_NOMATCH	1	/* Match failed. */

#define	FNM_NOESCAPE	0x01	/* Disable backslash escaping. */
#define	FNM_PATHNAME	0x02	/* Slash must be matched by slash. */
#define	FNM_PERIOD	0x04	/* Period must be matched by period. */
#define	FNM_LEADING_DIR	0x08	/* Ignore /<tail> after Imatch. */
#define	FNM_CASEFOLD	0x10	/* Case insensitive search. */
#define FNM_PREFIX_DIRS	0x20	/* Directory prefixes of pattern match too. */

#define	EOS	'\0'

int
fnmatch(const char *pattern, const char *string, int flags)
{
	const char *stringstart;
	char c, test;

	for (stringstart = string;;)
		switch (c = *pattern++) {
		case EOS:
			if ((flags & FNM_LEADING_DIR) && (*string == '/' || *string == '\\'))
				return (0);
			return (*string == EOS ? 0 : FNM_NOMATCH);
		case '?':
			if (*string == EOS)
				return (FNM_NOMATCH);
			if ((*string == '/' || *string == '\\') && (flags & FNM_PATHNAME))
				return (FNM_NOMATCH);
			if (*string == '.' && (flags & FNM_PERIOD) &&
			    (string == stringstart ||
			    ((flags & FNM_PATHNAME) && (*(string - 1) == '/' || *(string - 1) == '\\'))))
				return (FNM_NOMATCH);
			++string;
			break;
		case '*':
			c = *pattern;
			/* Collapse multiple stars. */
			while (c == '*')
				c = *++pattern;

			if (*string == '.' && (flags & FNM_PERIOD) &&
			    (string == stringstart ||
			    ((flags & FNM_PATHNAME) && (*(string - 1) == '/' || *(string - 1) == '\\'))))
				return (FNM_NOMATCH);

			/* Optimize for pattern with * at end or before /. */
			if (c == EOS)
				if (flags & FNM_PATHNAME)
					return ((flags & FNM_LEADING_DIR) ||
					    (strchr(string, '/') == NULL && strchr(string, '\\') == NULL) ?
					    0 : FNM_NOMATCH);
				else
					return (0);
			else if ((c == '/' || c == '\\') && flags & FNM_PATHNAME) {
				if ((string = strchr(string, '/')) == NULL && (string = strchr(string, '\\')) == NULL)
					return (FNM_NOMATCH);
				break;
			}

			/* General case, use recursion. */
			while ((test = *string) != EOS) {
				if (!fnmatch(pattern, string, flags & ~FNM_PERIOD))
					return (0);
				if ((test == '/' || test == '\\') && flags & FNM_PATHNAME)
					break;
				++string;
			}
			return (FNM_NOMATCH);
		default:
			if (c == *string)
				;
			else if ((flags & FNM_CASEFOLD) &&
				 (tolower((unsigned char)c) ==
				  tolower((unsigned char)*string)))
				;
			else if ((flags & FNM_PREFIX_DIRS) && *string == EOS &&
			     (((c == '/' || c == '\\') && string != stringstart) ||
			     (string == stringstart+1 && (*stringstart == '/' || *stringstart == '\\'))))
				return (0);
			else
				return (FNM_NOMATCH);
			string++;
			break;
		}
	/* NOTREACHED */
}


// For listing files that are within a packfile that aren't also physical files.
// This is called by a listFilePattern after already figuring out the list of physical files.
void listPackFilePattern(std::vector<std::string> &res, CStringRef pattern)
{
	int i, j;
	// Search the VFiles.
	for (i=filesmounted-1; i>=0; i--)
	{
		for (j=0; j<pack[i].numfiles; j++)
		{
			// If the file matches the pattern, and it isn't also physically existant.
			if (fnmatch(pattern.c_str(), (char*) pack[i].files[j].fname, FNM_PATHNAME | FNM_NOESCAPE | FNM_CASEFOLD) != FNM_NOMATCH && !Exist((char*) pack[i].files[j].fname))
			{
				res.push_back((char*) pack[i].files[j].fname);
			}
		}
	}
}
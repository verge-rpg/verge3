/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


void cfwrite(void *inbuf, int size1, int size2, FILE *out);
void cfread(void *ptr, int size1, int size2, FILE *f);
#ifdef VFILE_H

void cvread(byte *dest, int len, VFILE *f);
void cvread(word *dest, int len, VFILE *f);
void cvread(quad *dest, int len, VFILE *f);
void cvread(unsigned long *dest, int len, VFILE *f);
void cvread(double *dest, int len, VFILE *f);
void cvread(float *dest, int len, VFILE *f);
void cvread(char *dest, int len, VFILE *f);
void cvread(short *dest, int len, VFILE *f);
void cvread(int *dest, int len, VFILE *f);
void cvread(long *dest, int len, VFILE *f);

void _cvread(void *ptr, int size, VFILE *f);
#endif


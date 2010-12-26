/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


#define VFILE_H
#define vscanf _vscanf

struct VFILE
{
  FILE *fp;                         // real file pointer.
  int cachedSize;                   // The size of the file being read. Starts as -1.
};

bool Exist(const char *fname);
VFILE *vopen(const char *fname);

void flip(void *d, int size);


template<typename T>
inline void flip(T *d) { flip(d,sizeof(T)); }

int _vread(void *dest, int len, VFILE *f);

int vread(byte *dest, int len, VFILE *f);
int vread(word *dest, int len, VFILE *f);
int vread(quad *dest, int len, VFILE *f);
int vread(unsigned long *dest, int len, VFILE *f);
int vread(double *dest, int len, VFILE *f);
int vread(float *dest, int len, VFILE *f);
int vread(char *dest, int len, VFILE *f);
int vread(short *dest, int len, VFILE *f);
int vread(int *dest, int len, VFILE *f);
int vread(long *dest, int len, VFILE *f);

void vclose(VFILE *f);
int filesize(VFILE *f);
void vseek(VFILE *f, int offset, int origin);
void vscanf(VFILE *f, char *format, char *dest);
void vscanf(VFILE *f, char *format, int *dest);
char vgetc(VFILE *f);
word vgetw(VFILE *f);
void vgets(char *str, int len, VFILE *f);
int vtell(VFILE* f);
int veof(VFILE *f);
byte* getFileContent(const char *fname);

inline void fread_le(int* dest, FILE *fp) { fread(dest,1,sizeof(int),fp); flip(dest,sizeof(int)); }
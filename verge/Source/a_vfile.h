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

#include <memory>

// ovk(2022-12-03):
// - TODO: make vopen take an open mode, and use it in place of fopen everywhere. add support for writing/append to VFILEs
// - TODO: make Emscripten build use its own VFILE struct that replaces the FILE* with a pointer to in-memory file blob. (but still keep the other stuff, so we keep packfile support)
// - TODO: glob-style directory scanning for ListFilePattern that works with Emscripten.

struct VFILE {
    FILE *fp;                         // real file pointer.
    quad i;                           // which file index in vfile is it?
    char s;                           // 0=real file 1=vfile;
    int v;                            // if vfile, which vfile index
    int cachedSize;                   // The size of the file being read. Starts as -1.
};

struct filestruct {
    unsigned char fname[256];           // pathname thingo
    int size;                           // size of the file
    int packofs;                        // where the file can be found in PACK
    int curofs;                         // current file offset.
    char extractable;                   // irrelevant to runtime, but...
    char override;                      // should we override?
};

struct mountstruct {
    char mountname[80];                 // name of VRG packfile.
    FILE *vhandle;                      // Real file-handle of packfile.
    struct filestruct *files;           // File record array.
    int numfiles;                       // number of files in pack.
    int curofs;                         // Current filepointer.
};

extern mountstruct pack[10];
extern int filesmounted;

bool Exist(const char *fname);
bool VExist(char *fname);
VFILE *vopen(const char *fname);
void MountVFile(char *fname);

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
std::unique_ptr<byte[]> vreadfile(const char *fname);

inline void fread_le(int* dest, FILE *f) {
    fread(dest, 1, sizeof(int), f);
    flip(dest,sizeof(int));
}

int wildcmp(const char *wild, const char *string); // wildcmp - Written by Jack Handy - jakkhandy@hotmail.com
void listPackFilePattern(std::vector<std::string> &res, CStringRef pattern);
/*
2005 m.gambrell pi-r-squared.com

oakra is a very simple C library for mixing sounds
with a high degree of control and low overhead

it is being designed for use on PSP, but could find 
applications elsewhere.

the intent is for you to manually assemble 
decoding/transform graphs

another separate module can take care of automatically
assembling graphs or portions thereof for cases
where some information is unavailable, i.e. loading 
unknown waveforms at runtime.

but for many cases, the user will be happy assembling
his own chain because he knows how he is setting up
his mixers and decoders.

so there is no negotiation between modules for desired
formats. you make sure the formats are compatible
or you make a negotiator that handles it for you.

basically, each module that oakra provides will
assume that its inputs and outputs are all using
the same format.

other modules that allow you to change the format
will be separate and clearly marked


*/



#define _OAKRA_CPP_
#include "oakra.h"
#include <map>

int OAKRA_Manager::mem_usage;
std::map<void *, int> OAKRA_Manager::allocs;


#if defined(OAKRA_USE_DUMB)
#include "dumb.h"
void *_OAKRA_dumb_open(const char *filename) {
	return OAKRA_Module::fopen(filename,"rb");
}
int _OAKRA_dumb_skip(void *f, long n) {
	return OAKRA_Module::fseek((OAKRA_Module::FILE *)f,n,SEEK_CUR);
}
int _OAKRA_dumb_getc(void *f) {
	unsigned char b;
	if(OAKRA_Module::fread(&b,1,1,(OAKRA_Module::FILE *)f) != 1)
		return -1;
	else return b;
}
long _OAKRA_dumb_getnc(char *ptr, long n, void *f) {
	return (long)OAKRA_Module::fread(ptr,1,(int)n,(OAKRA_Module::FILE *)f);
}
void _OAKRA_dumb_close(void *f) {
	OAKRA_Module::fclose((OAKRA_Module::FILE *)f);
}
DUMBFILE_SYSTEM _OAKRA_dumb_filesystem = {
	_OAKRA_dumb_open,_OAKRA_dumb_skip,_OAKRA_dumb_getc,
		_OAKRA_dumb_getnc,_OAKRA_dumb_close };
#endif

int _OAKRA_initialized = 0;
void OAKRA_init() {
	if(_OAKRA_initialized) return;

#	if defined(OAKRA_USE_DUMB)
	register_dumbfile_system(&_OAKRA_dumb_filesystem);
#	endif
	_OAKRA_initialized = 1;

}

void OAKRA_setFilesystem(OAKRA_FileSystem *fs) {
	_OAKRA_fs = fs;
}
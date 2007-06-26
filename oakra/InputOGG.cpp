#include "oakra.h"

#if defined(OAKRA_USE_OGG)
#include <vorbis/vorbisfile.h>

size_t _OAKRA_ogg_read_func(void *ptr, size_t size, size_t nmemb, void *datasource) {
	return (size_t)OAKRA_Module::fread(ptr,(int)size,(int)nmemb,(OAKRA_Module::FILE *)datasource);
}
int _OAKRA_ogg_seek_func(void *datasource, ogg_int64_t offset, int whence) {
	return OAKRA_Module::fseek((OAKRA_Module::FILE *)datasource,(long)offset, whence);
}
int _OAKRA_ogg_close_func(void *datasource) {
	return OAKRA_Module::fclose((OAKRA_Module::FILE *)datasource);
}
long _OAKRA_ogg_tell_func(void *datasource) {
	return OAKRA_Module::ftell((OAKRA_Module::FILE *)datasource);
}
ov_callbacks _ogg_filesystem = {
	_OAKRA_ogg_read_func, _OAKRA_ogg_seek_func,
		_OAKRA_ogg_close_func, _OAKRA_ogg_tell_func };


#if	defined OAKRA_BIG_ENDIAN
static const int ogg_endian = 1;			
#else
static const int ogg_endian = 0;
#endif

OAKRA_Module_InputOGG::OAKRA_Module_InputOGG() {
	ovf = (OggVorbis_File *)malloc<OggVorbis_File>(1);
	status = OAKRA_STATUS_STOPPED;
	bFileLoaded = false;
}
OAKRA_Module_InputOGG::~OAKRA_Module_InputOGG() {
	close();
	delete ovf;
}
void OAKRA_Module_InputOGG::close() {
	if(bFileLoaded) ov_clear((OggVorbis_File *)ovf);
	bFileLoaded = false;
	status = OAKRA_STATUS_STOPPED;
}
bool OAKRA_Module_InputOGG::load(char *fname) {
	close();
	FILE *inf = fopen(fname,"rb");
	int ret = ov_open_callbacks(inf,(OggVorbis_File *)ovf,0,0,_ogg_filesystem);
	if(ret) return false;

	vorbis_info* vi = ov_info((OggVorbis_File *)ovf, -1);
	format.channels = vi->channels;
	format.rate = vi->rate;
	format.format = OAKRA_S16;
	format.size = format.channels * 2;
	byteshift = getFormatShift(format);
	status = OAKRA_STATUS_PLAYING;
	play();
	return true;
}

int OAKRA_Module_InputOGG::generate(int samples, void *buf) {
	if(status == OAKRA_STATUS_STOPPED || bPaused) return 0;
	int bitstream;
	int done = 0;
	char *out = (char *)buf;
	while(done != samples) {
		int read = ov_read((OggVorbis_File *)ovf,out,(samples-done)<<byteshift,ogg_endian,2,1,&bitstream);
		done += read>>byteshift;
		if(read == 0) {
			close();
			return done;
		}
		out += read;			
	}
	return samples;
}

#endif


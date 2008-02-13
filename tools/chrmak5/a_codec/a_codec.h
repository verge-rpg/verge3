/****************************************************************
	xerxes engine
	a_codec.h
	MODIFIED VERSION for maped3.
	DO NOT USE THIS VERSION AS A BASE FOR UPDATES
 ****************************************************************/

int cfwrite(void *inbuf, int size1, int size2, FILE *out);
int cfread(void *ptr, int size1, int size2, FILE *f);
#ifdef VFILE_H
int cvread(void *ptr, int size, VFILE *f);
#endif


void cfwrite(void *inbuf, int size1, int size2, FILE *out);
void cfread(void *ptr, int size1, int size2, FILE *f);
#ifdef VFILE_H
void cvread(void *ptr, int size, VFILE *f);
#endif


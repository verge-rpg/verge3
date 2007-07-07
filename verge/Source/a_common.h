/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


#ifndef COMMON_H
#define COMMON_H

extern bool logconsole;

byte* ImageTo24bpp(image *img);
void log_Init(bool on);
void log(char *text, ...);
char *va(char* format, ...);
int strcasecmp(char *s1, char *s2);
int fncmp(const char *s1, const char *s2);
bool ExtensionIs(char *f, char *e);
bool isletter(char c);
void strclean(char *s);
int sgn(int a);
int rnd(int lo, int hi);
void arandseed(unsigned int seed);
char *stripext(char *s);

#endif

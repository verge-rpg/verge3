/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


#ifndef SYSFONT_H
#define SYSFONT_H

void GotoXY(int, int);
void PrintRight(int x1, int y1, char *str, image *dest);
void PrintCenter(int x1, int y1, char *str, image *dest, ...);
void print_char(char, image *dest);
void PrintString(char *, image *dest, ...);
int  pixels(const char *str, const char* end=0);
void TextColor(int);
extern int fontx, fonty;

#endif

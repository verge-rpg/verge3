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
	vid_sysfont.cpp
 ****************************************************************/

#include "xerxes.h"
#include "fontdef.h"

/***************************** data *****************************/

int fontx;
int fonty;
int text_color = 0xFFFFFFFF;

char *smal_tbl[]=
{ sBlank,
    sYow,  sQuote,    sNum,   sBuck,sPercnt, sCarot, sQuotes, slParen,
 srParen,    star,    sPtr,  sComma, sMinus,sPeriod,  sSlash,      s0,
      s1,      s2,      s3,      s4,     s5,     s6,      s7,      _s8,
      s9,  sColon,  ssemic,      ss,     ss,    sra,  sQuest,     sAT,
     sbA,     sbB,     sbC,     sbD,    sbE,    sbF,     sbG,     sbH,
     sbI,     sbJ,     sbK,     sbL,    sbM,    sbN,     sbO,     sbP,
     sbQ,     sbR,     sbS,     sbT,    sbU,    sbV,     sbW,     sbX,
     sbY,     sbZ,      ss, sbSlash,     ss, sCarot,     usc,     sch,
     ssA,     ssB,     ssC,     ssD,    ssE,    ssF,     ssG,     ssH,
     ssI,     ssJ,     ssK,     ssL,    ssM,    ssN,     ssO,     ssP,
     ssQ,     ssR,     ssS,     ssT,    ssU,    ssV,     ssW,     ssX,
     ssY,     ssZ,      ss,  target,  check,  sCopy,  sBlock,     ss};

/***************************** code *****************************/

void GotoXY(int x1, int y1)
{
	fontx = x1;
	fonty = y1;
}

void print_char(char c, image *dest)
{
	char *img, w;
	int xc, yc;

	if (c < 32) return;
	c -= 32;
	if (c<0 || c>96) c = 2;
	img = smal_tbl[(int)c];
	w = *img; img++;
	for (yc=0; yc<7; yc++)
		for (xc=0; xc<w; xc++)
		{
			if (*img) PutPixel(xc + fontx, yc + fonty, text_color, dest);
			img++;
		}
	fontx += w + 1;
	if (c == '*' - 32) fontx -= 1;
}

static void PrintLine(char *str, char *end, image *dest)
{
	for (; str < end; ++str)
	{
		print_char(*str, dest);
		// No font subsets.
		if (*str == '\f')
		{
			if (!*++str) return;
			continue;
		}
	}
}

void PrintString(const char *str, image *dest, ...)
{
	va_list argptr;
	char msg[1024];

	va_start(argptr, dest);
	vsnprintf(msg, 1024, str, argptr);
	va_end(argptr);

	int x1 = fontx;  // Remember where x where the line should start. -- Overkill 2005-12-28.
	int start = 0, end = 0;

	for (end = 0; msg[end]; end++)
	{
		if (msg[end] == '\n' || msg[end] == '\r')
		{
			fontx = x1;
			PrintLine(&msg[start], &msg[end], dest);

			// Check for \r\n so they aren't parsed as two separate line breaks.
			if (msg[end] == '\r' && msg[end+1] && msg[end+1] == '\n')
				end++;
			start = end + 1;

			fonty += 7;
		}
	}
	fontx = x1;
    PrintLine(&msg[start],&msg[end+1],dest);
}

void PrintRight(int x, int y, const char *str, image* dest, ...)
{
	va_list argptr;
	char msg[1024];

	va_start(argptr, dest);
	vsnprintf(msg, 1024, str, argptr);
	va_end(argptr);

	GotoXY(x, y);
	int start = 0, end = 0;

	for (end = 0; msg[end]; end++)
	{
		if (msg[end] == '\n' || msg[end] == '\r')
		{
			fontx = x - pixels(&msg[start],&msg[end]);
			PrintLine(&msg[start], &msg[end], dest);

			// Check for \r\n so they aren't parsed as two separate line breaks.
			if (msg[end] == '\r' && msg[end+1] && msg[end+1] == '\n')
				end++;
			start = end + 1;

			fonty += 7;
		}
	}
	fontx = x - pixels(&msg[start],&msg[end+1]);
    PrintLine(&msg[start],&msg[end+1],dest);
}

void PrintCenter(int x, int y, const char *str, image *dest, ...)
{
	va_list argptr;
	char msg[1024];

	va_start(argptr, dest);
	vsnprintf(msg, 1024, str, argptr);
	va_end(argptr);

	GotoXY(x, y);
	int start = 0, end = 0;

	for (end = 0; msg[end]; end++)
	{
		if (msg[end] == '\n' || msg[end] == '\r')
		{
			fontx = x - pixels(&msg[start],&msg[end]) / 2;
			PrintLine(&msg[start], &msg[end], dest);

			// Check for \r\n so they aren't parsed as two separate line breaks.
			if (msg[end] == '\r' && msg[end+1] && msg[end+1] == '\n')
				end++;
			start = end + 1;

			fonty += 7;
		}
	}
	fontx = x - pixels(&msg[start],&msg[end+1]) / 2;
    PrintLine(&msg[start],&msg[end+1],dest);
}

int pixels(const char *str, const char *end)
{
	int pix;

	for (pix=0; *str && (!end||str<end); ++str)
	{
		// No font subsets.
		if (*str == '\f')
		{
			if (!*++str) break;
			continue;
		}
		else
		{
			char c = *str - 32;
			if (c<0 || c>96) c = 2;
			pix += *smal_tbl[(int)c]+1;
		}
	}
	return pix;
}

void TextColor(int newc)
{
	text_color = newc;
}

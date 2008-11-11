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

void PrintRight(int x1, int y1, const char *str, image* dest)
{
	GotoXY(x1 - pixels(str), y1);
	PrintString(str,dest);
}

void PrintCenter(int x1, int y1, const char *str, image *dest, ...)
{
	va_list argptr;
	char msg[256];

	va_start(argptr,dest);
	vsprintf(msg,str,argptr);
	va_end(argptr);
	str=msg;

	GotoXY(x1 - pixels(str)/2, y1);
	PrintString(str, dest);
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

void PrintString(const char *str, image *dest, ...)
{
	va_list argptr;
	char msg[1024];

	va_start(argptr, dest);
	vsprintf(msg, str, argptr);
	va_end(argptr);
	str = msg;
	int x1 = fontx;  // Remember where x where the line should start. -- Overkill 2005-12-28.
	for (; *str; ++str)
	{
		print_char(*str, dest);
		// New lines -- Overkill 2005-12-28.
		if (*str == '\n' || *str == '\r')
		{
			if (*str == '\r')
			{
				// Checks for \r\n so they aren't parsed as two seperate line breaks.
				if (!*++str) return;
				if (*str != '\n')
				{
					--str;
				}
			}
			GotoXY(x1, fonty + 7);
		}
	}
}

int pixels(const char *str, const char *end)
{
	int pix;

	for (pix=0; *str && (!end||str!=end); ++str)
	{
		char c = *str - 32;
		if (c<0 || c>96) c = 2;
		pix += *smal_tbl[(int)c]+1;
	}
	return pix;
}

void TextColor(int newc)
{
	text_color = newc;
}

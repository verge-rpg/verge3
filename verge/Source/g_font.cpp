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
	g_font.cpp
 ****************************************************************/

#include "xerxes.h"

/***************************** code *****************************/

Font::Font(const char *fname, int xsize, int ysize)
{
	width = xsize;
	height = ysize;
	container = new image(xsize, ysize);
	container->delete_data();

	image *workingimage = xLoadImage0(fname);
	subsets = workingimage->height / ((ysize*5)+4);
	selected = 0;
	incolor = false;

	rawdata = new image(xsize, ysize*100*subsets);
	for (int yl = 0; yl<5 * subsets; yl++)
		for (int xl = 0; xl<20; xl++)
			GrabRegion(1+(xl*(xsize+1)), 1+(yl*(ysize+1)), width+1+(xl*(xsize+1)), height+1+(yl*(ysize+1)),
				0, ((yl * 20) + xl) * ysize, workingimage, rawdata);

	for (int i=0; i<100; i++)
		fwidth[i] = xsize + 1;

	delete workingimage;
}

Font::Font(const char *fname)
{
	int w, h;
	// this constructor autodetected cell dimensions, B
	image *workingimage = xLoadImage0(fname);

	// Analyze image and guess cell dimensions.
	int c = ReadPixel(0, 0, workingimage);   // This is the image bg color;
	for (w=1; w<workingimage->width; w++)
	{
		int z = ReadPixel(w, 1, workingimage);
		if (z == c)
			break;
	}
	for (h=1; h<workingimage->height; h++)
	{
		int z = ReadPixel(1, h, workingimage);
		if (z == c)
			break;
	}

	int xsize = width = w-1;
	int ysize = height = h-1;

	container = new image(xsize, ysize);
	container->delete_data();
	container->shell = true;

	subsets = workingimage->height / ((ysize*5)+4);
	selected = 0;
	incolor = false;

	rawdata = new image(xsize, ysize*100*subsets);
	for (int yl = 0; yl<5 * subsets; yl++)
		for (int xl = 0; xl<20; xl++)
			GrabRegion(1+(xl*(xsize+1)), 1+(yl*(ysize+1)), width+1+(xl*(xsize+1)), height+1+(yl*(ysize+1)),
				0, ((yl * 20) + xl) * ysize, workingimage, rawdata);

	for (int i=0; i<100; i++)
		fwidth[i] = xsize;

	delete workingimage;
}

Font::~Font()
{
	container->data = 0;
	delete rawdata;
	delete container;
}

bool Font::ColumnEmpty(int cell, int column)
{
	container->data = (quad *) ((int) rawdata->data + ((cell)*width*height*vid_bytesperpixel));
	for (int y=0; y<container->height; y++)
		if (ReadPixel(column, y, container) != transColor)
			return false;
	return true;
}

void Font::EnableVariableWidth()
{
	fwidth[0] = width * 60 / 100;
	for (int i=1; i<100; i++)
	{
		fwidth[i] = 1;
		for (int x=width-1; x>0; x--)
		{
			if (!ColumnEmpty(i, x))
			{
				fwidth[i] = x + 1;
				break;
			}
		}
		if (fwidth[i]==1)
			fwidth[i] = width * 60 / 100;
	}
}

void Font::SetCharacterWidth(int character, int width)
{
	fwidth[character] = width;
}

void Font::PrintChar(char c, int x, int y, image *dest)
{
	if (c<32) return; // don't need to check for >= 128 because that would make c negative 
	container->data = (quad *) ((int) rawdata->data + (((c-32)+(100*selected))*width*height*vid_bytesperpixel));
	TBlit(x, y, container, dest);
}

// print a chunk of a string; doesn't care about newlines
// called from PrintString, PrintCenter, and PrintRight, which DO care about newlines
void Font::PrintLine(char *s, char *end, int x, int y, image *dest)
{
	for (; s < end; s++)
	{
		if (!*s)
			return;
		if (*s == '\f')
		{
			/*
			// what's wrong with just using \f0?
			if (incolor)
			{
				selected = 0;
				incolor = false;
				continue;
			}
			*/
			if (!*++s) return;
			selected = *s - '0';
			if (selected >= subsets || selected < 0)
				selected = 0;
			else
				incolor = true;
			continue;
		}
		PrintChar(*s, x, y, dest);
		if (*s < 32) continue;
		x += fwidth[*s - 32]+1;
	}
}

void Font::PrintString(char *str, int x, int y, image *dest, ...)
{
	va_list argptr;
	char msg[1024];

	va_start(argptr, dest);
	vsnprintf(msg, 1024, str, argptr);
	va_end(argptr);

	int start = 0, end = 0;

    for (end = 0; msg[end]; end++)
	{
		if (msg[end] == '\n' || msg[end] == '\r')
		{
			PrintLine(&msg[start],&msg[end],x,y,dest);

			// Check for \r\n so they aren't parsed as two separate line breaks.
			if (msg[end] == '\r' && msg[end+1] && msg[end+1] == '\n')
				end++;
			start = end + 1;

			y += height;
		}
	}

	PrintLine(&msg[start],&msg[end+1],x,y,dest);

	selected=0;
	incolor = false;
}

void Font::PrintRight(char *str, int x, int y, image *dest, ...)
{
	va_list argptr;
	char msg[1024];
	int xsize = 0;

	va_start(argptr, dest);
	vsnprintf(msg, 1024, str, argptr);
	va_end(argptr);

	int start = 0, end = 0;

    for (end = 0; msg[end]; end++)
	{
		if (msg[end] == '\n' || msg[end] == '\r')
		{
			xsize = Pixels(&msg[start],&msg[end]);
			PrintLine(&msg[start],&msg[end],x - (xsize),y,dest);

			// Check for \r\n so they aren't parsed as two separate line breaks.
			if (msg[end] == '\r' && msg[end+1] && msg[end+1] == '\n')
				end++;
			start = end + 1;

			y += height;
		}
	}
	xsize = Pixels(&msg[start],&msg[end+1]);
	PrintLine(&msg[start],&msg[end+1],x - (xsize),y,dest);

	selected=0;
	incolor = false;
}

void Font::PrintCenter(char *str, int x, int y, image *dest, ...)
{
	va_list argptr;
	char msg[1024];
	int xsize = 0;

	va_start(argptr, dest);
	vsnprintf(msg, 1024, str, argptr);
	va_end(argptr);

	int start = 0, end = 0;

    for (end = 0; msg[end]; end++)
	{
		if (msg[end] == '\n' || msg[end] == '\r')
		{
			xsize = Pixels(&msg[start],&msg[end]);
			PrintLine(&msg[start],&msg[end],x - (xsize/2),y,dest);

			// Check for \r\n so they aren't parsed as two separate line breaks.
			if (msg[end] == '\r' && msg[end+1] && msg[end+1] == '\n')
				end++;
			start = end + 1;

			y += height;
		}
	}
	xsize = Pixels(&msg[start],&msg[end+1]);
	PrintLine(&msg[start],&msg[end+1],x - (xsize/2),y,dest);

	selected=0;
	incolor = false;
}

int Font::Pixels(const char *str, const char* end)
{
	int xsize = 0;
	bool ic = incolor;

    for (const char *s = str; *s && (!end || s < end); s++)
	{
		if (*s == '\f')
		{
			/*
			// what's wrong with just using \f0?
			if (incolor)
			{
				incolor = false;
				continue;
			}
			*/
			if (!*++s) break;
			incolor = true;
			continue;
		}
		else
		{
			if (*s < 32) continue;
			xsize += fwidth[*s - 32]+1;
		}
	}

	incolor = ic; // reset the incolor flag
	return xsize;
}
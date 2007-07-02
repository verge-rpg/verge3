/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.

/******************************************************************
 * verge3: vc_library.cpp                                         *
 * copyright (c) 2002 vecna                                       *
 ******************************************************************/

#include <math.h>
#include "xerxes.h"
#include "sincos.h"
#include "opcodes.h"

/****************************** data ******************************/

#define VCFILES				51
#define VC_READ				1
#define VC_WRITE			2
#define VC_WRITE_APPEND		3 // Overkill (2006-07-05): Append mode added.

struct FileRecord
{
	bool active;
	FILE *fptr;
	VFILE *vfptr;
	int mode;
};

FileRecord vcfiles[VCFILES];


int cf_r1, cf_g1, cf_b1;
int cf_r2, cf_g2, cf_b2;
int cf_rr, cf_gr, cf_br;

/****************************** code ******************************/

void InitVCLibrary()
{
	memset(vcfiles, 0, sizeof (vcfiles));

	// allocate one dummy handle for dicts (0, null handle)
	Handle::forceAlloc(HANDLE_TYPE_DICT,1);
}

image *ImageForHandle(int handle)
{
	if (handle == 0)
		vc->vcerr("ImageForHandle() - Null image reference, probably an uninitialized image handle");

	if (handle<0 || handle >= Handle::getHandleCount(HANDLE_TYPE_IMAGE) )
		vc->vcerr("ImageForHandle() - Image reference is bogus! (%d)", handle);

	image* ptr = (image*) Handle::getPointer(HANDLE_TYPE_IMAGE,handle);

	if (ptr == NULL)
		vc->vcerr("ImageForHandle() - Image reference is valid but no image is allocated for this handle. You may have mistakenly freed it and continued to use it.");
	return ptr;
}


void FreeImageHandle(int handle)
{
	Handle::free(HANDLE_TYPE_IMAGE,handle);
}

void SetHandleImage(int handle, image *img)
{
	Handle::setPointer(HANDLE_TYPE_IMAGE, handle, (void*)img);
}

int HandleForImage(image *img)
{
	return Handle::alloc(HANDLE_TYPE_IMAGE, img);
}

dict *DictForHandle(int handle)
{
	if (handle == 0)
		vc->vcerr("DictForHandle() - Null dict reference, probably an uninitialized dict handle");

	if (handle<0 || handle >= Handle::getHandleCount(HANDLE_TYPE_DICT) )
		vc->vcerr("DictForHandle() - Dict reference is bogus! (%d)", handle);

	dict* ptr = (dict*) Handle::getPointer(HANDLE_TYPE_DICT,handle);

	if (ptr == NULL)
		vc->vcerr("DictForHandle() - Dict reference is valid but no dict is allocated for this handle. You may have mistakenly freed it and continued to use it.");
	return ptr;
}

void FreeDictHandle(int handle)
{
	Handle::free(HANDLE_TYPE_DICT,handle);
}

void SetHandleDict(int handle, dict *d)
{
	Handle::setPointer(HANDLE_TYPE_DICT, handle, (void*)d);
}

int HandleForDict(dict *d)
{
	return Handle::alloc(HANDLE_TYPE_DICT, d);
}



void EnforceNoDirectories(string s)
{
	int	n = 0;
	if (!s.length()) return;

    if (s[0]=='/' || s[0]=='\\')
		vc->vcerr("vc does not allow accessing dir: %s", s.c_str());

    if (s[1]==':')
		vc->vcerr("vc does not allow accessing dir: %s", s.c_str());

    n=0;
    while (n<s.length()-1)
    {
		if (s[n]=='.' && s[n+1]=='.')
			vc->vcerr("vc does not allow accessing dir: %s", s.c_str());
		n++;
    }
}

void vc_Exit()
{
	string message = vc->ResolveString();
	err("%s", message.c_str());
}

void vc_Log()
{
	string message = vc->ResolveString();
	log("%s", message.c_str());
}

void vc_NewImage()
{
	int xsize = vc->ResolveOperand();
	int ysize = vc->ResolveOperand();
	image *n = new image(xsize, ysize);
	vc->vcreturn = HandleForImage(n);
}

void vc_MakeColor()
{
	int r = vc->ResolveOperand();
	int g = vc->ResolveOperand();
	int b = vc->ResolveOperand();
	vc->vcreturn = MakeColor(r, g, b);
}

void vc_SetLucent()
{
	int p = vc->ResolveOperand();
	SetLucent(p);
}

void vc_SetClip()
{
	int x1 = vc->ResolveOperand();
	int y1 = vc->ResolveOperand();
	int x2 = vc->ResolveOperand();
	int y2 = vc->ResolveOperand();
	image *i = ImageForHandle(vc->ResolveOperand());
	i->SetClip(x1, y1, x2, y2);
}

void vc_LoadImage()
{
	string fn = vc->ResolveString();
	vc->vcreturn = HandleForImage(xLoadImage(fn.c_str()));
}

void vc_LoadImage0()
{
	string fn = vc->ResolveString();
	vc->vcreturn = HandleForImage(xLoadImage0(fn.c_str()));
}

void vc_LoadImage8()
{
	string fn = vc->ResolveString();
	vc->vcreturn = HandleForImage(xLoadImage8(fn.c_str()));
}

void vc_Blit()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int xxx = vc->ResolveOperand();
	int yyy = vc->ResolveOperand();
	image *s = ImageForHandle(xxx);
	image *d = ImageForHandle(yyy);
	Blit(x, y, s, d);
}

void vc_TBlit()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	image *s = ImageForHandle(vc->ResolveOperand());
	image *d = ImageForHandle(vc->ResolveOperand());
	TBlit(x, y, s, d);
}

void vc_AdditiveBlit()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	image *s = ImageForHandle(vc->ResolveOperand());
	image *d = ImageForHandle(vc->ResolveOperand());
	AdditiveBlit(x, y, s, d);
}

void vc_TAdditiveBlit()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	image *s = ImageForHandle(vc->ResolveOperand());
	image *d = ImageForHandle(vc->ResolveOperand());
	TAdditiveBlit(x, y, s, d);
}

void vc_SubtractiveBlit()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	image *s = ImageForHandle(vc->ResolveOperand());
	image *d = ImageForHandle(vc->ResolveOperand());
	SubtractiveBlit(x, y, s, d);
}

void vc_TSubtractiveBlit()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	image *s = ImageForHandle(vc->ResolveOperand());
	image *d = ImageForHandle(vc->ResolveOperand());
	TSubtractiveBlit(x, y, s, d);
}

void vc_WrapBlit()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	image *s = ImageForHandle(vc->ResolveOperand());
	image *d = ImageForHandle(vc->ResolveOperand());
	WrapBlit(x, y, s, d);
}

void vc_TWrapBlit()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	image *s = ImageForHandle(vc->ResolveOperand());
	image *d = ImageForHandle(vc->ResolveOperand());
	TWrapBlit(x, y, s, d);
}

void vc_ScaleBlit()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int dw = vc->ResolveOperand();
	int dh = vc->ResolveOperand();
	image *s = ImageForHandle(vc->ResolveOperand());
	image *d = ImageForHandle(vc->ResolveOperand());
	ScaleBlit(x, y, dw, dh, s, d);
}

void vc_TScaleBlit()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int dw = vc->ResolveOperand();
	int dh = vc->ResolveOperand();
	image *s = ImageForHandle(vc->ResolveOperand());
	image *d = ImageForHandle(vc->ResolveOperand());
	TScaleBlit(x, y, dw, dh, s, d);
}

void vc_RGB()
{
	int r = vc->ResolveOperand();
	int g = vc->ResolveOperand();
	int b = vc->ResolveOperand();
	vc->vcreturn = MakeColor(r, g, b);
}

void vc_SetPixel()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int c = vc->ResolveOperand();
	image *d = ImageForHandle(vc->ResolveOperand());
	PutPixel(x, y, c, d);
}

void vc_GetPixel()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	image *s = ImageForHandle(vc->ResolveOperand());
	vc->vcreturn = ReadPixel(x, y, s);
}

void vc_Line()
{
	int x1 = vc->ResolveOperand();
	int y1 = vc->ResolveOperand();
	int x2 = vc->ResolveOperand();
	int y2 = vc->ResolveOperand();
	int c = vc->ResolveOperand();
	image *d = ImageForHandle(vc->ResolveOperand());
	Line(x1, y1, x2, y2, c, d);
}

void vc_Rect()
{
	int x1 = vc->ResolveOperand();
	int y1 = vc->ResolveOperand();
	int x2 = vc->ResolveOperand();
	int y2 = vc->ResolveOperand();
	int c = vc->ResolveOperand();
	image *d = ImageForHandle(vc->ResolveOperand());
	Box(x1, y1, x2, y2, c, d);
}

void vc_RectFill()
{
	int x1 = vc->ResolveOperand();
	int y1 = vc->ResolveOperand();
	int x2 = vc->ResolveOperand();
	int y2 = vc->ResolveOperand();
	int c = vc->ResolveOperand();
	image *d = ImageForHandle(vc->ResolveOperand());
	Rect(x1, y1, x2, y2, c, d);
}

void vc_Circle()
{
	int x1 = vc->ResolveOperand();
	int y1 = vc->ResolveOperand();
	int xr = vc->ResolveOperand();
	int yr = vc->ResolveOperand();
	int c = vc->ResolveOperand();
	image *d = ImageForHandle(vc->ResolveOperand());
	Circle(x1, y1, xr, yr, c, d);
}

void vc_CircleFill()
{
	int x1 = vc->ResolveOperand();
	int y1 = vc->ResolveOperand();
	int xr = vc->ResolveOperand();
	int yr = vc->ResolveOperand();
	int c = vc->ResolveOperand();
	image *d = ImageForHandle(vc->ResolveOperand());
	Sphere(x1, y1, xr, yr, c, d);
}

void vc_GetR()
{
	int c = vc->ResolveOperand();
	int g, b;
	GetColor(c, vc->vcreturn, g, b);
}

void vc_GetG()
{
	int c = vc->ResolveOperand();
	int r, b;
	GetColor(c, r, vc->vcreturn, b);
}

void vc_GetB()
{
	int c = vc->ResolveOperand();
	int r, g;
	GetColor(c, r, g, vc->vcreturn);
}

void vc_RotScale()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int angle = vc->ResolveOperand();
	int scale = vc->ResolveOperand();
	image *s = ImageForHandle(vc->ResolveOperand());
	image *d = ImageForHandle(vc->ResolveOperand());
	RotScale(x, y,  angle*(float)3.14159/(float)180.0, scale/(float)1000.0, s, d);
}

// Overkill (2006-07-28):
// Fixed the bug where FreeImage() didn't do what its name implied.
void vc_FreeImage()
{
	int handle = vc->ResolveOperand();
	if (handle == 0)
	{
 		vc->vcerr("vc_FreeImage() - cannot free a null image reference!");
	}
	if (handle == 1)
	{
 		vc->vcerr("vc_FreeImage() - cannot free the screen reference");
	}
	delete ImageForHandle(handle);
	FreeImageHandle(handle);
}

void vc_Silhouette()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int c = vc->ResolveOperand();
	image *s = ImageForHandle(vc->ResolveOperand());
	image *d = ImageForHandle(vc->ResolveOperand());
	Silhouette(x, y, c, s, d);
}

void vc_GrabRegion()
{
	int sx1 = vc->ResolveOperand();
	int sy1 = vc->ResolveOperand();
	int sx2 = vc->ResolveOperand();
	int sy2 = vc->ResolveOperand();
	int dx = vc->ResolveOperand();
	int dy = vc->ResolveOperand();
	image *s = ImageForHandle(vc->ResolveOperand());
	image *d = ImageForHandle(vc->ResolveOperand());

	int dcx1, dcy1, dcx2, dcy2;
	d->GetClip(dcx1, dcy1, dcx2, dcy2);

	if (sx1>sx2) SWAP(sx1, sx2);
	if (sy1>sy2) SWAP(sy1, sy2);
	int grabwidth = sx2 - sx1;
	int grabheight = sy2 - sy1;
	if (dx+grabwidth<0 || dy+grabheight<0) return;
	d->SetClip(dx, dy, dx+grabwidth, dy+grabheight);
	Blit(dx-sx1, dy-sy1, s, d);

	d->SetClip(dcx1, dcy1, dcx2, dcy2);
}

void vc_TGrabRegion()
{
	int sx1 = vc->ResolveOperand();
	int sy1 = vc->ResolveOperand();
	int sx2 = vc->ResolveOperand();
	int sy2 = vc->ResolveOperand();
	int dx = vc->ResolveOperand();
	int dy = vc->ResolveOperand();
	image *s = ImageForHandle(vc->ResolveOperand());
	image *d = ImageForHandle(vc->ResolveOperand());

	int dcx1, dcy1, dcx2, dcy2;
	d->GetClip(dcx1, dcy1, dcx2, dcy2);

	if (sx1>sx2) SWAP(sx1, sx2);
	if (sy1>sy2) SWAP(sy1, sy2);
	int grabwidth = sx2 - sx1;
	int grabheight = sy2 - sy1;
	if (dx+grabwidth<0 || dy+grabheight<0) return;
	d->SetClip(dx, dy, dx+grabwidth, dy+grabheight);
	TBlit(dx-sx1, dy-sy1, s, d);

	d->SetClip(dcx1, dcy1, dcx2, dcy2);
}

void vc_Mosaic()
{
	int xgran = vc->ResolveOperand();
	int ygran = vc->ResolveOperand();
	image *dest = ImageForHandle(vc->ResolveOperand());
	Mosaic(xgran, ygran, dest);
}

void vc_DuplicateImage()
{
	image *src = ImageForHandle(vc->ResolveOperand());
	image *img = new image(src->width, src->height);
	Blit(0, 0, src, img);
	vc->vcreturn = HandleForImage(img);
}

void vc_Triangle()
{
	int x1 = vc->ResolveOperand();
	int y1 = vc->ResolveOperand();
	int x2 = vc->ResolveOperand();
	int y2 = vc->ResolveOperand();
	int x3 = vc->ResolveOperand();
	int y3 = vc->ResolveOperand();
	int c = vc->ResolveOperand();
	image *dest = ImageForHandle(vc->ResolveOperand());
	Triangle(x1, y1, x2, y2, x3, y3, c, dest);
}

void vc_LoadFont()
{
	string filename = vc->ResolveString();
	int width = vc->ResolveOperand();
	int height = vc->ResolveOperand();
	vc->vcreturn = (int) new Font(filename.c_str(), width, height);
}

void vc_SetCharacterWidth()
{
	Font *font = (Font*)vc->ResolveOperand();
	int character = vc->ResolveOperand();
	int width = vc->ResolveOperand();
	font->SetCharacterWidth(character,width);
}

void vc_LoadFont2()
{
	string filename = vc->ResolveString();
	vc->vcreturn = (int) new Font(filename.c_str());
}

void vc_EnableVariableWidth()
{
	Font *font = (Font*) vc->ResolveOperand();
	font->EnableVariableWidth();
}

void vc_PrintString()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	image *dest = ImageForHandle(vc->ResolveOperand());
	Font *font = (Font*) vc->ResolveOperand();
	string text = vc->ResolveString();
	if (font == 0)
	{
		GotoXY(x, y);
		PrintString(va("%s",text.c_str()), dest);
	}
	else
		font->PrintString("%s", x, y, dest, text.c_str());
}

void vc_PrintRight()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	image *dest = ImageForHandle(vc->ResolveOperand());
	Font *font = (Font*) vc->ResolveOperand();
	string text = vc->ResolveString();
	if (font == 0)
		PrintRight(x, y, va("%s",text.c_str()), dest);
	else
		font->PrintRight("%s", x, y, dest, text.c_str());
}

void vc_PrintCenter()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	image *dest = ImageForHandle(vc->ResolveOperand());
	Font *font = (Font*) vc->ResolveOperand();
	string text = vc->ResolveString();
	if (font == 0)
		PrintCenter(x, y, va("%s",text.c_str()), dest);
	else
		font->PrintCenter("%s", x, y, dest, text.c_str());
}

void vc_TextWidth()
{
	Font *font = (Font*) vc->ResolveOperand();
	string text = vc->ResolveString();
	if (font == 0)
		vc->vcreturn = pixels(text.c_str());
	else
		vc->vcreturn = font->Pixels(text.c_str());
}

void vc_FreeFont()
{
	Font *font = (Font*) vc->ResolveOperand();
	if (font)
		delete font;
}

void vc_Random()
{
	int min = vc->ResolveOperand();
	int max = vc->ResolveOperand();
	vc->vcreturn = rnd(min, max);
}

void vc_len()
{
	string s = vc->ResolveString();
	vc->vcreturn = s.length();
}

void vc_val()
{
	string s = vc->ResolveString();
	vc->vcreturn = atoi(s.c_str());
}

void vc_Unpress()
{
	int n = vc->ResolveOperand();
	switch (n)
	{
		case 0: if (b1) UnB1(); if (b2) UnB2(); if (b3) UnB3(); if (b4) UnB4();	break;
		case 1: if (b1) UnB1(); break;
		case 2: if (b2) UnB2(); break;
		case 3: if (b3) UnB3(); break;
		case 4: if (b4) UnB4(); break;
		case 5: if (up) UnUp(); break;
		case 6: if (down) UnDown(); break;
		case 7: if (left) UnLeft(); break;
		case 8: if (right) UnRight(); break;
	}
}

int vc_GetYear()
{
	return getYear();
}

int vc_GetMonth()
{
	return getMonth();
}

int vc_GetDay()
{
	return getDay();
}

int vc_GetDayOfWeek()
{
	return getDayOfWeek();
}

int vc_GetHour()
{
	return getHour();
}

int vc_GetMinute()
{
	return getMinute();
}

int vc_GetSecond()
{
	return getSecond();
}

void vc_FileOpen()
{
	int index;

	#ifdef __APPLE__
	char *temp;
	#endif
	
	string fname = vc->ResolveString();
	int filemode = vc->ResolveOperand();

	for (index=1; index<VCFILES; index++)
		if (!vcfiles[index].active)
			break;
	if (index == VCFILES)
		vc->vcerr("vc_FileOpen() - Out of file handles! \nTry closing files you're done with, or if you really need more, \nemail vecna@verge-rpg.com and pester me!");

	switch (filemode)
	{
		case VC_READ:
			vcfiles[index].vfptr = vopen(fname.c_str());
			if (!vcfiles[index].vfptr)
			{
				log("opening of %s for reading failed.", fname.c_str());
				vc->vcreturn = 0;
				return;
			}
			vcfiles[index].active = true;
			vcfiles[index].mode = VC_READ;
			break;
		case VC_WRITE:
#ifdef __APPLE__
			// swap backslashes in path for forward slashes
			// (windows -> unix/max) -- JR Dec 11/05
			temp = fname.c_str();
			while(*temp) {
				if(*temp == '\\')
					*temp = '/';
				temp++;
			}	
#endif
			vcfiles[index].fptr = fopen(fname.c_str(), "wb");
			if (!vcfiles[index].fptr)
			{
				log("opening of %s for writing failed.", fname.c_str());
				vc->vcreturn = 0;
				return;
			}
			vcfiles[index].active = true;
			vcfiles[index].mode = VC_WRITE;
			break;
		case VC_WRITE_APPEND: // Overkill (2006-07-05): Append mode added.
#ifdef __APPLE__
			// swap backslashes in path for forward slashes
			// (windows -> unix/max) -- JR Dec 11/05
			temp = fname.c_str();
			while(*temp) {
				if(*temp == '\\')
					*temp = '/';
				temp++;
			}	
#endif
			vcfiles[index].fptr = fopen(fname.c_str(), "ab");
			if (!vcfiles[index].fptr)
			{
				//log("opening of %s for writing failed.", fname.c_str());
				vc->vcreturn = 0;
				return;
			}
			vcfiles[index].active = true;
			vcfiles[index].mode = VC_WRITE;
			break;
		default:
			vc->vcerr("vc_FileOpen() - not a valid file mode!");
	}
	vc->vcreturn = index;
}

void vc_FileClose()
{
	int handle = vc->ResolveOperand();
	if (!handle) return;
	if (handle > VCFILES)
		vc->vcerr("vc_FileClose() - uhh, given file handle is not a valid file handle.");
	if (!vcfiles[handle].active)
		vc->vcerr("vc_FileClose() - given file handle is not open.");

	switch (vcfiles[handle].mode)
	{
		case VC_READ:
			vclose(vcfiles[handle].vfptr);
			vcfiles[handle].vfptr = 0;
			vcfiles[handle].mode = 0;
			vcfiles[handle].active = false;
			break;
		case VC_WRITE:
			fclose(vcfiles[handle].fptr);
			vcfiles[handle].fptr = 0;
			vcfiles[handle].mode = 0;
			vcfiles[handle].active = false;
			break;
		default:
			vc->vcerr("vc_FileClose() - uhhh. file mode is not valid?? you did something very bad!");
	}
}

void vc_FileWrite()
{
	int handle = vc->ResolveOperand();
	string s = vc->ResolveString();

	if (!handle) vc->vcerr("vc_FileWrite() - Yo, you be writin' to a file that aint open, foo.");
	if (handle > VCFILES) vc->vcerr("vc_FileWrite() - given file handle is not a valid file handle.");
	if (!vcfiles[handle].active) vc->vcerr("vc_FileWrite() - given file handle is not open.");
	if (vcfiles[handle].mode != VC_WRITE) vc->vcerr("vc_FileWrite() - given file handle is a read-mode file.");

	fwrite(s.c_str(), 1, s.length(), vcfiles[handle].fptr);
}

void vc_FileWriteln()
{
	int handle = vc->ResolveOperand();
	string s = vc->ResolveString() + "\r\n";

	if (!handle) vc->vcerr("vc_FileWriteln() - Yo, you be writin' to a file that aint open, foo.");
	if (handle > VCFILES) vc->vcerr("vc_FileWriteln() - given file handle is not a valid file handle.");
	if (!vcfiles[handle].active) vc->vcerr("vc_FileWriteln() - given file handle is not open.");
	if (vcfiles[handle].mode != VC_WRITE) vc->vcerr("vc_FileWriteln() - given file handle is a read-mode file.");

	fwrite(s.c_str(), 1, s.length(), vcfiles[handle].fptr);
}

void vc_FileReadln()
{
	int handle = vc->ResolveOperand();

	if (!handle) vc->vcerr("vc_FileReadln() - File is not open.");
	if (handle > VCFILES) vc->vcerr("vc_FileReadln() - given file handle is not a valid file handle.");
	if (!vcfiles[handle].active) vc->vcerr("vc_FileReadln() - given file handle is not open.");
	if (vcfiles[handle].mode != VC_READ) vc->vcerr("vc_FileReadln() - given file handle is a write-mode file.");


	char buffer[255]; 	// buffer for each read
	string result = ""; // all the text so far
	int eol = 0;        // flag for when we've hit the end of a line
	do {
		vgets(buffer, 255, vcfiles[handle].vfptr); // read it

		if(buffer[0] == '\0')  {
			eol = 1; // we didn't read anything, this is eof
		} else if(buffer[strlen(buffer)-1] == 10 || buffer[strlen(buffer)-1] == 13) {
			// last character is a EOL character, so it's the end of a line
			eol = 1;
		}

		strclean(buffer);
		result += buffer;
	} while(!eol);

	vc->vcretstr = result;
}

void vc_FileReadToken()
{
	int handle = vc->ResolveOperand();

	if (!handle) vc->vcerr("vc_FileReadToken() - File is not open.");
	if (handle > VCFILES) vc->vcerr("vc_FileReadToken() - given file handle is not a valid file handle.");
	if (!vcfiles[handle].active) vc->vcerr("vc_FileReadToken() - given file handle is not open.");
	if (vcfiles[handle].mode != VC_READ) vc->vcerr("vc_FileReadToken() - given file handle is a write-mode file.");

	char buffer[255];
	buffer[0] = '\0'; // ensure sending back "" on error
	vscanf(vcfiles[handle].vfptr, "%s", buffer);
	strclean(buffer);
	vc->vcretstr.assign(buffer);
}

void vc_FileSeekLine()
{
	int handle = vc->ResolveOperand();
	int line = vc->ResolveOperand();

	if (!handle) vc->vcerr("vc_FileSeekLine() - File is not open.");
	if (handle > VCFILES) vc->vcerr("vc_FileSeekLine() - given file handle is not a valid file handle.");
	if (!vcfiles[handle].active) vc->vcerr("vc_FileSeekLine() - given file handle is not open.");
	if (vcfiles[handle].mode != VC_READ) vc->vcerr("vc_FileSeekLine() - given file handle is a write-mode file.");

	vseek(vcfiles[handle].vfptr, 0, SEEK_SET);
	char temp[256+1];
	while (line-->0)
        vgets(temp, 256, vcfiles[handle].vfptr);
}


void vc_FileEOF()
{
	int handle = vc->ResolveOperand();
	if (!handle) vc->vcerr("vc_FileEOF() - File is not open.");
	if (handle > VCFILES) vc->vcerr("vc_FileEOF() - given file handle is not a valid file handle.");
	if (!vcfiles[handle].active) vc->vcerr("vc_FileEOF() - given file handle is not open.");
	if (vcfiles[handle].mode != VC_READ) vc->vcerr("vc_FileEOF() - given file handle is a write-mode file.");

	vc->vcreturn = veof(vcfiles[handle].vfptr);
}

void vc_LoadSound()
{
	string sname = vc->ResolveString();
	vc->vcreturn = (int) LoadSample(sname.c_str());
}

void vc_FreeSound()
{
	int slot = vc->ResolveOperand();
	FreeSample((FSOUND_SAMPLE *) slot);
}

void vc_PlaySound()
{
	int slot = vc->ResolveOperand();
	int volume = vc->ResolveOperand();
	PlaySample((FSOUND_SAMPLE *) slot, volume * 255 / 100);
}

void vc_CallFunction()
{
	string func = vc->ResolveString();
	std::vector<argument_t> arguments;
	if (vc->CheckForVarargs())
	{
		int i;
		vc->ReadVararg(arguments);
		for (i = 0; i < arguments.size(); i++)
		{
			switch (arguments[i].type_id)
			{
				case t_INT:
					vc->ArgumentPassAddInt(arguments[i].int_value);
					break;
				case t_STRING:
					vc->ArgumentPassAddString(arguments[i].string_value);
					break;
			}
		}
	}
	vc->ExecuteFunctionString(func.c_str());
}

void vc_AssignArray()
{
	// FIXME: restore this. (cannot read globalint from here)
	/*int n = vc->ResolveOperand();
	string s = vc->ResolveString();

	if (s[0] != '{')
		vc->vcerr("vc_AssignArray() - string not properly formatted!");

	for (int ofs=1;;ofs++)
	{
		string numtoken;
		while (s[ofs] != ',' && s[ofs] != '}' )
			numtoken += s[ofs++];
		globalint[n++] = atoi(numtoken.c_str());
		if (s[ofs] == '}') break;
	}*/
}

void vc_FileSeekPos()
{
	int handle = vc->ResolveOperand();
	int offset = vc->ResolveOperand();
	int mode = vc->ResolveOperand();

	if (!handle || handle > VCFILES || !vcfiles[handle].active)
		vc->vcerr("vc_FileSeekPos() - file handle is either invalid or file is not open.");

	switch (vcfiles[handle].mode)
	{
		case VC_READ:
			vseek(vcfiles[handle].vfptr, offset, mode);
			break;
		case VC_WRITE:
			fseek(vcfiles[handle].fptr, offset, mode);
			break;
		default:
			vc->vcerr("vc_SeekPos() - File mode not valid! That's bad!");
	}
}

void vc_FileCurrentPos()
{
	int handle = vc->ResolveOperand();

	if (!handle || handle > VCFILES || !vcfiles[handle].active)
		vc->vcerr("vc_FileCurrentPos() - file handle is either invalid or file is not open.");

	switch (vcfiles[handle].mode)
	{
		case VC_READ:
			vc->vcreturn = vtell(vcfiles[handle].vfptr);
			break;
		case VC_WRITE:
			vc->vcreturn = ftell(vcfiles[handle].fptr);
			break;
		default:
			vc->vcerr("vc_FileCurentPos() - File mode not valid! That's bad!");
	}
}

void vc_FileWriteByte()
{
	int handle = vc->ResolveOperand();
	int var = vc->ResolveOperand();

	if (!handle || handle > VCFILES || !vcfiles[handle].active)
		vc->vcerr("vc_FileWriteByte() - file handle is either invalid or file is not open.");
	if (vcfiles[handle].mode != VC_WRITE)
		vc->vcerr("vc_FileWriteByte() - given file handle is a read-mode file.");
	flip(&var, sizeof(var)); // ensure little-endian writing
	fwrite(&var, 1, 1, vcfiles[handle].fptr);
}

void vc_FileWriteWord()
{
	int handle = vc->ResolveOperand();
	int var = vc->ResolveOperand();

	if (!handle || handle > VCFILES || !vcfiles[handle].active)
		vc->vcerr("vc_FileWriteWord() - file handle is either invalid or file is not open.");
	if (vcfiles[handle].mode != VC_WRITE)
		vc->vcerr("vc_FileWriteWord() - given file handle is a read-mode file.");
	flip(&var, sizeof(var)); // ensure little-endian writing
	fwrite(&var, 1, 2, vcfiles[handle].fptr);
}

void vc_FileWriteQuad()
{
	int handle = vc->ResolveOperand();
	int var = vc->ResolveOperand();

	if (!handle || handle > VCFILES || !vcfiles[handle].active)
		vc->vcerr("vc_FileWriteQuad() - file handle is either invalid or file is not open.");
	if (vcfiles[handle].mode != VC_WRITE)
		vc->vcerr("vc_FileWriteQuad() - given file handle is a read-mode file.");
	flip(&var, sizeof(var)); // ensure little-endian writing
	fwrite(&var, 1, 4, vcfiles[handle].fptr);
}

void vc_FileWriteString()
{
	int handle = vc->ResolveOperand();
	string s = vc->ResolveString();

	if (!handle || handle > VCFILES || !vcfiles[handle].active)
		vc->vcerr("vc_FileWriteString() - file handle is either invalid or file is not open.");
	if (vcfiles[handle].mode != VC_WRITE)
		vc->vcerr("vc_FileWriteString() - given file handle is a read-mode file.");

	int l = s.length();
	int writeLength = l;

	flip(&writeLength, sizeof(writeLength)); // ensure little-endian writing
	fwrite(&writeLength, 1, 2, vcfiles[handle].fptr);
	fwrite(s.c_str(), 1, l, vcfiles[handle].fptr);
}

void vc_FileReadByte()
{
	int handle = vc->ResolveOperand();

	if (!handle || handle > VCFILES || !vcfiles[handle].active)
		vc->vcerr("vc_FileReadByte() - file handle is either invalid or file is not open.");
	if (vcfiles[handle].mode != VC_READ)
		vc->vcerr("vc_FileReadByte() - given file handle is a write-mode file.");

	vc->vcreturn = 0;
	vread(&vc->vcreturn, 1, vcfiles[handle].vfptr);
}

void vc_FileReadWord()
{
	int handle = vc->ResolveOperand();

	if (!handle || handle > VCFILES || !vcfiles[handle].active)
		vc->vcerr("vc_FileReadWord() - file handle is either invalid or file is not open.");
	if (vcfiles[handle].mode != VC_READ)
		vc->vcerr("vc_FileReadWord() - given file handle is a write-mode file.");

	vc->vcreturn = 0;
	vread(&vc->vcreturn, 2, vcfiles[handle].vfptr);
}

void vc_FileReadQuad()
{
	int handle = vc->ResolveOperand();

	if (!handle || handle > VCFILES || !vcfiles[handle].active)
		vc->vcerr("vc_FileReadQuad() - file handle is either invalid or file is not open.");
	if (vcfiles[handle].mode != VC_READ)
		vc->vcerr("vc_FileReadQuad() - given file handle is a write-mode file.");

	vread(&vc->vcreturn, 4, vcfiles[handle].vfptr);
}

void vc_FileReadString()
{
	int handle = vc->ResolveOperand();
	int len = 0;
	char *buffer;

	if (!handle || handle > VCFILES || !vcfiles[handle].active)
		vc->vcerr("vc_FileReadString() - file handle is either invalid or file is not open.");
	if (vcfiles[handle].mode != VC_READ)
		vc->vcerr("vc_FileReadString() - given file handle is a write-mode file.");

	vread(&len, 2, vcfiles[handle].vfptr);
	buffer = new char[len+1];
	vread(buffer, len, vcfiles[handle].vfptr);
	buffer[len]=0;
	vc->vcretstr.assign(buffer);
	delete[] buffer;
}

void vc_sqrt()
{
	vc->vcreturn = (int) (float) sqrt((float) vc->ResolveOperand());
}

void vc_pow()
{
	int a = vc->ResolveOperand();
	int b = vc->ResolveOperand();
	vc->vcreturn = (int) pow((double)a, (double)b);
}

void vc_SetAppName()
{
	string s = vc->ResolveString();
	setWindowTitle(s.c_str());
}

void vc_SetResolution()
{
	v3_xres = vc->ResolveOperand();
	v3_yres = vc->ResolveOperand();

	vid_SetMode(v3_xres, v3_yres, vid_bpp, vid_window, MODE_SOFTWARE);
}

void vc_BlitLucent()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int lucent = vc->ResolveOperand();
	image *s = ImageForHandle(vc->ResolveOperand());
	image *d = ImageForHandle(vc->ResolveOperand());

	int oldalpha = alpha;
	SetLucent(lucent);
	Blit(x, y, s, d);
	SetLucent(oldalpha);
}

void vc_TBlitLucent()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int lucent = vc->ResolveOperand();
	image *s = ImageForHandle(vc->ResolveOperand());
	image *d = ImageForHandle(vc->ResolveOperand());

	int oldalpha = alpha;
	SetLucent(lucent);
	TBlit(x, y, s, d);
	SetLucent(oldalpha);
}

void vc_Map()
{
	string map = vc->ResolveString();
	strcpy(mapname, map.c_str());
	die = 1;
	done = 1;
	return;
}

void vc_strcmp()
{
	string s1 = vc->ResolveString();
	string s2 = vc->ResolveString();
	vc->vcreturn = strcmp(s1.c_str(), s2.c_str());
}

void vc_strdup()
{
	string s = vc->ResolveString();
	int times = vc->ResolveOperand();
	vc->vcretstr = "";
	for (int i=0; i<times; i++)
		vc->vcretstr += s;
}

void vc_HookTimer()
{
	hooktimer = 0;
	string s = vc->ResolveString();
	strcpy(timerfunc, s.c_str());
}

void vc_HookRetrace()
{
	string s = vc->ResolveString();
	strcpy(renderfunc, s.c_str());
}

void vc_HookKey()
{
	int k = vc->ResolveOperand();
	string s = vc->ResolveString();

	if (k<0 || k>127) return;
	bindarray[k] = s;
}

void vc_HookButton()
{
	int b = vc->ResolveOperand()-1;
	string s = vc->ResolveString();

	if (b<0 || b>3) return;
	bindbutton[b] = s;
}

void vc_HookEntityRender()
{
	int i = vc->ResolveOperand();
	string s = vc->ResolveString();

	if (i<0 || i>=entities)
		err("vc_HookEntityRender() - no such entity %d", i);
	entity[i]->hookrender = s;
}

void vc_BlitTile()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int t = vc->ResolveOperand();
	image *d = ImageForHandle(vc->ResolveOperand());
	if (current_map)
		current_map->tileset->Blit(x, y, t, d);
}

void vc_TBlitTile()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int t = vc->ResolveOperand();
	image *d = ImageForHandle(vc->ResolveOperand());
	if (current_map)
		current_map->tileset->TBlit(x, y, t, d);
}

void vc_BlitEntityFrame()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int e = vc->ResolveOperand();
	int f = vc->ResolveOperand();
	image *d = ImageForHandle(vc->ResolveOperand());

	if (!current_map || e<0 || e >= entities)
		return;
	entity[e]->chr->render(x, y, f, d);
}

void vc_GetTile()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int i = vc->ResolveOperand();
	if (!current_map)
		return;
	if (i>= current_map->numlayers)
		return;

	vc->vcreturn = current_map->layers[i]->GetTile(x, y);
}

void vc_SetTile()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int i = vc->ResolveOperand();
	int z = vc->ResolveOperand();

	if (!current_map)
		return;
	if (i>= current_map->numlayers)
		return;
	current_map->layers[i]->SetTile(x, y, z);
}

void vc_GetZone()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	if (!current_map)
		return;
	vc->vcreturn = current_map->zone(x, y);
}

void vc_SetZone()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int z = vc->ResolveOperand();
	if (!current_map)
		return;
	if (z >= current_map->numzones)
		return;

	current_map->SetZone(x, y, z);
}

void vc_SuperSecretThingy()
{
	int xskew = vc->ResolveOperand();
	int yofs = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	image *s = ImageForHandle(vc->ResolveOperand());
	image *d = ImageForHandle(vc->ResolveOperand());

	if (s->width != 256 || s->height != 256)
		err("SuperSecretThingy() - Source image MUST be 256x256!!");

	Timeless(xskew, yofs, y, s, d);
}

void vc_BlitWrap()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	image *s = ImageForHandle(vc->ResolveOperand());
	image *d = ImageForHandle(vc->ResolveOperand());
	BlitWrap(x, y, s, d);
}

void vc_ColorFilter()
{
	int filter = vc->ResolveOperand();
	image *d = ImageForHandle(vc->ResolveOperand());
	ColorFilter(filter, d);
}

void vc_ImageShell()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int w = vc->ResolveOperand();
	int h = vc->ResolveOperand();
	image *s = ImageForHandle(vc->ResolveOperand());
	if (w+x > s->width || y+h > s->height)
		err("ImageShell() - Bad arguements. x/y+w/h greater than original image dimensions");

	image *d = new image(w, h);
	d->delete_data();
	d->shell = 1;

	switch (s->bpp)
	{
		case 15:
		case 16:
			d->data = (void *) ((word *) s->data + (y*s->pitch)+x);
			d->pitch = s->pitch;
			break;
		case 32:
			d->data = (void *) ((quad *) s->data + (y*s->pitch)+x);
			d->pitch = s->pitch;
			break;
	}
	vc->vcreturn = HandleForImage(d);
}

void vc_Malloc()
{
	int s = vc->ResolveOperand();
	vc->vcreturn = (int) malloc(s);
}

void vc_MemFree()
{
	free((void *) vc->ResolveOperand());
}

void vc_MemCopy()
{
	int src = vc->ResolveOperand();
	int dst = vc->ResolveOperand();
	int len = vc->ResolveOperand();

	memcpy((void*) dst, (void *) src, len);
}

int mydtoi(double d)
{
	return (int)floor(d + 0.5);
}

void vc_asin()
{
	int val = vc->ResolveOperand();
	double dv = (double) val / 65535;
	double as = asin(dv);
	as = as * 180 / 3.14159265358979; // convert radians to degrees
	vc->vcreturn = mydtoi(as);
}

void vc_fasin()
{
	int val = vc->ResolveOperand();
	double dv = (double) val / 65535;
	double as = asin(dv);
	as *= 65536; // Convert to 16.16 fixed point
	vc->vcreturn = mydtoi(as);
}

void vc_fsin()
{
	int val = vc->ResolveOperand();
	double magnitude = sin((double) val / 65536);
	vc->vcreturn = mydtoi(magnitude * 65536);
}

void vc_fcos()
{
	int val = vc->ResolveOperand();
	double magnitude = cos((double) val / 65536);
	vc->vcreturn = mydtoi(magnitude * 65536);
}

void vc_ftan()
{
	int val = vc->ResolveOperand();
	double magnitude = tan((double) val / 65536);
	vc->vcreturn = mydtoi(magnitude * 65536);
}

void vc_acos()
{
	int val = vc->ResolveOperand();
	double dv = (double) val / 65535;
	double ac = acos(dv);
	ac = ac * 180 / 3.14159265358979; // convert radians to degrees
	vc->vcreturn = mydtoi(ac);
}

void vc_facos()
{
	int val = vc->ResolveOperand();
	double dv = (double) val / 65535;
	double ac = acos(dv);
	ac *= 65536; // Convert to 16.16 fixed point
	vc->vcreturn = mydtoi(ac);
}


void vc_atan()
{
	int val = vc->ResolveOperand();
	double dv = (double) val / 65535;
	double at = atan(dv);
	at = at * 180 / 3.14159265358979; // convert radians to degrees
	vc->vcreturn = mydtoi(at);
}

void vc_fatan()
{
	int val = vc->ResolveOperand();
	double dv = (double) val / 65535;
	double at = atan(dv);
	at *= 65536; // Convert to 16.16 fixed point
	vc->vcreturn = mydtoi(at);
}

void vc_AlphaBlit()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	image *s = ImageForHandle(vc->ResolveOperand());
	image *a = ImageForHandle(vc->ResolveOperand());
	image *d = ImageForHandle(vc->ResolveOperand());
	AlphaBlit(x, y, s, a, d);
}


void vc_WindowCreate()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int w = vc->ResolveOperand();
	int h = vc->ResolveOperand();
	string s = vc->ResolveString();

	AuxWindow *auxwin = vid_createAuxWindow();
	auxwin->setTitle(s.c_str());
	auxwin->setPosition(x,y);
	auxwin->setResolution(w,h);
	auxwin->setSize(w,h);
	auxwin->setVisibility(true);
	vc->vcreturn = auxwin->getHandle();
}

void ___vc_window_checkhandle(char *func, int handle, AuxWindow *auxwin)
{
	if(!handle)
		vc->vcerr("%s() - cannot access a null window handle!",func);
	if(!auxwin)
		vc->vcerr("%s() - invalid window handle!",func);
}

void vc_WindowGetImage()
{
	int win = vc->ResolveOperand();
	AuxWindow *auxwin = vid_findAuxWindow(win);
	___vc_window_checkhandle("vc_WindowGetImage",win,auxwin);
	vc->vcreturn = auxwin->getImageHandle();
}

void vc_WindowClose()
{
	int win = vc->ResolveOperand();
	if(win == 1)
		vc->vcerr("vc_WindowClose() - cannot close gameWindow");

	AuxWindow *auxwin = vid_findAuxWindow(win);
	___vc_window_checkhandle("vc_WindowClose",win,auxwin);
	auxwin->dispose();
}

void vc_WindowSetSize()
{
	int win = vc->ResolveOperand();
	int w = vc->ResolveOperand();
	int h = vc->ResolveOperand();

	AuxWindow *auxwin = vid_findAuxWindow(win);
	___vc_window_checkhandle("vc_WindowSetSize",win,auxwin);
	auxwin->setSize(w,h);
}

void vc_WindowSetResolution()
{
	int win = vc->ResolveOperand();
	int w = vc->ResolveOperand();
	int h = vc->ResolveOperand();

	AuxWindow *auxwin = vid_findAuxWindow(win);
	___vc_window_checkhandle("vc_WindowSetSize",win,auxwin);
	auxwin->setResolution(w,h);
	auxwin->setSize(w,h);
}

void vc_WindowSetPosition()
{
	int win = vc->ResolveOperand();
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();

	AuxWindow *auxwin = vid_findAuxWindow(win);
	___vc_window_checkhandle("vc_WindowSetPosition",win,auxwin);
	auxwin->setPosition(x,y);
}

void vc_WindowSetTitle()
{
	int win = vc->ResolveOperand();
	string s = vc->ResolveString();

	AuxWindow *auxwin = vid_findAuxWindow(win);
	___vc_window_checkhandle("vc_WindowSetTitle",win,auxwin);
	auxwin->setTitle(s.c_str());
}

void vc_WindowHide()
{
	int win = vc->ResolveOperand();

	AuxWindow *auxwin = vid_findAuxWindow(win);
	___vc_window_checkhandle("vc_WindowHide",win,auxwin);
	auxwin->setVisibility(false);
}

void vc_WindowShow()
{
	int win = vc->ResolveOperand();

	AuxWindow *auxwin = vid_findAuxWindow(win);
	___vc_window_checkhandle("vc_WindowShow",win,auxwin);
	auxwin->setVisibility(true);
}

void vc_WindowGetXRes()
{
	int win = vc->ResolveOperand();

	AuxWindow *auxwin = vid_findAuxWindow(win);
	___vc_window_checkhandle("vc_WindowGetXRes",win,auxwin);
	vc->vcreturn = auxwin->getXres();
}

void vc_WindowGetYRes()
{
	int win = vc->ResolveOperand();

	AuxWindow *auxwin = vid_findAuxWindow(win);
	___vc_window_checkhandle("vc_WindowGetYRes",win,auxwin);
	vc->vcreturn = auxwin->getYres();
}

void vc_WindowGetWidth()
{
	int win = vc->ResolveOperand();

	AuxWindow *auxwin = vid_findAuxWindow(win);
	___vc_window_checkhandle("vc_WindowGetWidth",win,auxwin);
	vc->vcreturn = auxwin->getWidth();
}

void vc_WindowGetHeight()
{
	int win = vc->ResolveOperand();

	AuxWindow *auxwin = vid_findAuxWindow(win);
	___vc_window_checkhandle("vc_WindowGetHeight",win,auxwin);
	vc->vcreturn = auxwin->getHeight();
}

void vc_WindowPositionCommand()
{
	int win = vc->ResolveOperand();
	int command = vc->ResolveOperand();
	int arg1 = vc->ResolveOperand();
	int arg2 = vc->ResolveOperand();

	AuxWindow *auxwin = vid_findAuxWindow(win);
	___vc_window_checkhandle("vc_WindowGetHeight",win,auxwin);

	auxwin->positionCommand(command,arg1,arg2);
}

void vc_SetSongPaused()
{
	int h = vc->ResolveOperand();
	int p = vc->ResolveOperand();
	SetPaused(h, p);
}

void vc_SetSongVolume()
{
	int h = vc->ResolveOperand();
	int v = vc->ResolveOperand();
	SetSongVol(h, v);
}

void vc_SetSongPos()
{
	int h = vc->ResolveOperand();
	int v = vc->ResolveOperand();
	SetSongPos(h, v);
}

void vc_SetMusicVolume()
{
	int v = vc->ResolveOperand();
	SetMusicVolume(v);
}


bool isdelim(char c, string s)
{
	for (int i=0; i<s.length(); i++)
		if (c==s[i])
			return true;
	return false;
}

void vc_TokenCount()
{
	string s = vc->ResolveString();
	string d = vc->ResolveString();

	int n = 0;
	int tokenindex = 0;
	while (n < s.length())
	{
		bool tp = false;

		while (n < s.length() && isdelim(s[n], d))
			n++;
		while (n < s.length() && !isdelim(s[n], d))
		{
			tp = true;
			n++;
		}
		if (tp) tokenindex++;
	}
	vc->vcreturn = tokenindex;
}

void vc_StringToken()
{
	string s = vc->ResolveString();
	string d = vc->ResolveString();
	int i = vc->ResolveOperand();

	int n = 0;
	int tokenindex = 0;
	while (n < s.length())
	{
		string token = "";

		while (n < s.length() && isdelim(s[n], d))
			n++;
		while (n < s.length() && !isdelim(s[n], d))
		{
			token += s[n];
			n++;
		}
		if (i == tokenindex)
		{
			vc->vcretstr = token;
			return;
		}
		tokenindex++;
	}

	vc->vcretstr = "";
}


// Overkill: 2005-12-28
// Helper function.
int strpos(string source, string sub, int start)
{
	int i;
	int count = 0;
	int lensub = sub.length();
	int lensource = source.length();
	for (i = start; i < lensource; i++)
	{
		if (!strcmp(sub.c_str(), source.mid(i, lensub).c_str())) return i;
	}
	return lensource;
}

// Overkill: 2005-12-28
// Thank you, Zip.
void vc_strpos()
{
	string sub = vc->ResolveString();
	string source = vc->ResolveString();
	int start = vc->ResolveOperand();
	vc->vcreturn = strpos(source, sub, start);
}

// Overkill: 2005-12-28
// Helper function.
int GetTokenPos(string teststr, string tokens, int pos, int tok)
{
	int i = 0;
	bool last = false;
	int count = 0;
	int length = teststr.length();
	if (pos == 0)
	{
		return 0;
	}
	for (i = 0; i < teststr.length(); i++)
	{
		if (isdelim(teststr[i], tokens))
		{
			if (!last)
			{
				count++;
				if (count == pos && tok == 0)
				{
					return i;
				}
				last = true;
			}
		}
		else if (last)
		{
			if (count == pos)
			{
				return i;
			}
			last = false;
		}
	}
	return 0;
}

// Overkill: 2005-12-28
// Thank you, Zip.
void vc_GetTokenPos()
{

	string teststr = vc->ResolveString();
	string tokens = vc->ResolveString();
	int pos = vc->ResolveOperand();
	int tok = vc->ResolveOperand();
	vc->vcreturn = GetTokenPos(teststr, tokens, pos, tok);
}


// Overkill: 2005-12-28
// Thank you, Zip.
void vc_TokenLeft()	// Excludes token.
{
	string full = vc->ResolveString();
	string tokens = vc->ResolveString();
	int pos = vc->ResolveOperand();
	if (pos < 1)
	{
		vc->vcretstr = "";
		return;
	}
	pos = GetTokenPos(full, tokens, pos, 0);
	if (pos == full.length())
	{
		vc->vcretstr = full;
		return;
	}
	else
	{
		vc->vcretstr = full.left(pos);
	}
}

// Overkill: 2005-12-28
// Thank you, Zip.
// Overkill (2006-07-28):
//	Fixed a bug where it included the delimiter character in TokenRight()
void vc_TokenRight()
{
	string full = vc->ResolveString();
	string tokens = vc->ResolveString();
	int pos = vc->ResolveOperand();
	if (pos < 1)
	{
		vc->vcretstr = full;
		return;
	}
	pos = GetTokenPos(full, tokens, pos, 1);
	if (pos == full.length())
	{
		vc->vcretstr = "";
		return;
	}
	else
	{
		vc->vcretstr = full.right(full.length() - pos);
	}
}

// Overkill: 2005-12-28
// Helper function for WrapText.
int TextWidth(int f, string text)
{
	Font *font = (Font*) f;
	if (font == 0)
		return pixels(text.c_str());
	else
		return font->Pixels(text.c_str());
}

// Overkill: 2005-12-28
// Thank you, Zip.
string strovr(string source, string rep, int offset)
// Pass: The offset in the source to overwrite from, the string to overwrite with, the source string
// Return: The string after overwrite
// Assmes: Offset is less than source length
{
	int length = source.length();
	int replen = rep.length();
	if (length < replen + offset) return source.left(offset) + rep;
	return source.left(offset) + rep + source.right(length - offset - replen);
}


// Overkill: 2005-12-28
// Thank you, Zip.
void vc_strovr()
{
	string rep = vc->ResolveString();
	string source = vc->ResolveString();
	int offset = vc->ResolveOperand();
	vc->vcretstr = strovr(source, rep, offset);
}

// Overkill: 2005-12-19
// Thank you, Zip.
void vc_WrapText()
// Pass: The font to use, the string to wrap, the length in pixels to fit into
// Return: The passed string with \n characters inserted as breaks
// Assmes: The font is valid, and will overrun if a word is longer than linelen
// Note: Existing breaks will be respected, but adjacent \n characters will be
//     replaced with a single \n so add a space for multiple line breaks
{

	int wt_font = vc->ResolveOperand();
	string wt_s = vc->ResolveString();
	int wt_linelen = vc->ResolveOperand();

	string wt_tpara = "";
	int curpara = 0;
	int nextpara, lenpara, curchr, nextchr, width = 0;
	int length = wt_s.length();

	while (curpara < length)
	{
		nextpara = strpos(wt_s, "\n", curpara + 1);
		if (nextpara > curpara + 1)
		{
			lenpara = nextpara - curpara - 1;
			wt_tpara = wt_s.mid(curpara + 1, lenpara);
			if (TextWidth(wt_font, wt_tpara) > wt_linelen)
			{
				curchr = -1;
				width = 0;
				while (curchr < lenpara)
				{
					nextchr = strpos(wt_tpara, " ", curchr + 1);
					width += TextWidth(wt_font, wt_tpara.mid(curchr, nextchr - curchr));
					if (width > wt_linelen)
					{
						// Overkill (2006-07-23): D'oh.
						// Forgot to set the string to the return value >__>
						wt_s = strovr(wt_s, "\n", curpara + curchr + 1);
						width = TextWidth(wt_font, wt_tpara.mid(curchr + 1, nextchr - curchr - 1));
					}
					curchr = nextchr;
				}
			}
		}
		curpara = nextpara;
	}
	vc->vcretstr = wt_s;
}

void vc_ToUpper()
{
	string s = vc->ResolveString();
	vc->vcretstr = s.upper();
}

void vc_FontHeight()
{
	int f = vc->ResolveOperand();
	if (!f) vc->vcreturn = 7;
	else vc->vcreturn = ((Font *) f)->height;
}

void vc_MixColor()
{
	int c1 = vc->ResolveOperand();
	int c2 = vc->ResolveOperand();
	int p = vc->ResolveOperand();

	if (p>255) p=255;
	if (p<0) p=0;

	int r1, g1, b1;
	int r2, g2, b2;
	GetColor(c1, r1, g1, b1);
	GetColor(c2, r2, g2, b2);

	vc->vcreturn = MakeColor((r1*(255-p)/255)+(r2*p/255), (g1*(255-p)/255)+(g2*p/255), (b1*(255-p)/255)+(b2*p/255));
}

void vc_CHR()
{
	int c = vc->ResolveOperand();

	vc->vcretstr = va("%c", c);
}

void vc_PlayMovie()
{
	string s = vc->ResolveString();
	vc->vcreturn = win_movie_playSimple(s.c_str());
}

//mbg 12/11/05
void vc_AbortMovie()
{
	win_movie_abortSimple();
}

void vc_MovieLoad()
{
	string s = vc->ResolveString();
	int mute = vc->ResolveOperand();

	vc->vcreturn = win_movie_load(s.c_str(), mute!=0);
}

void vc_MoviePlay()
{
	int m = vc->ResolveOperand();
	int loop = vc->ResolveOperand();
	win_movie_play(m,loop);
}

void vc_MovieGetImage()
{
	int m = vc->ResolveOperand();
	vc->vcreturn = win_movie_getImage(m);
}

void vc_MovieRender()
{
	int m = vc->ResolveOperand();
	win_movie_render(m);
}

void vc_MovieClose()
{
	int m = vc->ResolveOperand();
	win_movie_close(m);
}

void vc_MovieGetCurrFrame()
{
	int m = vc->ResolveOperand();
	vc->vcreturn = win_movie_getCurrFrame(m);
}

void vc_MovieGetFramerate()
{
	int m = vc->ResolveOperand();
	vc->vcreturn = win_movie_getFramerate(m);
}

void vc_MovieNextFrame()
{
	int m = vc->ResolveOperand();
	win_movie_nextFrame(m);
}

void vc_MovieSetFrame()
{
	int m = vc->ResolveOperand();
	int f = vc->ResolveOperand();
	win_movie_setFrame(m,f);
}

void vc_GetObsPixel()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();

	if (!current_map)
	{
		vc->vcreturn = 0;
		return;
	}

	vc->vcreturn = current_map->obstructpixel(x, y);
}

void vc_GetObs()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();

	if (!current_map)
	{
		vc->vcreturn = 0;
		return;
	}
	vc->vcreturn = current_map->obstruct(x, y);
}

void vc_SetObs()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int c = vc->ResolveOperand();

	if (!current_map)
	{
		vc->vcreturn = 0;
		return;
	}
	current_map->SetObs(x, y, c);
}

void vc_EntitySpawn()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	string s = vc->ResolveString();

	vc->vcreturn = AllocateEntity(x*16,y*16,s.c_str());
}

void vc_SetPlayer()
{
	int e = vc->ResolveOperand();
	if (e<0 || e>=entities)
	{
		player = -1;
		myself = 0;
		return;
	}
	myself = entity[e];
	player = e;
	myself->SetMotionless();
	myself->obstructable = true;
}

void vc_StalkEntity()
{
	int stalker = vc->ResolveOperand();
	int stalkee = vc->ResolveOperand();
	if (stalker<0 || stalker>=entities)
		return;
	if (stalkee<0 || stalkee>=entities)
	{
		entity[stalker]->clear_stalk();
		return;
	}
	entity[stalker]->stalk(entity[stalkee]);
}

void vc_EntityMove()
{
	int e = vc->ResolveOperand();
	string s = vc->ResolveString();
	if (e<0 || e >= entities)
		return;
	entity[e]->SetMoveScript(s.c_str());
}

void vc_PlayerMove()
{
	string s = vc->ResolveString();
    if (!myself) return;
	myself->SetMoveScript(s.c_str());
	while (myself->movecode)
	{
		TimedProcessEntities();
		Render();
		ShowPage();
	}
}

void vc_ChangeCHR()
{
	int e = vc->ResolveOperand();
	string c = vc->ResolveString();
	if (e<0 || e >= entities)
		return;
	entity[e]->set_chr(c);
}

void vc_EntitySetWanderZone()
{
	int e = vc->ResolveOperand();
	if (e<0 || e >= entities)
		return;
	entity[e]->SetWanderZone();
}

void vc_EntitySetWanderRect()
{
	int e = vc->ResolveOperand();
	int x1 = vc->ResolveOperand();
	int y1 = vc->ResolveOperand();
	int x2 = vc->ResolveOperand();
	int y2 = vc->ResolveOperand();
	if (e<0 || e >= entities)
		return;
	entity[e]->SetWanderBox(x1, y1, x2, y2);
}

void vc_EntityStop()
{
	int e = vc->ResolveOperand();
	if (e<0 || e >= entities)
		return;
	entity[e]->SetMotionless();
}

void vc_EntitySetWanderDelay()
{
	int e = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	if (e<0 || e >= entities)
		return;
	entity[e]->SetWanderDelay(d);
}

void vc_SetEntitiesPaused()
{
	int i = vc->ResolveOperand();
	entitiespaused = i ? true : false;
	if (!entitiespaused)
		lastentitythink = systemtime;
}

void vc_RenderMap()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	image *dest = ImageForHandle(vc->ResolveOperand());
	if (current_map)
		current_map->render(x, y, dest);
}

void vc_SetButtonKey()
{
	int b = vc->ResolveOperand();
	int k = vc->ResolveOperand();

	switch (b)
	{
		case 1: k_b1 = k; break;
		case 2: k_b2 = k; break;
		case 3: k_b3 = k; break;
		case 4: k_b4 = k; break;
		// Overkill (2006-06-25): Can set the directionals as well, now.
		case 5: k_up = k; break;
		case 6: k_down = k; break;
		case 7: k_left = k; break;
		case 8: k_right = k; break;
	}
}

void vc_SetButtonJB()
{
	int b = vc->ResolveOperand();
	int jb = vc->ResolveOperand();

	switch (b)
	{
		case 1: j_b1 = jb; break;
		case 2: j_b2 = jb; break;
		case 3: j_b3 = jb; break;
		case 4: j_b4 = jb; break;
	}
}

void vc_FunctionExists()
{
	string f = vc->ResolveString();
	vc->vcreturn = vc->FunctionExists(f.c_str());
}

void vc_atan2()
{
	int y = vc->ResolveOperand();
	int x = vc->ResolveOperand();
	float f = atan2((float)y,(float)x);
	vc->vcreturn = (int)(f/2.0/3.14159265358979*360.0);
}

void vc_fatan2()
{
	int y = vc->ResolveOperand();
	int x = vc->ResolveOperand();
	double theta = atan2((double) y, (double) x);
	vc->vcreturn = mydtoi(theta * 65536);
}

void vc_CopyImageToClipboard()
{
	image *src = ImageForHandle(vc->ResolveOperand());
	clipboard_putImage(src);
}

void vc_GetImageFromClipboard()
{
	image *t = clipboard_getImage();
	if (!t)
	{
		vc->vcreturn = 0;
		return;
	}

	else vc->vcreturn = HandleForImage(t);
}

void vc_SetInt()
{
	string intname = vc->ResolveString();
	int value = vc->ResolveOperand();
	vc->SetInt(intname.c_str(), value);
}

void vc_GetInt()
{
	string intname = vc->ResolveString();
	vc->vcreturn = vc->GetInt(intname.c_str());
}

void vc_SetString()
{
	string strname = vc->ResolveString();
	string value = vc->ResolveString();
	vc->SetStr(strname.c_str(), value);
}

void vc_GetString()
{
	string strname = vc->ResolveString();
	vc->vcretstr = vc->GetStr(strname.c_str());
}

void vc_SetIntArray()
{
	string intname = vc->ResolveString();
	int index = vc->ResolveOperand();
	int value = vc->ResolveOperand();
	vc->SetIntArray(intname.c_str(), index, value);
}

void vc_GetIntArray()
{
	string intname = vc->ResolveString();
	int index = vc->ResolveOperand();
	vc->vcreturn = vc->GetIntArray(intname.c_str(), index);
}

void vc_SetStringArray()
{
	string strname = vc->ResolveString();
	int index = vc->ResolveOperand();
	string value = vc->ResolveString();
	vc->SetStrArray(strname.c_str(), index, value);
}

void vc_GetStringArray()
{
	string strname = vc->ResolveString();
	int index = vc->ResolveOperand();
	vc->vcretstr = vc->GetStrArray(strname.c_str(), index);
}

void vc_FlipBlit()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int fx = vc->ResolveOperand();
	int fy = vc->ResolveOperand();
	image *s = ImageForHandle(vc->ResolveOperand());
	image *d = ImageForHandle(vc->ResolveOperand());
	FlipBlit(x, y, fx, fy, s, d);
}

ServerSocket *vcserver = 0;

void vc_Connect()
{
	string ip = vc->ResolveString();
	Socket *s;
	try
	{
		s = new Socket(ip.c_str(), 45150);
	}
	catch (NetworkException ne)
	{
		vc->vcreturn = 0;
		return;
	}
	vc->vcreturn = (int) s;
}

void vc_GetConnection()
{
	try
    {
		if (!vcserver)
			vcserver = new ServerSocket(45150);
		Socket *s = vcserver->accept();
		vc->vcreturn = (int) s;
	}
	catch(NetworkException e)
    {
		vc->vcreturn = 0;
		return;
    }
}

void vc_SocketConnected()
{
	Socket *s = (Socket *) vc->ResolveOperand();
	vc->vcreturn = s->connected();
}

void vc_SocketHasData()
{
	Socket *s = (Socket *) vc->ResolveOperand();
	vc->vcreturn = s->dataready();
}

void vc_SocketGetString()
{
	static char buf[4096];
	Socket *s = (Socket *) vc->ResolveOperand();
	int stlen = 0, ret;
	char t;
	ret = s->blockread(1, &t);
	if (t != '3')
		err("SocketGetString() - packet being received is not a string");
	ret = s->blockread(2, &stlen);
	if (!ret)
	{
		vc->vcretstr = "";
		return;
	}
#ifdef __APPLE__
#ifdef __BIG_ENDIAN__
	stlen >>= 16;
#endif
#endif
	if (stlen>4095) err("yeah uh dont send such big strings thru the network plz0r");
	ret = s->blockread(stlen, buf);
	buf[stlen] = 0;
	vc->vcretstr = buf;
}

void vc_SocketSendString()
{
	Socket *s = (Socket *) vc->ResolveOperand();
	string str = vc->ResolveString();
	int len = str.length();
	if (len>4095) err("yeah uh dont send such big strings thru the network plz0r");
	char t = '3';
	s->write(1, &t);
#ifdef __APPLE__
#ifdef __BIG_ENDIAN__
	len <<= 16;
#endif
#endif
	s->write(2, &len);
#ifdef __APPLE__
#ifdef __BIG_ENDIAN__
	len >>= 16;
#endif
#endif
	s->write(len, str.c_str());
}

void vc_SocketClose()
{
	Socket *s = (Socket *) vc->ResolveOperand();
	delete s;
}

void vc_SetCustomColorFilter()
{
	int c1 = vc->ResolveOperand();
	int c2 = vc->ResolveOperand();
	GetColor(c1, cf_r1, cf_g1, cf_b1);
	GetColor(c2, cf_r2, cf_g2, cf_b2);
	cf_rr = cf_r2 - cf_r1;
	cf_gr = cf_g2 - cf_g1;
	cf_br = cf_b2 - cf_b1;
}

void vc_SocketSendInt()
{
	Socket *s = (Socket *) vc->ResolveOperand();
	int i = vc->ResolveOperand();
	char t = '1';
	s->write(1, &t);
	s->write(4, &i);
}

void vc_SocketGetInt()
{
	Socket *s = (Socket *) vc->ResolveOperand();
	int ret;
	char t;
	ret = s->blockread(1, &t);
	if (t != '1')
		err("SocketGetInt() - packet being received is not an int");
	ret = s->blockread(4, &vc->vcreturn);
}

void vc_GetUrlText()
{
	string url = vc->ResolveString();
	vc->vcretstr = getUrlText(url);
}

void vc_GetUrlImage()
{
	string url = vc->ResolveString();
	vc->vcreturn = getUrlImage(url);
}

void vc_SocketSendFile()
{
	Socket *s = (Socket *) vc->ResolveOperand();
	string fn = vc->ResolveString();

	EnforceNoDirectories(fn);

	VFILE *f = vopen(fn.c_str());
	if (!f)
		err("ehhhhhh here's a tip. SocketSendFile can't send a file that doesnt exist (you tried to send %s)", fn.c_str());

	int i = fn.length();
	s->write(2, &i);
	s->write(i, fn.c_str());

	int l = filesize(f);
	s->write(4, &l);
	char *buf = new char[l];
	vread(buf, l, f);
	s->write(l, buf);
	delete[] buf;
	vclose(f);
}

void vc_SocketGetFile()
{
	static char stbuf[4096];
	Socket *s = (Socket *) vc->ResolveOperand();
	string override = vc->ResolveString();

	EnforceNoDirectories(override);

	int stlen = 0, ret;
	ret = s->blockread(2, &stlen);
	if (!ret)
	{
		vc->vcretstr = "";
		return;
	}
	ret = s->blockread(stlen, stbuf);
	stbuf[stlen] = 0;

	string fn = stbuf;
	EnforceNoDirectories(fn);

	int fl;
	s->blockread(4, &fl);

	char *buf = new char[fl];
	s->blockread(fl, buf);

	FILE *f;
	if (override.length())
	{
		vc->vcretstr = override;
		f = fopen(override.c_str(), "wb");
	}
	else
	{
		vc->vcretstr = fn;
		f = fopen(fn.c_str(), "wb");
	}
	if (!f)
		err("SocketGetFile: couldn't open file for writing!");
	fwrite(buf, 1, fl, f);
	fclose(f);
	delete[] buf;

	vc->vcretstr = buf;
}

void vc_ListFilePattern()
{
	string pattern = vc->ResolveString();
	std::vector<string> result = listFilePattern(pattern);
	vc->vcretstr = "";

	for(std::vector<string>::iterator i = result.begin();
		i != result.end();
		i++)
	{
		vc->vcretstr += *i + "|";
	}
}

void vc_ImageValid()
{
	int handle = vc->ResolveOperand();

	if (handle <= 0 || handle >= Handle::getHandleCount(HANDLE_TYPE_IMAGE) || (Handle::getPointer(HANDLE_TYPE_IMAGE,handle) == NULL) ) {
		vc->vcreturn = 0;
	} else {
		vc->vcreturn = 1;
	}
}

void vc_Asc()
{
	string s = vc->ResolveString();
	if(s == "")
		vc->vcreturn = 0;
	else
		vc->vcreturn = (int)s[0];
}


void vc_DictNew() {
	dict *d = new dict();
	vc->vcreturn = HandleForDict(d);
}

void vc_DictFree() {
	int handle = vc->ResolveOperand();
	dict *d = DictForHandle(handle);
	FreeDictHandle(handle);
	delete d;
}

void vc_DictGetString() {
	dict *d = DictForHandle(vc->ResolveOperand());
	vc->vcretstr = d->GetString(vc->ResolveString());
}

void vc_DictSetString() {
	dict *d = DictForHandle(vc->ResolveOperand());
	string key = vc->ResolveString();
	string value = vc->ResolveString();
	d->SetString(key, value);
}

void vc_DictContainsString() {
	dict *d = DictForHandle(vc->ResolveOperand());
	vc->vcreturn = d->ContainsString(vc->ResolveString());
}

void vc_DictSize() {
	dict *d = DictForHandle(vc->ResolveOperand());
	vc->vcreturn = d->Size();
}

void vc_DictGetInt() {
	dict *d = DictForHandle(vc->ResolveOperand());
	vc->vcreturn = atoi(d->GetString(vc->ResolveString()).c_str());
}

void vc_DictSetInt() {
	dict *d = DictForHandle(vc->ResolveOperand());
	string key = vc->ResolveString();
	int value = vc->ResolveOperand();
	d->SetString(key, va("%d", value));
}

void vc_DictRemove() {
	dict *d = DictForHandle(vc->ResolveOperand());
	string key = vc->ResolveString();
	d->RemoveString(key);
}

// Overkill: 2007-06-20
void vc_DictListKeys() {
	dict *d = DictForHandle(vc->ResolveOperand());
	string separator = vc->ResolveString();
	vc->vcretstr = d->ListKeys(separator);
}

// Overkill: 12/18/05
void vc_max()
{
	int a = vc->ResolveOperand();
	int b = vc->ResolveOperand();
	vc->vcreturn = (a < b ? b : a);
}

// Overkill: 12/18/05
void vc_min()
{
	int a = vc->ResolveOperand();
	int b = vc->ResolveOperand();
	vc->vcreturn = (a > b ? b : a);
}

// Overkill: 12/18/05
void vc_abs()
{
	int a = vc->ResolveOperand();
	vc->vcreturn = (a < 0 ? -a : a);
}

// Overkill: 12/19/05
void vc_sgn()
{
	int a = vc->ResolveOperand();
	vc->vcreturn = (((a) < 0) ? -1 : ((a) > 0) ? 1 : 0);
}

// Overkill 2006-02-04
void vc_RectVGrad()
{
	int x1 = vc->ResolveOperand();
	int y1 = vc->ResolveOperand();
	int x2 = vc->ResolveOperand();
	int y2 = vc->ResolveOperand();
	int c = vc->ResolveOperand();
	int c2 = vc->ResolveOperand();
	image *d = ImageForHandle(vc->ResolveOperand());
	RectVGrad(x1, y1, x2, y2, c, c2, d);
}

// Overkill 2006-02-04
void vc_RectHGrad()
{
	int x1 = vc->ResolveOperand();
	int y1 = vc->ResolveOperand();
	int x2 = vc->ResolveOperand();
	int y2 = vc->ResolveOperand();
	int c = vc->ResolveOperand();
	int c2 = vc->ResolveOperand();
	image *d = ImageForHandle(vc->ResolveOperand());
	RectHGrad(x1, y1, x2, y2, c, c2, d);
}

// janus 2006-07-22
void vc_RectRGrad()
{
	int x1 = vc->ResolveOperand();
	int y1 = vc->ResolveOperand();
	int x2 = vc->ResolveOperand();
	int y2 = vc->ResolveOperand();
	int c1 = vc->ResolveOperand();
	int c2 = vc->ResolveOperand();
	image *d = ImageForHandle(vc->ResolveOperand());
	RectRGrad(x1, y1, x2, y2, c1, c2, d);
}

// janus 2006-07-22
void vc_Rect4Grad()
{
	int x1 = vc->ResolveOperand();
	int y1 = vc->ResolveOperand();
	int x2 = vc->ResolveOperand();
	int y2 = vc->ResolveOperand();
	int c1 = vc->ResolveOperand();
	int c2 = vc->ResolveOperand();
	int c3 = vc->ResolveOperand();
	int c4 = vc->ResolveOperand();
	image *d = ImageForHandle(vc->ResolveOperand());
	Rect4Grad(x1, y1, x2, y2, c1, c2, c3, c4, d);
}

// Overkill (2006-06-25): Returns the cube root of a number.
void vc_cbrt()
{
	vc->vcreturn = (int) (float) pow((float) vc->ResolveOperand(), (float) 1 / 3);
}

// Overkill (2006-06-30): Gets the contents of the key buffer.
// TODO: Implement for other platforms.
void vc_GetKeyBuffer()
{
	#ifdef __WIN32__
		vc->vcretstr = keybuffer;
	#else 
		err("The function GetKeyBuffer() is not defined for this platform.");
	#endif
}

// Overkill (2006-06-30): Clears the contents of the key buffer.
// TODO: Implement for other platforms.
void vc_FlushKeyBuffer()
{
	#ifdef __WIN32__
		FlushKeyBuffer();
	#else 
		err("The function FlushKeyBuffer() is not defined for this platform.");
	#endif
}

// Overkill (2006-06-30): Sets the delay in centiseconds before key repeat.
// TODO: Implement for other platforms.
void vc_SetKeyDelay()
{
	int d = vc->ResolveOperand();
	if (d <= 0)
	{
		d = 0;
	}
	#ifdef __WIN32__
		key_input_delay = d;
	#else 
		err("The function SetKeyDelay() is not defined for this platform.");
	#endif
}
// Overkill (2006-07-20):
// Saves a CHR file, using an open file handle, saving the specified entity.
void vc_FileWriteCHR()
{
	int handle = vc->ResolveOperand();
	int ent = vc->ResolveOperand();

	if (!handle || handle > VCFILES || !vcfiles[handle].active)
		vc->vcerr("vc_FileWriteCHR() - file handle is either invalid or file is not open.");
	if (vcfiles[handle].mode != VC_WRITE)
		vc->vcerr("vc_FileWriteCHR() - given file handle is a read-mode file.");
	if (ent < 0 || ent >= entities)
		vc->vcerr("Tried saving an invalid or inactive ent index (%d).", ent);

	entity[ent]->chr->save(vcfiles[handle].fptr);	
}

// Overkill (2006-07-20):
// Saves a MAP file, using an open file handle, saving the current map.
void vc_FileWriteMAP()
{
	int handle = vc->ResolveOperand();

	if (!handle || handle > VCFILES || !vcfiles[handle].active)
		vc->vcerr("vc_FileWriteMAP() - file handle is either invalid or file is not open.");
	if (vcfiles[handle].mode != VC_WRITE)
		vc->vcerr("vc_FileWriteMAP() - given file handle is a read-mode file.");
	if (!current_map)
	{
		vc->vcerr("vc_FileWriteMAP() - There is no active map, therefore making it not possible to save this map.");
	}

	current_map->save(vcfiles[handle].fptr);	
}
// Overkill (2006-07-20):
// Saves a VSP file, using an open file handle, saving the current map's VSP.
void vc_FileWriteVSP()
{
	int handle = vc->ResolveOperand();

	if (!handle || handle > VCFILES || !vcfiles[handle].active)
		vc->vcerr("vc_FileWriteVSP() - file handle is either invalid or file is not open.");
	if (vcfiles[handle].mode != VC_WRITE)
		vc->vcerr("vc_FileWriteVSP() - given file handle is a read-mode file.");
	if (!current_map)
	{
		vc->vcerr("vc_FileWriteVSP() - There is no active map, therefore making it not possible to save the map's vsp.");
	}

	current_map->tileset->save(vcfiles[handle].fptr);	
}

// Overkill (2006-07-20):
// Compiles the specified MAP filename.
void vc_CompileMap()
{
	if(releasemode)
	{
		vc->vcerr("vc_CompileMap() - Can't compile map in release mode!");
	}
	string filename = vc->ResolveString();
	// Get the filename sans .map extension.
	char *s = stripext(filename.c_str());
	// If the vc file exists, compile it in.
	if (Exist(va("%s.vc", s)))
	{
		if(!vcc->CompileMap(s)) {
			showMessageBox(vcc->errmsg);
			return;
		}
	}
	else
	{
		vc->vcerr("vc_CompileMap() - could not compile %s.vc!", s);
	}
}

void vc_ListStructMembers()
{
	string structname = vc->ResolveString();
	std::vector<string> result = vc->ListStructMembers(structname.c_str());
	vc->vcretstr = "";

	for(std::vector<string>::iterator i = result.begin();
		i != result.end();
		i++)
	{
		vc->vcretstr += *i + "|";
	}
}

void vc_CopyArray()
{
	string src = vc->ResolveString();
	string dest = vc->ResolveString();
	bool result = vc->CopyArray(src.c_str(), dest.c_str());
	vc->vcreturn = result;
}

// Overkill (2006-11-20)
void vc_SoundIsPlaying()
{
	int chan = vc->ResolveOperand();
	vc->vcreturn = SoundIsPlaying(chan);
}

// Overkill (2007-05-04)
void vc_GetH()
{
	int c = vc->ResolveOperand();
	int s, v;
	GetHSV(c, vc->vcreturn, s, v);
}

// Overkill (2007-05-04)
void vc_GetS()
{
	int c = vc->ResolveOperand();
	int h, v;
	GetHSV(c, h, vc->vcreturn, v);
}

// Overkill (2007-05-04)
void vc_GetV()
{
	int c = vc->ResolveOperand();
	int h, s;
	GetHSV(c, h, s, vc->vcreturn);
}

// Overkill (2007-05-04)
void vc_HSV()
{
	int h = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int v = vc->ResolveOperand();
	vc->vcreturn = HSVtoColor(h, s, v);
}

// Overkill (2007-05-04)
void vc_HueReplace()
{
	int hue_find = vc->ResolveOperand();
	int hue_tolerance = vc->ResolveOperand();
	int hue_replace = vc->ResolveOperand();
	image *dest = ImageForHandle(vc->ResolveOperand());
	HueReplace(hue_find, hue_tolerance, hue_replace, dest);
}

// Overkill (2007-05-04)
void vc_ColorReplace()
{
	int find = vc->ResolveOperand();
	int replace = vc->ResolveOperand();
	image *dest = ImageForHandle(vc->ResolveOperand());
	ColorReplace(find, replace, dest);
}

void vc_ListBuiltinFunctions()
{
	vc->vcretstr = "";
	for (int i = 0; i < NUM_LIBFUNCS; i++)
	{
		vc->vcretstr += string(libfuncs[i][1]) + "|";
	}
}

void vc_ListBuiltinVariables()
{
	vc->vcretstr = "";
	for (int i = 0; i < NUM_HVARS; i++)
	{
		vc->vcretstr += string(libvars[i][1]) + "|";
	}
}

void vc_ListBuiltinDefines()
{
	vc->vcretstr = "";
	for (int i = 0; i < NUM_HDEFS; i++)
	{
		vc->vcretstr += string(hdefs[i][0]) + "|";
	}
}


// ===================== End VC Standard Function Library =====================

void VCCore::HandleLibFunc()
{
	// Overkill (2006-06-07): Now functions past 255 work.
	// Yay! We'll probably never reach the 65535 mark,
	// so we're safe again.
	word c = currentvc->GrabW();
	switch (c)
	{
		case 0: vc_Exit(); break;
		case 1: vc_Log(); break;
		case 2: vc_NewImage(); break;
		case 3: vc_MakeColor(); break;
		case 4: vc_SetLucent(); break;
		case 5: vc_SetClip(); break;
		case 6: vc_LoadImage(); break;
		case 7: vc_LoadImage0(); break;
		case 8: UpdateControls(); ShowPage(); break;
		case 9: UpdateControls(); break;
		case 10: vc_Blit(); break;
		case 11: vc_TBlit(); break;
		case 12: vc_AdditiveBlit(); break;
		case 13: vc_TAdditiveBlit(); break;
		case 14: vc_SubtractiveBlit(); break;
		case 15: vc_TSubtractiveBlit(); break;
		case 16: vc_WrapBlit(); break;
		case 17: vc_TWrapBlit(); break;
		case 18: vc_ScaleBlit(); break;
		case 19: vc_TScaleBlit(); break;
		case 20: vc_RGB(); break;
		case 21: vc_SetPixel(); break;
		case 22: vc_GetPixel(); break;
		case 23: vc_Line(); break;
		case 24: vc_Rect(); break;
		case 25: vc_RectFill(); break;
		case 26: vc_Circle(); break;
		case 27: vc_CircleFill(); break;
		case 28: vc_GetR(); break;
		case 29: vc_GetG(); break;
		case 30: vc_GetB(); break;
		case 31: vc_RotScale(); break;
		case 32: vc_FreeImage(); break;
		case 33: vc->vcreturn = LoadSong(vc->ResolveString().c_str()); break;
		case 34: PlaySong(vc->ResolveOperand()); break;
		case 35: StopSong(vc->ResolveOperand()); break;
		case 36: PlayMusic(vc->ResolveString().c_str()); break;
		case 37: StopMusic(); break;
		case 38: StopSound(vc->ResolveOperand()); break;
		case 39: FreeSong(vc->ResolveOperand()); break;
		case 40: vc->vcerr("mask"); break;
		case 41: vc_Silhouette(); break;
		case 42: vc_GrabRegion(); break;
		case 43: vc_TGrabRegion(); break;
		case 44: vc_Mosaic(); break;
		case 45: vc_DuplicateImage(); break;
		case 46: vc_Triangle(); break;
		case 47: vc_BlitTile(); break;
		case 48: vc_TBlitTile(); break;
		case 49: vc->vcerr("horzflip"); break;
		case 50: vc->vcerr("vertflip"); break;
		case 51: vc->vcreturn = ((image*)ImageForHandle(vc->ResolveOperand()))->width; break;
		case 52: vc->vcreturn = ((image*)ImageForHandle(vc->ResolveOperand()))->height; break;
		case 53: vc_LoadFont(); break;
		case 54: vc_EnableVariableWidth(); break;
		case 55: vc_PrintString(); break;
		case 56: vc_PrintRight(); break;
		case 57: vc_PrintCenter(); break;
		case 58: vc_TextWidth(); break;
		case 59: vc_FreeFont(); break;
		case 60: vc_Random(); break;
		case 61: vc_len(); break;
		case 62: vc_val(); break;
		case 63: vc_Unpress(); break;
		case 64: DebugBreakpoint("User breakpoint"); break;
		case 65: vc_FileOpen(); break;
		case 66: vc_FileClose(); break;
		case 67: vc_FileWrite(); break;
		case 68: vc_FileWriteln(); break;
		case 69: vc_FileReadln(); break;
		case 70: vc_FileReadToken(); break;
		case 71: vc_FileSeekLine(); break;
		case 72: vc_LoadSound(); break;
		case 73: vc_FreeSound(); break;
		case 74: vc_PlaySound(); break;
		case 75: vc_CallFunction(); break;
		case 76: vc_AssignArray(); break;
		case 77: vc_FileSeekPos(); break;
		case 78: vc_FileCurrentPos(); break;
		case 79: vc_FileWriteByte(); break;
		case 80: vc_FileWriteWord(); break;
		case 81: vc_FileWriteQuad(); break;
		case 82: vc_FileWriteString(); break;
		case 83: vc_FileReadByte(); break;
		case 84: vc_FileReadWord(); break;
		case 85: vc_FileReadQuad(); break;
		case 86: vc_FileReadString(); break;
		case 87: vc_sqrt(); break;
		case 88: vc_pow(); break;
		case 89: vc_SetAppName(); break;
		case 90: vc_SetResolution(); break;
		case 91: vc_BlitLucent(); break;
		case 92: vc_TBlitLucent(); break;
		case 93: vc_Map(); break;
		case 94: vc_strcmp(); break;
		case 95: vc_strdup(); break;
		case 96: vc_HookTimer(); break;
		case 97: vc_HookRetrace(); break;
		case 98: vc_HookEntityRender(); break;
		case 99: vc_HookKey(); break;
		case 100: vc_HookButton(); break;
		case 101: vc_BlitEntityFrame(); break;
		case 102: vc_SetEntitiesPaused(); break;
		case 103: vc_GetObsPixel(); break;
		case 104: vc_GetTile(); break;
		case 105: vc_SetTile(); break;
		case 106: vc_GetZone(); break;
		case 107: vc_SetZone(); break;
		case 108: showMessageBox(vc->ResolveString()); break;
		case 109:
			{	// sin()
				int n = ResolveOperand();
                while (n < 0) n += 360;
                while (n >= 360) n -= 360;
				vcreturn = sintbl[n];
			}
			break;
		case 110:
			{	// cos()
				int n = ResolveOperand();
                while (n < 0) n += 360;
                while (n >= 360) n -= 360;
				vcreturn = costbl[n];
			}
			break;
		case 111:
			{	// tan()
				int n = ResolveOperand();
                while (n < 0) n += 360;
                while (n >= 360) n -= 360;
				vcreturn = tantbl[n];
			}
			break;
		case 112: vc_SuperSecretThingy(); break;
		case 113: vc_BlitWrap(); break;
		case 114: vc_ColorFilter(); break;
		case 115: vc_ImageShell(); break;
		case 116: vc_Malloc(); break;
		case 117: vc_MemFree(); break;
		case 118: vc_MemCopy(); break;
		case 119: vc_asin(); break;
		case 120: vc_acos(); break;
		case 121: vc_atan(); break;
		case 122: vc_AlphaBlit(); break;
		case 123: vc_WindowCreate(); break;
		case 124: vc_WindowGetImage(); break;
		case 125: vc_WindowClose(); break;
		case 126: vc_WindowSetSize(); break;
		case 127: vc_WindowSetResolution(); break;
		case 128: vc_WindowSetPosition(); break;
		case 129: vc_WindowSetTitle(); break;
		case 130: vc_WindowHide(); break;
		case 131: vc_WindowShow(); break;
		case 132: vc_WindowGetXRes(); break;
		case 133: vc_WindowGetYRes(); break;
		case 134: vc_WindowGetWidth(); break;
		case 135: vc_WindowGetHeight(); break;
		case 136: vc_WindowPositionCommand(); break;
		case 137: vc_SetSongPaused(); break;
		case 138: vc_SetSongVolume(); break;
		case 139: vc->vcreturn = GetSongVol(vc->ResolveOperand()); break;
		case 140: vc->vcreturn = GetSongPos(vc->ResolveOperand()); break;
		case 141: vc_SetSongPos(); break;
		case 142: vc_TokenCount(); break;
		case 143: vc_StringToken(); break;
		case 144: vc->vcretstr = vc->ResolveString().upper(); break;
		case 145: vc->vcretstr = vc->ResolveString().lower(); break;
		case 146: vc_LoadFont2(); break;
		case 147: vc_FontHeight(); break;
		case 148: vc_MixColor(); break;
		case 149: vc_CHR(); break;
		case 150: vc_PlayMovie(); break;
		case 151: vc_MovieLoad(); break;
		case 152: vc_MoviePlay(); break;
		case 153: vc_MovieGetImage(); break;
		case 154: vc_MovieRender(); break;
		case 155: vc_MovieClose(); break;
		case 156: vc_MovieGetCurrFrame(); break;
		case 157: vc_MovieGetFramerate(); break;
		case 158: vc_MovieNextFrame(); break;
		case 159: vc_MovieSetFrame(); break;
		case 160: TimedProcessEntities(); Render(); break;
		case 161: vc_GetObs(); break;
		case 162: vc_SetObs(); break;
		case 163: vc_EntitySpawn(); break;
		case 164: vc_SetPlayer(); break;
		case 165: vc_StalkEntity(); break;
		case 166: vc_EntityMove(); break;
		case 167: vc_SetMusicVolume(); break;
		case 168: vc_PlayerMove(); break;
		case 169: vc_ChangeCHR(); break;
		case 170: vc_EntitySetWanderZone(); break;
		case 171: vc_EntitySetWanderRect(); break;
		case 172: vc_EntityStop(); break;
		case 173: vc_EntitySetWanderDelay(); break;
		case 174: arandseed(vc->ResolveOperand()); break;
		case 175: ResetSprites(); break;
		case 176: vc->vcreturn = GetSprite(); break;
		case 177: vc_RenderMap(); break;
		case 178: vc_SetButtonKey(); break;
		case 179: vc_SetButtonJB(); break;
		case 180: vc_FunctionExists(); break;
		case 181: vc_atan2(); break;
		case 182: vc_CopyImageToClipboard(); break;
		case 183: vc_GetImageFromClipboard(); break;
		case 184: vc_SetInt(); break;
		case 185: vc_GetInt(); break;
		case 186: vc_SetString(); break;
		case 187: vc_GetString(); break;
		case 188: vc_SetIntArray(); break;
		case 189: vc_GetIntArray(); break;
		case 190: vc_SetStringArray(); break;
		case 191: vc_GetStringArray(); break;
		case 192: vc_FlipBlit(); break;
		case 193: vc_Connect(); break;
		case 194: vc_GetConnection(); break;
		case 195: vc_SocketConnected(); break;
		case 196: vc_SocketHasData(); break;
		case 197: vc_SocketGetString(); break;
		case 198: vc_SocketSendString(); break;
		case 199: vc_SocketClose(); break;
		case 200: Sleep(vc->ResolveOperand()); break;
		case 201: vc_SetCustomColorFilter(); break;
		case 202: vc_SocketSendInt(); break;
		case 203: vc_SocketGetInt(); break;
		case 204: // sendfloat
		case 205: // getfloat
		case 206: vc_GetUrlText(); break;
		case 207: vc_GetUrlImage(); break;
		case 208: vc_SocketSendFile(); break;
		case 209: vc_SocketGetFile(); break;
		case 210: vc_ListFilePattern(); break;
		case 211: vc_ImageValid(); break;
		case 212: vc_Asc(); break;
		case 213: vc_FileEOF(); break;
		case 214: vc_DictNew(); break;
		case 215: vc_DictFree(); break;
		case 216: vc_DictGetString(); break;
		case 217: vc_DictSetString(); break;
		case 218: vc_DictContainsString(); break;
		case 219: vc_DictSize(); break;
		case 220: vc_DictGetInt(); break;
		case 221: vc_DictSetInt(); break;
		case 222: vc_DictRemove(); break;
		case 223: vc_LoadImage8(); break;
		case 224: vc_AbortMovie(); break; //mbg 12/11/05
		case 225: vc_max(); break; // Overkill: 12/28/05
		case 226: vc_min(); break; // Overkill: 12/28/05
		case 227: vc_abs(); break; // Overkill: 12/28/05
		case 228: vc_sgn(); break; // Overkill: 12/28/05
		case 229: vc_GetTokenPos(); break; // Overkill: 12/28/05
		case 230: vc_TokenLeft(); break; // Overkill: 12/28/05
		case 231: vc_TokenRight(); break; // Overkill: 12/28/05
		case 232: vc_strpos(); break; // Overkill: 12/28/05
		case 233: vc_strovr(); break; // Overkill: 12/28/05
		case 234: vc_WrapText(); break; // Overkill: 12/28/05
		case 235: vc_RectVGrad(); break; // Overkill: 02/04/06, 06/25/06
		case 236: vc_RectHGrad(); break; // Overkill: 02/04/06, 06/25/06
		case 237: vc_cbrt(); break; // Overkill: 06/25/06
		case 238: vc_GetKeyBuffer(); break; // Overkill: 06/29/06
		case 239: vc_FlushKeyBuffer(); break; // Overkill: 06/29/06
		case 240: vc_SetKeyDelay(); break; // Overkill: 06/30/06
		case 241: vc_FileWriteCHR(); break; // Overkill: 05/20/06
		case 242: vc_FileWriteMAP(); break; // Overkill: 05/21/06
		case 243: vc_FileWriteVSP(); break; // Overkill: 05/21/06
		case 244: vc_CompileMap(); break; // Overkill: 05/21/06
		case 245: vc_RectRGrad(); break; // janus: 07/22/06
		case 246: vc_Rect4Grad(); break; // janus: 07/22/06
		case 247: vc_ListStructMembers(); break; // Overkill (2006-08-16)
		case 248: vc_CopyArray(); break; // Overkill (2006-08-16)
		case 249: vc_SoundIsPlaying(); break; // Overkill (2006-11-20)
		case 250: vc_GetH(); break; // Overkill: (2007-05-04)
		case 251: vc_GetS(); break; // Overkill: (2007-05-04)
		case 252: vc_GetV(); break; // Overkill: (2007-05-04)
		case 253: vc_HSV(); break; // Overkill: (2007-05-04)
		case 254: vc_HueReplace(); break; // Overkill: (2007-05-04)
		case 255: vc_ColorReplace(); break; // Overkill: (2007-05-04)
		case 256: vc_fatan(); break;
		case 257: vc_fatan2(); break;
		case 258: vc_fasin(); break;
		case 259: vc_facos(); break;
		case 260: vc_fsin(); break;
		case 261: vc_fcos(); break;
		case 262: vc_ftan(); break;
		case 263: vc_SetCharacterWidth(); break; //mbg (2007-06-18)
		case 264: vc_DictListKeys(); break; // Overkill: (2007-06-20)
		case 265: vc_ListBuiltinFunctions(); break;
		case 266: vc_ListBuiltinVariables(); break;
		case 267: vc_ListBuiltinDefines(); break;
		default:
			vc->vcerr("VC Execution error: Invalid STDLIB index. (%d)", (int) c);
	}
}

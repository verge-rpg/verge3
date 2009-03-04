/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.

//TODO - flipblit doesnt respect lucent properties
//TODO - switch lucent to 255 based for more precision
//TODO - blittiles dont respect lucent properties


/****************************************************************
	xerxes engine
	vid_ddblit.cpp
 ****************************************************************/

#include "xerxes.h"
#include <math.h>

//windows defines this. yuck!
#undef TRANSPARENT


/***************************** data *****************************/

int alpha=0, ialpha=100;

//this holds a bitmask used for the 50% alpha blits which causes the LSB of each color 
//component to be masked out.
const int tmask           = 0xFEFEFEFE;

//used as a template arg
enum LUCENT_TYPE {
	NONE, HALF, ANY
};

//TODO - change for big endian
union Color {
	quad q;
	struct {
		byte r,g,b,a;
	};
};

byte _tbl_getcolor_16bpp[65536][3];
byte _tbl_getcolor_15bpp[65536][3];

void SetupTables()
{
	static bool initialized = false;
	if(initialized) return;
	initialized = true;

	for(int c=0;c<65536;c++) {
		int b = (c & 0x1F) << 3;
		int g = ((c >> 5) & 0x3f) << 2;
		int r = ((c >> 11) & 0x1f) << 3;

		_tbl_getcolor_16bpp[c][0] = r;
		_tbl_getcolor_16bpp[c][1] = g;
		_tbl_getcolor_16bpp[c][2] = b;
	}

	for(int c=0;c<65536;c++) {
		int b = (c & 0x1F) << 3;
		int g = ((c >> 5) & 0x1f) << 3;
		int r = ((c >> 10) & 0x1f) << 3;

		_tbl_getcolor_15bpp[c][0] = r;
		_tbl_getcolor_15bpp[c][1] = g;
		_tbl_getcolor_15bpp[c][2] = b;
	}
}


/***************************** code *****************************/

// Overkill (2007-05-04): Converts HSV into a color
int dd_HSVtoColor(int h, int s, int v)
// Pass: Hue, Saturation, Value
// Credit goes to Zip for original conversion code.
// Returns: RGB color
{
	int r, g, b = 0;
	int ixz;
	int f;

	h = (h + 360) % 360;
	if (s > 255) s = 255;
	else if (s < 0) s = 0;
	if (v > 255) v = 255;
	else if (v < 0) v = 0;

	ixz = h / 60;
	h = (h << 8) / 60;
	f = h - (ixz << 8);
	h = (v * (255 - ((s * (255 - f)) >> 8))) >> 8; // t =
	f = (v * (255 - ((s * f) >> 8))) >> 8; // q =
	s = (v * (255 - s)) >> 8; // p =

	switch (ixz)
	{
		case 0:
			r = v; g = h; b = s;
			break;
		case 1:
			r = f; g = v; b = s;
			break;
		case 2:
			r = s; g = v; b = h;
			break;
		case 3:
			r = s; g = f; b = v;
			break;
		case 4:
			r = h; g = s; b = v;
			break;
		case 5:
			r = v; g = s; b = f;
			break;
	}
	return MakeColor(r, g, b);
}

// Overkill (2007-05-04): Converts color into RGB, then RGB->HSV 
void dd_GetHSV(int color, int &h, int &s, int &v)
// Pass: Color, Hue reference, Saturation reference, Value reference
// Credit goes to Zip for original conversion code.
{
	int r, g, b;
	int maximum;
	int delta;

	GetColor(color, r, g, b);
	// Figure out the maximum, the minimum
	// and the change between them.
	maximum = r;
	delta = r;
	if (g > maximum) maximum = g;
	else if (g < delta) delta = g;
	if (b > maximum) maximum = b;
	else if (b < delta) delta = b;
	delta = maximum - delta;

	// Value
	v = maximum;
	// Saturation
	if (maximum == 0) s = 0;
	else s = (delta * 255) / maximum;

	// Grey
	if (delta == 0) h = 0;	
	// (300) magenta -> (0) red -> (60) yellow
	else if (r == maximum) h = (360 + ((60 * (g - b)) / delta)) % 360;
	// (60) yellow -> (120) green -> (180) cyan
	else if (g == maximum) h = (120) + ((60 * (b - r)) / delta);
	// (180) cyan -> (240) blue -> (300) magenta
	else h = (240) + ((60 * (r - g)) / delta);
}

// Overkill (2007-05-04): Finds a hue range in an image,
// and replaces it with a different shade.
void dd_HueReplace(int hue_find, int hue_tolerance, int hue_replace, image *img)
// Pass: hue to find, range of tolerance, replacement hue, destination image
// Credit goes to Zip for original replace code.
{
	int pixel;
	int h, s, v;
	for (int x = img->cx1; x <= img->cx2; x++)
	{
		for (int y = img->cy1; y <= img->cy2; y++)
		{
			pixel = ReadPixel(x, y, img);
			if (pixel != transColor)
			{
				// Find out HSV info
				GetHSV(pixel, h, s, v);
				// Calculate tolerance
				//if (hue_tolerance != 360)
				//{
				pixel = (h - hue_find) % (360 - hue_tolerance);
				//}
				//else
				//{
				//	pixel = 360 + (h - hue_find) % 360;
				//}
				// If this pixel matches the old shade
				if (pixel <= hue_tolerance && pixel >= (0 - hue_tolerance))
				{
					// Set the pixel to the new shade
					PutPixel(x, y, HSVtoColor(hue_replace + pixel, s, v), img);
				}
			}
		}
	}
}

// Overkill (2007-05-04): Finds an exact color in an image,
// and replaces it with a new color.
void dd_ColorReplace(int color_find, int color_replace, image *img)
{
	for (int x = img->cx1; x <= img->cx2; x++)
	{
		for (int y = img->cy1; y <= img->cy2; y++)
		{
			if (ReadPixel(x, y, img) == color_find)
			{
				PutPixel(x, y, color_replace, img);
			}
		}
	}
}  









// Overkill 2006-02-04
void dd_RectVGrad(int x, int y, int x2, int y2, int color, int color2, image *dest)
{
	int r, g, b;
	int color_r, color_g, color_b;
	int color_r2, color_g2, color_b2;
	int i = 0;

	GetColor(color, color_r, color_g, color_b);
	GetColor(color2, color_r2, color_g2, color_b2);
	
	if (y2 < y) SWAP(y,y2);

	for(i = 0; i <= y2 - y; i++)
	{
		r = ((i * (color_r2 - color_r) / (y2 - y)) + color_r);
		g = ((i * (color_g2 - color_g) / (y2 - y)) + color_g);
		b = ((i * (color_b2 - color_b) / (y2 - y)) + color_b);
		HLine(x, y + i, x2, MakeColor(r, g, b), dest);
	}
}

// Overkill 2006-02-04
void dd_RectHGrad(int x, int y, int x2, int y2, int color, int color2, image *dest)
{
	int r, g, b;
	int color_r, color_g, color_b;
	int color_r2, color_g2, color_b2;
	int i = 0;

	GetColor(color, color_r, color_g, color_b);
	GetColor(color2, color_r2, color_g2, color_b2);
	
	if (x2 < x) SWAP(x,x2);

	for(i = 0; i <= x2 - x; i++)
	{
		r = ((i * (color_r2 - color_r) / (x2 - x)) + color_r);
		g = ((i * (color_g2 - color_g) / (x2 - x)) + color_g);
		b = ((i * (color_b2 - color_b) / (x2 - x)) + color_b);
		VLine(x + i, y, y2, MakeColor(r, g, b), dest);
	}
}

// janus 2006-07-22

// radial gradient: color1 is edge color, color2 is center color
void dd_RectRGrad(int x1, int y1, int x2, int y2, int color1, int color2, image *dest) 
{		
  int r1, r2, g1, g2, b1, b2;
  GetColor(color1, r1, g1, b1);
  GetColor(color2, r2, g2, b2);
	if (x2 < x1) SWAP(x1,x2);
	if (y2 < y1) SWAP(y1,y2);
  int w = (x2 - x1) + 1;
  int h = (y2 - y1) + 1;
  if ((w == 0) || (h == 0)) return;
  // lookup table to eliminate the sqrt-per-pixel
  static bool tableInitialized = false;
  static struct pythagorasTable { byte v[256]; } pythagorasLookup[256];
  if (!tableInitialized) {
    // we initialize this table on the fly the first time this function is invoked
    // the output of the formula is not in the value range we require for 8bpp so we scale it
    // if you want a nonscaled gradient, set this constant to 1.0f
    const float pythagorasConstant = 0.707106781186547f;
    for (unsigned a = 0; a < 256; a++) {
      for (unsigned b = 0; b < 256; b++) {
        float value = sqrt((float)(a * a) + (b * b)) * pythagorasConstant;
        if (value <= 0)
          pythagorasLookup[a].v[b] = 0;
        else if (value >= 255)
          pythagorasLookup[a].v[b] = 255;
        else
          pythagorasLookup[a].v[b] = unsigned(value) & 0xFF;
      }
    }
    tableInitialized = true;
  }
  // color lookup table to reduce per-pixel blending computations
  int colorTable[256];
  for (unsigned i = 0; i < 256; i++) {
    unsigned r = r2 + ((r1 - r2) * i / 255);
    unsigned g = g2 + ((g1 - g2) * i / 255);
    unsigned b = b2 + ((b1 - b2) * i / 255);
    colorTable[i] = MakeColor(r, g, b);
  }
  // x and y weight tables which unfortunately must be dynamically allocated due to the fact that their size depends on the size of the rect
  byte* xTable = new byte[w];
  byte* yTable = new byte[h];
  {
    // the float->int conversions in here are kind of expensive, it might be alright to do this in integer.
    // haven't tested the math enough to be sure though, so float it is ('cause i'm lazy)
    float xMul = 255.0f / (w / 2), yMul = 255.0f / (h / 2);
    int xOffset = (w / 2), yOffset = (h / 2);
    // these tables convert x/y coordinates into 0-255 weights which can be used to index the sqrt table
    for (int x = 0; x < w; x++) {
      xTable[x] = unsigned((float)abs(x - xOffset) * xMul) & 0xFF;
    }
    for (int y = 0; y < h; y++) {
      yTable[y] = unsigned((float)abs(y - yOffset) * yMul) & 0xFF;
    }
  }
  for (unsigned y = 0; y < h; y++) {
    for (unsigned x = 0; x < w; x++) {
      unsigned w = pythagorasLookup[yTable[y]].v[xTable[x]];
      PutPixel(x+x1, y+y1, colorTable[w], dest);
    }
  }
  // cleanup locals
  delete[] xTable;
  delete[] yTable;
}

// 4-corner gradient: color1-4 are edges top left, top right, bottom left, bottom right
void dd_Rect4Grad(int x1, int y1, int x2, int y2, int color1, int color2, int color3, int color4, image *dest) 
{		
  int cr[4], cg[4], cb[4];
  GetColor(color1, cr[0], cg[0], cb[0]);
  GetColor(color2, cr[1], cg[1], cb[1]);
  GetColor(color3, cr[2], cg[2], cb[2]);
  GetColor(color4, cr[3], cg[3], cb[3]);
	if (x2 < x1) SWAP(x1,x2);
	if (y2 < y1) SWAP(y1,y2);
  int w = (x2 - x1) + 1;
  int h = (y2 - y1) + 1;
  if ((w == 0) || (h == 0)) return;
  unsigned wDiv = w-1;
  unsigned hDiv = h-1;
  for (unsigned y = 0; y < h; y++) {
    for (unsigned x = 0; x < w; x++) {
      // calculate x and y weights
      unsigned aX = x * 255 / wDiv;
      unsigned aY = y * 255 / hDiv;
      // calculate per-color weights
      unsigned a[4]; // tl tr bl br
      a[0] = (aX^0xFF) * (aY^0xFF) / 255;
      a[1] = aX * (aY^0xFF) / 255;
      a[2] = (aX^0xFF) * aY / 255;
      a[3] = 255 - (a[0] + a[1] + a[2]);
      // blend
      unsigned r = ((cr[0] * a[0]) + (cr[1] * a[1]) + (cr[2] * a[2]) + (cr[3] * a[3])) / 255;
      unsigned g = ((cg[0] * a[0]) + (cg[1] * a[1]) + (cg[2] * a[2]) + (cg[3] * a[3])) / 255;
      unsigned b = ((cb[0] * a[0]) + (cb[1] * a[1]) + (cb[2] * a[2]) + (cb[3] * a[3])) / 255;
      PutPixel(x+x1, y+y1, MakeColor(r, g, b), dest);
    }
  }
}




void GrabRegion(int sx1, int sy1, int sx2, int sy2, int dx, int dy, image *s, image *d)
{
	int dcx1, dcy1, dcx2, dcy2;
	d->GetClip(dcx1, dcy1, dcx2, dcy2);

	if (sx1>sx2) SWAP(sx1, sx2);
	if (sy1>sy2) SWAP(sy1, sy2);
	int grabwidth = sx2 - sx1;
	int grabheight = sy2 - sy1;
	d->SetClip(dx, dy, dx+grabwidth, dy+grabheight);
	Blit(dx-sx1, dy-sy1, s, d);

	d->SetClip(dcx1, dcy1, dcx2, dcy2);
}

void Clear(int color, image *dest)
{
	DrawRect(0,0,dest->width-1,dest->height-1,color,dest);
}


void dd32_AlphaBlit(int x, int y, image *src, image *alpha, image *dest)
{
	int *s=(int *)src->data,
		*d=(int *)dest->data,
		*a=(int *)alpha->data;
	int spitch=src->pitch,
		dpitch=dest->pitch,
		apitch=alpha->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	if (src->width != alpha->width || src->height != alpha->height)
		err("AlphaBlit: Alpha image MUST have same dimensions as source image");

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		a +=(cx1-x);
		xlen-=(cx1-x);
		x = cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		a +=(cy1-y)*apitch;
		ylen-=(cy1-y);
		y = cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			quad r1, g1, b1;
			quad r2, g2, b2;
			quad dp;

			int c = s[x];
			int pa = a[x]&0xff;
			if (!pa) continue;
			if (pa == 255)
			{
				d[x] = s[x];
				continue;
			}
			int ipa = 255-pa;
			dp = d[x];

			r1 = (c >> 16) & 0xff,
			g1 = (c >> 8) & 0xff,
			b1 = (c & 0xff);

			r2 = (dp >> 16) & 0xff,
			g2 = (dp >> 8) & 0xff,
			b2 = (dp & 0xff);

			d[x] = ((((r1 * pa) + (r2 * ipa)) / 255) << 16) |
		           ((((g1 * pa) + (g2 * ipa)) / 255) << 8) |
				   ((((b1 * pa) + (b2 * ipa)) / 255));
		}
		s+=spitch;
		d+=dpitch;
		a+=apitch;
	}
}


image *Image32bppFrom8bpp(byte *src, int width, int height, byte *pal)
{
	quad palconv[256], *p;
	image *b;
	int i;

	b = new image(width, height);
	p = (quad *) b->data;
	for (i=0; i<256; i++)
		palconv[i] = MakeColor(pal[i*3], pal[(i*3)+1], pal[(i*3)+2]);
	for (i=0; i<width*height; i++)
		p[i] = palconv[src[i]];
	return b;
}

template<int BPP>
image *Image32bppFromWordType(byte *src, int width, int height)
{
	quad *dest;
	image *img;
	int i;
	int r, g, b;
	word* wsrc = (word*)src;

	img = new image(width, height, 32);
	dest = (quad *) img->data;
	for (i=0; i<width*height; i++)
	{
		T_GetColor<BPP>(*wsrc++,r,g,b);
		dest[i] = T_MakeColor<32>(r,g,b);
	}
	return img;
}

image *Image32bppFrom24bpp(byte *src, int width, int height)
{
	quad *dest;
	image *img;
	int i;
	byte r, g, b;

	img = new image(width, height);
	dest = (quad *) img->data;
	for (i=0; i<width*height; i++)
	{
		r = *src++;
		g = *src++;
		b = *src++;
		dest[i] = MakeColor(r,g,b);
	}
	return img;
}

image *Image32bppFrom32bpp(byte *src, int width, int height)
{
	quad *dest;
	image *img;

	img = new image(width, height);
	img->alpha = true;
	dest = (quad *)img->data;
	memcpy(dest,src,width*height*4);
	return img;
}

template<int BPP>
image *ImageWordTypeFrom8bpp(byte *src, int width, int height, byte *pal)
{
	word palconv[256], *p;
	image *b;
	int i;

	b = new image(width, height);
	p = (word *) b->data;
	for (i=0; i<256; i++)
		palconv[i] = T_MakeColor<BPP>(pal[i*3], pal[(i*3)+1], pal[(i*3)+2]);
	for (i=0; i<width*height; i++)
		p[i] = palconv[src[i]];
	return b;
}

image *Image16bppFrom16bpp(byte *src, int width, int height)
{
	word *dest;
	image *img;

	img = new image(width, height);
	img->alpha = true;
	dest = (word *)img->data;
	memcpy(dest,src,width*height*2);
	return img;
}

image *Image16bppFrom24bpp(byte *src, int width, int height)
{
	word *dest;
	image *img;
	int i;
	byte r, g, b;

	img = new image(width, height);
	dest = (word *) img->data;
	for (i=0; i<width*height; i++)
	{
		r = *src++;
		g = *src++;
		b = *src++;
		dest[i] = MakeColor(r,g,b);
	}
	return img;
}

template<int BPP>
image *ImageWordTypeFrom32bpp(byte *src, int width, int height)
{
	word *dest;
	image *img;
	int i;
	byte r, g, b;

	img = new image(width, height);
	img->alloc_alpha();
	dest = (word *) img->data;
	byte* adest = img->alphaChannel;
	for (i=0; i<width*height; i++)
	{
		b = *src++;
		g = *src++;
		r = *src++;
		adest[i] = *src++;
		dest[i] = T_MakeColor<BPP>(r,g,b);
	}
	return img;
}

/*********************** blitter managment **********************/

image* ImageAdapt(image* src, int srcbpp, int dstbpp) {
	//TODO - this needs to be a lot better (handle pitch, for one thing)
	switch(dstbpp) {
		case 32:
			switch(srcbpp) {
				case 15: return Image32bppFromWordType<15>((byte*)src->data,src->width,src->height);
				case 16: return Image32bppFromWordType<16>((byte*)src->data,src->width,src->height);
			}
	}
	err("Unsupported!");
	return 0;
}


#define BPP 15
#define PT word
namespace Blitter15 {
#include "blitter_include.h"
}
#undef BPP
#undef PT

#define BPP 16
#define PT word
namespace Blitter16 {
#include "blitter_include.h"
}
#undef BPP
#undef PT

#define BPP 32
#define PT quad
namespace Blitter32 {
#include "blitter_include.h"
}
#undef BPP
#undef PT


int SetLucent(int percent)
{
	if (percent < 0) percent = 0;
	if (percent > 100) percent = 100;
	int oldalpha = alpha;
	alpha = percent;
	ialpha = 100 - alpha;

	if(alpha == oldalpha) return oldalpha;

	if(vid_bpp == 32)
		Blitter32::SetForLucent(percent);
	else if(vid_bpp==16)
		Blitter16::SetForLucent(percent);
	else Blitter15::SetForLucent(percent);

	return oldalpha;
}


void dd_RegisterBlitters()
{
	SetupTables();

	if(vid_bpp == 32)
		Blitter32::SetForLucent(alpha );
	else if(vid_bpp==16)
		Blitter16::SetForLucent(alpha );
	else Blitter15::SetForLucent(alpha );

	if(vid_bpp == 32) {
		ImageFrom8bpp	= Image32bppFrom8bpp;
		ImageFrom15bpp	= Image32bppFromWordType<15>;
		ImageFrom16bpp	= Image32bppFromWordType<16>;
		ImageFrom24bpp	= Image32bppFrom24bpp;
		ImageFrom32bpp	= Image32bppFrom32bpp;
	} else if(vid_bpp == 16) {
		ImageFrom8bpp	= ImageWordTypeFrom8bpp<16>;
		ImageFrom16bpp	= Image16bppFrom16bpp;
		ImageFrom24bpp	= Image16bppFrom24bpp;
		ImageFrom32bpp	= ImageWordTypeFrom32bpp<16>;
	} else {
		ImageFrom8bpp	= ImageWordTypeFrom8bpp<15>;
		ImageFrom16bpp	= Image16bppFrom16bpp;
		ImageFrom24bpp	= Image16bppFrom24bpp;
		ImageFrom32bpp	= ImageWordTypeFrom32bpp<15>;
	}
	
	RectVGrad = dd_RectVGrad;
	RectHGrad = dd_RectHGrad;
	RectRGrad = dd_RectRGrad;
	Rect4Grad = dd_Rect4Grad;
	HSVtoColor = dd_HSVtoColor;
	GetHSV = dd_GetHSV;
	HueReplace = dd_HueReplace;
	ColorReplace = dd_ColorReplace;
	#ifdef ENABLE_2XSAI
	void Init_2xSAI();
	Init_2xSAI();
	#endif
	transColor = MakeColor(255, 0, 255);
	
	//force the alpha-parameterized blitters to setup
	alpha = -1;
	SetLucent(0);
}



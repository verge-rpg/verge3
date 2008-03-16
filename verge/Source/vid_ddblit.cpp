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
	vid_ddblit.cpp
 ****************************************************************/

#include "xerxes.h"
#include <math.h>

#ifndef NOTIMELESS
#include "timeless.h"
int skewlines[100] = { 0 };
#endif

/***************************** data *****************************/

int alpha=0, ialpha=100;
int tmask;

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

void dd_Line(int x, int y, int xe, int ye, int color, image *dest)
{
	if (x == xe) { VLine(x,y,ye,color,dest); return; }
	if (y == ye) { HLine(x,y,xe,color,dest); return; }

	// Bresenham's Line Drawing Algorithm

	int cx, cy, dx, dy;
	int xaccum, yaccum, xincre, yincre, xreset, yreset, xchange, ychange;
	int finished = 0;

	dx = abs(xe-x);
	dy = abs(ye-y);
	cx = x; cy = y;
	
	if(xe == x) xchange = 0; else if(xe < x) xchange = 0-1; else xchange = 1;
	if(ye == y) ychange = 0; else if(ye < y) ychange = 0-1; else ychange = 1;
	if(dx > dy) {
		xaccum = 0; xreset = 0; xincre = 0;
		yaccum = dy*2 - dx;
		yincre = dy*2;
		yreset = (dy-dx)*2;
	} else {
		yaccum = 0; yreset = 0; yincre = 0;
		xaccum = dx*2 - dy;
		xincre = dx*2;
		xreset = (dx-dy)*2;
	}
	
	while(!finished) {
		if(xaccum < 0) {
			xaccum += xincre;
		} else {
			cx += xchange;
			xaccum += xreset;
		}
		
		if(yaccum < 0) {
			yaccum += yincre;
		} else {
			cy += ychange;
			yaccum += yreset;
		}
		
		PutPixel(cx,cy,color,dest);
		
		if(xreset == 0 && cx == xe) finished = 1;
		if(yreset == 0 && cy == ye) finished = 1;
	}
	
	/*
	int dx = xe - x, dy = ye - y,
		xg = sgn(dx), yg = sgn(dy),
		i = 0;
	float slope = 0;

	if (abs(dx) >= abs(dy))
	{
		slope = (float) dy / (float) dx;
		for (i=0; i!=dx; i+=xg)
			PutPixel(x+i, y+(int)(slope*i), color, dest);
	}
	else
	{
		slope = (float) dx / (float) dy;
		for (i=0; i!=dy; i+=yg)
			PutPixel(x+(int)(slope*i), y+i, color, dest);
	}
	PutPixel(xe, ye, color, dest);
	*/
}

void dd_Triangle(int x1, int y1, int x2, int y2, int x3, int y3, int color, image *dest)
{
	int xstep,xstep2;
	int xval,xval2;
	int yon;
	int swaptemp;

	if (y1 > y3)
	{
		swaptemp=x1; x1=x3; x3=swaptemp;
		swaptemp=y1; y1=y3; y3=swaptemp;
	}
	if (y2 > y3)
	{
		swaptemp=x2; x2=x3; x3=swaptemp;
		swaptemp=y2; y2=y3; y3=swaptemp;
	}
	if (y1 > y2)
	{
		swaptemp=x1; x1=x2; x2=swaptemp;
		swaptemp=y1; y1=y2; y2=swaptemp;
	}

	xstep2=((x3-x1) << 16) / ((y3-y1) ? (y3-y1) : 1);
	xval2=x1 << 16;

	if (y1 != y2)
	{
		xstep = ((x2-x1) << 16) / (y2-y1);
		xval = x1 << 16;
		for (yon=y1; yon < y2; yon++)
		{
			if (yon >= dest->cy1 && yon <= dest->cy2)
				HLine(xval>>16, yon, xval2>>16, color, dest);
			xval+=xstep;
			xval2+=xstep2;
		}
	}
	if (y2 != y3)
	{
		xstep = ((x3-x2) << 16) / ((y3-y2) ? (y3-y2) : 1);
		xval = x2 << 16;
		for (yon=y2;yon < y3; yon++)
		{
			if (yon >= dest->cy1 && yon <= dest->cy2)
				HLine(xval>>16, yon, xval2>>16, color, dest);
			xval+=xstep;
			xval2+=xstep2;
		}
	}
}


void dd_ColorFilter(int filter, image *img)
{
	int rr, gg, bb, z, c;
	if (!filter) return;

	for (int y=img->cy1; y<=img->cy2; y++)
	{
		for (int x=img->cx1; x<=img->cx2; x++)
		{
			if (ReadPixel(x,y,img) == transColor) continue; // Overkill (2006-07-27): Ignore trans pixels
			GetColor(ReadPixel(x,y,img), rr, gg, bb);
			switch (filter)
			{
				case 0:
				case 1: z = (rr+gg+bb)/3; c = MakeColor(z,z,z); break;
				case 2: z = 255-((rr+gg+bb)/3); c = MakeColor(z,z,z); break;
				case 3: c = MakeColor(255-rr, 255-gg, 255-bb); break;
				case 4: z = (rr+gg+bb)/3; c = MakeColor(z, 0, 0); break;
				case 5: z = (rr+gg+bb)/3; c = MakeColor(0, z, 0); break;
				case 6: z = (rr+gg+bb)/3; c = MakeColor(0, 0, z); break;
				case 7: z = (rr+gg+bb)/3; c = MakeColor(cf_r1+(cf_rr*z/255), cf_g1+(cf_gr*z/255), cf_b1+(cf_br*z/255)); break;
			}
			PutPixel(x, y, c, img);
		}
	}
}



void dd_Box(int x, int y, int x2, int y2, int color, image *dest)
{
	if (x2<x) SWAP(x,x2);
	if (y2<y) SWAP(y,y2);
	HLine(x, y, x2, color, dest);
	HLine(x, y2, x2, color, dest);
	VLine(x, y+1, y2-1, color, dest);
	VLine(x2, y+1, y2-1, color, dest);
}


void dd_Rect(int x, int y, int x2, int y2, int color, image *dest)
{
	if (y2<y) SWAP(y,y2);
	for (; y<=y2; y++)
		HLine(x, y, x2, color, dest);
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

void dd_Oval(int x, int y, int xe, int ye, int color, int Fill, image *dest)
{
	int m=xe-x, n=ye-y,
		//mi=m/2,  //mbg 9/5/05 this variable is not being used. why? probably unnecessary
		ni=n/2,
		dx=4*m*m,
		dy=4*n*n,
		r=m*n*n,
		rx=2*r,
		ry=0,
		xx=m,
		lasty=9999;

	y+=ni;
	if (Fill)
		HLine(x, y, x+xx-1, color, dest);
	else {
		PutPixel(x, y, color, dest);
		PutPixel(x+xx, y, color, dest);
	}

	xe=x, ye=y;
	if (ni+ni==n)
	{
		ry=-2*m*m;
	}
	else
	{
		ry=2*m*m;
		ye++;

		if (Fill)
			HLine(xe, ye, xe+xx-1, color, dest);
		else {
			PutPixel(xe, ye, color, dest);
			PutPixel(xe+xx, ye, color, dest);
		}
	}

	while (xx>0)
	{
		if (r<=0)
		{
			xx-=2;
			x++, xe++;
			rx-=dy;
			r+=rx;
		}
		if (r>0)
		{
			y--, ye++;
			ry+=dx;
			r-=ry;
		}

		if (Fill && y != lasty)
		{
			HLine(x, y, x+xx-1, color, dest);
			HLine(xe, ye, xe+xx-1, color, dest);
		}
		else {
			PutPixel(x, y, color, dest);
			PutPixel(x+xx, y, color, dest);
			PutPixel(xe, ye, color, dest);
			PutPixel(xe+xx, ye, color, dest);
		}
		lasty = y;
	}
}


void dd_Sphere(int x, int y, int xradius, int yradius, int color, image *dest)
{
	dd_Oval(x-xradius, y-yradius, x+xradius-1, y+yradius-1, color, 1, dest);
}


void dd_Circle(int x, int y, int xradius, int yradius, int color, image *dest)
{
	dd_Oval(x-xradius, y-yradius, x+xradius-1, y+yradius-1, color, 0, dest);
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

/************** 2xSAI **************/
//mbg 9/10/05 TODO
//if anyone wants to use 2xsai, we may have to add stubs in the
//non-defined section
#ifdef ENABLE_2XSAI

static quad ColorMask;
static quad LowPixelMask;
static quad QColorMask;
static quad QLowPixelMask;
static quad RedBlueMask;
static quad GreenMask;
static int PixelsPerMask;

static byte *src_line[4];
static byte *dst_line[2];

void Init_2xSAI(int bpp)
{
	int min_r=0, min_g=0, min_b=0;

	/* Get lowest color bit */
	for (int i=0; i<255; i++)
	{
		if (!min_r) min_r = MakeColor(i, 0, 0);
		if (!min_g) min_g = MakeColor(0, i, 0);
		if (!min_b)	min_b = MakeColor(0, 0, i);
	}
	ColorMask = (MakeColor(255,0,0)-min_r) | (MakeColor(0,255,0)-min_g) | (MakeColor(0,0,255)-min_b);
	LowPixelMask = min_r | min_g | min_b;
	QColorMask = (MakeColor(255,0,0)-3*min_r) | (MakeColor(0,255,0)-3*min_g) | (MakeColor(0,0,255)-3*min_b);
	QLowPixelMask = (min_r*3) | (min_g*3) | (min_b*3);
	RedBlueMask = MakeColor(255, 0, 255);
	GreenMask = MakeColor(0, 255, 0);

	PixelsPerMask = (bpp <= 16) ? 2 : 1;

	if (PixelsPerMask == 2)
	{
		ColorMask |= (ColorMask << 16);
		QColorMask |= (QColorMask << 16);
		LowPixelMask |= (LowPixelMask << 16);
		QLowPixelMask |= (QLowPixelMask << 16);
	}
}

#define GET_RESULT(A, B, C, D) ((A != C || A != D) - (B != C || B != D))
#define INTERPOLATE(A, B) (((A & ColorMask) >> 1) + ((B & ColorMask) >> 1) + (A & B & LowPixelMask))
#define Q_INTERPOLATE(A, B, C, D) ((A & QColorMask) >> 2) + ((B & QColorMask) >> 2) + ((C & QColorMask) >> 2) + ((D & QColorMask) >> 2) \
	+ ((((A & QLowPixelMask) + (B & QLowPixelMask) + (C & QLowPixelMask) + (D & QLowPixelMask)) >> 2) & QLowPixelMask)


void run2xSAI_engine(byte *src, quad src_pitch, image *dest, quad width, quad height);
void run2xSAI(image *src, image *dest)
{
	/* are both images non-null? */
	if (!src || !dest)
		return;

	/* image must be at least 4x4 */
	if (src->width < 4 || src->height < 4)
		return;

	/* make sure the destination image is at least 2x as big as the source. */
	if (dest->width < src->width*2 || dest->height < src->height*2)
		return;

	run2xSAI_engine((byte*)src->data, src->pitch*vid_bytesperpixel, dest, src->width, src->height);
}

void run2xSAI_engine(byte *src, quad src_pitch, image *dest, quad width, quad height)
{
	quad x, y;
	unsigned long color[16];

	/* Point to the first 3 lines. */
	src_line[0] = src;
	src_line[1] = src;
	src_line[2] = src + src_pitch;
	src_line[3] = src + src_pitch * 2;

	/* Can we write the results directly? */
	dst_line[0] = &((byte*)dest->data)[0];
	dst_line[1] = &((byte*)dest->data)[dest->pitch*vid_bytesperpixel];

	x = 0, y = 0;

	if (PixelsPerMask == 2)
	{
		word *sbp;
		sbp = (word*) src_line[0];
		color[0] = *sbp;       color[1] = color[0];   color[2] = color[0];    color[3] = color[0];
		color[4] = color[0];   color[5] = color[0];   color[6] = *(sbp + 1);  color[7] = *(sbp + 2);
		sbp = (word*)src_line[2];
		color[8] = *sbp;     color[9] = color[8];     color[10] = *(sbp + 1); color[11] = *(sbp + 2);
		sbp = (word*)src_line[3];
		color[12] = *sbp;    color[13] = color[12];   color[14] = *(sbp + 1); color[15] = *(sbp + 2);
	}
	else
	{
		unsigned long *lbp;
		lbp = (unsigned long*) src_line[0];
		color[0] = *lbp;       color[1] = color[0];   color[2] = color[0];    color[3] = color[0];
		color[4] = color[0];   color[5] = color[0];   color[6] = *(lbp + 1);  color[7] = *(lbp + 2);
		lbp = (unsigned long*) src_line[2];
		color[8] = *lbp;     color[9] = color[8];     color[10] = *(lbp + 1); color[11] = *(lbp + 2);
		lbp = (unsigned long*) src_line[3];
		color[12] = *lbp;    color[13] = color[12];   color[14] = *(lbp + 1); color[15] = *(lbp + 2);
	}

	for (y = 0; y < height; y++)
	{
		/* Todo: x = width - 2, x = width - 1 */
		for (x = 0; x < width; x++) {
			unsigned long product1a, product1b, product2a, product2b;

//---------------------------------------  B0 B1 B2 B3    0  1  2  3
//                                         4  5* 6  S2 -> 4  5* 6  7
//                                         1  2  3  S1    8  9 10 11
//                                         A0 A1 A2 A3   12 13 14 15
//--------------------------------------
			if (color[9] == color[6] && color[5] != color[10])
			{
				product2b = color[9];
				product1b = product2b;
			}
			else if (color[5] == color[10] && color[9] != color[6])
			{
				product2b = color[5];
				product1b = product2b;
			}
			else if (color[5] == color[10] && color[9] == color[6])
			{
				int r = 0;

				r += GET_RESULT(color[6], color[5], color[8], color[13]);
				r += GET_RESULT(color[6], color[5], color[4], color[1]);
				r += GET_RESULT(color[6], color[5], color[14], color[11]);
				r += GET_RESULT(color[6], color[5], color[2], color[7]);

				if (r > 0)
					product1b = color[6];
				else if (r < 0)
					product1b = color[5];
				else
					product1b = INTERPOLATE(color[5], color[6]);

				product2b = product1b;

			}
			else
			{
				if (color[6] == color[10] && color[10] == color[13] && color[9] != color[14] && color[10] != color[12])
					product2b = Q_INTERPOLATE(color[10], color[10], color[10], color[9]);
				else if (color[5] == color[9] && color[9] == color[14] && color[13] != color[10] && color[9] != color[15])
					product2b = Q_INTERPOLATE(color[9], color[9], color[9], color[10]);
				else
					product2b = INTERPOLATE(color[9], color[10]);

				if (color[6] == color[10] && color[6] == color[1] && color[5] != color[2] && color[6] != color[0])
					product1b = Q_INTERPOLATE(color[6], color[6], color[6], color[5]);
				else if (color[5] == color[9] && color[5] == color[2] && color[1] != color[6] && color[5] != color[3])
					product1b = Q_INTERPOLATE(color[6], color[5], color[5], color[5]);
				else
					product1b = INTERPOLATE(color[5], color[6]);
			}

			if (color[5] == color[10] && color[9] != color[6] && color[4] == color[5] && color[5] != color[14])
				product2a = INTERPOLATE(color[9], color[5]);
			else if (color[5] == color[8] && color[6] == color[5] && color[4] != color[9] && color[5] != color[12])
				product2a = INTERPOLATE(color[9], color[5]);
			else
				product2a = color[9];

			if (color[9] == color[6] && color[5] != color[10] && color[8] == color[9] && color[9] != color[2])
				product1a = INTERPOLATE(color[9], color[5]);
			else if (color[4] == color[9] && color[10] == color[9] && color[8] != color[5] && color[9] != color[0])
				product1a = INTERPOLATE(color[9], color[5]);
			else
				product1a = color[5];

			if (PixelsPerMask == 2)
			{
				*((unsigned long *) (&dst_line[0][x * 4])) = product1a | (product1b << 16);
				*((unsigned long *) (&dst_line[1][x * 4])) = product2a | (product2b << 16);
			}
			else
			{
				*((unsigned long *) (&dst_line[0][x * 8])) = product1a;
				*((unsigned long *) (&dst_line[0][x * 8 + 4])) = product1b;
				*((unsigned long *) (&dst_line[1][x * 8])) = product2a;
				*((unsigned long *) (&dst_line[1][x * 8 + 4])) = product2b;
			}

			/* Move color matrix forward */
			color[0] = color[1]; color[4] = color[5]; color[8] = color[9];   color[12] = color[13];
			color[1] = color[2]; color[5] = color[6]; color[9] = color[10];  color[13] = color[14];
			color[2] = color[3]; color[6] = color[7]; color[10] = color[11]; color[14] = color[15];

			if (x < width - 3)
			{
				x += 3;
				if (PixelsPerMask == 2)
				{
					color[3] = *(((unsigned short*)src_line[0]) + x);
					color[7] = *(((unsigned short*)src_line[1]) + x);
					color[11] = *(((unsigned short*)src_line[2]) + x);
					color[15] = *(((unsigned short*)src_line[3]) + x);
				}
				else
				{
					color[3] = *(((unsigned long*)src_line[0]) + x);
					color[7] = *(((unsigned long*)src_line[1]) + x);
					color[11] = *(((unsigned long*)src_line[2]) + x);
					color[15] = *(((unsigned long*)src_line[3]) + x);
				}
				x -= 3;
			}
		}

		/* We're done with one line, so we shift the source lines up */
		src_line[0] = src_line[1];
		src_line[1] = src_line[2];
		src_line[2] = src_line[3];

		/* Read next line */
		if (y + 3 >= height)
			src_line[3] = src_line[2];
		else
			src_line[3] = src_line[2] + src_pitch;

		/* Then shift the color matrix up */
		if (PixelsPerMask == 2)
		{
			unsigned short *sbp;
			sbp = (unsigned short*)src_line[0];
			color[0] = *sbp;     color[1] = color[0];    color[2] = *(sbp + 1);  color[3] = *(sbp + 2);
			sbp = (unsigned short*)src_line[1];
			color[4] = *sbp;     color[5] = color[4];    color[6] = *(sbp + 1);  color[7] = *(sbp + 2);
			sbp = (unsigned short*)src_line[2];
			color[8] = *sbp;     color[9] = color[9];    color[10] = *(sbp + 1); color[11] = *(sbp + 2);
			sbp = (unsigned short*)src_line[3];
			color[12] = *sbp;    color[13] = color[12];  color[14] = *(sbp + 1); color[15] = *(sbp + 2);
		}
		else
		{
			unsigned long *lbp;
			lbp = (unsigned long*)src_line[0];
			color[0] = *lbp;     color[1] = color[0];    color[2] = *(lbp + 1);  color[3] = *(lbp + 2);
			lbp = (unsigned long*)src_line[1];
			color[4] = *lbp;     color[5] = color[4];    color[6] = *(lbp + 1);  color[7] = *(lbp + 2);
			lbp = (unsigned long*)src_line[2];
			color[8] = *lbp;     color[9] = color[9];    color[10] = *(lbp + 1); color[11] = *(lbp + 2);
			lbp = (unsigned long*)src_line[3];
			color[12] = *lbp;    color[13] = color[12];  color[14] = *(lbp + 1); color[15] = *(lbp + 2);
		}

		/* Write the 2 lines, if not already done so */
		if (y < height - 1)
		{
			dst_line[0] = &((byte*)dest->data)[dest->pitch*vid_bytesperpixel*(y * 2 + 2)];
			dst_line[1] = &((byte*)dest->data)[dest->pitch*vid_bytesperpixel*(y * 2 + 3)];
		}
	}
}
#else
void Init_2xSAI(int bpp) {}
#endif //ENABLE_2XSAI

/********************** 15bpp blitter code **********************/
#ifdef BLITTER_15BPP

int dd15_MakeColor(int r, int g, int b)
{
	return (((r>>3)<<10)|((g>>3)<<5)|(b>>3));
}


bool dd15_GetColor(int c, int &r, int &g, int &b)
{
//	if (c == transColor) return false;
	b = (c & 0x1F) << 3;
	g = ((c >> 5) & 0x1f) << 3;
	r = ((c >> 10) & 0x1f) << 3;
    return true;
}


void dd15_PutPixel_lucent(int x, int y, int color, image *dest)
{
	word *s = (word *) dest -> data + (y * dest -> pitch) + x;
	word r1, g1, b1;
	word r2, g2, b2;
	word d;

	if (x<dest->cx1 || y<dest->cy1 || x>dest->cx2 || y>dest->cy2)
		return;

	r1 = (*s & 0x7C00) >> 10,
	g1 = (*s & 0x03E0) >> 5,
	b1 = (*s & 0x001F);

	r2 = (color & 0x7C00) >> 10,
	g2 = (color & 0x03E0) >> 5,
	b2 = (color & 0x001F);

	d =
		((((r1 * alpha) + (r2 * ialpha)) / 100) << 10) |
		((((g1 * alpha) + (g2 * ialpha)) / 100) << 5) |
		((((b1 * alpha) + (b2 * ialpha)) / 100));

	*s = d;
}


void dd15_Blit_lucent(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			word r1, g1, b1;
			word r2, g2, b2;
			word dp;

			int c = s[x];
			dp = d[x];

			r1 = (c & 0x7C00) >> 10,
			g1 = (c & 0x03E0) >> 5,
			b1 = (c & 0x001F);

			r2 = (dp & 0x7C00) >> 10,
			g2 = (dp & 0x03E0) >> 5,
			b2 = (dp & 0x001F);

			dp = ((((r1 * ialpha) + (r2 * alpha)) / 100) << 10) |
			     ((((g1 * ialpha) + (g2 * alpha)) / 100) << 5) |
		         ((((b1 * ialpha) + (b2 * alpha)) / 100));

			d[x] = dp;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd15_TBlit_lucent(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			word r1, g1, b1;
			word r2, g2, b2;
			word dp;

			int c = s[x];
			if (c == transColor) continue;
			dp = d[x];

			r1 = (c & 0x7C00) >> 10,
			g1 = (c & 0x03E0) >> 5,
			b1 = (c & 0x001F);

			r2 = (dp & 0x7C00) >> 10,
			g2 = (dp & 0x03E0) >> 5,
			b2 = (dp & 0x001F);

			dp = ((((r1 * ialpha) + (r2 * alpha)) / 100) << 10) |
			     ((((g1 * ialpha) + (g2 * alpha)) / 100) << 5) |
		         ((((b1 * ialpha) + (b2 * alpha)) / 100));

			d[x] = dp;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd15_ScaleBlit_lucent(int x, int y, int dw, int dh, image *src, image *dest)
{
	int i, j;
	int xerr, yerr;
	int xerr_start, yerr_start;
	int xadj, yadj;
	word *d, *s, c;
	int xl, yl, xs, ys;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	if (dw < 1 || dh < 1)
		return;

	xl = dw;
	yl = dh;
	xs = ys = 0;
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x > cx2 || y > cy2
	|| x + xl < cx1 || y + yl < cy1)
		return;
	if (x + xl > cx2)
		xl = cx2 - x + 1;
	if (y + yl > cy2)
		yl = cy2 - y + 1;
	if (x < cx1) { xs = cx1 - x; xl -= xs; x = cx1; }
	if (y < cy1) { ys = cy1 - y; yl -= ys; y = cy1; }

	xadj = (src->width << 16)/dw;
	yadj = (src->height << 16)/dh;
	xerr_start = xadj * xs;
	yerr_start = yadj * ys;

	s = (word *) src->data;
	s += ((yerr_start >> 16) * src->pitch);
	d = ((word *) dest->data) + (y * dest->pitch) + x;
	yerr = yerr_start & 0xffff;

	for (i = 0; i < yl; i += 1)
	{
		xerr = xerr_start;
		for (j = 0; j < xl; j += 1)
		{
			word r1, g1, b1;
			word r2, g2, b2;
			word dp;

			c = s[xerr>>16];
			dp = d[j];

			r1 = (c & 0x7C00) >> 10,
			g1 = (c & 0x03E0) >> 5,
			b1 = (c & 0x001F);

			r2 = (dp & 0x7C00) >> 10,
			g2 = (dp & 0x03E0) >> 5,
			b2 = (dp & 0x001F);

			dp = ((((r1 * ialpha) + (r2 * alpha)) / 100) << 10) |
			     ((((g1 * ialpha) + (g2 * alpha)) / 100) << 5) |
		         ((((b1 * ialpha) + (b2 * alpha)) / 100));

			d[j] = dp;
			xerr += xadj;
		}
		d    += dest->pitch;
		yerr += yadj;
		s    += (yerr>>16)*src->pitch;
		yerr &= 0xffff;
	}
}


void dd15_TScaleBlit_lucent(int x, int y, int dw, int dh, image *src, image *dest)
{
	int i, j;
	int xerr, yerr;
	int xerr_start, yerr_start;
	int xadj, yadj;
	word *d, *s, c;
	int xl, yl, xs, ys;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	if (dw < 1 || dh < 1)
		return;

	xl = dw;
	yl = dh;
	xs = ys = 0;
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x > cx2 || y > cy2
	|| x + xl < cx1 || y + yl < cy1)
		return;
	if (x + xl > cx2)
		xl = cx2 - x + 1;
	if (y + yl > cy2)
		yl = cy2 - y + 1;
	if (x < cx1) { xs = cx1 - x; xl -= xs; x = cx1; }
	if (y < cy1) { ys = cy1 - y; yl -= ys; y = cy1; }

	xadj = (src->width << 16)/dw;
	yadj = (src->height << 16)/dh;
	xerr_start = xadj * xs;
	yerr_start = yadj * ys;

	s = (word *) src->data;
	s += ((yerr_start >> 16) * src->pitch);
	d = ((word *) dest->data) + (y * dest->pitch) + x;
	yerr = yerr_start & 0xffff;

	for (i = 0; i < yl; i += 1)
	{
		xerr = xerr_start;
		for (j = 0; j < xl; j += 1)
		{
			word r1, g1, b1;
			word r2, g2, b2;
			word dp;

			c = s[xerr>>16];
			if (c != transColor)
			{
				dp = d[j];

				r1 = (c & 0x7C00) >> 10,
				g1 = (c & 0x03E0) >> 5,
				b1 = (c & 0x001F);

				r2 = (dp & 0x7C00) >> 10,
				g2 = (dp & 0x03E0) >> 5,
				b2 = (dp & 0x001F);

				dp = ((((r1 * ialpha) + (r2 * alpha)) / 100) << 10) |
					 ((((g1 * ialpha) + (g2 * alpha)) / 100) << 5) |
					 ((((b1 * ialpha) + (b2 * alpha)) / 100));

				d[j] = dp;
			}
			xerr += xadj;
		}
		d    += dest->pitch;
		yerr += yadj;
		s    += (yerr>>16)*src->pitch;
		yerr &= 0xffff;
	}
}


void dd15_WrapBlit_lucent(int x, int y, image *src, image *dst)
{
	int i;
	int cliph, clipw;
	int curx, cury;
	int spanx, spany;
	word *source, *dest;
	int sc;

	cliph = dst->cy2 - dst->cy1 + 1;
	clipw = dst->cx2 - dst->cx1 + 1;
	y %= src->height;
	if (y<0) y = src->height+y;
	curx = 0;

	do
	{
		x %= curx ? 1 : src->width;
		if (x<0) x = src->width+x;
		spanx = src->width - x;
		if (curx + spanx >= clipw)
			spanx = clipw - curx;
		source = (word *) src -> data + (y * src->pitch) + x;
		dest = (word *) dst -> data + (dst->cy1 * dst->pitch) + dst->cx1 + curx;
		cury = 0;

		do
		{
			spany = src->height - (cury ? 0 : y);

			if (cury + spany >= cliph)
				spany = cliph - cury;

			for (i = 0; i < spany; i++, source += src->pitch, dest += dst->pitch)
			{
				for (int x = 0; x < spanx; x++)
				{
					word r1, g1, b1;
					word r2, g2, b2;
					word d;

					sc=source[x];
					d=dest[x];

					r1 = (sc & 0x7C00) >> 10,
					g1 = (sc & 0x03E0) >> 5,
					b1 = (sc & 0x001F);

					r2 = (d & 0x7C00) >> 10,
					g2 = (d & 0x03E0) >> 5,
					b2 = (d & 0x001F);

					d = ((((r1 * ialpha) + (r2 * alpha)) / 100) << 10) |
						((((g1 * ialpha) + (g2 * alpha)) / 100) << 5) |
						((((b1 * ialpha) + (b2 * alpha)) / 100));

					dest[x] = d;
				}
			}

			source = (word *) src->data + x;
			cury += spany;
		} while (cury < cliph);
		curx +=	spanx;
	} while (curx < clipw);
}


void dd15_TWrapBlit_lucent(int x, int y, image *src, image *dst)
{
	int i;
	int cliph, clipw;
	int curx, cury;
	int spanx, spany;
	word *source, *dest;
	int sc;

	cliph = dst->cy2 - dst->cy1 + 1;
	clipw = dst->cx2 - dst->cx1 + 1;
	y %= src->height;
	if (y<0) y = src->height+y;
	curx = 0;

	do
	{
		x %= curx ? 1 : src->width;
		if (x<0) x = src->width+x;
		spanx = src->width - x;
		if (curx + spanx >= clipw)
			spanx = clipw - curx;
		source = (word *) src -> data + (y * src->pitch) + x;
		dest = (word *) dst -> data + (dst->cy1 * dst->pitch) + dst->cx1 + curx;
		cury = 0;

		do
		{
			spany = src->height - (cury ? 0 : y);
			if (cury + spany >= cliph)
				spany = cliph - cury;

			for (i = 0; i < spany; i++, source += src->pitch, dest += dst->pitch)
			{
				for (int x = 0; x < spanx; x++)
				{
					word r1, g1, b1;
					word r2, g2, b2;
					word d;

					sc=source[x];
					d=dest[x];
					if (sc == transColor) continue;

					r1 = (sc & 0x7C00) >> 10,
					g1 = (sc & 0x03E0) >> 5,
					b1 = (sc & 0x001F);

					r2 = (d & 0x7C00) >> 10,
					g2 = (d & 0x03E0) >> 5,
					b2 = (d & 0x001F);

					d = ((((r1 * ialpha) + (r2 * alpha)) / 100) << 10) |
						((((g1 * ialpha) + (g2 * alpha)) / 100) << 5) |
						((((b1 * ialpha) + (b2 * alpha)) / 100));

					dest[x] = d;
				}
			}

			source = (word *) src->data + x;
			cury += spany;
		} while (cury < cliph);
		curx +=	spanx;
	} while (curx < clipw);
}


void dd15_RotScale_lucent(int posx, int posy, float angle, float scale, image* src, image *dest)
{
	int xs,ys,xl,yl;
	int srcx,srcy,x,y,tempx,tempy;

	word *source = (word*) src->data, *d;
	int width=src->width;
	int height=src->height;
	int clip_x = dest->cx1;
	int clip_y = dest->cy1;
	int clip_xend = dest->cx2;
	int clip_yend = dest->cy2;
	float ft=atan2((float)width,(float)height);
	int T_WIDTH_CENTER=width>>1;
	int T_HEIGHT_CENTER=height>>1;
	int W_WIDTH=(int)((float)width/scale*sin(ft) + (float)height/scale*cos(ft));
	int W_HEIGHT=W_WIDTH;
	int W_HEIGHT_CENTER=W_HEIGHT>>1;
	int W_WIDTH_CENTER=W_HEIGHT_CENTER;

	int sinas = (int)(sin(-angle)*65536*scale);
	int cosas = (int)(cos(-angle)*65536*scale);

	int xc = T_WIDTH_CENTER*65536 - (W_HEIGHT_CENTER*(cosas+sinas));
	int yc = T_HEIGHT_CENTER*65536 - (W_WIDTH_CENTER*(cosas-sinas));
	posx -= W_WIDTH_CENTER;
	posy -= W_HEIGHT_CENTER;

	// clipping
	if (W_WIDTH<2 || W_HEIGHT<2) return;
	xl=W_WIDTH;
	yl=W_HEIGHT;
	xs=ys=0;
	if (posx>clip_xend || posy>clip_yend || posx+xl<clip_x || posy+yl<clip_y)
		return;
	if (posx+xl > clip_xend) xl=clip_xend-posx+1;
	if (posy+yl > clip_yend) yl=clip_yend-posy+1;
	if (posx<clip_x)
	{
		xs=clip_x-posx;
		xl-=xs;
		posx=clip_x;

		xc+=cosas*xs;
		yc-=sinas*xs;
	}
	if (posy<clip_y)
	{
		ys=clip_y-posy;
		yl-=ys;
		posy=clip_y;

		xc+=sinas*ys;
		yc+=cosas*ys;
	}

	d=(word*) dest->data+posx+posy*dest->pitch;
	for (y=0; y<yl; y++)
	{
		srcx=xc;
		srcy=yc;

		for (x=0; x<xl; x++)
		{
			tempx=(srcx>>16);
			tempy=(srcy>>16);

			if (tempx>=0 && tempx<width && tempy>=0 && tempy<height)
			{
				int sofs=tempx+tempy*width;
				if (source[sofs] != transColor)
				{
					word r1, g1, b1;
					word r2, g2, b2;
					word dp;

					int sc = source[sofs];
					dp = d[x];

					r1 = (sc & 0x7C00) >> 10,
					g1 = (sc & 0x03E0) >> 5,
					b1 = (sc & 0x001F);

					r2 = (dp & 0x7C00) >> 10,
					g2 = (dp & 0x03E0) >> 5,
					b2 = (dp & 0x001F);

					dp = ((((r1 * ialpha) + (r2 * alpha)) / 100) << 10) |
						 ((((g1 * ialpha) + (g2 * alpha)) / 100) << 5) |
						 ((((b1 * ialpha) + (b2 * alpha)) / 100));

					d[x] = dp;
				}
			}
			srcx+=cosas;
			srcy-=sinas;
		}
		d+=dest->pitch;
		xc+=sinas;
		yc+=cosas;
	}
}


void dd15_Silhouette_lucent(int x, int y, int c, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			word r1, g1, b1;
			word r2, g2, b2;
			word dp;

			if (s[x] == transColor) continue;
			dp = d[x];

			r1 = (c & 0x7C00) >> 10,
			g1 = (c & 0x03E0) >> 5,
			b1 = (c & 0x001F);

			r2 = (dp & 0x7C00) >> 10,
			g2 = (dp & 0x03E0) >> 5,
			b2 = (dp & 0x001F);

			d[x] = ((((r1 * ialpha) + (r2 * alpha)) / 100) << 10) |
				   ((((g1 * ialpha) + (g2 * alpha)) / 100) << 5) |
				   ((((b1 * ialpha) + (b2 * alpha)) / 100));
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd15_AlphaBlit(int x, int y, image *src, image *alpha, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data,
		 *a=(word *)alpha->data;
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
/*	for (x=0; x<xlen; x++)
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
*/
			word r1, g1, b1;
			word r2, g2, b2;
			word dp;

			word c = s[x];
			word pa = (a[x] & 0x001F);
			word ipa = 32-pa;
			if (!pa) continue;
			if (pa == 32)
			{
				d[x] = s[x];
				continue;
			}
			dp = d[x];

			r1 = (c & 0x7C00) >> 10,
			g1 = (c & 0x03E0) >> 5,
			b1 = (c & 0x001F);

			r2 = (dp & 0x7C00) >> 10,
			g2 = (dp & 0x03E0) >> 5,
			b2 = (dp & 0x001F);

			d[x] = ((((r1 * pa) + (r2 * ipa)) / 31) << 10) |
				   ((((g1 * pa) + (g2 * ipa)) / 31) << 5) |
				   ((((b1 * pa) + (b2 * ipa)) / 31));
		}
		s+=spitch;
		d+=dpitch;
		a+=apitch;
	}
}
#endif

#ifdef BLITTER_61BPP
int dd61_MakeColor(int r, int g, int b)
{
	return (((b>>3)<<11)|((g>>2)<<5)|(r>>3));
}

bool dd61_GetColor(int c, int &r, int &g, int &b)
{
//	if (c == transColor) return false;
	b = ((c >> 11) & 0x1f) << 3;
	g = ((c >> 5) & 0x3f) << 2;
	r = (c & 0x1F) << 3;
    return true;
}

void dd61_Clear(int color, image *dest)
{
	word *d = (word *)dest->data;
	int bytes = dest->pitch * dest->height;
	while (bytes--)
		*d++ = color;
}


void dd61_Mosaic(int xf, int yf, image *src)
{
	int x,y,x2,y2,xc,yc,xr,yr;
	int x2max,y2max,xmax,ymax;
	int r,g,b;
	int i,j,c;
	int w,h;
	word *data;
	word color;
	int add,topleft;

	if(!(xf&&yf))
		return;

	data=(word *)src->data;
	w=src->cx2-src->cx1+1;
	h=src->cy2-src->cy1+1;
	xc=w/xf;
	yc=h/yf;
	xr=w%xf;
	yr=h%yf;
	add=src->pitch;

	if(xr)
		xmax=xc+1;
	else
		xmax=xc;
	if(yr)
		ymax=yc+1;
	else
		ymax=yc;

	for(x=0;x<xmax;x++)
	{
		topleft=x*xf+src->cx1+src->cy1*src->pitch;
		for(y=0;y<ymax;y++)
		{
			r=g=b=c=0;
			j=topleft;
			if(y<yc)
				y2max=yf;
			else
				y2max=yr;
			if(x<xc)
				x2max=xf;
			else
				x2max=xr;
			for(y2=0;y2<y2max;y2++)
			{
				i=j;
				for(x2=0;x2<x2max;x2++)
				{
					r+=(data[i]&0x1F)<<3;
					g+=(data[i]&0x07E0)>>3;
					b+=(data[i]&0xF800)>>8;
					i++;
				}
				j+=add;
			}
			j=topleft;
			c=y2max*x2max;
			r/=c;
			g/=c;
			b/=c;
			color=dd61_MakeColor(r,g,b);
			for(y2=0;y2<y2max;y2++)
			{
				i=j;
				for(x2=0;x2<x2max;x2++)
					data[i++]=color;
				j+=add;
			}
			topleft=j;
		}
	}
}



int dd61_ReadPixel(int x, int y, image *source)
{
	word *ptr = (word*)source->data;
	return ptr[(y * source->pitch) + x];
}


void dd61_PutPixel(int x, int y, int color, image *dest)
{
	word *ptr = (word *)dest->data;
	if (x<dest->cx1 || x>dest->cx2 || y<dest->cy1 || y>dest->cy2)
		return;
	ptr[(y * dest->pitch) + x] = color;
}


void dd61_PutPixel_50lucent(int x, int y, int color, image *dest)
{
	quad s, c;
	word *d = (word *) dest->data;

	if (x<dest->cx1 || y<dest->cy1 || x>dest->cx2 || y>dest->cy2)
		return;

	s=d[(y * dest->pitch) + x];
	c=(s & tmask) + (color & tmask);
	d[(y * dest->pitch) + x] = (word) (c >> 1);
}

void dd61_PutPixel_lucent(int x, int y, int color, image *dest)
{
	word *s = (word *) dest -> data + (y * dest -> pitch) + x;
	word r1, g1, b1;
	word r2, g2, b2;
	word d;

	if (x<dest->cx1 || y<dest->cy1 || x>dest->cx2 || y>dest->cy2)
		return;

	r1 = (*s & 0x001F),
	g1 = (*s & 0x07E0) >> 5,
	b1 = (*s & 0xF800) >> 11;

	r2 = (color & 0x001F),
	g2 = (color & 0x07E0) >> 5,
	b2 = (color & 0xF800) >> 11;

	d =
		((((r1 * alpha) + (r2 * ialpha)) / 100)) |
		((((g1 * alpha) + (g2 * ialpha)) / 100) << 5) |
		((((b1 * alpha) + (b2 * ialpha)) / 100) << 11);

	*s = d;
}


void dd61_HLine(int x, int y, int xe, int color, image *dest)
{
	word *d = (word *) dest->data;
	int cx1=0, cy1=0, cx2=0, cy2=0;
	if (xe<x) SWAP(x,xe);
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || xe<cx1 || y<cy1)
		return;
	if (xe>cx2) xe=cx2;
	if (x<cx1)  x =cx1;

	d += (y * dest->pitch) + x;
	for (; x<=xe; x++)
		*d++ = color;
}


void dd61_HLine_50lucent(int x, int y, int xe, int color, image *dest)
{
	word *d = (word *)dest->data;
	int cx1=0, cy1=0, cx2=0, cy2=0;
	int s;
	if (xe<x) SWAP(x,xe);
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || xe<cx1 || y<cy1)
		return;

	if (xe>cx2) xe=cx2;
	if (x<cx1)  x =cx1;

	d+=(y*dest->pitch)+x;
	for (; x<=xe; x++)
	{
		s=*d;
		s=(s & tmask) + (color & tmask);
		*d++ = (word) (s >> 1);
	}
}


void dd61_HLine_lucent(int x, int y, int xe, int color, image *dest)
{
	word *d = (word *)dest->data;
	int cx1=0, cy1=0, cx2=0, cy2=0;
	if (xe<x) SWAP(x,xe);
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || xe<cx1 || y<cy1)
		return;

	if (xe>cx2) xe=cx2;
	if (x<cx1)  x =cx1;

	d+=(y*dest->pitch)+x;
	for (; x<=xe; x++)
		PutPixel(x, y, color, dest);
}


void dd61_VLine(int x, int y, int ye, int color, image *dest)
{
	word *d = (word *) dest->data;
	int cx1=0, cy1=0, cx2=0, cy2=0;
	if (ye<y) SWAP(y,ye);
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x<cx1 || ye<cy1)
		return;
	if (ye>cy2) ye=cy2;
	if (y<cy1)  y =cy1;

	d += (y * dest->pitch) + x;
	for (; y<=ye; y++, d+=dest->pitch)
		*d = color;
}


void dd61_VLine_50lucent(int x, int y, int ye, int color, image *dest)
{
	word *d = (word *) dest->data;
	int cx1=0, cy1=0, cx2=0, cy2=0;
	if (ye<y) SWAP(y,ye);
	int s;
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x<cx1 || ye<cy1)
		return;
	if (ye>cy2) ye=cy2;
	if (y<cy1)  y =cy1;

	d += (y * dest->pitch) + x;
	for (; y<=ye; y++, d+=dest->pitch)
	{
		s=*d;
		s=(s & tmask) + (color & tmask);
		*d = (word) (s >> 1);
	}
}


void dd61_VLine_lucent(int x, int y, int ye, int color, image *dest)
{
	word *d = (word *)dest->data;
	int cx1=0, cy1=0, cx2=0, cy2=0;
	if (ye<y) SWAP(y,ye);
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x<cx1 || ye<cy1)
		return;

	if (ye>cy2) ye=cy2;
	if (y<cy1)  y =cy1;

	d+=(y*dest->pitch)+x;
	for (; y<=ye; y++)
		PutPixel(x, y, color, dest);
}


void dd61_AddBlit(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp5[2];
	int dp6[2];
	int yi;

	dp5[1]=0x1F;
	dp6[1]=0x3F;

	for(yi=0;yi<ylen;yi++)
	{
#ifndef DDBLIT_ASM
		int xi, b, g, r;
		for(xi=0;xi<xlen;xi++)
		{
			dp5[0]=((d[xi]&0x1F)+(s[xi]&0x1F));
			r=dp5[dp5[0]>>5];

			dp6[0]=((d[xi]&0x07E0)+(s[xi]&0x07E0))>>5;
			g=dp6[dp6[0]>>6];

			dp5[0]=((d[xi]&0xF800)>>11)+((s[xi]&0xF800)>>11);
			b=dp5[dp5[0]>>5];
			d[xi]=(b<<11)|(g<<5)|r;
		}
#else/*
		__asm
		{
			mov esi,s;
			mov edi,d;
			mov ecx,xlen;

			push ebp;

			dd16lab0:
			lodsw;
			mov dx,[edi];
			mov bx,ax;
			mov bp,dx;
			ror ebx,16;
			mov bx,ax;
			ror eax,16;


			//eax src,???
			//ebx src,src
			//dx  dest
			//bp  dest

			//b
			xor al,al;
			and bx,0x001F;
			and dx,0x001F;
			shl bx,3;
			shl dx,3;
			add dl,bl;
			adc al,0;
			neg al;
			//at this point ah is either 00000000 or 11111111
			or al,dl;
			//go ahead and put BBBBB in the bottom bits
			shr al,3;

			//g
			xor ah,ah;
			ror ebx,16;
			mov dx,bp;
			and bx,0x07E0;
			and dx,0x07E0;
			shl bx,5;
			shl dx,5;
			add dh,bh;
			adc ah,0;
			shl ah,2;
			neg ah;
			//at this point ah is either 00000000 or 11111100
			or dh,ah;

			//go ahead and put 00000GGGGGGBBBBB in ax
			shr dx,5;
			xor ah,ah;
			or ax,dx;

			//r
			//the 00000GGGGGGBBBBB word will now be in the top word of eax
			ror eax,16;
			mov bx,ax;
			xor ah,ah;
			mov dx,bp;
			and bx,0xF800;
			and dx,0xF800;
			add dh,bh;
			adc ah,0;
			shl ah,3;
			neg ah;
			//at this point ah is either 00000000 or 11111000
			or dh,ah;
			//now DH only has RRRRR00000000000


			//build new rgb value
			//rotate the 00000GGGGGGBBBBB word back into AX
			ror eax,16;
			//OR it with dh=RRRRR00000000000
			or ah,dh;

			//store new rgb value
			stosw;

			//next x;
			dec ecx;
			jnz  dd16lab0;
			//loop dd16lab0;

			pop ebp;
		}*/
#endif
		s+=spitch;
		d+=dpitch;
	}
}


void dd61_AddBlit_50lucent(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp5[2];
	int dp6[2];
	int r,g,b;
	int xi,yi;

	dp5[1]=0x1F;
	dp6[1]=0x3F;

	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			dp5[0]=((d[xi]&0x1F)+((s[xi]&0x1F)/2));
			b=dp5[dp5[0]>>5];
			dp6[0]=((d[xi]&0x07E0)+((s[xi]&0x07E0)/2))>>5;
			g=dp6[dp6[0]>>6];
			dp5[0]=((d[xi]&0xF800)>>11)+(((s[xi]&0xF800)>>11)/2);
			r=dp5[dp5[0]>>5];
			d[xi]=(b<<11)|(g<<5)|r;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd61_AddBlit_lucent(int x, int y, image *src, image *dest)
{
	if(!ialpha)
		return;

	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp5[2];
	int dp6[2];
	int r,g,b;
	int xi,yi;

	dp5[1]=0x1F;
	dp6[1]=0x3F;

	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			dp5[0]=((d[xi]&0x1F)+(((int)(s[xi]&0x1F)*ialpha)/100));
			b=dp5[dp5[0]>>5];
			dp6[0]=((d[xi]&0x07E0)+(((int)(s[xi]&0x07E0)*ialpha)/100))>>5;
			g=dp6[dp6[0]>>6];
			dp5[0]=((d[xi]&0xF800)>>11)+(((int)((s[xi]&0xF800)*ialpha)/100)>>11);
			r=dp5[dp5[0]>>5];
			d[xi]=(b<<11)|(g<<5)|r;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd61_TAddBlit(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp5[2];
	int dp6[2];
	int r,g,b;
	int xi,yi;

	dp5[1]=0x1F;
	dp6[1]=0x3F;

	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			if (s[xi] == transColor) continue;
			dp5[0]=((d[xi]&0x1F)+(s[xi]&0x1F));
			b=dp5[dp5[0]>>5];
			dp6[0]=((d[xi]&0x07E0)+(s[xi]&0x07E0))>>5;
			g=dp6[dp6[0]>>6];
			dp5[0]=((d[xi]&0xF800)>>11)+((s[xi]&0xF800)>>11);
			r=dp5[dp5[0]>>5];
			d[xi]=(b<<11)|(g<<5)|r;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd61_TAddBlit_50lucent(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp5[2];
	int dp6[2];
	int r,g,b;
	int xi,yi;

	dp5[1]=0x1F;
	dp6[1]=0x3F;

	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			if (s[xi] == transColor) continue;
			dp5[0]=((d[xi]&0x1F)+((s[xi]&0x1F)/2));
			b=dp5[dp5[0]>>5];
			dp6[0]=((d[xi]&0x07E0)+((s[xi]&0x07E0)/2))>>5;
			g=dp6[dp6[0]>>6];
			dp5[0]=((d[xi]&0xF800)>>11)+(((s[xi]&0xF800)>>11)/2);
			r=dp5[dp5[0]>>5];
			d[xi]=(b<<11)|(g<<5)|r;
		}
		s+=spitch;
		d+=dpitch;
	}
}

void dd61_TAddBlit_lucent(int x, int y, image *src, image *dest)
{
	if(!ialpha)
		return;

	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp5[2];
	int dp6[2];
	int r,g,b;
	int xi,yi;

	dp5[1]=0x1F;
	dp6[1]=0x3F;

	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			if (s[xi] == transColor) continue;
			dp5[0]=((d[xi]&0x1F)+(((int)(s[xi]&0x1F)*ialpha)/100));
			b=dp5[dp5[0]>>5];
			dp6[0]=((d[xi]&0x07E0)+(((int)(s[xi]&0x07E0)*ialpha)/100))>>5;
			g=dp6[dp6[0]>>6];
			dp5[0]=((d[xi]&0xF800)>>11)+(((int)((s[xi]&0xF800)*ialpha)/100)>>11);
			r=dp5[dp5[0]>>5];
			d[xi]=(b<<11)|(g<<5)|r;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd61_SubtractBlit(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp[2];
	int r,g,b;
	int xi,yi;

	dp[1]=0;

	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			dp[0]=((d[xi]&0x1F)-(s[xi]&0x1F));
			b=dp[(dp[0]>>31)&1];
			dp[0]=((d[xi]&0x07E0)-(s[xi]&0x07E0))>>5;
			g=dp[(dp[0]>>31)&1];
			dp[0]=((d[xi]&0xF800)-(s[xi]&0xF800))>>11;
			r=dp[(dp[0]>>31)&1];
			d[xi]=(b<<11)|(g<<5)|r;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd61_SubtractBlit_50lucent(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp[2];
	int r,g,b;
	int xi,yi;

	dp[1]=0;

	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			dp[0]=((d[xi]&0x1F)-(s[xi]&0x1F)/2);
			b=dp[(dp[0]>>31)&1];
			dp[0]=((d[xi]&0x07E0)-(s[xi]&0x07E0)/2)>>5;
			g=dp[(dp[0]>>31)&1];
			dp[0]=((d[xi]&0xF800)-(s[xi]&0xF800)/2)>>11;
			r=dp[(dp[0]>>31)&1];
			d[xi]=(b<<11)|(g<<5)|r;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd61_SubtractBlit_lucent(int x, int y, image *src, image *dest)
{
	if(!ialpha)
		return;
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp[2];
	int r,g,b;
	int xi,yi;

	dp[1]=0;

	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			dp[0]=((d[xi]&0x1F)-((int)(s[xi]&0x1F)*ialpha)/100);
			b=dp[(dp[0]>>31)&1];
			dp[0]=((d[xi]&0x07E0)-((int)(s[xi]&0x07E0)*ialpha)/100)>>5;
			g=dp[(dp[0]>>31)&1];
			dp[0]=((d[xi]&0xF800)-((int)(s[xi]&0xF800)*ialpha)/100)>>11;
			r=dp[(dp[0]>>31)&1];
			d[xi]=(b<<11)|(g<<5)|r;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd61_TSubtractBlit(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp[2];
	int r,g,b;
	int xi,yi;

	dp[1]=0;

	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			if (s[xi] == transColor) continue;
			dp[0]=((d[xi]&0x1F)-(s[xi]&0x1F));
			b=dp[(dp[0]>>31)&1];
			dp[0]=((d[xi]&0x07E0)-(s[xi]&0x07E0))>>5;
			g=dp[(dp[0]>>31)&1];
			dp[0]=((d[xi]&0xF800)-(s[xi]&0xF800))>>11;
			r=dp[(dp[0]>>31)&1];
			d[xi]=(b<<11)|(g<<5)|r;
		}
		s+=spitch;
		d+=dpitch;
	}
}

void dd61_TSubtractBlit_50lucent(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp[2];
	int r,g,b;
	int xi,yi;

	dp[1]=0;

	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			if (s[xi] == transColor) continue;
			dp[0]=((d[xi]&0x1F)-(s[xi]&0x1F)/2);
			b=dp[(dp[0]>>31)&1];
			dp[0]=((d[xi]&0x07E0)-(s[xi]&0x07E0)/2)>>5;
			g=dp[(dp[0]>>31)&1];
			dp[0]=((d[xi]&0xF800)-(s[xi]&0xF800)/2)>>11;
			r=dp[(dp[0]>>31)&1];
			d[xi]=(b<<11)|(g<<5)|r;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd61_TSubtractBlit_lucent(int x, int y, image *src, image *dest)
{
	if(!ialpha)
		return;
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp[2];
	int r,g,b;
	int xi,yi;

	dp[1]=0;

	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			if(s[xi]==transColor)continue;
			dp[0]=((d[xi]&0x1F)-((int)(s[xi]&0x1F)*ialpha)/100);
			b=dp[(dp[0]>>31)&1];
			dp[0]=((d[xi]&0x07E0)-((int)(s[xi]&0x07E0)*ialpha)/100)>>5;
			g=dp[(dp[0]>>31)&1];
			dp[0]=((d[xi]&0xF800)-((int)(s[xi]&0xF800)*ialpha)/100)>>11;
			r=dp[(dp[0]>>31)&1];
			d[xi]=(b<<11)|(g<<5)|r;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd61_Blit(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;
	for (xlen *= 2; ylen--; s+=spitch, d+=dpitch)
		memcpy(d, s, xlen);
}


void dd61_Blit_50lucent(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;
	int sc;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			sc=s[x];
	        sc=(sc & tmask) + (d[x] & tmask);
			d[x] = (word) (sc >> 1);
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd61_Blit_lucent(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			word r1, g1, b1;
			word r2, g2, b2;
			word dp;

			int c = s[x];
			dp = d[x];

			r1 = (c & 0x001F),
			g1 = (c & 0x07E0) >> 5,
			b1 = (c & 0xF800) >> 11;

			r2 = (dp & 0x001F),
			g2 = (dp & 0x07E0) >> 5,
			b2 = (dp & 0xF800) >> 11;

			dp = ((((r1 * ialpha) + (r2 * alpha)) / 100)) |
				 ((((g1 * ialpha) + (g2 * alpha)) / 100) << 5) |
				 ((((b1 * ialpha) + (b2 * alpha)) / 100) << 11);

			d[x] = dp;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd61_TBlit(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,c,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			c=s[x];
			if (c != transColor) d[x]=c;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd61_TBlit_50lucent(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;
	int sc;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			sc=s[x]; if (sc == transColor) continue;
	        sc=(sc & tmask) + (d[x] & tmask);
			d[x] = (word) (sc >> 1);
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd61_TBlit_lucent(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			word r1, g1, b1;
			word r2, g2, b2;
			word dp;

			int c = s[x];
			if (c == transColor) continue;
			dp = d[x];

			r1 = (c & 0x001F),
			g1 = (c & 0x07E0) >> 5,
			b1 = (c & 0xF800) >> 11;

			r2 = (dp & 0x001F),
			g2 = (dp & 0x07E0) >> 5,
			b2 = (dp & 0xF800) >> 11;

			dp = ((((r1 * ialpha) + (r2 * alpha)) / 100)) |
				 ((((g1 * ialpha) + (g2 * alpha)) / 100) << 5) |
				 ((((b1 * ialpha) + (b2 * alpha)) / 100) << 11);

			d[x] = dp;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd61_BlitTile(int x, int y, char *src, image *dest)
{
	word *s=(word *) src,
		 *d=(word *) dest->data;
	int spitch=16,
		dpitch=dest->pitch;
	int xlen=16,
		ylen=16;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d+=(y*dest->pitch)+x;
	for (xlen*=2; ylen--; s+=spitch,d+=dpitch)
    	memcpy(d, s, xlen);
}


void dd61_TBlitTile(int x, int y, char *src, image *dest)
{
	word *s=(word *) src,
		 *d=(word *) dest->data;
	int spitch=16,
		dpitch=dest->pitch;
	int xlen=16,
		ylen=16;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;
	word c;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;

	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x = cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y=cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			c=s[x];
			if (c != transColor) d[x]=c;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd61_TBlitTile_lucent(int x, int y, char *src, image *dest)
{
	word *s=(word *) src,
		 *d=(word *) dest->data;
	int spitch=16,
		dpitch=dest->pitch;
	int xlen=16,
		ylen=16;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;

	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x = cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y=cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			word r1, g1, b1;
			word r2, g2, b2;
			word dp;

			int c = s[x];
			if (c == transColor) continue;
			dp = d[x];

			r1 = (c & 0x001F),
			g1 = (c & 0x07E0) >> 5,
			b1 = (c & 0xF800) >> 11;

			r2 = (dp & 0x001F),
			g2 = (dp & 0x07E0) >> 5,
			b2 = (dp & 0xF800) >> 11;

			dp = ((((r1 * ialpha) + (r2 * alpha)) / 100)) |
				 ((((g1 * ialpha) + (g2 * alpha)) / 100) << 5) |
				 ((((b1 * ialpha) + (b2 * alpha)) / 100) << 11);

			d[x] = dp;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd61_ScaleBlit(int x, int y, int dw, int dh, image *src, image *dest)
{
	int i, j;
	int xerr, yerr;
	int xerr_start, yerr_start;
	int xadj, yadj;
	word *d, *s;
	int xl, yl, xs, ys;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	if (dw < 1 || dh < 1)
		return;

	xl = dw;
	yl = dh;
	xs = ys = 0;
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x > cx2 || y > cy2
	|| x + xl < cx1 || y + yl < cy1)
		return;
	if (x + xl > cx2)
		xl = cx2 - x + 1;
	if (y + yl > cy2)
		yl = cy2 - y + 1;
	if (x < cx1) { xs = cx1 - x; xl -= xs; x = cx1; }
	if (y < cy1) { ys = cy1 - y; yl -= ys; y = cy1; }

	xadj = (src->width << 16)/dw;
	yadj = (src->height << 16)/dh;
	xerr_start = xadj * xs;
	yerr_start = yadj * ys;

	s = (word *) src->data;
	s += ((yerr_start >> 16) * src->pitch);
	d = ((word *) dest->data) + (y * dest->pitch) + x;
	yerr = yerr_start & 0xffff;

	for (i = 0; i < yl; i += 1)
	{
		xerr = xerr_start;
		for (j = 0; j < xl; j += 1)
		{
			d[j] = s[(xerr >> 16)];
			xerr += xadj;
		}
		d    += dest->pitch;
		yerr += yadj;
		s    += (yerr>>16)*src->pitch;
		yerr &= 0xffff;
	}
}


void dd61_ScaleBlit_50lucent(int x, int y, int dw, int dh, image *src, image *dest)
{
	int i, j;
	int xerr, yerr;
	int xerr_start, yerr_start;
	int xadj, yadj;
	word *d, *s, c;
	int xl, yl, xs, ys;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	if (dw < 1 || dh < 1)
		return;

	xl = dw;
	yl = dh;
	xs = ys = 0;
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x > cx2 || y > cy2
	|| x + xl < cx1 || y + yl < cy1)
		return;
	if (x + xl > cx2)
		xl = cx2 - x + 1;
	if (y + yl > cy2)
		yl = cy2 - y + 1;
	if (x < cx1) { xs = cx1 - x; xl -= xs; x = cx1; }
	if (y < cy1) { ys = cy1 - y; yl -= ys; y = cy1; }

	xadj = (src->width << 16)/dw;
	yadj = (src->height << 16)/dh;
	xerr_start = xadj * xs;
	yerr_start = yadj * ys;

	s = (word *) src->data;
	s += ((yerr_start >> 16) * src->pitch);
	d = ((word *) dest->data) + (y * dest->pitch) + x;
	yerr = yerr_start & 0xffff;

	for (i = 0; i < yl; i += 1)
	{
		xerr = xerr_start;
		for (j = 0; j < xl; j += 1)
		{
			c = s[xerr >> 16];
			c = (c & tmask) + (d[j] & tmask);
			d[j] = (word) (c >> 1);
			xerr += xadj;
		}
		d    += dest->pitch;
		yerr += yadj;
		s    += (yerr>>16)*src->pitch;
		yerr &= 0xffff;
	}
}


void dd61_ScaleBlit_lucent(int x, int y, int dw, int dh, image *src, image *dest)
{
	int i, j;
	int xerr, yerr;
	int xerr_start, yerr_start;
	int xadj, yadj;
	word *d, *s, c;
	int xl, yl, xs, ys;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	if (dw < 1 || dh < 1)
		return;

	xl = dw;
	yl = dh;
	xs = ys = 0;
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x > cx2 || y > cy2
	|| x + xl < cx1 || y + yl < cy1)
		return;
	if (x + xl > cx2)
		xl = cx2 - x + 1;
	if (y + yl > cy2)
		yl = cy2 - y + 1;
	if (x < cx1) { xs = cx1 - x; xl -= xs; x = cx1; }
	if (y < cy1) { ys = cy1 - y; yl -= ys; y = cy1; }

	xadj = (src->width << 16)/dw;
	yadj = (src->height << 16)/dh;
	xerr_start = xadj * xs;
	yerr_start = yadj * ys;

	s = (word *) src->data;
	s += ((yerr_start >> 16) * src->pitch);
	d = ((word *) dest->data) + (y * dest->pitch) + x;
	yerr = yerr_start & 0xffff;

	for (i = 0; i < yl; i += 1)
	{
		xerr = xerr_start;
		for (j = 0; j < xl; j += 1)
		{
			word r1, g1, b1;
			word r2, g2, b2;
			word dp;

			c = s[xerr>>16];
			dp = d[j];

			r1 = (c & 0x001F),
			g1 = (c & 0x07E0) >> 5,
			b1 = (c & 0xF800) >> 11;

			r2 = (dp & 0x001F),
			g2 = (dp & 0x07E0) >> 5,
			b2 = (dp & 0xF800) >> 11;

			dp = ((((r1 * ialpha) + (r2 * alpha)) / 100)) |
				 ((((g1 * ialpha) + (g2 * alpha)) / 100) << 5) |
				 ((((b1 * ialpha) + (b2 * alpha)) / 100) << 11);

			d[j] = dp;
			xerr += xadj;
		}
		d    += dest->pitch;
		yerr += yadj;
		s    += (yerr>>16)*src->pitch;
		yerr &= 0xffff;
	}
}


void dd61_TScaleBlit(int x, int y, int dw, int dh, image *src, image *dest)
{
	int i, j;
	int xerr, yerr;
	int xerr_start, yerr_start;
	int xadj, yadj;
	word *d, *s;
	int xl, yl, xs, ys;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	if (dw < 1 || dh < 1)
		return;

	xl = dw;
	yl = dh;
	xs = ys = 0;
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x > cx2 || y > cy2
	|| x + xl < cx1 || y + yl < cy1)
		return;
	if (x + xl > cx2)
		xl = cx2 - x + 1;
	if (y + yl > cy2)
		yl = cy2 - y + 1;
	if (x < cx1) { xs = cx1 - x; xl -= xs; x = cx1; }
	if (y < cy1) { ys = cy1 - y; yl -= ys; y = cy1; }

	xadj = (src->width << 16)/dw;
	yadj = (src->height << 16)/dh;
	xerr_start = xadj * xs;
	yerr_start = yadj * ys;

	s = (word *) src->data;
	s += ((yerr_start >> 16) * src->pitch);
	d = ((word *) dest->data) + (y * dest->pitch) + x;
	yerr = yerr_start & 0xffff;

	for (i = 0; i < yl; i += 1)
	{
		xerr = xerr_start;
		for (j = 0; j < xl; j += 1)
		{
			int c = s[(xerr >> 16)];
			if (c != transColor)
				d[j] = c;
			xerr += xadj;
		}
		d    += dest->pitch;
		yerr += yadj;
		s    += (yerr>>16)*src->pitch;
		yerr &= 0xffff;
	}
}


void dd61_TScaleBlit_50lucent(int x, int y, int dw, int dh, image *src, image *dest)
{
	int i, j;
	int xerr, yerr;
	int xerr_start, yerr_start;
	int xadj, yadj;
	word *d, *s;
	int xl, yl, xs, ys;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	if (dw < 1 || dh < 1)
		return;

	xl = dw;
	yl = dh;
	xs = ys = 0;
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x > cx2 || y > cy2
	|| x + xl < cx1 || y + yl < cy1)
		return;
	if (x + xl > cx2)
		xl = cx2 - x + 1;
	if (y + yl > cy2)
		yl = cy2 - y + 1;
	if (x < cx1) { xs = cx1 - x; xl -= xs; x = cx1; }
	if (y < cy1) { ys = cy1 - y; yl -= ys; y = cy1; }

	xadj = (src->width << 16)/dw;
	yadj = (src->height << 16)/dh;
	xerr_start = xadj * xs;
	yerr_start = yadj * ys;

	s = (word *) src->data;
	s += ((yerr_start >> 16) * src->pitch);
	d = ((word *) dest->data) + (y * dest->pitch) + x;
	yerr = yerr_start & 0xffff;

	for (i = 0; i < yl; i += 1)
	{
		xerr = xerr_start;
		for (j = 0; j < xl; j += 1)
		{
			int c = s[xerr >> 16];
			if (c != transColor)
			{
				c = (c & tmask) + (d[j] & tmask);
				d[j] = (word) (c >> 1);
			}
			xerr += xadj;
		}
		d    += dest->pitch;
		yerr += yadj;
		s    += (yerr>>16)*src->pitch;
		yerr &= 0xffff;
	}
}


void dd61_TScaleBlit_lucent(int x, int y, int dw, int dh, image *src, image *dest)
{
	int i, j;
	int xerr, yerr;
	int xerr_start, yerr_start;
	int xadj, yadj;
	word *d, *s;
	int xl, yl, xs, ys;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	if (dw < 1 || dh < 1)
		return;

	xl = dw;
	yl = dh;
	xs = ys = 0;
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x > cx2 || y > cy2
	|| x + xl < cx1 || y + yl < cy1)
		return;
	if (x + xl > cx2)
		xl = cx2 - x + 1;
	if (y + yl > cy2)
		yl = cy2 - y + 1;
	if (x < cx1) { xs = cx1 - x; xl -= xs; x = cx1; }
	if (y < cy1) { ys = cy1 - y; yl -= ys; y = cy1; }

	xadj = (src->width << 16)/dw;
	yadj = (src->height << 16)/dh;
	xerr_start = xadj * xs;
	yerr_start = yadj * ys;

	s = (word *) src->data;
	s += ((yerr_start >> 16) * src->pitch);
	d = ((word *) dest->data) + (y * dest->pitch) + x;
	yerr = yerr_start & 0xffff;

	for (i = 0; i < yl; i += 1)
	{
		xerr = xerr_start;
		for (j = 0; j < xl; j += 1)
		{
			word r1, g1, b1;
			word r2, g2, b2;
			word dp;

			int c = s[xerr>>16];
			if (c != transColor)
			{
				dp = d[j];

				r1 = (c & 0x001F),
				g1 = (c & 0x07E0) >> 5,
				b1 = (c & 0xF800) >> 11;

				r2 = (dp & 0x001F),
				g2 = (dp & 0x07E0) >> 5,
				b2 = (dp & 0xF800) >> 11;

				dp = ((((r1 * ialpha) + (r2 * alpha)) / 100)) |
					 ((((g1 * ialpha) + (g2 * alpha)) / 100) << 5) |
					 ((((b1 * ialpha) + (b2 * alpha)) / 100) << 11);

				d[j] = dp;
			}
			xerr += xadj;
		}
		d    += dest->pitch;
		yerr += yadj;
		s    += (yerr>>16)*src->pitch;
		yerr &= 0xffff;
	}
}


void dd61_WrapBlit(int x, int y, image *src, image *dst)
{
	int i;
	int cliph, clipw;
	int curx, cury;
	int spanx, spany;
	word *source, *dest;

	cliph = dst->cy2 - dst->cy1 + 1;
	clipw = dst->cx2 - dst->cx1 + 1;
	y %= src->height;
	if (y<0) y = src->height+y;
	curx = 0;

	do
	{
		x %= curx ? 1 : src->width;
		if (x<0) x = src->width+x;
		spanx = src->width - x;
		if (curx + spanx >= clipw)
			spanx = clipw - curx;
		source = (word *) src -> data + (y * src->pitch) + x;
		dest = (word *) dst -> data + (dst->cy1 * dst->pitch) + dst->cx1 + curx;
		cury = 0;

		do
		{
			spany = src->height - (cury ? 0 : y);
			if (cury + spany >= cliph)
				spany = cliph - cury;

			for (i = 0; i < spany; i++, source += src->pitch, dest += dst->pitch)
				memcpy(dest, source, spanx*2);

			source = (word *) src->data + x;
			cury += spany;
		} while (cury < cliph);
		curx +=	spanx;
	} while (curx < clipw);
}


void dd61_WrapBlit_50lucent(int x, int y, image *src, image *dst)
{
	int i;
	int cliph, clipw;
	int curx, cury;
	int spanx, spany;
	word *source, *dest;
	int sc;

	cliph = dst->cy2 - dst->cy1 + 1;
	clipw = dst->cx2 - dst->cx1 + 1;
	y %= src->height;
	if (y<0) y = src->height+y;
	curx = 0;

	do
	{
		x %= curx ? 1 : src->width;
		if (x<0) x = src->width+x;
		spanx = src->width - x;
		if (curx + spanx >= clipw)
			spanx = clipw - curx;
		source = (word *) src -> data + (y * src->pitch) + x;
		dest = (word *) dst -> data + (dst->cy1 * dst->pitch) + dst->cx1 + curx;
		cury = 0;

		do
		{
			spany = src->height - (cury ? 0 : y);
			if (cury + spany >= cliph)
				spany = cliph - cury;

			for (i = 0; i < spany; i++, source += src->pitch, dest += dst->pitch)
			{
				for (int x = 0; x < spanx; x++)
				{
					sc=source[x];
					sc=(sc & tmask) + (dest[x] & tmask);
					dest[x] = (word) (sc >> 1);
				}
			}

			source = (word *) src->data + x;
			cury += spany;
		} while (cury < cliph);
		curx +=	spanx;
	} while (curx < clipw);
}


void dd61_WrapBlit_lucent(int x, int y, image *src, image *dst)
{
	int i;
	int cliph, clipw;
	int curx, cury;
	int spanx, spany;
	word *source, *dest;
	int sc;

	cliph = dst->cy2 - dst->cy1 + 1;
	clipw = dst->cx2 - dst->cx1 + 1;
	y %= src->height;
	if (y<0) y = src->height+y;
	curx = 0;

	do
	{
		x %= curx ? 1 : src->width;
		if (x<0) x = src->width+x;
		spanx = src->width - x;
		if (curx + spanx >= clipw)
			spanx = clipw - curx;
		source = (word *) src -> data + (y * src->pitch) + x;
		dest = (word *) dst -> data + (dst->cy1 * dst->pitch) + dst->cx1 + curx;
		cury = 0;

		do
		{
			spany = src->height - (cury ? 0 : y);
			if (cury + spany >= cliph)
				spany = cliph - cury;

			for (i = 0; i < spany; i++, source += src->pitch, dest += dst->pitch)
			{
				for (int x = 0; x < spanx; x++)
				{
					word r1, g1, b1;
					word r2, g2, b2;
					word d;

					sc=source[x];
					d=dest[x];

					r1 = (sc & 0x001F),
					g1 = (sc & 0x07E0) >> 5,
					b1 = (sc & 0xF800) >> 11;

					r2 = (d & 0x001F),
					g2 = (d & 0x07E0) >> 5,
					b2 = (d & 0xF800) >> 11;

					d = ((((r1 * ialpha) + (r2 * alpha)) / 100)) |
						((((g1 * ialpha) + (g2 * alpha)) / 100) << 5) |
						((((b1 * ialpha) + (b2 * alpha)) / 100) << 11);

					dest[x] = d;
				}
			}

			source = (word *) src->data + x;
			cury += spany;
		} while (cury < cliph);
		curx +=	spanx;
	} while (curx < clipw);
}


void dd61_TWrapBlit(int x, int y, image *src, image *dst)
{
	int i;
	int cliph, clipw;
	int curx, cury;
	int spanx, spany;
	word *source, *dest;
	int sc;

	cliph = dst->cy2 - dst->cy1 + 1;
	clipw = dst->cx2 - dst->cx1 + 1;
	y %= src->height;
	if (y<0) y = src->height+y;
	curx = 0;

	do
	{
		x %= curx ? 1 : src->width;
		if (x<0) x = src->width+x;
		spanx = src->width - x;
		if (curx + spanx >= clipw)
			spanx = clipw - curx;
		source = (word *) src -> data + (y * src->pitch) + x;
		dest = (word *) dst -> data + (dst->cy1 * dst->pitch) + dst->cx1 + curx;
		cury = 0;

		do
		{
			spany = src->height - (cury ? 0 : y);
			if (cury + spany >= cliph)
				spany = cliph - cury;

			for (i = 0; i < spany; i++, source += src->pitch, dest += dst->pitch)
			{
				for (int x = 0; x < spanx; x++)
				{
					sc=source[x];
					if (sc != transColor)
						dest[x] = sc;
				}
			}

			source = (word *) src->data + x;
			cury += spany;
		} while (cury < cliph);
		curx +=	spanx;
	} while (curx < clipw);
}


void dd61_TWrapBlit_50lucent(int x, int y, image *src, image *dst)
{
	int i;
	int cliph, clipw;
	int curx, cury;
	int spanx, spany;
	word *source, *dest;
	int sc;

	cliph = dst->cy2 - dst->cy1 + 1;
	clipw = dst->cx2 - dst->cx1 + 1;
	y %= src->height;
	if (y<0) y = src->height+y;
	curx = 0;

	do
	{
		x %= curx ? 1 : src->width;
		if (x<0) x = src->width+x;
		spanx = src->width - x;
		if (curx + spanx >= clipw)
			spanx = clipw - curx;
		source = (word *) src -> data + (y * src->pitch) + x;
		dest = (word *) dst -> data + (dst->cy1 * dst->pitch) + dst->cx1 + curx;
		cury = 0;

		do
		{
			spany = src->height - (cury ? 0 : y);
			if (cury + spany >= cliph)
				spany = cliph - cury;

			for (i = 0; i < spany; i++, source += src->pitch, dest += dst->pitch)
			{
				for (int x = 0; x < spanx; x++)
				{
					sc=source[x];
					if (sc == transColor) continue;
					sc=(sc & tmask) + (dest[x] & tmask);
					dest[x] = (word) (sc >> 1);
				}
			}

			source = (word *) src->data + x;
			cury += spany;
		} while (cury < cliph);
		curx +=	spanx;
	} while (curx < clipw);
}


void dd61_TWrapBlit_lucent(int x, int y, image *src, image *dst)
{
	int i;
	int cliph, clipw;
	int curx, cury;
	int spanx, spany;
	word *source, *dest;
	int sc;

	cliph = dst->cy2 - dst->cy1 + 1;
	clipw = dst->cx2 - dst->cx1 + 1;
	y %= src->height;
	if (y<0) y = src->height+y;
	curx = 0;

	do
	{
		x %= curx ? 1 : src->width;
		if (x<0) x = src->width+x;
		spanx = src->width - x;
		if (curx + spanx >= clipw)
			spanx = clipw - curx;
		source = (word *) src -> data + (y * src->pitch) + x;
		dest = (word *) dst -> data + (dst->cy1 * dst->pitch) + dst->cx1 + curx;
		cury = 0;

		do
		{
			spany = src->height - (cury ? 0 : y);
			if (cury + spany >= cliph)
				spany = cliph - cury;

			for (i = 0; i < spany; i++, source += src->pitch, dest += dst->pitch)
			{
				for (int x = 0; x < spanx; x++)
				{
					word r1, g1, b1;
					word r2, g2, b2;
					word d;

					sc=source[x];
					d=dest[x];
					if (sc == transColor) continue;

					r1 = (sc & 0x001F),
					g1 = (sc & 0x07E0) >> 5,
					b1 = (sc & 0xF800) >> 11;

					r2 = (d & 0x001F),
					g2 = (d & 0x07E0) >> 5,
					b2 = (d & 0xF800) >> 11;

					d = ((((r1 * ialpha) + (r2 * alpha)) / 100)) |
						((((g1 * ialpha) + (g2 * alpha)) / 100) << 5) |
						((((b1 * ialpha) + (b2 * alpha)) / 100) << 11);

					dest[x] = d;
				}
			}

			source = (word *) src->data + x;
			cury += spany;
		} while (cury < cliph);
		curx +=	spanx;
	} while (curx < clipw);
}


void dd61_RotScale(int posx, int posy, float angle, float scale, image* src, image *dest)
{
	int xs,ys,xl,yl;
	int srcx,srcy,x,y,tempx,tempy;

	word *source = (word*) src->data, *d;
	int width=src->width;
	int height=src->height;
	int clip_x = dest->cx1;
	int clip_y = dest->cy1;
	int clip_xend = dest->cx2;
	int clip_yend = dest->cy2;
	float ft=atan2((float)width,(float)height);
	int T_WIDTH_CENTER=width>>1;
	int T_HEIGHT_CENTER=height>>1;
	int W_WIDTH=(int)((float)width/scale*sin(ft) + (float)height/scale*cos(ft));
	int W_HEIGHT=W_WIDTH;
	int W_HEIGHT_CENTER=W_HEIGHT>>1;
	int W_WIDTH_CENTER=W_HEIGHT_CENTER;

	int sinas = (int)(sin(-angle)*65536*scale);
	int cosas = (int)(cos(-angle)*65536*scale);

	int xc = T_WIDTH_CENTER*65536 - (W_HEIGHT_CENTER*(cosas+sinas));
	int yc = T_HEIGHT_CENTER*65536 - (W_WIDTH_CENTER*(cosas-sinas));
	posx -= W_WIDTH_CENTER;
	posy -= W_HEIGHT_CENTER;

	// clipping
	if (W_WIDTH<2 || W_HEIGHT<2) return;
	xl=W_WIDTH;
	yl=W_HEIGHT;
	xs=ys=0;
	if (posx>clip_xend || posy>clip_yend || posx+xl<clip_x || posy+yl<clip_y)
		return;
	if (posx+xl > clip_xend) xl=clip_xend-posx+1;
	if (posy+yl > clip_yend) yl=clip_yend-posy+1;
	if (posx<clip_x)
	{
		xs=clip_x-posx;
		xl-=xs;
		posx=clip_x;

		xc+=cosas*xs;
		yc-=sinas*xs;
	}
	if (posy<clip_y)
	{
		ys=clip_y-posy;
		yl-=ys;
		posy=clip_y;

		xc+=sinas*ys;
		yc+=cosas*ys;
	}

	d = (word*)dest->data+posx+posy*dest->pitch;
	for (y=0; y<yl; y++)
	{
		srcx=xc;
		srcy=yc;

		for (x=0; x<xl; x++)
		{
			tempx=(srcx>>16);
			tempy=(srcy>>16);

			if (tempx>=0 && tempx<width && tempy>=0 && tempy<height)
			{
				int sofs=tempx+tempy*width;
				if (source[sofs] != transColor)
					d[x]=source[sofs];
			}

			srcx+=cosas;
			srcy-=sinas;
		}
		d+=dest->pitch;
		xc+=sinas;
		yc+=cosas;
	}
}


void dd61_RotScale_50lucent(int posx, int posy, float angle, float scale, image* src, image *dest)
{
	int xs,ys,xl,yl;
	int srcx,srcy,x,y,tempx,tempy;

	word *source = (word*) src->data, *d;
	int width=src->width;
	int height=src->height;
	int clip_x = dest->cx1;
	int clip_y = dest->cy1;
	int clip_xend = dest->cx2;
	int clip_yend = dest->cy2;
	float ft=atan2((float)width,(float)height);
	int T_WIDTH_CENTER=width>>1;
	int T_HEIGHT_CENTER=height>>1;
	int W_WIDTH=(int)((float)width/scale*sin(ft) + (float)height/scale*cos(ft));
	int W_HEIGHT=W_WIDTH;
	int W_HEIGHT_CENTER=W_HEIGHT>>1;
	int W_WIDTH_CENTER=W_HEIGHT_CENTER;

	int sinas = (int)(sin(-angle)*65536*scale);
	int cosas = (int)(cos(-angle)*65536*scale);

	int xc = T_WIDTH_CENTER*65536 - (W_HEIGHT_CENTER*(cosas+sinas));
	int yc = T_HEIGHT_CENTER*65536 - (W_WIDTH_CENTER*(cosas-sinas));
	posx -= W_WIDTH_CENTER;
	posy -= W_HEIGHT_CENTER;

	// clipping
	if (W_WIDTH<2 || W_HEIGHT<2) return;
	xl=W_WIDTH;
	yl=W_HEIGHT;
	xs=ys=0;
	if (posx>clip_xend || posy>clip_yend || posx+xl<clip_x || posy+yl<clip_y)
		return;
	if (posx+xl > clip_xend) xl=clip_xend-posx+1;
	if (posy+yl > clip_yend) yl=clip_yend-posy+1;
	if (posx<clip_x)
	{
		xs=clip_x-posx;
		xl-=xs;
		posx=clip_x;

		xc+=cosas*xs;
		yc-=sinas*xs;
	}
	if (posy<clip_y)
	{
		ys=clip_y-posy;
		yl-=ys;
		posy=clip_y;

		xc+=sinas*ys;
		yc+=cosas*ys;
	}

	d = (word*)dest->data+posx+posy*dest->pitch;
	for (y=0; y<yl; y++)
	{
		srcx=xc;
		srcy=yc;

		for (x=0; x<xl; x++)
		{
			tempx=(srcx>>16);
			tempy=(srcy>>16);

			if (tempx>=0 && tempx<width && tempy>=0 && tempy<height)
			{

				int sofs=tempx+tempy*width;
				int sc = source[sofs];
				if (sc != transColor)
				{
					sc=(sc & tmask) + (d[x] & tmask);
					d[x] = (sc >> 1);
				}
			}

			srcx+=cosas;
			srcy-=sinas;
		}
		d+=dest->pitch;
		xc+=sinas;
		yc+=cosas;
	}
}


void dd61_RotScale_lucent(int posx, int posy, float angle, float scale, image* src, image *dest)
{
	int xs,ys,xl,yl;
	int srcx,srcy,x,y,tempx,tempy;

	word *source = (word*) src->data, *d;
	int width=src->width;
	int height=src->height;
	int clip_x = dest->cx1;
	int clip_y = dest->cy1;
	int clip_xend = dest->cx2;
	int clip_yend = dest->cy2;
	float ft=atan2((float)width,(float)height);
	int T_WIDTH_CENTER=width>>1;
	int T_HEIGHT_CENTER=height>>1;
	int W_WIDTH=(int)((float)width/scale*sin(ft) + (float)height/scale*cos(ft));
	int W_HEIGHT=W_WIDTH;
	int W_HEIGHT_CENTER=W_HEIGHT>>1;
	int W_WIDTH_CENTER=W_HEIGHT_CENTER;

	int sinas = (int)(sin(-angle)*65536*scale);
	int cosas = (int)(cos(-angle)*65536*scale);

	int xc = T_WIDTH_CENTER*65536 - (W_HEIGHT_CENTER*(cosas+sinas));
	int yc = T_HEIGHT_CENTER*65536 - (W_WIDTH_CENTER*(cosas-sinas));
	posx -= W_WIDTH_CENTER;
	posy -= W_HEIGHT_CENTER;

	// clipping
	if (W_WIDTH<2 || W_HEIGHT<2) return;
	xl=W_WIDTH;
	yl=W_HEIGHT;
	xs=ys=0;
	if (posx>clip_xend || posy>clip_yend || posx+xl<clip_x || posy+yl<clip_y)
		return;
	if (posx+xl > clip_xend) xl=clip_xend-posx+1;
	if (posy+yl > clip_yend) yl=clip_yend-posy+1;
	if (posx<clip_x)
	{
		xs=clip_x-posx;
		xl-=xs;
		posx=clip_x;

		xc+=cosas*xs;
		yc-=sinas*xs;
	}
	if (posy<clip_y)
	{
		ys=clip_y-posy;
		yl-=ys;
		posy=clip_y;

		xc+=sinas*ys;
		yc+=cosas*ys;
	}

	d=(word*) dest->data+posx+posy*dest->pitch;
	for (y=0; y<yl; y++)
	{
		srcx=xc;
		srcy=yc;

		for (x=0; x<xl; x++)
		{
			tempx=(srcx>>16);
			tempy=(srcy>>16);

			if (tempx>=0 && tempx<width && tempy>=0 && tempy<height)
			{
				int sofs=tempx+tempy*width;
				if (source[sofs] != transColor)
				{
					word r1, g1, b1;
					word r2, g2, b2;
					word dp;

					int sc = source[sofs];
					dp = d[x];

					r1 = (sc & 0x001F),
					g1 = (sc & 0x07E0) >> 5,
					b1 = (sc & 0xF800) >> 11;

					r2 = (dp & 0x001F),
					g2 = (dp & 0x07E0) >> 5,
					b2 = (dp & 0xF800) >> 11;

					dp = ((((r1 * ialpha) + (r2 * alpha)) / 100)) |
						 ((((g1 * ialpha) + (g2 * alpha)) / 100) << 5) |
						 ((((b1 * ialpha) + (b2 * alpha)) / 100) << 11);

					d[x] = dp;
				}
			}

			srcx+=cosas;
			srcy-=sinas;
		}
		d+=dest->pitch;
		xc+=sinas;
		yc+=cosas;
	}
}


void dd61_Silhouette(int x, int y, int c, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
			if (s[x] != transColor) d[x]=c;
		s+=spitch;
		d+=dpitch;
	}
}


void dd61_Silhouette_50lucent(int x, int y, int c, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			word sc = s[x];
			if (sc == transColor) continue;
	        sc=(c & tmask) + (d[x] & tmask);
			d[x] = (sc >> 1);
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd61_Silhouette_lucent(int x, int y, int c, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			word r1, g1, b1;
			word r2, g2, b2;
			word dp;

			if (s[x] == transColor) continue;
			dp = d[x];

			r1 = (c & 0x001F),
			g1 = (c & 0x07E0) >> 5,
			b1 = (c & 0xF800) >> 11;

			r2 = (dp & 0x001F),
			g2 = (dp & 0x07E0) >> 5,
			b2 = (dp & 0xF800) >> 11;

			d[x] = ((((r1 * ialpha) + (r2 * alpha)) / 100)) |
				   ((((g1 * ialpha) + (g2 * alpha)) / 100) << 5) |
				   ((((b1 * ialpha) + (b2 * alpha)) / 100) << 11);
		}
		s+=spitch;
		d+=dpitch;
	}
}

#ifndef NOTIMELESS
void dd61_Timeless(int x, int y1, int y, image *src, image *dest)
{
	quad xofs = x < 0 ? (256-x) % 256 : x % 256;
	quad rot = x < 0 ? (320-x) % 320 : x % 320;
	int yofs = (y1)%256;
	word ofs = 0;

	word *s=(word *)src->data,
	 	 *d=(word *)dest->data+(y*dest->pitch);

	int chunkindex;
	int lutindex;
	int startofs = (yofs<<8) | xofs;
	for(int i=0;i<4;i++)
	{
		chunkindex = rot+i;
		for(int xx=0;xx<80;xx++)
		{
			lutindex = (chunkindex%320)*100;
			ofs = startofs;
			for(int yy=0;yy<100;yy++)
			{
				ofs += timeless[lutindex] + skewlines[yy];
				lutindex++;
				d[yy*dest->pitch+xx*4+i] = s[ofs];
				d[(199-yy)*dest->pitch+xx*4+i] = s[ofs];
			}
			chunkindex += 4;
		}
	}
}
#endif


void dd61_BlitWrap(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,c,
		 *d=(word *)dest->data;
	int dpitch=dest->pitch;
	int dwidth=dest->width;

	for (int yy=0; yy < src->height; yy++)
	{
		for (int xx=0; xx < src->width; xx++)
		{
			c=s[(yy*src->pitch)+xx];
			if (c != transColor) d[(((yy+y)%dest->height)*dpitch)+((xx+x)%dwidth)]=c;
		}
	}
}

void dd61_BlitWrap_lucent(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,c;
	int dwidth=dest->width;
	int dheight=dest->height;

	for (int yy=0; yy < src->height; yy++)
	{
		for (int xx=0; xx < src->width; xx++)
		{
			c=s[(yy*src->pitch)+xx];
			if (c != transColor) PutPixel((xx+x)%dwidth, (yy+y)%dheight, c, dest);
		}
	}
}

void dd61_AlphaBlit(int x, int y, image *src, image *alpha, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data,
		 *a=(word *)alpha->data;
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
/*	for (x=0; x<xlen; x++)
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
*/
			word r1, g1, b1;
			word r2, g2, b2;
			word dp;

			word c = s[x];
			word pa = (a[x] & 0x07E0) >> 5;
			word ipa = 63-pa;
			if (!pa) continue;
			if (pa == 63)
			{
				d[x] = s[x];
				continue;
			}
			dp = d[x];

			r1 = (c & 0x001F),
			g1 = (c & 0x07E0) >> 5,
			b1 = (c & 0xF800) >> 11;

			r2 = (dp & 0x001F),
			g2 = (dp & 0x07E0) >> 5,
			b2 = (dp & 0xF800) >> 11;

			d[x] = ((((r1 * pa) + (r2 * ipa)) / 63)) |
				   ((((g1 * pa) + (g2 * ipa)) / 63) << 5) |
				   ((((b1 * pa) + (b2 * ipa)) / 63) << 11);
		}
		s+=spitch;
		d+=dpitch;
		a+=apitch;
	}
}

void dd61_FlipBlit(int x, int y, int fx, int fy, image *src, image *dest)
{
	if(!fx && !fy) dd61_Blit(x,y,src,dest);

	word *s=(word *)src->data,
		*d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	if(fx && !fy)
		for (; ylen--; s+=spitch, d+=dpitch)
			for(int i=0;i<xlen;i++)
				d[i]=s[xlen-i-1];

	else if(!fx && fy)
	{
		s+=spitch*(ylen-1);
		for (xlen *= 2; ylen--; s-=spitch, d+=dpitch)
			memcpy(d, s, xlen);
	}
	else if(fx && fy)
	{
		s+=spitch*(ylen-1);
		for (; ylen--; s-=spitch, d+=dpitch)
			for(int i=0;i<xlen;i++)
				d[i]=s[xlen-i-1];

	}
}

image *dd61_ImageFrom8bpp(byte *src, int width, int height, byte *pal)
{
	word palconv[256], *p;
	image *b;
	int i;

	b = new image(width, height);
	p = (word *) b->data;
	for (i=0; i<256; i++)
		palconv[i] = MakeColor(pal[i*3], pal[(i*3)+1], pal[(i*3)+2]);
	for (i=0; i<width*height; i++)
		p[i] = palconv[src[i]];
	return b;
}


image *dd61_ImageFrom24bpp(byte *src, int width, int height)
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

image *dd61_ImageFrom32bpp(byte *src, int width, int height)
{
	word *dest;
	image *img;
	int i;
	byte r, g, b;

	img = new image(width, height);
	dest = (word *) img->data;
	for (i=0; i<width*height; i++)
	{
		b = *src++;
		g = *src++;
		r = *src++;
		src++; //alpha
		dest[i] = MakeColor(r,g,b);
	}
	return img;
}
#endif

/********************** 16bpp blitter code **********************/
#if defined(BLITTER_16BPP) || defined(BLITTER_15BPP)

int dd16_MakeColor(int r, int g, int b)
{
	return (((r>>3)<<11)|((g>>2)<<5)|(b>>3));
}

bool dd16_GetColor(int c, int &r, int &g, int &b)
{
//	if (c == transColor) return false;
	b = (c & 0x1F) << 3;
	g = ((c >> 5) & 0x3f) << 2;
	r = ((c >> 11) & 0x1f) << 3;
    return true;
}


void dd16_Clear(int color, image *dest)
{
	word *d = (word *)dest->data;
	int bytes = dest->pitch * dest->height;
	while (bytes--)
		*d++ = color;
}

void dd16_Mosaic(int xf, int yf, image *src)
{
	int x,y,x2,y2,xc,yc,xr,yr;
	int x2max,y2max,xmax,ymax;
	int r,g,b;
	int i,j,c;
	int w,h;
	word *data;
	word color;
	int add,topleft;

	if(!(xf&&yf))
		return;

	data=(word *)src->data;
	w=src->cx2-src->cx1+1;
	h=src->cy2-src->cy1+1;
	xc=w/xf;
	yc=h/yf;
	xr=w%xf;
	yr=h%yf;
	add=src->pitch;

	if(xr)
		xmax=xc+1;
	else
		xmax=xc;
	if(yr)
		ymax=yc+1;
	else
		ymax=yc;

	for(x=0;x<xmax;x++)
	{
		topleft=x*xf+src->cx1+src->cy1*src->pitch;
		for(y=0;y<ymax;y++)
		{
			r=g=b=c=0;
			j=topleft;
			if(y<yc)
				y2max=yf;
			else
				y2max=yr;
			if(x<xc)
				x2max=xf;
			else
				x2max=xr;
			for(y2=0;y2<y2max;y2++)
			{
				i=j;
				for(x2=0;x2<x2max;x2++)
				{
					r+=(data[i]&0xF800)>>8;
					g+=(data[i]&0x07E0)>>3;
					b+=(data[i]&0x1F)<<3;
					i++;
				}
				j+=add;
			}
			j=topleft;
			c=y2max*x2max;
			r/=c;
			g/=c;
			b/=c;
			color=dd16_MakeColor(r,g,b);
			for(y2=0;y2<y2max;y2++)
			{
				i=j;
				for(x2=0;x2<x2max;x2++)
					data[i++]=color;
				j+=add;
			}
			topleft=j;
		}
	}
}



int dd16_ReadPixel(int x, int y, image *source)
{
	word *ptr = (word*)source->data;
	return ptr[(y * source->pitch) + x];
}


void dd16_PutPixel(int x, int y, int color, image *dest)
{
	word *ptr = (word *)dest->data;
	if (x<dest->cx1 || x>dest->cx2 || y<dest->cy1 || y>dest->cy2)
		return;
	ptr[(y * dest->pitch) + x] = color;
}


void dd16_PutPixel_50lucent(int x, int y, int color, image *dest)
{
	quad s, c;
	word *d = (word *) dest->data;

	if (x<dest->cx1 || y<dest->cy1 || x>dest->cx2 || y>dest->cy2)
		return;

	s=d[(y * dest->pitch) + x];
	c=(s & tmask) + (color & tmask);
	d[(y * dest->pitch) + x] = (word) (c >> 1);
}


void dd16_PutPixel_lucent(int x, int y, int color, image *dest)
{
	word *s = (word *) dest -> data + (y * dest -> pitch) + x;
	word r1, g1, b1;
	word r2, g2, b2;
	word d;

	if (x<dest->cx1 || y<dest->cy1 || x>dest->cx2 || y>dest->cy2)
		return;

	r1 = (*s & 0xF800) >> 11,
	g1 = (*s & 0x07E0) >> 5,
	b1 = (*s & 0x001F);

	r2 = (color & 0xF800) >> 11,
	g2 = (color & 0x07E0) >> 5,
	b2 = (color & 0x001F);

	d =
		((((r1 * alpha) + (r2 * ialpha)) / 100) << 11) |
		((((g1 * alpha) + (g2 * ialpha)) / 100) << 5) |
		((((b1 * alpha) + (b2 * ialpha)) / 100));

	*s = d;
}


void dd16_HLine(int x, int y, int xe, int color, image *dest)
{
	word *d = (word *) dest->data;
	int cx1=0, cy1=0, cx2=0, cy2=0;
	if (xe<x) SWAP(x,xe);
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || xe<cx1 || y<cy1)
		return;
	if (xe>cx2) xe=cx2;
	if (x<cx1)  x =cx1;

	d += (y * dest->pitch) + x;
	for (; x<=xe; x++)
		*d++ = color;
}


void dd16_HLine_50lucent(int x, int y, int xe, int color, image *dest)
{
	word *d = (word *)dest->data;
	int cx1=0, cy1=0, cx2=0, cy2=0;
	int s;
	if (xe<x) SWAP(x,xe);
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || xe<cx1 || y<cy1)
		return;

	if (xe>cx2) xe=cx2;
	if (x<cx1)  x =cx1;

	d+=(y*dest->pitch)+x;
	for (; x<=xe; x++)
	{
		s=*d;
		s=(s & tmask) + (color & tmask);
		*d++ = (word) (s >> 1);
	}
}


void dd16_HLine_lucent(int x, int y, int xe, int color, image *dest)
{
	word *d = (word *)dest->data;
	int cx1=0, cy1=0, cx2=0, cy2=0;
	if (xe<x) SWAP(x,xe);
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || xe<cx1 || y<cy1)
		return;

	if (xe>cx2) xe=cx2;
	if (x<cx1)  x =cx1;

	d+=(y*dest->pitch)+x;
	for (; x<=xe; x++)
		PutPixel(x, y, color, dest);
}


void dd16_VLine(int x, int y, int ye, int color, image *dest)
{
	word *d = (word *) dest->data;
	int cx1=0, cy1=0, cx2=0, cy2=0;
	if (ye<y) SWAP(y,ye);
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x<cx1 || ye<cy1)
		return;
	if (ye>cy2) ye=cy2;
	if (y<cy1)  y =cy1;

	d += (y * dest->pitch) + x;
	for (; y<=ye; y++, d+=dest->pitch)
		*d = color;
}


void dd16_VLine_50lucent(int x, int y, int ye, int color, image *dest)
{
	word *d = (word *) dest->data;
	int cx1=0, cy1=0, cx2=0, cy2=0;
	if (ye<y) SWAP(y,ye);
	int s;
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x<cx1 || ye<cy1)
		return;
	if (ye>cy2) ye=cy2;
	if (y<cy1)  y =cy1;

	d += (y * dest->pitch) + x;
	for (; y<=ye; y++, d+=dest->pitch)
	{
		s=*d;
		s=(s & tmask) + (color & tmask);
		*d = (word) (s >> 1);
	}
}


void dd16_VLine_lucent(int x, int y, int ye, int color, image *dest)
{
	word *d = (word *)dest->data;
	int cx1=0, cy1=0, cx2=0, cy2=0;
	if (ye<y) SWAP(y,ye);
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x<cx1 || ye<cy1)
		return;

	if (ye>cy2) ye=cy2;
	if (y<cy1)  y =cy1;

	d+=(y*dest->pitch)+x;
	for (; y<=ye; y++)
		PutPixel(x, y, color, dest);
}

void dd16_AddBlit(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp5[2];
	int dp6[2];
	int yi;

	dp5[1]=0x1F;
	dp6[1]=0x3F;

	for(yi=0;yi<ylen;yi++)
	{
#ifndef DDBLIT_ASM
		int xi, b, g, r;
		for(xi=0;xi<xlen;xi++)
		{
			dp5[0]=((d[xi]&0x1F)+(s[xi]&0x1F));
			b=dp5[dp5[0]>>5];
			dp6[0]=((d[xi]&0x07E0)+(s[xi]&0x07E0))>>5;
			g=dp6[dp6[0]>>6];
			dp5[0]=((d[xi]&0xF800)>>11)+((s[xi]&0xF800)>>11);
			r=dp5[dp5[0]>>5];
			d[xi]=(r<<11)|(g<<5)|b;
		}
#else
		__asm
		{
			mov esi,s;
			mov edi,d;
			mov ecx,xlen;

			push ebp;

			dd16lab0:
			lodsw;
			mov dx,[edi];
			mov bx,ax;
			mov bp,dx;
			ror ebx,16;
			mov bx,ax;
			ror eax,16;


			//eax src,???
			//ebx src,src
			//dx  dest
			//bp  dest

			//b
			xor al,al;
			and bx,0x001F;
			and dx,0x001F;
			shl bx,3;
			shl dx,3;
			add dl,bl;
			adc al,0;
			neg al;
			//at this point ah is either 00000000 or 11111111
			or al,dl;
			//go ahead and put BBBBB in the bottom bits
			shr al,3;

			//g
			xor ah,ah;
			ror ebx,16;
			mov dx,bp;
			and bx,0x07E0;
			and dx,0x07E0;
			shl bx,5;
			shl dx,5;
			add dh,bh;
			adc ah,0;
			shl ah,2;
			neg ah;
			//at this point ah is either 00000000 or 11111100
			or dh,ah;

			//go ahead and put 00000GGGGGGBBBBB in ax
			shr dx,5;
			xor ah,ah;
			or ax,dx;

			//r
			//the 00000GGGGGGBBBBB word will now be in the top word of eax
			ror eax,16;
			mov bx,ax;
			xor ah,ah;
			mov dx,bp;
			and bx,0xF800;
			and dx,0xF800;
			add dh,bh;
			adc ah,0;
			shl ah,3;
			neg ah;
			//at this point ah is either 00000000 or 11111000
			or dh,ah;
			//now DH only has RRRRR00000000000


			//build new rgb value
			//rotate the 00000GGGGGGBBBBB word back into AX
			ror eax,16;
			//OR it with dh=RRRRR00000000000
			or ah,dh;

			//store new rgb value
			stosw;

			//next x;
			dec ecx;
			jnz  dd16lab0;
			//loop dd16lab0;

			pop ebp;
		}
#endif
		s+=spitch;
		d+=dpitch;
	}
}


void dd16_AddBlit_50lucent(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp5[2];
	int dp6[2];
	int r,g,b;
	int xi,yi;

	dp5[1]=0x1F;
	dp6[1]=0x3F;

	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			dp5[0]=((d[xi]&0x1F)+((s[xi]&0x1F)/2));
			b=dp5[dp5[0]>>5];
			dp6[0]=((d[xi]&0x07E0)+((s[xi]&0x07E0)/2))>>5;
			g=dp6[dp6[0]>>6];
			dp5[0]=((d[xi]&0xF800)>>11)+(((s[xi]&0xF800)>>11)/2);
			r=dp5[dp5[0]>>5];
			d[xi]=(r<<11)|(g<<5)|b;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd16_AddBlit_lucent(int x, int y, image *src, image *dest)
{
	if(!ialpha)
		return;

	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp5[2];
	int dp6[2];
	int r,g,b;
	int xi,yi;

	dp5[1]=0x1F;
	dp6[1]=0x3F;

	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			dp5[0]=((d[xi]&0x1F)+(((int)(s[xi]&0x1F)*ialpha)/100));
			b=dp5[dp5[0]>>5];
			dp6[0]=((d[xi]&0x07E0)+(((int)(s[xi]&0x07E0)*ialpha)/100))>>5;
			g=dp6[dp6[0]>>6];
			dp5[0]=((d[xi]&0xF800)>>11)+(((int)((s[xi]&0xF800)*ialpha)/100)>>11);
			r=dp5[dp5[0]>>5];
			d[xi]=(r<<11)|(g<<5)|b;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd16_TAddBlit(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp5[2];
	int dp6[2];
	int r,g,b;
	int xi,yi;

	dp5[1]=0x1F;
	dp6[1]=0x3F;

	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			if (s[xi] == transColor) continue;
			dp5[0]=((d[xi]&0x1F)+(s[xi]&0x1F));
			b=dp5[dp5[0]>>5];
			dp6[0]=((d[xi]&0x07E0)+(s[xi]&0x07E0))>>5;
			g=dp6[dp6[0]>>6];
			dp5[0]=((d[xi]&0xF800)>>11)+((s[xi]&0xF800)>>11);
			r=dp5[dp5[0]>>5];
			d[xi]=(r<<11)|(g<<5)|b;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd16_TAddBlit_50lucent(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp5[2];
	int dp6[2];
	int r,g,b;
	int xi,yi;

	dp5[1]=0x1F;
	dp6[1]=0x3F;

	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			if (s[xi] == transColor) continue;
			dp5[0]=((d[xi]&0x1F)+((s[xi]&0x1F)/2));
			b=dp5[dp5[0]>>5];
			dp6[0]=((d[xi]&0x07E0)+((s[xi]&0x07E0)/2))>>5;
			g=dp6[dp6[0]>>6];
			dp5[0]=((d[xi]&0xF800)>>11)+(((s[xi]&0xF800)>>11)/2);
			r=dp5[dp5[0]>>5];
			d[xi]=(r<<11)|(g<<5)|b;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd16_TAddBlit_lucent(int x, int y, image *src, image *dest)
{
	if(!ialpha)
		return;

	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp5[2];
	int dp6[2];
	int r,g,b;
	int xi,yi;

	dp5[1]=0x1F;
	dp6[1]=0x3F;

	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			if (s[xi] == transColor) continue;
			dp5[0]=((d[xi]&0x1F)+(((int)(s[xi]&0x1F)*ialpha)/100));
			b=dp5[dp5[0]>>5];
			dp6[0]=((d[xi]&0x07E0)+(((int)(s[xi]&0x07E0)*ialpha)/100))>>5;
			g=dp6[dp6[0]>>6];
			dp5[0]=((d[xi]&0xF800)>>11)+(((int)((s[xi]&0xF800)*ialpha)/100)>>11);
			r=dp5[dp5[0]>>5];
			d[xi]=(r<<11)|(g<<5)|b;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd16_SubtractBlit(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp[2];
	int r,g,b;
	int xi,yi;

	dp[1]=0;

	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			dp[0]=((d[xi]&0x1F)-(s[xi]&0x1F));
			b=dp[(dp[0]>>31)&1];
			dp[0]=((d[xi]&0x07E0)-(s[xi]&0x07E0))>>5;
			g=dp[(dp[0]>>31)&1];
			dp[0]=((d[xi]&0xF800)-(s[xi]&0xF800))>>11;
			r=dp[(dp[0]>>31)&1];
			d[xi]=(r<<11)|(g<<5)|b;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd16_SubtractBlit_50lucent(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp[2];
	int r,g,b;
	int xi,yi;

	dp[1]=0;

	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			dp[0]=((d[xi]&0x1F)-(s[xi]&0x1F)/2);
			b=dp[(dp[0]>>31)&1];
			dp[0]=((d[xi]&0x07E0)-(s[xi]&0x07E0)/2)>>5;
			g=dp[(dp[0]>>31)&1];
			dp[0]=((d[xi]&0xF800)-(s[xi]&0xF800)/2)>>11;
			r=dp[(dp[0]>>31)&1];
			d[xi]=(r<<11)|(g<<5)|b;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd16_SubtractBlit_lucent(int x, int y, image *src, image *dest)
{
	if(!ialpha)
		return;
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp[2];
	int r,g,b;
	int xi,yi;

	dp[1]=0;

	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			dp[0]=((d[xi]&0x1F)-((int)(s[xi]&0x1F)*ialpha)/100);
			b=dp[(dp[0]>>31)&1];
			dp[0]=((d[xi]&0x07E0)-((int)(s[xi]&0x07E0)*ialpha)/100)>>5;
			g=dp[(dp[0]>>31)&1];
			dp[0]=((d[xi]&0xF800)-((int)(s[xi]&0xF800)*ialpha)/100)>>11;
			r=dp[(dp[0]>>31)&1];
			d[xi]=(r<<11)|(g<<5)|b;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd16_TSubtractBlit(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp[2];
	int r,g,b;
	int xi,yi;

	dp[1]=0;

	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			if (s[xi] == transColor) continue;
			dp[0]=((d[xi]&0x1F)-(s[xi]&0x1F));
			b=dp[(dp[0]>>31)&1];
			dp[0]=((d[xi]&0x07E0)-(s[xi]&0x07E0))>>5;
			g=dp[(dp[0]>>31)&1];
			dp[0]=((d[xi]&0xF800)-(s[xi]&0xF800))>>11;
			r=dp[(dp[0]>>31)&1];
			d[xi]=(r<<11)|(g<<5)|b;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd16_TSubtractBlit_50lucent(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp[2];
	int r,g,b;
	int xi,yi;

	dp[1]=0;

	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			if (s[xi] == transColor) continue;
			dp[0]=((d[xi]&0x1F)-(s[xi]&0x1F)/2);
			b=dp[(dp[0]>>31)&1];
			dp[0]=((d[xi]&0x07E0)-(s[xi]&0x07E0)/2)>>5;
			g=dp[(dp[0]>>31)&1];
			dp[0]=((d[xi]&0xF800)-(s[xi]&0xF800)/2)>>11;
			r=dp[(dp[0]>>31)&1];
			d[xi]=(r<<11)|(g<<5)|b;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd16_TSubtractBlit_lucent(int x, int y, image *src, image *dest)
{
	if(!ialpha)
		return;
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp[2];
	int r,g,b;
	int xi,yi;

	dp[1]=0;

	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			if(s[xi]==transColor)continue;
			dp[0]=((d[xi]&0x1F)-((int)(s[xi]&0x1F)*ialpha)/100);
			b=dp[(dp[0]>>31)&1];
			dp[0]=((d[xi]&0x07E0)-((int)(s[xi]&0x07E0)*ialpha)/100)>>5;
			g=dp[(dp[0]>>31)&1];
			dp[0]=((d[xi]&0xF800)-((int)(s[xi]&0xF800)*ialpha)/100)>>11;
			r=dp[(dp[0]>>31)&1];
			d[xi]=(r<<11)|(g<<5)|b;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd16_Blit(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;
	for (xlen *= 2; ylen--; s+=spitch, d+=dpitch)
		memcpy(d, s, xlen);
}


void dd16_Blit_50lucent(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;
	int sc;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			sc=s[x];
	        sc=(sc & tmask) + (d[x] & tmask);
			d[x] = (word) (sc >> 1);
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd16_Blit_lucent(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			word r1, g1, b1;
			word r2, g2, b2;
			word dp;

			int c = s[x];
			dp = d[x];

			r1 = (c & 0xF800) >> 11,
			g1 = (c & 0x07E0) >> 5,
			b1 = (c & 0x001F);

			r2 = (dp & 0xF800) >> 11,
			g2 = (dp & 0x07E0) >> 5,
			b2 = (dp & 0x001F);

			dp = ((((r1 * ialpha) + (r2 * alpha)) / 100) << 11) |
				 ((((g1 * ialpha) + (g2 * alpha)) / 100) << 5) |
				 ((((b1 * ialpha) + (b2 * alpha)) / 100));

			d[x] = dp;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd16_TBlit(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,c,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			c=s[x];
			if (c != transColor) d[x]=c;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd16_TBlit_50lucent(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;
	int sc;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			sc=s[x]; if (sc == transColor) continue;
	        sc=(sc & tmask) + (d[x] & tmask);
			d[x] = (word) (sc >> 1);
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd16_TBlit_lucent(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			word r1, g1, b1;
			word r2, g2, b2;
			word dp;

			int c = s[x];
			if (c == transColor) continue;
			dp = d[x];

			r1 = (c & 0xF800) >> 11,
			g1 = (c & 0x07E0) >> 5,
			b1 = (c & 0x001F);

			r2 = (dp & 0xF800) >> 11,
			g2 = (dp & 0x07E0) >> 5,
			b2 = (dp & 0x001F);

			dp = ((((r1 * ialpha) + (r2 * alpha)) / 100) << 11) |
				 ((((g1 * ialpha) + (g2 * alpha)) / 100) << 5) |
				 ((((b1 * ialpha) + (b2 * alpha)) / 100));

			d[x] = dp;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd16_BlitTile(int x, int y, char *src, image *dest)
{
	word *s=(word *) src,
		 *d=(word *) dest->data;
	int spitch=16,
		dpitch=dest->pitch;
	int xlen=16,
		ylen=16;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d+=(y*dest->pitch)+x;
	for (xlen*=2; ylen--; s+=spitch,d+=dpitch)
    	memcpy(d, s, xlen);
}


void dd16_TBlitTile(int x, int y, char *src, image *dest)
{
	word *s=(word *) src,
		 *d=(word *) dest->data;
	int spitch=16,
		dpitch=dest->pitch;
	int xlen=16,
		ylen=16;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;
	word c;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;

	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x = cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y=cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			c=s[x];
			if (c != transColor) d[x]=c;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd16_TBlitTile_lucent(int x, int y, char *src, image *dest)
{
	word *s=(word *) src,
		 *d=(word *) dest->data;
	int spitch=16,
		dpitch=dest->pitch;
	int xlen=16,
		ylen=16;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;

	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x = cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y=cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			word r1, g1, b1;
			word r2, g2, b2;
			word dp;

			int c = s[x];
			if (c == transColor) continue;
			dp = d[x];

			r1 = (c & 0xF800) >> 11,
			g1 = (c & 0x07E0) >> 5,
			b1 = (c & 0x001F);

			r2 = (dp & 0xF800) >> 11,
			g2 = (dp & 0x07E0) >> 5,
			b2 = (dp & 0x001F);

			dp = ((((r1 * ialpha) + (r2 * alpha)) / 100) << 11) |
				 ((((g1 * ialpha) + (g2 * alpha)) / 100) << 5) |
				 ((((b1 * ialpha) + (b2 * alpha)) / 100));

			d[x] = dp;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd16_ScaleBlit(int x, int y, int dw, int dh, image *src, image *dest)
{
	int i, j;
	int xerr, yerr;
	int xerr_start, yerr_start;
	int xadj, yadj;
	word *d, *s;
	int xl, yl, xs, ys;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	if (dw < 1 || dh < 1)
		return;

	xl = dw;
	yl = dh;
	xs = ys = 0;
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x > cx2 || y > cy2
	|| x + xl < cx1 || y + yl < cy1)
		return;
	if (x + xl > cx2)
		xl = cx2 - x + 1;
	if (y + yl > cy2)
		yl = cy2 - y + 1;
	if (x < cx1) { xs = cx1 - x; xl -= xs; x = cx1; }
	if (y < cy1) { ys = cy1 - y; yl -= ys; y = cy1; }

	xadj = (src->width << 16)/dw;
	yadj = (src->height << 16)/dh;
	xerr_start = xadj * xs;
	yerr_start = yadj * ys;

	s = (word *) src->data;
	s += ((yerr_start >> 16) * src->pitch);
	d = ((word *) dest->data) + (y * dest->pitch) + x;
	yerr = yerr_start & 0xffff;

	for (i = 0; i < yl; i += 1)
	{
		xerr = xerr_start;
		for (j = 0; j < xl; j += 1)
		{
			d[j] = s[(xerr >> 16)];
			xerr += xadj;
		}
		d    += dest->pitch;
		yerr += yadj;
		s    += (yerr>>16)*src->pitch;
		yerr &= 0xffff;
	}
}


void dd16_ScaleBlit_50lucent(int x, int y, int dw, int dh, image *src, image *dest)
{
	int i, j;
	int xerr, yerr;
	int xerr_start, yerr_start;
	int xadj, yadj;
	word *d, *s, c;
	int xl, yl, xs, ys;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	if (dw < 1 || dh < 1)
		return;

	xl = dw;
	yl = dh;
	xs = ys = 0;
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x > cx2 || y > cy2
	|| x + xl < cx1 || y + yl < cy1)
		return;
	if (x + xl > cx2)
		xl = cx2 - x + 1;
	if (y + yl > cy2)
		yl = cy2 - y + 1;
	if (x < cx1) { xs = cx1 - x; xl -= xs; x = cx1; }
	if (y < cy1) { ys = cy1 - y; yl -= ys; y = cy1; }

	xadj = (src->width << 16)/dw;
	yadj = (src->height << 16)/dh;
	xerr_start = xadj * xs;
	yerr_start = yadj * ys;

	s = (word *) src->data;
	s += ((yerr_start >> 16) * src->pitch);
	d = ((word *) dest->data) + (y * dest->pitch) + x;
	yerr = yerr_start & 0xffff;

	for (i = 0; i < yl; i += 1)
	{
		xerr = xerr_start;
		for (j = 0; j < xl; j += 1)
		{
			c = s[xerr >> 16];
			c = (c & tmask) + (d[j] & tmask);
			d[j] = (word) (c >> 1);
			xerr += xadj;
		}
		d    += dest->pitch;
		yerr += yadj;
		s    += (yerr>>16)*src->pitch;
		yerr &= 0xffff;
	}
}


void dd16_ScaleBlit_lucent(int x, int y, int dw, int dh, image *src, image *dest)
{
	int i, j;
	int xerr, yerr;
	int xerr_start, yerr_start;
	int xadj, yadj;
	word *d, *s, c;
	int xl, yl, xs, ys;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	if (dw < 1 || dh < 1)
		return;

	xl = dw;
	yl = dh;
	xs = ys = 0;
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x > cx2 || y > cy2
	|| x + xl < cx1 || y + yl < cy1)
		return;
	if (x + xl > cx2)
		xl = cx2 - x + 1;
	if (y + yl > cy2)
		yl = cy2 - y + 1;
	if (x < cx1) { xs = cx1 - x; xl -= xs; x = cx1; }
	if (y < cy1) { ys = cy1 - y; yl -= ys; y = cy1; }

	xadj = (src->width << 16)/dw;
	yadj = (src->height << 16)/dh;
	xerr_start = xadj * xs;
	yerr_start = yadj * ys;

	s = (word *) src->data;
	s += ((yerr_start >> 16) * src->pitch);
	d = ((word *) dest->data) + (y * dest->pitch) + x;
	yerr = yerr_start & 0xffff;

	for (i = 0; i < yl; i += 1)
	{
		xerr = xerr_start;
		for (j = 0; j < xl; j += 1)
		{
			word r1, g1, b1;
			word r2, g2, b2;
			word dp;

			c = s[xerr>>16];
			dp = d[j];

			r1 = (c & 0xF800) >> 11,
			g1 = (c & 0x07E0) >> 5,
			b1 = (c & 0x001F);

			r2 = (dp & 0xF800) >> 11,
			g2 = (dp & 0x07E0) >> 5,
			b2 = (dp & 0x001F);

			dp = ((((r1 * ialpha) + (r2 * alpha)) / 100) << 11) |
				 ((((g1 * ialpha) + (g2 * alpha)) / 100) << 5) |
				 ((((b1 * ialpha) + (b2 * alpha)) / 100));

			d[j] = dp;
			xerr += xadj;
		}
		d    += dest->pitch;
		yerr += yadj;
		s    += (yerr>>16)*src->pitch;
		yerr &= 0xffff;
	}
}


void dd16_TScaleBlit(int x, int y, int dw, int dh, image *src, image *dest)
{
	int i, j;
	int xerr, yerr;
	int xerr_start, yerr_start;
	int xadj, yadj;
	word *d, *s;
	int xl, yl, xs, ys;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	if (dw < 1 || dh < 1)
		return;

	xl = dw;
	yl = dh;
	xs = ys = 0;
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x > cx2 || y > cy2
	|| x + xl < cx1 || y + yl < cy1)
		return;
	if (x + xl > cx2)
		xl = cx2 - x + 1;
	if (y + yl > cy2)
		yl = cy2 - y + 1;
	if (x < cx1) { xs = cx1 - x; xl -= xs; x = cx1; }
	if (y < cy1) { ys = cy1 - y; yl -= ys; y = cy1; }

	xadj = (src->width << 16)/dw;
	yadj = (src->height << 16)/dh;
	xerr_start = xadj * xs;
	yerr_start = yadj * ys;

	s = (word *) src->data;
	s += ((yerr_start >> 16) * src->pitch);
	d = ((word *) dest->data) + (y * dest->pitch) + x;
	yerr = yerr_start & 0xffff;

	for (i = 0; i < yl; i += 1)
	{
		xerr = xerr_start;
		for (j = 0; j < xl; j += 1)
		{
			int c = s[(xerr >> 16)];
			if (c != transColor)
				d[j] = c;
			xerr += xadj;
		}
		d    += dest->pitch;
		yerr += yadj;
		s    += (yerr>>16)*src->pitch;
		yerr &= 0xffff;
	}
}


void dd16_TScaleBlit_50lucent(int x, int y, int dw, int dh, image *src, image *dest)
{
	int i, j;
	int xerr, yerr;
	int xerr_start, yerr_start;
	int xadj, yadj;
	word *d, *s;
	int xl, yl, xs, ys;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	if (dw < 1 || dh < 1)
		return;

	xl = dw;
	yl = dh;
	xs = ys = 0;
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x > cx2 || y > cy2
	|| x + xl < cx1 || y + yl < cy1)
		return;
	if (x + xl > cx2)
		xl = cx2 - x + 1;
	if (y + yl > cy2)
		yl = cy2 - y + 1;
	if (x < cx1) { xs = cx1 - x; xl -= xs; x = cx1; }
	if (y < cy1) { ys = cy1 - y; yl -= ys; y = cy1; }

	xadj = (src->width << 16)/dw;
	yadj = (src->height << 16)/dh;
	xerr_start = xadj * xs;
	yerr_start = yadj * ys;

	s = (word *) src->data;
	s += ((yerr_start >> 16) * src->pitch);
	d = ((word *) dest->data) + (y * dest->pitch) + x;
	yerr = yerr_start & 0xffff;

	for (i = 0; i < yl; i += 1)
	{
		xerr = xerr_start;
		for (j = 0; j < xl; j += 1)
		{
			int c = s[xerr >> 16];
			if (c != transColor)
			{
				c = (c & tmask) + (d[j] & tmask);
				d[j] = (word) (c >> 1);
			}
			xerr += xadj;
		}
		d    += dest->pitch;
		yerr += yadj;
		s    += (yerr>>16)*src->pitch;
		yerr &= 0xffff;
	}
}


void dd16_TScaleBlit_lucent(int x, int y, int dw, int dh, image *src, image *dest)
{
	int i, j;
	int xerr, yerr;
	int xerr_start, yerr_start;
	int xadj, yadj;
	word *d, *s;
	int xl, yl, xs, ys;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	if (dw < 1 || dh < 1)
		return;

	xl = dw;
	yl = dh;
	xs = ys = 0;
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x > cx2 || y > cy2
	|| x + xl < cx1 || y + yl < cy1)
		return;
	if (x + xl > cx2)
		xl = cx2 - x + 1;
	if (y + yl > cy2)
		yl = cy2 - y + 1;
	if (x < cx1) { xs = cx1 - x; xl -= xs; x = cx1; }
	if (y < cy1) { ys = cy1 - y; yl -= ys; y = cy1; }

	xadj = (src->width << 16)/dw;
	yadj = (src->height << 16)/dh;
	xerr_start = xadj * xs;
	yerr_start = yadj * ys;

	s = (word *) src->data;
	s += ((yerr_start >> 16) * src->pitch);
	d = ((word *) dest->data) + (y * dest->pitch) + x;
	yerr = yerr_start & 0xffff;

	for (i = 0; i < yl; i += 1)
	{
		xerr = xerr_start;
		for (j = 0; j < xl; j += 1)
		{
			word r1, g1, b1;
			word r2, g2, b2;
			word dp;

			int c = s[xerr>>16];
			if (c != transColor)
			{
				dp = d[j];

				r1 = (c & 0xF800) >> 11,
				g1 = (c & 0x07E0) >> 5,
				b1 = (c & 0x001F);

				r2 = (dp & 0xF800) >> 11,
				g2 = (dp & 0x07E0) >> 5,
				b2 = (dp & 0x001F);

				dp = ((((r1 * ialpha) + (r2 * alpha)) / 100) << 11) |
					 ((((g1 * ialpha) + (g2 * alpha)) / 100) << 5) |
					 ((((b1 * ialpha) + (b2 * alpha)) / 100));

				d[j] = dp;
			}
			xerr += xadj;
		}
		d    += dest->pitch;
		yerr += yadj;
		s    += (yerr>>16)*src->pitch;
		yerr &= 0xffff;
	}
}


void dd16_WrapBlit(int x, int y, image *src, image *dst)
{
	int i;
	int cliph, clipw;
	int curx, cury;
	int spanx, spany;
	word *source, *dest;

	cliph = dst->cy2 - dst->cy1 + 1;
	clipw = dst->cx2 - dst->cx1 + 1;
	y %= src->height;
	if (y<0) y = src->height+y;
	curx = 0;

	do
	{
		x %= curx ? 1 : src->width;
		if (x<0) x = src->width+x;
		spanx = src->width - x;
		if (curx + spanx >= clipw)
			spanx = clipw - curx;
		source = (word *) src -> data + (y * src->pitch) + x;
		dest = (word *) dst -> data + (dst->cy1 * dst->pitch) + dst->cx1 + curx;
		cury = 0;

		do
		{
			spany = src->height - (cury ? 0 : y);
			if (cury + spany >= cliph)
				spany = cliph - cury;

			for (i = 0; i < spany; i++, source += src->pitch, dest += dst->pitch)
				memcpy(dest, source, spanx*2);

			source = (word *) src->data + x;
			cury += spany;
		} while (cury < cliph);
		curx +=	spanx;
	} while (curx < clipw);
}


void dd16_WrapBlit_50lucent(int x, int y, image *src, image *dst)
{
	int i;
	int cliph, clipw;
	int curx, cury;
	int spanx, spany;
	word *source, *dest;
	int sc;

	cliph = dst->cy2 - dst->cy1 + 1;
	clipw = dst->cx2 - dst->cx1 + 1;
	y %= src->height;
	if (y<0) y = src->height+y;
	curx = 0;

	do
	{
		x %= curx ? 1 : src->width;
		if (x<0) x = src->width+x;
		spanx = src->width - x;
		if (curx + spanx >= clipw)
			spanx = clipw - curx;
		source = (word *) src -> data + (y * src->pitch) + x;
		dest = (word *) dst -> data + (dst->cy1 * dst->pitch) + dst->cx1 + curx;
		cury = 0;

		do
		{
			spany = src->height - (cury ? 0 : y);
			if (cury + spany >= cliph)
				spany = cliph - cury;

			for (i = 0; i < spany; i++, source += src->pitch, dest += dst->pitch)
			{
				for (int x = 0; x < spanx; x++)
				{
					sc=source[x];
					sc=(sc & tmask) + (dest[x] & tmask);
					dest[x] = (word) (sc >> 1);
				}
			}

			source = (word *) src->data + x;
			cury += spany;
		} while (cury < cliph);
		curx +=	spanx;
	} while (curx < clipw);
}


void dd16_WrapBlit_lucent(int x, int y, image *src, image *dst)
{
	int i;
	int cliph, clipw;
	int curx, cury;
	int spanx, spany;
	word *source, *dest;
	int sc;

	cliph = dst->cy2 - dst->cy1 + 1;
	clipw = dst->cx2 - dst->cx1 + 1;
	y %= src->height;
	if (y<0) y = src->height+y;
	curx = 0;

	do
	{
		x %= curx ? 1 : src->width;
		if (x<0) x = src->width+x;
		spanx = src->width - x;
		if (curx + spanx >= clipw)
			spanx = clipw - curx;
		source = (word *) src -> data + (y * src->pitch) + x;
		dest = (word *) dst -> data + (dst->cy1 * dst->pitch) + dst->cx1 + curx;
		cury = 0;

		do
		{
			spany = src->height - (cury ? 0 : y);
			if (cury + spany >= cliph)
				spany = cliph - cury;

			for (i = 0; i < spany; i++, source += src->pitch, dest += dst->pitch)
			{
				for (int x = 0; x < spanx; x++)
				{
					word r1, g1, b1;
					word r2, g2, b2;
					word d;

					sc=source[x];
					d=dest[x];

					r1 = (sc & 0xF800) >> 11,
					g1 = (sc & 0x07E0) >> 5,
					b1 = (sc & 0x001F);

					r2 = (d & 0xF800) >> 11,
					g2 = (d & 0x07E0) >> 5,
					b2 = (d & 0x001F);

					d = ((((r1 * ialpha) + (r2 * alpha)) / 100) << 11) |
						((((g1 * ialpha) + (g2 * alpha)) / 100) << 5) |
						((((b1 * ialpha) + (b2 * alpha)) / 100));

					dest[x] = d;
				}
			}

			source = (word *) src->data + x;
			cury += spany;
		} while (cury < cliph);
		curx +=	spanx;
	} while (curx < clipw);
}


void dd16_TWrapBlit(int x, int y, image *src, image *dst)
{
	int i;
	int cliph, clipw;
	int curx, cury;
	int spanx, spany;
	word *source, *dest;
	int sc;

	cliph = dst->cy2 - dst->cy1 + 1;
	clipw = dst->cx2 - dst->cx1 + 1;
	y %= src->height;
	if (y<0) y = src->height+y;
	curx = 0;

	do
	{
		x %= curx ? 1 : src->width;
		if (x<0) x = src->width+x;
		spanx = src->width - x;
		if (curx + spanx >= clipw)
			spanx = clipw - curx;
		source = (word *) src -> data + (y * src->pitch) + x;
		dest = (word *) dst -> data + (dst->cy1 * dst->pitch) + dst->cx1 + curx;
		cury = 0;

		do
		{
			spany = src->height - (cury ? 0 : y);
			if (cury + spany >= cliph)
				spany = cliph - cury;

			for (i = 0; i < spany; i++, source += src->pitch, dest += dst->pitch)
			{
				for (int x = 0; x < spanx; x++)
				{
					sc=source[x];
					if (sc != transColor)
						dest[x] = sc;
				}
			}

			source = (word *) src->data + x;
			cury += spany;
		} while (cury < cliph);
		curx +=	spanx;
	} while (curx < clipw);
}


void dd16_TWrapBlit_50lucent(int x, int y, image *src, image *dst)
{
	int i;
	int cliph, clipw;
	int curx, cury;
	int spanx, spany;
	word *source, *dest;
	int sc;

	cliph = dst->cy2 - dst->cy1 + 1;
	clipw = dst->cx2 - dst->cx1 + 1;
	y %= src->height;
	if (y<0) y = src->height+y;
	curx = 0;

	do
	{
		x %= curx ? 1 : src->width;
		if (x<0) x = src->width+x;
		spanx = src->width - x;
		if (curx + spanx >= clipw)
			spanx = clipw - curx;
		source = (word *) src -> data + (y * src->pitch) + x;
		dest = (word *) dst -> data + (dst->cy1 * dst->pitch) + dst->cx1 + curx;
		cury = 0;

		do
		{
			spany = src->height - (cury ? 0 : y);
			if (cury + spany >= cliph)
				spany = cliph - cury;

			for (i = 0; i < spany; i++, source += src->pitch, dest += dst->pitch)
			{
				for (int x = 0; x < spanx; x++)
				{
					sc=source[x];
					if (sc == transColor) continue;
					sc=(sc & tmask) + (dest[x] & tmask);
					dest[x] = (word) (sc >> 1);
				}
			}

			source = (word *) src->data + x;
			cury += spany;
		} while (cury < cliph);
		curx +=	spanx;
	} while (curx < clipw);
}


void dd16_TWrapBlit_lucent(int x, int y, image *src, image *dst)
{
	int i;
	int cliph, clipw;
	int curx, cury;
	int spanx, spany;
	word *source, *dest;
	int sc;

	cliph = dst->cy2 - dst->cy1 + 1;
	clipw = dst->cx2 - dst->cx1 + 1;
	y %= src->height;
	if (y<0) y = src->height+y;
	curx = 0;

	do
	{
		x %= curx ? 1 : src->width;
		if (x<0) x = src->width+x;
		spanx = src->width - x;
		if (curx + spanx >= clipw)
			spanx = clipw - curx;
		source = (word *) src -> data + (y * src->pitch) + x;
		dest = (word *) dst -> data + (dst->cy1 * dst->pitch) + dst->cx1 + curx;
		cury = 0;

		do
		{
			spany = src->height - (cury ? 0 : y);
			if (cury + spany >= cliph)
				spany = cliph - cury;

			for (i = 0; i < spany; i++, source += src->pitch, dest += dst->pitch)
			{
				for (int x = 0; x < spanx; x++)
				{
					word r1, g1, b1;
					word r2, g2, b2;
					word d;

					sc=source[x];
					d=dest[x];
					if (sc == transColor) continue;

					r1 = (sc & 0xF800) >> 11,
					g1 = (sc & 0x07E0) >> 5,
					b1 = (sc & 0x001F);

					r2 = (d & 0xF800) >> 11,
					g2 = (d & 0x07E0) >> 5,
					b2 = (d & 0x001F);

					d = ((((r1 * ialpha) + (r2 * alpha)) / 100) << 11) |
						((((g1 * ialpha) + (g2 * alpha)) / 100) << 5) |
						((((b1 * ialpha) + (b2 * alpha)) / 100));

					dest[x] = d;
				}
			}

			source = (word *) src->data + x;
			cury += spany;
		} while (cury < cliph);
		curx +=	spanx;
	} while (curx < clipw);
}


void dd16_RotScale(int posx, int posy, float angle, float scale, image* src, image *dest)
{
	int xs,ys,xl,yl;
	int srcx,srcy,x,y,tempx,tempy;

	word *source = (word*) src->data, *d;
	int width=src->width;
	int height=src->height;
	int clip_x = dest->cx1;
	int clip_y = dest->cy1;
	int clip_xend = dest->cx2;
	int clip_yend = dest->cy2;
	float ft=atan2((float)width,(float)height);
	int T_WIDTH_CENTER=width>>1;
	int T_HEIGHT_CENTER=height>>1;
	int W_WIDTH=(int)((float)width/scale*sin(ft) + (float)height/scale*cos(ft));
	int W_HEIGHT=W_WIDTH;
	int W_HEIGHT_CENTER=W_HEIGHT>>1;
	int W_WIDTH_CENTER=W_HEIGHT_CENTER;

	int sinas = (int)(sin(-angle)*65536*scale);
	int cosas = (int)(cos(-angle)*65536*scale);

	int xc = T_WIDTH_CENTER*65536 - (W_HEIGHT_CENTER*(cosas+sinas));
	int yc = T_HEIGHT_CENTER*65536 - (W_WIDTH_CENTER*(cosas-sinas));
	posx -= W_WIDTH_CENTER;
	posy -= W_HEIGHT_CENTER;

	// clipping
	if (W_WIDTH<2 || W_HEIGHT<2) return;
	xl=W_WIDTH;
	yl=W_HEIGHT;
	xs=ys=0;
	if (posx>clip_xend || posy>clip_yend || posx+xl<clip_x || posy+yl<clip_y)
		return;
	if (posx+xl > clip_xend) xl=clip_xend-posx+1;
	if (posy+yl > clip_yend) yl=clip_yend-posy+1;
	if (posx<clip_x)
	{
		xs=clip_x-posx;
		xl-=xs;
		posx=clip_x;

		xc+=cosas*xs;
		yc-=sinas*xs;
	}
	if (posy<clip_y)
	{
		ys=clip_y-posy;
		yl-=ys;
		posy=clip_y;

		xc+=sinas*ys;
		yc+=cosas*ys;
	}

	d = (word*)dest->data+posx+posy*dest->pitch;
	for (y=0; y<yl; y++)
	{
		srcx=xc;
		srcy=yc;

		for (x=0; x<xl; x++)
		{
			tempx=(srcx>>16);
			tempy=(srcy>>16);

			if (tempx>=0 && tempx<width && tempy>=0 && tempy<height)
			{
				int sofs=tempx+tempy*width;
				if (source[sofs] != transColor)
					d[x]=source[sofs];
			}

			srcx+=cosas;
			srcy-=sinas;
		}
		d+=dest->pitch;
		xc+=sinas;
		yc+=cosas;
	}
}


void dd16_RotScale_50lucent(int posx, int posy, float angle, float scale, image* src, image *dest)
{
	int xs,ys,xl,yl;
	int srcx,srcy,x,y,tempx,tempy;

	word *source = (word*) src->data, *d;
	int width=src->width;
	int height=src->height;
	int clip_x = dest->cx1;
	int clip_y = dest->cy1;
	int clip_xend = dest->cx2;
	int clip_yend = dest->cy2;
	float ft=atan2((float)width,(float)height);
	int T_WIDTH_CENTER=width>>1;
	int T_HEIGHT_CENTER=height>>1;
	int W_WIDTH=(int)((float)width/scale*sin(ft) + (float)height/scale*cos(ft));
	int W_HEIGHT=W_WIDTH;
	int W_HEIGHT_CENTER=W_HEIGHT>>1;
	int W_WIDTH_CENTER=W_HEIGHT_CENTER;

	int sinas = (int)(sin(-angle)*65536*scale);
	int cosas = (int)(cos(-angle)*65536*scale);

	int xc = T_WIDTH_CENTER*65536 - (W_HEIGHT_CENTER*(cosas+sinas));
	int yc = T_HEIGHT_CENTER*65536 - (W_WIDTH_CENTER*(cosas-sinas));
	posx -= W_WIDTH_CENTER;
	posy -= W_HEIGHT_CENTER;

	// clipping
	if (W_WIDTH<2 || W_HEIGHT<2) return;
	xl=W_WIDTH;
	yl=W_HEIGHT;
	xs=ys=0;
	if (posx>clip_xend || posy>clip_yend || posx+xl<clip_x || posy+yl<clip_y)
		return;
	if (posx+xl > clip_xend) xl=clip_xend-posx+1;
	if (posy+yl > clip_yend) yl=clip_yend-posy+1;
	if (posx<clip_x)
	{
		xs=clip_x-posx;
		xl-=xs;
		posx=clip_x;

		xc+=cosas*xs;
		yc-=sinas*xs;
	}
	if (posy<clip_y)
	{
		ys=clip_y-posy;
		yl-=ys;
		posy=clip_y;

		xc+=sinas*ys;
		yc+=cosas*ys;
	}

	d = (word*)dest->data+posx+posy*dest->pitch;
	for (y=0; y<yl; y++)
	{
		srcx=xc;
		srcy=yc;

		for (x=0; x<xl; x++)
		{
			tempx=(srcx>>16);
			tempy=(srcy>>16);

			if (tempx>=0 && tempx<width && tempy>=0 && tempy<height)
			{

				int sofs=tempx+tempy*width;
				int sc = source[sofs];
				if (sc != transColor)
				{
					sc=(sc & tmask) + (d[x] & tmask);
					d[x] = (sc >> 1);
				}
			}

			srcx+=cosas;
			srcy-=sinas;
		}
		d+=dest->pitch;
		xc+=sinas;
		yc+=cosas;
	}
}


void dd16_RotScale_lucent(int posx, int posy, float angle, float scale, image* src, image *dest)
{
	int xs,ys,xl,yl;
	int srcx,srcy,x,y,tempx,tempy;

	word *source = (word*) src->data, *d;
	int width=src->width;
	int height=src->height;
	int clip_x = dest->cx1;
	int clip_y = dest->cy1;
	int clip_xend = dest->cx2;
	int clip_yend = dest->cy2;
	float ft=atan2((float)width,(float)height);
	int T_WIDTH_CENTER=width>>1;
	int T_HEIGHT_CENTER=height>>1;
	int W_WIDTH=(int)((float)width/scale*sin(ft) + (float)height/scale*cos(ft));
	int W_HEIGHT=W_WIDTH;
	int W_HEIGHT_CENTER=W_HEIGHT>>1;
	int W_WIDTH_CENTER=W_HEIGHT_CENTER;

	int sinas = (int)(sin(-angle)*65536*scale);
	int cosas = (int)(cos(-angle)*65536*scale);

	int xc = T_WIDTH_CENTER*65536 - (W_HEIGHT_CENTER*(cosas+sinas));
	int yc = T_HEIGHT_CENTER*65536 - (W_WIDTH_CENTER*(cosas-sinas));
	posx -= W_WIDTH_CENTER;
	posy -= W_HEIGHT_CENTER;

	// clipping
	if (W_WIDTH<2 || W_HEIGHT<2) return;
	xl=W_WIDTH;
	yl=W_HEIGHT;
	xs=ys=0;
	if (posx>clip_xend || posy>clip_yend || posx+xl<clip_x || posy+yl<clip_y)
		return;
	if (posx+xl > clip_xend) xl=clip_xend-posx+1;
	if (posy+yl > clip_yend) yl=clip_yend-posy+1;
	if (posx<clip_x)
	{
		xs=clip_x-posx;
		xl-=xs;
		posx=clip_x;

		xc+=cosas*xs;
		yc-=sinas*xs;
	}
	if (posy<clip_y)
	{
		ys=clip_y-posy;
		yl-=ys;
		posy=clip_y;

		xc+=sinas*ys;
		yc+=cosas*ys;
	}

	d=(word*) dest->data+posx+posy*dest->pitch;
	for (y=0; y<yl; y++)
	{
		srcx=xc;
		srcy=yc;

		for (x=0; x<xl; x++)
		{
			tempx=(srcx>>16);
			tempy=(srcy>>16);

			if (tempx>=0 && tempx<width && tempy>=0 && tempy<height)
			{
				int sofs=tempx+tempy*width;
				if (source[sofs] != transColor)
				{
					word r1, g1, b1;
					word r2, g2, b2;
					word dp;

					int sc = source[sofs];
					dp = d[x];

					r1 = (sc & 0xF800) >> 11,
					g1 = (sc & 0x07E0) >> 5,
					b1 = (sc & 0x001F);

					r2 = (dp & 0xF800) >> 11,
					g2 = (dp & 0x07E0) >> 5,
					b2 = (dp & 0x001F);

					dp = ((((r1 * ialpha) + (r2 * alpha)) / 100) << 11) |
						 ((((g1 * ialpha) + (g2 * alpha)) / 100) << 5) |
						 ((((b1 * ialpha) + (b2 * alpha)) / 100));

					d[x] = dp;
				}
			}

			srcx+=cosas;
			srcy-=sinas;
		}
		d+=dest->pitch;
		xc+=sinas;
		yc+=cosas;
	}
}


void dd16_Silhouette(int x, int y, int c, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
			if (s[x] != transColor) d[x]=c;
		s+=spitch;
		d+=dpitch;
	}
}


void dd16_Silhouette_50lucent(int x, int y, int c, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			word sc = s[x];
			if (sc == transColor) continue;
	        sc=(c & tmask) + (d[x] & tmask);
			d[x] = (sc >> 1);
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd16_Silhouette_lucent(int x, int y, int c, image *src, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			word r1, g1, b1;
			word r2, g2, b2;
			word dp;

			if (s[x] == transColor) continue;
			dp = d[x];

			r1 = (c & 0xF800) >> 11,
			g1 = (c & 0x07E0) >> 5,
			b1 = (c & 0x001F);

			r2 = (dp & 0xF800) >> 11,
			g2 = (dp & 0x07E0) >> 5,
			b2 = (dp & 0x001F);

			d[x] = ((((r1 * ialpha) + (r2 * alpha)) / 100) << 11) |
				   ((((g1 * ialpha) + (g2 * alpha)) / 100) << 5) |
				   ((((b1 * ialpha) + (b2 * alpha)) / 100));
		}
		s+=spitch;
		d+=dpitch;
	}
}

#ifndef NOTIMELESS
void dd16_Timeless(int x, int y1, int y, image *src, image *dest)
{
	quad xofs = x < 0 ? (256-x) % 256 : x % 256;
	quad rot = x < 0 ? (320-x) % 320 : x % 320;
	int yofs = (y1)%256;
	word ofs = 0;

	word *s=(word *)src->data,
	 	 *d=(word *)dest->data+(y*dest->pitch);

	int chunkindex;
	int lutindex;
	int startofs = (yofs<<8) | xofs;
	for(int i=0;i<4;i++)
	{
		chunkindex = rot+i;
		for(int xx=0;xx<80;xx++)
		{
			lutindex = (chunkindex%320)*100;
			ofs = startofs;
			for(int yy=0;yy<100;yy++)
			{
				ofs += timeless[lutindex] + skewlines[yy];
				lutindex++;
				d[yy*dest->pitch+xx*4+i] = s[ofs];
				d[(199-yy)*dest->pitch+xx*4+i] = s[ofs];
			}
			chunkindex += 4;
		}
	}
}
#endif


void dd16_BlitWrap(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,c,
		 *d=(word *)dest->data;
	int dpitch=dest->pitch;
	int dwidth=dest->width;

	for (int yy=0; yy < src->height; yy++)
	{
		for (int xx=0; xx < src->width; xx++)
		{
			c=s[(yy*src->pitch)+xx];
			if (c != transColor) d[(((yy+y)%dest->height)*dpitch)+((xx+x)%dwidth)]=c;
		}
	}
}

void dd16_BlitWrap_lucent(int x, int y, image *src, image *dest)
{
	word *s=(word *)src->data,c;
	int dwidth=dest->width;
	int dheight=dest->height;

	for (int yy=0; yy < src->height; yy++)
	{
		for (int xx=0; xx < src->width; xx++)
		{
			c=s[(yy*src->pitch)+xx];
			if (c != transColor) PutPixel((xx+x)%dwidth, (yy+y)%dheight, c, dest);
		}
	}
}

void dd16_AlphaBlit(int x, int y, image *src, image *alpha, image *dest)
{
	word *s=(word *)src->data,
		 *d=(word *)dest->data,
		 *a=(word *)alpha->data;
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
/*	for (x=0; x<xlen; x++)
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
*/
			word r1, g1, b1;
			word r2, g2, b2;
			word dp;

			word c = s[x];
			word pa = (a[x] & 0x07E0) >> 5;
			word ipa = 63-pa;
			if (!pa) continue;
			if (pa == 63)
			{
				d[x] = s[x];
				continue;
			}
			dp = d[x];

			r1 = (c & 0xF800) >> 11,
			g1 = (c & 0x07E0) >> 5,
			b1 = (c & 0x001F);

			r2 = (dp & 0xF800) >> 11,
			g2 = (dp & 0x07E0) >> 5,
			b2 = (dp & 0x001F);

			d[x] = ((((r1 * pa) + (r2 * ipa)) / 63) << 11) |
				   ((((g1 * pa) + (g2 * ipa)) / 63) << 5) |
				   ((((b1 * pa) + (b2 * ipa)) / 63));
		}
		s+=spitch;
		d+=dpitch;
		a+=apitch;
	}
}

void dd16_FlipBlit(int x, int y, int fx, int fy, image *src, image *dest)
{
	if(!fx && !fy) dd16_Blit(x,y,src,dest);

	word *s=(word *)src->data,
		*d=(word *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	if(fx && !fy)
		for (; ylen--; s+=spitch, d+=dpitch)
			for(int i=0;i<xlen;i++)
				d[i]=s[xlen-i-1];

	else if(!fx && fy)
	{
		s+=spitch*(ylen-1);
		for (xlen *= 2; ylen--; s-=spitch, d+=dpitch)
			memcpy(d, s, xlen);
	}
	else if(fx && fy)
	{
		s+=spitch*(ylen-1);
		for (; ylen--; s-=spitch, d+=dpitch)
			for(int i=0;i<xlen;i++)
				d[i]=s[xlen-i-1];

	}
}

image *dd16_ImageFrom8bpp(byte *src, int width, int height, byte *pal)
{
	word palconv[256], *p;
	image *b;
	int i;

	b = new image(width, height);
	p = (word *) b->data;
	for (i=0; i<256; i++)
		palconv[i] = MakeColor(pal[i*3], pal[(i*3)+1], pal[(i*3)+2]);
	for (i=0; i<width*height; i++)
		p[i] = palconv[src[i]];
	return b;
}


image *dd16_ImageFrom24bpp(byte *src, int width, int height)
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

image *dd16_ImageFrom32bpp(byte *src, int width, int height)
{
	word *dest;
	image *img;
	int i;
	byte r, g, b;

	img = new image(width, height);
	dest = (word *) img->data;
	for (i=0; i<width*height; i++)
	{
		b = *src++;
		g = *src++;
		r = *src++;
		src++; //alpha
		dest[i] = MakeColor(r,g,b);
	}
	return img;
}

#endif  //defined(BLITTER_16BPP) || defined(BLITTER_15BPP)
/********************** 32bpp blitter code **********************/
#ifdef BLITTER_32BPP

int dd32_MakeColor(int r, int g, int b)
{
	return ((r<<16)|(g<<8)|b);
}


bool dd32_GetColor(int c, int &r, int &g, int &b)
{
//	if (c == transColor) return false;
	b = c & 0xff;
	g = (c >> 8) & 0xff;
	r = (c >> 16) & 0xff;
    return true;
}


void dd32_Clear(int color, image *dest)
{
	int *d = (int *)dest->data;
	int bytes = dest->pitch * dest->height;
	while (bytes--)
		*d++ = color;
}


int dd32_ReadPixel(int x, int y, image *source)
{
	quad *ptr = (quad*)source->data;
	return ptr[(y * source->pitch) + x];
}


void dd32_PutPixel(int x, int y, int color, image *dest)
{
	int *ptr = (int *)dest->data;
	if (x<dest->cx1 || x>dest->cx2 || y<dest->cy1 || y>dest->cy2)
		return;
	ptr[(y * dest->pitch) + x] = color;
}


void dd32_PutPixel_50lucent(int x, int y, int color, image *dest)
{
	quad s, c;
	int *d=(int *)dest->data;

	if (x<dest->cx1 || x>dest->cx2 || y<dest->cy1 || y>dest->cy2)
		return;

	s=d[(y * dest->pitch) + x];
	c=(s & tmask) + (color & tmask);
	d[(y * dest->pitch) + x] = (int) (c >> 1);
}


void dd32_PutPixel_lucent(int x, int y, int color, image *dest)
{
	byte *d, *c;

	if (x<dest->cx1 || x>dest->cx2 || y<dest->cy1 || y>dest->cy2)
		return;

	c = (byte *) &color;
	d = (byte *) dest->data;
	d += ((y*dest->pitch)+x)<<2;
#ifdef __APPLE__
#ifdef __BIG_ENDIAN__
	d++;
	c++;
#endif
#endif
	*d = ((*d * alpha) + (*c * ialpha)) / 100; d++; c++;
	*d = ((*d * alpha) + (*c * ialpha)) / 100; d++; c++;
	*d = ((*d * alpha) + (*c * ialpha)) / 100;

}


void dd32_HLine(int x, int y, int xe, int color, image *dest)
{
	int *d = (int *) dest->data;
	int cx1=0, cy1=0, cx2=0, cy2=0;
	if (xe<x) SWAP(x,xe);
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || xe<cx1 || y<cy1)
		return;
	if (xe>cx2) xe=cx2;
	if (x<cx1)  x =cx1;

	d += (y * dest->pitch) + x;
	for (; x<=xe; x++)
		*d++ = color;
}


void dd32_HLine_50lucent(int x, int y, int xe, int color, image *dest)
{
	int *d = (int *)dest->data;
	int cx1=0, cy1=0, cx2=0, cy2=0;
	int s;
	if (xe<x) SWAP(x,xe);
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || xe<cx1 || y<cy1)
		return;

	if (xe>cx2) xe=cx2;
	if (x<cx1)  x =cx1;

	d+=(y*dest->pitch)+x;
	for (; x<=xe; x++)
	{
		s=*d;
		s=(s & tmask) + (color & tmask);
		*d++ = (s >> 1);
	}
}


void dd32_HLine_lucent(int x, int y, int xe, int color, image *dest)
{
	int *d = (int *)dest->data;
	int cx1=0, cy1=0, cx2=0, cy2=0;
	if (xe<x) SWAP(x,xe);
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || xe<cx1 || y<cy1)
		return;

	if (xe>cx2) xe=cx2;
	if (x<cx1)  x =cx1;

	d+=(y*dest->pitch)+x;
	for (; x<=xe; x++)
		PutPixel(x, y, color, dest);
}


void dd32_VLine(int x, int y, int ye, int color, image *dest)
{
	int *d = (int *) dest->data;
	int cx1=0, cy1=0, cx2=0, cy2=0;
	if (ye<y) SWAP(y,ye);
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x<cx1 || ye<cy1)
		return;
	if (ye>cy2) ye=cy2;
	if (y<cy1)  y =cy1;

	d += (y * dest->pitch) + x;
	for (; y<=ye; y++, d+=dest->pitch)
		*d = color;
}


void dd32_VLine_50lucent(int x, int y, int ye, int color, image *dest)
{
	int *d = (int *)dest->data;
	int cx1=0, cy1=0, cx2=0, cy2=0;
	int s;
	if (ye<y) SWAP(y,ye);
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x<cx1 || ye<cy1)
		return;

	if (ye>cy2) ye=cy2;
	if (y<cy1)  y =cy1;

	d+=(y*dest->pitch)+x;
	for (; y<=ye; y++, d+=dest->pitch)
	{
		s=*d;
		s=(s & tmask) + (color & tmask);
		*d = (s >> 1);
	}
}


void dd32_VLine_lucent(int x, int y, int ye, int color, image *dest)
{
	quad *d = (quad *)dest->data;
	int cx1=0, cy1=0, cx2=0, cy2=0;
	if (ye<y) SWAP(y,ye);
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x<cx1 || ye<cy1)
		return;

	if (ye>cy2) ye=cy2;
	if (y<cy1)  y =cy1;

	d+=(y*dest->pitch)+x;
	for (; y<=ye; y++)
		PutPixel(x, y, color, dest);
}


void dd32_AddBlit(int x, int y, image *src, image *dest)
{
	int *s=(int *)src->data,
		*d=(int *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp8[2];
	int yi;

	dp8[1]=0xFF;


	for(yi=0;yi<ylen;yi++)
	{
#ifndef DDBLIT_ASM
		int xi, b, g, r;
		for(xi=0;xi<xlen;xi++)
		{
			dp8[0]=((d[xi]&0xFF)+(s[xi]&0xFF));
			b=dp8[dp8[0]>>8];
			dp8[0]=((d[xi]&0xFF00)+(s[xi]&0xFF00))>>8;
			g=dp8[dp8[0]>>8];
			dp8[0]=((d[xi]&0x00FF0000)+(s[xi]&0x00FF0000))>>16;
			r=dp8[dp8[0]>>8];

			d[xi]=(r<<16)|(g<<8)|b;
		}
#else
		__asm
		{
			mov esi,s;
			mov edi,d;
			mov ecx,xlen;

			push ebp;

			dd32lab0:
			lodsd;
			mov edx,[edi];
			mov ebx,eax;
			mov ebp,edx;

			//b
			xor eax,eax;
			add dl,bl;
			adc al,0;
			neg al;
			or al,dl;
			mov bl,al;

			//g
			mov edx,ebp;
			xor eax,eax;
			add dh,bh;
			adc ah,0;
			neg ah
			or ah,dh;
			mov bh,ah;

			//r
			mov edx,ebp;
			ror ebx,16;
			shr edx,16;
			xor eax,eax;
			add dl,bl;
			adc al,0
			neg al;
			or al,dl;

			//build new rgb value
			ror ebx,16;
			shl eax,16;
			mov ax,bx;

			//store new rgb value
			stosd;

			//next x
			loop dd32lab0;

			pop ebp;
		}
#endif
		s+=spitch;
		d+=dpitch;
	}
}


void dd32_AddBlit_50lucent(int x, int y, image *src, image *dest)
{
	int *s=(int *)src->data,
		*d=(int *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp8[2];
	int r,g,b;
	int xi,yi;

	dp8[1]=0xFF;


	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			dp8[0]=((d[xi]&0xFF)+(s[xi]&0xFF)/2);
			b=dp8[dp8[0]>>8];
			dp8[0]=((d[xi]&0xFF00)+(s[xi]&0xFF00)/2)>>8;
			g=dp8[dp8[0]>>8];
			dp8[0]=((d[xi]&0x00FF0000)+(s[xi]&0x00FF0000)/2)>>16;
			r=dp8[dp8[0]>>8];

			d[xi]=(r<<16)|(g<<8)|b;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd32_AddBlit_lucent(int x, int y, image *src, image *dest)
{
	if(!ialpha)
		return;

	int *s=(int *)src->data,
		*d=(int *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp8[2];
	int r,g,b;
	int xi,yi;

	dp8[1]=0xFF;


	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			dp8[0]=((d[xi]&0xFF)+((s[xi]&0xFF)*ialpha)/100);
			b=dp8[dp8[0]>>8];
			dp8[0]=((d[xi]&0xFF00)+((s[xi]&0xFF00)*ialpha)/100)>>8;
			g=dp8[dp8[0]>>8];
			dp8[0]=((d[xi]&0x00FF0000)+((s[xi]&0x00FF0000)*ialpha)/100)>>16;
			r=dp8[dp8[0]>>8];

			d[xi]=(r<<16)|(g<<8)|b;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd32_TAddBlit(int x, int y, image *src, image *dest)
{
	int *s=(int *)src->data,
		*d=(int *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp8[2];
	int r,g,b;
	int xi,yi;

	dp8[1]=0xFF;


	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			if (s[xi] == transColor) continue;
			dp8[0]=((d[xi]&0xFF)+(s[xi]&0xFF));
			b=dp8[dp8[0]>>8];
			dp8[0]=((d[xi]&0xFF00)+(s[xi]&0xFF00))>>8;
			g=dp8[dp8[0]>>8];
			dp8[0]=((d[xi]&0x00FF0000)+(s[xi]&0x00FF0000))>>16;
			r=dp8[dp8[0]>>8];
			d[xi]=(r<<16)|(g<<8)|b;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd32_TAddBlit_50lucent(int x, int y, image *src, image *dest)
{
	int *s=(int *)src->data,
		*d=(int *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp8[2];
	int r,g,b;
	int xi,yi;

	dp8[1]=0xFF;


	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			if (s[xi] == transColor) continue;
			dp8[0]=((d[xi]&0xFF)+(s[xi]&0xFF)/2);
			b=dp8[dp8[0]>>8];
			dp8[0]=((d[xi]&0xFF00)+(s[xi]&0xFF00)/2)>>8;
			g=dp8[dp8[0]>>8];
			dp8[0]=((d[xi]&0x00FF0000)+(s[xi]&0x00FF0000)/2)>>16;
			r=dp8[dp8[0]>>8];

			d[xi]=(r<<16)|(g<<8)|b;
		}

		s+=spitch;
		d+=dpitch;

	}
}

void dd32_TAddBlit_lucent(int x, int y, image *src, image *dest)
{
	if(!ialpha)
		return;

	int *s=(int *)src->data,
		*d=(int *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp8[2];
	int r,g,b;
	int xi,yi;

	dp8[1]=0xFF;


	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			if (s[xi] == transColor) continue;
			dp8[0]=((d[xi]&0xFF)+((s[xi]&0xFF)*ialpha)/100);
			b=dp8[dp8[0]>>8];
			dp8[0]=((d[xi]&0xFF00)+((s[xi]&0xFF00)*ialpha)/100)>>8;
			g=dp8[dp8[0]>>8];
			dp8[0]=((d[xi]&0x00FF0000)+((s[xi]&0x00FF0000)*ialpha)/100)>>16;
			r=dp8[dp8[0]>>8];

			d[xi]=(r<<16)|(g<<8)|b;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd32_SubtractBlit(int x, int y, image *src, image *dest)
{
	int *s=(int *)src->data,
		*d=(int *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp8[2];
	int r,g,b;
	int xi,yi;

	dp8[1]=0;


	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			dp8[0]=((d[xi]&0xFF)-(s[xi]&0xFF));
			b=dp8[(dp8[0]>>31)&1];
			dp8[0]=((d[xi]&0xFF00)-(s[xi]&0xFF00))>>8;
			g=dp8[(dp8[0]>>31)&1];
			dp8[0]=((d[xi]&0x00FF0000)-(s[xi]&0x00FF0000))>>16;
			r=dp8[(dp8[0]>>31)&1];

			d[xi]=(r<<16)|(g<<8)|b;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd32_SubtractBlit_50lucent(int x, int y, image *src, image *dest)
{
	int *s=(int *)src->data,
		*d=(int *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp8[2];
	int r,g,b;
	int xi,yi;

	dp8[1]=0x00;


	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			dp8[0]=((d[xi]&0xFF)-(s[xi]&0xFF)/2);
			b=dp8[(dp8[0]>>31)&1];
			dp8[0]=((d[xi]&0xFF00)-(s[xi]&0xFF00)/2)>>8;
			g=dp8[(dp8[0]>>31)&1];
			dp8[0]=((d[xi]&0x00FF0000)-(s[xi]&0x00FF0000)/2)>>16;
			r=dp8[(dp8[0]>>31)&1];

			d[xi]=(r<<16)|(g<<8)|b;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd32_SubtractBlit_lucent(int x, int y, image *src, image *dest)
{
	if(!ialpha)
		return;

	int *s=(int *)src->data,
		*d=(int *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp8[2];
	int r,g,b;
	int xi,yi;

	dp8[1]=0;


	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			dp8[0]=((d[xi]&0xFF)-((s[xi]&0xFF)*ialpha)/100);
			b=dp8[(dp8[0]>>31)&1];
			dp8[0]=((d[xi]&0xFF00)-((s[xi]&0xFF00)*ialpha)/100)>>8;
			g=dp8[(dp8[0]>>31)&1];
			dp8[0]=((d[xi]&0x00FF0000)-((s[xi]&0x00FF0000)*ialpha)/100)>>16;
			r=dp8[(dp8[0]>>31)&1];

			d[xi]=(r<<16)|(g<<8)|b;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd32_TSubtractBlit(int x, int y, image *src, image *dest)
{
	int *s=(int *)src->data,
		*d=(int *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp8[2];
	int r,g,b;
	int xi,yi;

	dp8[1]=0;


	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			if (s[xi] == transColor) continue;
			dp8[0]=((d[xi]&0xFF)-(s[xi]&0xFF));
			b=dp8[(dp8[0]>>31)&1];
			dp8[0]=((d[xi]&0xFF00)-(s[xi]&0xFF00))>>8;
			g=dp8[(dp8[0]>>31)&1];
			dp8[0]=((d[xi]&0x00FF0000)-(s[xi]&0x00FF0000))>>16;
			r=dp8[(dp8[0]>>31)&1];

			d[xi]=(r<<16)|(g<<8)|b;
		}
		s+=spitch;
		d+=dpitch;
	}
}

void dd32_TSubtractBlit_50lucent(int x, int y, image *src, image *dest)
{
	int *s=(int *)src->data,
		*d=(int *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp8[2];
	int r,g,b;
	int xi,yi;

	dp8[1]=0x00;


	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			if (s[xi] == transColor) continue;
			dp8[0]=((d[xi]&0xFF)-(s[xi]&0xFF)/2);
			b=dp8[(dp8[0]>>31)&1];
			dp8[0]=((d[xi]&0xFF00)-(s[xi]&0xFF00)/2)>>8;
			g=dp8[(dp8[0]>>31)&1];
			dp8[0]=((d[xi]&0x00FF0000)-(s[xi]&0x00FF0000)/2)>>16;
			r=dp8[(dp8[0]>>31)&1];

			d[xi]=(r<<16)|(g<<8)|b;
		}

		s+=spitch;
		d+=dpitch;
	}
}

void dd32_TSubtractBlit_lucent(int x, int y, image *src, image *dest)
{
	if(!ialpha)
		return;

	int *s=(int *)src->data,
		*d=(int *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	int dp8[2];
	int r,g,b;
	int xi,yi;

	dp8[1]=0;


	for(yi=0;yi<ylen;yi++)
	{
		for(xi=0;xi<xlen;xi++)
		{
			if (s[xi] == transColor) continue;
			dp8[0]=((d[xi]&0xFF)-((s[xi]&0xFF)*ialpha)/100);
			b=dp8[(dp8[0]>>31)&1];
			dp8[0]=((d[xi]&0xFF00)-((s[xi]&0xFF00)*ialpha)/100)>>8;
			g=dp8[(dp8[0]>>31)&1];
			dp8[0]=((d[xi]&0x00FF0000)-((s[xi]&0x00FF0000)*ialpha)/100)>>16;
			r=dp8[(dp8[0]>>31)&1];

			d[xi]=(r<<16)|(g<<8)|b;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd32_Blit(int x, int y, image *src, image *dest)
{
	int *s=(int *)src->data,
		*d=(int *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;
	for (xlen *= 4; ylen--; s+=spitch, d+=dpitch)
		memcpy(d, s, xlen);
}


void dd32_Blit_50lucent(int x, int y, image *src, image *dest)
{
	int *s=(int *)src->data,
		*d=(int *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;
	int sc;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			sc=s[x];
	        sc=(sc & tmask) + (d[x] & tmask);
			d[x] = (sc >> 1);
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd32_Blit_lucent(int x, int y, image *src, image *dest)
{
	int *s=(int *)src->data,
		*d=(int *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
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
			dp = d[x];

			r1 = (c >> 16) & 0xff,
			g1 = (c >> 8) & 0xff,
			b1 = (c & 0xff);

			r2 = (dp >> 16) & 0xff,
			g2 = (dp >> 8) & 0xff,
			b2 = (dp & 0xff);

			d[x] = ((((r1 * ialpha) + (r2 * alpha)) / 100) << 16) |
		           ((((g1 * ialpha) + (g2 * alpha)) / 100) << 8) |
				   ((((b1 * ialpha) + (b2 * alpha)) / 100));
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd32_TBlit(int x, int y, image *src, image *dest)
{
	int *s=(int *)src->data,c,
		*d=(int *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			c=s[x];
			if (c != transColor) d[x]=c;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd32_TBlit_50lucent(int x, int y, image *src, image *dest)
{
	int *s=(int *)src->data,
		*d=(int *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;
	int sc;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			sc=s[x]; if (sc == transColor) continue;
	        sc=(sc & tmask) + (d[x] & tmask);
			d[x] = (sc >> 1);
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd32_TBlit_lucent(int x, int y, image *src, image *dest)
{
	int *s=(int *)src->data,
		*d=(int *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
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
			if (c == transColor) continue;
			dp = d[x];

			r1 = (c >> 16) & 0xff,
			g1 = (c >> 8) & 0xff,
			b1 = (c & 0xff);

			r2 = (dp >> 16) & 0xff,
			g2 = (dp >> 8) & 0xff,
			b2 = (dp & 0xff);

			d[x] = ((((r1 * ialpha) + (r2 * alpha)) / 100) << 16) |
		           ((((g1 * ialpha) + (g2 * alpha)) / 100) << 8) |
				   ((((b1 * ialpha) + (b2 * alpha)) / 100));
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd32_BlitTile(int x, int y, char *src, image *dest)
{
	quad *s=(quad *) src,
		 *d=(quad *)dest->data;
	int spitch=16,
		dpitch=dest->pitch;
	int xlen=16,
		ylen=16;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d+=(y*dest->pitch)+x;
	for (xlen*=4; ylen--; s+=spitch,d+=dpitch)
    	memcpy(d, s, xlen);
}


void dd32_TBlitTile(int x, int y, char *src, image *dest)
{
	quad *s=(quad *) src,
		 *d=(quad *)dest->data;
	int spitch=16,
		dpitch=dest->pitch;
	int xlen=16,
		ylen=16;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;
	quad c;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;

	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x = cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y=cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			c=s[x];
			if (c != (quad)transColor) d[x]=c;
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd32_TBlitTile_lucent(int x, int y, char *src, image *dest)
{
	quad *s=(quad *) src,
		 *d=(quad *)dest->data;
	int spitch=16,
		dpitch=dest->pitch;
	int xlen=16,
		ylen=16;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;
	quad c;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;

	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x = cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y=cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			c=s[x];
			if (c == (quad)transColor) continue;

			quad r1, g1, b1;
			quad r2, g2, b2;
			quad dp;

			dp = d[x];

			r1 = (c >> 16) & 0xff,
			g1 = (c >> 8) & 0xff,
			b1 = (c & 0xff);

			r2 = (dp >> 16) & 0xff,
			g2 = (dp >> 8) & 0xff,
			b2 = (dp & 0xff);

			d[x] = ((((r1 * ialpha) + (r2 * alpha)) / 100) << 16) |
		           ((((g1 * ialpha) + (g2 * alpha)) / 100) << 8) |
				   ((((b1 * ialpha) + (b2 * alpha)) / 100));
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd32_ScaleBlit(int x, int y, int dw, int dh, image *src, image *dest)
{
	int i, j;
	int xerr, yerr;
	int xerr_start, yerr_start;
	int xadj, yadj;
	quad *d, *s;
	int xl, yl, xs, ys;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	if (dw < 1 || dh < 1)
		return;

	xl = dw;
	yl = dh;
	xs = ys = 0;
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x > cx2 || y > cy2
	|| x + xl < cx1 || y + yl < cy1)
		return;
	if (x + xl > cx2)
		xl = cx2 - x + 1;
	if (y + yl > cy2)
		yl = cy2 - y + 1;
	if (x < cx1) { xs = cx1 - x; xl -= xs; x = cx1; }
	if (y < cy1) { ys = cy1 - y; yl -= ys; y = cy1; }

	xadj = (src->width << 16)/dw;
	yadj = (src->height << 16)/dh;
	xerr_start = xadj * xs;
	yerr_start = yadj * ys;

	s = (quad *) src->data;
	s += ((yerr_start >> 16) * src->pitch);
	d = ((quad *) dest->data) + (y * dest->pitch) + x;
	yerr = yerr_start & 0xffff;

	for (i = 0; i < yl; i += 1)
	{
		xerr = xerr_start;
		for (j = 0; j < xl; j += 1)
		{
			d[j] = s[(xerr >> 16)];
			xerr += xadj;
		}
		d    += dest->pitch;
		yerr += yadj;
		s    += (yerr>>16)*src->pitch;
		yerr &= 0xffff;
	}
}


void dd32_ScaleBlit_50lucent(int x, int y, int dw, int dh, image *src, image *dest)
{
	int i, j;
	int xerr, yerr;
	int xerr_start, yerr_start;
	int xadj, yadj;
	quad *d, *s, c;
	int xl, yl, xs, ys;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	if (dw < 1 || dh < 1)
		return;

	xl = dw;
	yl = dh;
	xs = ys = 0;
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x > cx2 || y > cy2
	|| x + xl < cx1 || y + yl < cy1)

		return;
	if (x + xl > cx2)
		xl = cx2 - x + 1;
	if (y + yl > cy2)
		yl = cy2 - y + 1;
	if (x < cx1) { xs = cx1 - x; xl -= xs; x = cx1; }
	if (y < cy1) { ys = cy1 - y; yl -= ys; y = cy1; }

	xadj = (src->width << 16)/dw;
	yadj = (src->height << 16)/dh;
	xerr_start = xadj * xs;
	yerr_start = yadj * ys;

	s = (quad *) src->data;
	s += ((yerr_start >> 16) * src->pitch);
	d = ((quad *) dest->data) + (y * dest->pitch) + x;
	yerr = yerr_start & 0xffff;

	for (i = 0; i < yl; i += 1)
	{
		xerr = xerr_start;
		for (j = 0; j < xl; j += 1)
		{
			c = s[(xerr >> 16)];
			c = (c & tmask) + (d[j] & tmask);
			d[j] = (c >> 1);
			xerr += xadj;
		}
		d    += dest->pitch;
		yerr += yadj;
		s    += (yerr>>16)*src->pitch;
		yerr &= 0xffff;
	}
}


void dd32_ScaleBlit_lucent(int x, int y, int dw, int dh, image *src, image *dest)
{
	int i, j;
	int xerr, yerr;
	int xerr_start, yerr_start;
	int xadj, yadj;
	quad *d, *s, c;
	int xl, yl, xs, ys;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	if (dw < 1 || dh < 1)
		return;

	xl = dw;
	yl = dh;
	xs = ys = 0;
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x > cx2 || y > cy2 || x + xl < cx1 || y + yl < cy1)
		return;
	if (x + xl > cx2)
		xl = cx2 - x + 1;
	if (y + yl > cy2)
		yl = cy2 - y + 1;
	if (x < cx1) { xs = cx1 - x; xl -= xs; x = cx1; }
	if (y < cy1) { ys = cy1 - y; yl -= ys; y = cy1; }

	xadj = (src->width << 16)/dw;
	yadj = (src->height << 16)/dh;
	xerr_start = xadj * xs;
	yerr_start = yadj * ys;

	s = (quad *) src->data;
	s += ((yerr_start >> 16) * src->pitch);
	d = ((quad *) dest->data) + (y * dest->pitch) + x;
	yerr = yerr_start & 0xffff;

	for (i = 0; i < yl; i += 1)
	{
		xerr = xerr_start;
		for (j = 0; j < xl; j += 1)
		{
			quad r1, g1, b1;
			quad r2, g2, b2;
			quad dp;

			c = s[xerr >> 16];
			dp = d[j];

			r1 = (c >> 16) & 0xff,
			g1 = (c >> 8) & 0xff,
			b1 = (c & 0xff);

			r2 = (dp >> 16) & 0xff,
			g2 = (dp >> 8) & 0xff,
			b2 = (dp & 0xff);

			d[j] = ((((r1 * ialpha) + (r2 * alpha)) / 100) << 16) |
		           ((((g1 * ialpha) + (g2 * alpha)) / 100) << 8) |
				   ((((b1 * ialpha) + (b2 * alpha)) / 100));
			xerr += xadj;
		}
		d    += dest->pitch;
		yerr += yadj;
		s    += (yerr>>16)*src->pitch;
		yerr &= 0xffff;
	}
}


void dd32_TScaleBlit(int x, int y, int dw, int dh, image *src, image *dest)
{
	int i, j;
	int xerr, yerr;
	int xerr_start, yerr_start;
	int xadj, yadj;
	quad *d, *s;
	int xl, yl, xs, ys;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	if (dw < 1 || dh < 1)
		return;

	xl = dw;
	yl = dh;
	xs = ys = 0;
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x > cx2 || y > cy2
	|| x + xl < cx1 || y + yl < cy1)
		return;
	if (x + xl > cx2)
		xl = cx2 - x + 1;
	if (y + yl > cy2)
		yl = cy2 - y + 1;
	if (x < cx1) { xs = cx1 - x; xl -= xs; x = cx1; }
	if (y < cy1) { ys = cy1 - y; yl -= ys; y = cy1; }

	xadj = (src->width << 16)/dw;
	yadj = (src->height << 16)/dh;
	xerr_start = xadj * xs;
	yerr_start = yadj * ys;

	s = (quad *) src->data;
	s += ((yerr_start >> 16) * src->pitch);
	d = ((quad *) dest->data) + (y * dest->pitch) + x;
	yerr = yerr_start & 0xffff;

	for (i = 0; i < yl; i += 1)
	{
		xerr = xerr_start;
		for (j = 0; j < xl; j += 1)
		{
			int c = s[(xerr >> 16)];
			if (c !=	 transColor)
				d[j] = c;
			xerr += xadj;
		}
		d    += dest->pitch;
		yerr += yadj;
		s    += (yerr>>16)*src->pitch;
		yerr &= 0xffff;
	}
}


void dd32_TScaleBlit_50lucent(int x, int y, int dw, int dh, image *src, image *dest)
{
	int i, j;
	int xerr, yerr;
	int xerr_start, yerr_start;
	int xadj, yadj;
	quad *d, *s;
	int xl, yl, xs, ys;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	if (dw < 1 || dh < 1)
		return;

	xl = dw;
	yl = dh;
	xs = ys = 0;
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x > cx2 || y > cy2
	|| x + xl < cx1 || y + yl < cy1)

		return;
	if (x + xl > cx2)
		xl = cx2 - x + 1;
	if (y + yl > cy2)
		yl = cy2 - y + 1;
	if (x < cx1) { xs = cx1 - x; xl -= xs; x = cx1; }
	if (y < cy1) { ys = cy1 - y; yl -= ys; y = cy1; }

	xadj = (src->width << 16)/dw;
	yadj = (src->height << 16)/dh;
	xerr_start = xadj * xs;
	yerr_start = yadj * ys;

	s = (quad *) src->data;
	s += ((yerr_start >> 16) * src->pitch);
	d = ((quad *) dest->data) + (y * dest->pitch) + x;
	yerr = yerr_start & 0xffff;

	for (i = 0; i < yl; i += 1)
	{
		xerr = xerr_start;
		for (j = 0; j < xl; j += 1)
		{
			int c = s[(xerr >> 16)];
			if (c != transColor)
			{
				c = (c & tmask) + (d[j] & tmask);
				d[j] = (c >> 1);
			}
			xerr += xadj;
		}
		d    += dest->pitch;
		yerr += yadj;
		s    += (yerr>>16)*src->pitch;
		yerr &= 0xffff;
	}
}


void dd32_TScaleBlit_lucent(int x, int y, int dw, int dh, image *src, image *dest)
{
	int i, j;
	int xerr, yerr;
	int xerr_start, yerr_start;
	int xadj, yadj;
	quad *d, *s;
	int xl, yl, xs, ys;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	if (dw < 1 || dh < 1)
		return;

	xl = dw;
	yl = dh;
	xs = ys = 0;
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x > cx2 || y > cy2 || x + xl < cx1 || y + yl < cy1)
		return;
	if (x + xl > cx2)
		xl = cx2 - x + 1;
	if (y + yl > cy2)
		yl = cy2 - y + 1;
	if (x < cx1) { xs = cx1 - x; xl -= xs; x = cx1; }
	if (y < cy1) { ys = cy1 - y; yl -= ys; y = cy1; }

	xadj = (src->width << 16)/dw;
	yadj = (src->height << 16)/dh;
	xerr_start = xadj * xs;
	yerr_start = yadj * ys;

	s = (quad *) src->data;
	s += ((yerr_start >> 16) * src->pitch);
	d = ((quad *) dest->data) + (y * dest->pitch) + x;
	yerr = yerr_start & 0xffff;

	for (i = 0; i < yl; i += 1)
	{
		xerr = xerr_start;
		for (j = 0; j < xl; j += 1)
		{
			quad r1, g1, b1;
			quad r2, g2, b2;
			quad dp;

			int c = s[xerr >> 16];
			if (c != transColor)
			{
				dp = d[j];

				r1 = (c >> 16) & 0xff,
				g1 = (c >> 8) & 0xff,
				b1 = (c & 0xff);

				r2 = (dp >> 16) & 0xff,
				g2 = (dp >> 8) & 0xff,
				b2 = (dp & 0xff);

				d[j] = ((((r1 * ialpha) + (r2 * alpha)) / 100) << 16) |
					   ((((g1 * ialpha) + (g2 * alpha)) / 100) << 8) |
					   ((((b1 * ialpha) + (b2 * alpha)) / 100));
			}
			xerr += xadj;
		}
		d    += dest->pitch;
		yerr += yadj;
		s    += (yerr>>16)*src->pitch;
		yerr &= 0xffff;
	}
}


void dd32_WrapBlit(int x, int y, image *src, image *dst)
{
	int i;
	int cliph, clipw;
	int curx, cury;
	int spanx, spany;
	quad *source, *dest;

	cliph = dst->cy2 - dst->cy1 + 1;
	clipw = dst->cx2 - dst->cx1 + 1;
	y %= src->height;
	if (y<0) y = src->height+y;
	curx = 0;

	do
	{
		x %= curx ? 1 : src->width;
		if (x<0) x = src->width+x;
		spanx = src->width - x;
		if (curx + spanx >= clipw)
			spanx = clipw - curx;
		source = (quad *) src -> data + (y * src->pitch) + x;
		dest = (quad *) dst -> data + (dst->cy1 * dst->pitch) + dst->cx1 + curx;
		cury = 0;

		do
		{
			spany = src->height - (cury ? 0 : y);
			if (cury + spany >= cliph)
				spany = cliph - cury;

			for (i = 0; i < spany; i++, source += src->pitch, dest += dst->pitch)
				memcpy(dest, source, spanx*4);

			source = (quad *) src->data + x;
			cury += spany;
		} while (cury < cliph);
		curx +=	spanx;
	} while (curx < clipw);
}


void dd32_WrapBlit_50lucent(int x, int y, image *src, image *dst)
{
	int i;
	int cliph, clipw;
	int curx, cury;
	int spanx, spany;
	quad *source, *dest;
	int sc;

	cliph = dst->cy2 - dst->cy1 + 1;
	clipw = dst->cx2 - dst->cx1 + 1;
	y %= src->height;
	if (y<0) y = src->height+y;
	curx = 0;

	do
	{
		x %= curx ? 1 : src->width;
		if (x<0) x = src->width+x;
		spanx = src->width - x;
		if (curx + spanx >= clipw)
			spanx = clipw - curx;
		source = (quad *) src -> data + (y * src->width) + x;
		dest = (quad *) dst -> data + (dst->cy1 * dst->pitch) + dst->cx1 + curx;
		cury = 0;

		do
		{
			spany = src->height - (cury ? 0 : y);
			if (cury + spany >= cliph)
				spany = cliph - cury;

			for (i = 0; i < spany; i++, source += src->width, dest += dst->pitch)
			{
				for (int x = 0; x < spanx; x++)
				{
					sc=source[x];
					sc=(sc & tmask) + (dest[x] & tmask);
					dest[x] = (sc >> 1);
				}
			}

			source = (quad *) src->data + x;
			cury += spany;
		} while (cury < cliph);
		curx +=	spanx;
	} while (curx < clipw);
}


void dd32_WrapBlit_lucent(int x, int y, image *src, image *dst)
{
	int i;
	int cliph, clipw;
	int curx, cury;
	int spanx, spany;
	quad *source, *dest;
	int sc;

	cliph = dst->cy2 - dst->cy1 + 1;
	clipw = dst->cx2 - dst->cx1 + 1;
	y %= src->height;
	if (y<0) y = src->height+y;
	curx = 0;

	do
	{
		x %= curx ? 1 : src->width;
		if (x<0) x = src->width+x;
		spanx = src->width - x;
		if (curx + spanx >= clipw)
			spanx = clipw - curx;
		source = (quad *) src -> data + (y * src->width) + x;
		dest = (quad *) dst -> data + (dst->cy1 * dst->pitch) + dst->cx1 + curx;
		cury = 0;

		do
		{
			spany = src->height - (cury ? 0 : y);
			if (cury + spany >= cliph)
				spany = cliph - cury;

			for (i = 0; i < spany; i++, source += src->width, dest += dst->pitch)
			{
				for (int x = 0; x < spanx; x++)
				{
					quad r1, g1, b1;
					quad r2, g2, b2;
					quad d;

					sc=source[x];
					d=dest[x];

					r1 = (sc >> 16) & 0xff,
					g1 = (sc >> 8) & 0xff,
					b1 = (sc & 0xff);

					r2 = (d >> 16) & 0xff,
					g2 = (d >> 8) & 0xff,
					b2 = (d & 0xff);

					d = ((((r1 * ialpha) + (r2 * alpha)) / 100) << 16) |
						((((g1 * ialpha) + (g2 * alpha)) / 100) << 8) |
						((((b1 * ialpha) + (b2 * alpha)) / 100));

					dest[x] = d;
				}
			}

			source = (quad *) src->data + x;
			cury += spany;
		} while (cury < cliph);
		curx +=	spanx;
	} while (curx < clipw);
}


void dd32_TWrapBlit(int x, int y, image *src, image *dst)
{
	int i;
	int cliph, clipw;
	int curx, cury;
	int spanx, spany;
	quad *source, *dest;
	int sc;

	cliph = dst->cy2 - dst->cy1 + 1;
	clipw = dst->cx2 - dst->cx1 + 1;
	y %= src->height;
	if (y<0) y = src->height+y;
	curx = 0;

	do
	{
		x %= curx ? 1 : src->width;
		if (x<0) x = src->width+x;
		spanx = src->width - x;
		if (curx + spanx >= clipw)
			spanx = clipw - curx;
		source = (quad *) src -> data + (y * src->width) + x;
		dest = (quad *) dst -> data + (dst->cy1 * dst->pitch) + dst->cx1 + curx;
		cury = 0;

		do
		{
			spany = src->height - (cury ? 0 : y);
			if (cury + spany >= cliph)
				spany = cliph - cury;

			for (i = 0; i < spany; i++, source += src->width, dest += dst->pitch)
			{
				for (int x = 0; x < spanx; x++)
				{
					sc=source[x];
					if (sc != transColor)
						dest[x] = sc;
				}
			}

			source = (quad *) src->data + x;
			cury += spany;
		} while (cury < cliph);
		curx +=	spanx;
	} while (curx < clipw);
}


void dd32_TWrapBlit_50lucent(int x, int y, image *src, image *dst)
{
	int i;
	int cliph, clipw;
	int curx, cury;
	int spanx, spany;
	quad *source, *dest;
	int sc;

	cliph = dst->cy2 - dst->cy1 + 1;
	clipw = dst->cx2 - dst->cx1 + 1;
	y %= src->height;
	if (y<0) y = src->height+y;
	curx = 0;

	do
	{
		x %= curx ? 1 : src->width;
		if (x<0) x = src->width+x;
		spanx = src->width - x;
		if (curx + spanx >= clipw)
			spanx = clipw - curx;
		source = (quad *) src -> data + (y * src->width) + x;
		dest = (quad *) dst -> data + (dst->cy1 * dst->pitch) + dst->cx1 + curx;
		cury = 0;

		do
		{
			spany = src->height - (cury ? 0 : y);
			if (cury + spany >= cliph)
				spany = cliph - cury;

			for (i = 0; i < spany; i++, source += src->width, dest += dst->pitch)
			{
				for (int x = 0; x < spanx; x++)
				{
					sc=source[x];
					if (sc != transColor)
					{
						sc=(sc & tmask) + (dest[x] & tmask);
						dest[x] = (sc >> 1);
					}
				}
			}

			source = (quad *) src->data + x;
			cury += spany;
		} while (cury < cliph);
		curx +=	spanx;
	} while (curx < clipw);
}


void dd32_TWrapBlit_lucent(int x, int y, image *src, image *dst)
{
	int i;
	int cliph, clipw;
	int curx, cury;
	int spanx, spany;
	quad *source, *dest;
	int sc;

	cliph = dst->cy2 - dst->cy1 + 1;
	clipw = dst->cx2 - dst->cx1 + 1;
	y %= src->height;
	if (y<0) y = src->height+y;
	curx = 0;

	do
	{
		x %= curx ? 1 : src->width;
		if (x<0) x = src->width+x;
		spanx = src->width - x;
		if (curx + spanx >= clipw)
			spanx = clipw - curx;
		source = (quad *) src -> data + (y * src->width) + x;
		dest = (quad *) dst -> data + (dst->cy1 * dst->pitch) + dst->cx1 + curx;
		cury = 0;

		do
		{
			spany = src->height - (cury ? 0 : y);
			if (cury + spany >= cliph)
				spany = cliph - cury;

			for (i = 0; i < spany; i++, source += src->width, dest += dst->pitch)
			{
				for (int x = 0; x < spanx; x++)
				{
					quad r1, g1, b1;
					quad r2, g2, b2;
					quad d;

					sc=source[x];
					d=dest[x];
					if (sc == transColor) continue;

					r1 = (sc >> 16) & 0xff,
					g1 = (sc >> 8) & 0xff,
					b1 = (sc & 0xff);

					r2 = (d >> 16) & 0xff,
					g2 = (d >> 8) & 0xff,
					b2 = (d & 0xff);

					d = ((((r1 * ialpha) + (r2 * alpha)) / 100) << 16) |
						((((g1 * ialpha) + (g2 * alpha)) / 100) << 8) |
						((((b1 * ialpha) + (b2 * alpha)) / 100));

					dest[x] = d;
				}
			}

			source = (quad *) src->data + x;
			cury += spany;
		} while (cury < cliph);
		curx +=	spanx;
	} while (curx < clipw);
}


void dd32_RotScale(int posx, int posy, float angle, float scale, image* src, image *dest)
{
	int xs,ys,xl,yl;
	int srcx,srcy,x,y,tempx,tempy;

	quad *source = (quad*) src->data, *d;
	int width=src->width;
	int height=src->height;
	int clip_x = dest->cx1;
	int clip_y = dest->cy1;
	int clip_xend = dest->cx2;
	int clip_yend = dest->cy2;
	float ft=atan2((float)width,(float)height);
	int T_WIDTH_CENTER=width>>1;
	int T_HEIGHT_CENTER=height>>1;
	int W_WIDTH=(int)((float)width/scale*sin(ft) + (float)height/scale*cos(ft));
	int W_HEIGHT=W_WIDTH;
	int W_HEIGHT_CENTER=W_HEIGHT>>1;
	int W_WIDTH_CENTER=W_HEIGHT_CENTER;

	int sinas = (int)(sin(-angle)*65536*scale);
	int cosas = (int)(cos(-angle)*65536*scale);

	int xc = T_WIDTH_CENTER*65536 - (W_HEIGHT_CENTER*(cosas+sinas));
	int yc = T_HEIGHT_CENTER*65536 - (W_WIDTH_CENTER*(cosas-sinas));
	posx -= W_WIDTH_CENTER;
	posy -= W_HEIGHT_CENTER;

	// clipping
	if (W_WIDTH<2 || W_HEIGHT<2) return;
	xl=W_WIDTH;
	yl=W_HEIGHT;
	xs=ys=0;
	if (posx>clip_xend || posy>clip_yend || posx+xl<clip_x || posy+yl<clip_y)
		return;
	if (posx+xl > clip_xend) xl=clip_xend-posx+1;
	if (posy+yl > clip_yend) yl=clip_yend-posy+1;
	if (posx<clip_x)
	{
		xs=clip_x-posx;
		xl-=xs;
		posx=clip_x;

		xc+=cosas*xs;
		yc-=sinas*xs;
	}
	if (posy<clip_y)
	{
		ys=clip_y-posy;
		yl-=ys;
		posy=clip_y;

		xc+=sinas*ys;
		yc+=cosas*ys;
	}

	d=(quad*) dest->data+posx+posy*dest->pitch;
	for (y=0; y<yl; y++)
	{
		srcx=xc;
		srcy=yc;

		for (x=0; x<xl; x++)
		{
			tempx=(srcx>>16);
			tempy=(srcy>>16);

			if (tempx>=0 && tempx<width && tempy>=0 && tempy<height)
			{
				int sofs=tempx+tempy*src->pitch;
				if (source[sofs] != (quad)transColor)
					d[x]=source[sofs];
			}

			srcx+=cosas;
			srcy-=sinas;
		}
		d+=dest->pitch;
		xc+=sinas;
		yc+=cosas;
	}
}


void dd32_RotScale_50lucent(int posx, int posy, float angle, float scale, image* src, image *dest)
{
	int xs,ys,xl,yl;
	int srcx,srcy,x,y,tempx,tempy;

	quad *source = (quad*) src->data, *d;
	int width=src->width;
	int height=src->height;
	int clip_x = dest->cx1;
	int clip_y = dest->cy1;
	int clip_xend = dest->cx2;
	int clip_yend = dest->cy2;
	float ft=atan2((float)width,(float)height);
	int T_WIDTH_CENTER=width>>1;
	int T_HEIGHT_CENTER=height>>1;
	int W_WIDTH=(int)((float)width/scale*sin(ft) + (float)height/scale*cos(ft));
	int W_HEIGHT=W_WIDTH;
	int W_HEIGHT_CENTER=W_HEIGHT>>1;
	int W_WIDTH_CENTER=W_HEIGHT_CENTER;

	int sinas = (int)(sin(-angle)*65536*scale);
	int cosas = (int)(cos(-angle)*65536*scale);

	int xc = T_WIDTH_CENTER*65536 - (W_HEIGHT_CENTER*(cosas+sinas));
	int yc = T_HEIGHT_CENTER*65536 - (W_WIDTH_CENTER*(cosas-sinas));
	posx -= W_WIDTH_CENTER;
	posy -= W_HEIGHT_CENTER;

	// clipping
	if (W_WIDTH<2 || W_HEIGHT<2) return;
	xl=W_WIDTH;
	yl=W_HEIGHT;
	xs=ys=0;
	if (posx>clip_xend || posy>clip_yend || posx+xl<clip_x || posy+yl<clip_y)
		return;
	if (posx+xl > clip_xend) xl=clip_xend-posx+1;
	if (posy+yl > clip_yend) yl=clip_yend-posy+1;
	if (posx<clip_x)
	{
		xs=clip_x-posx;
		xl-=xs;
		posx=clip_x;

		xc+=cosas*xs;
		yc-=sinas*xs;
	}
	if (posy<clip_y)
	{
		ys=clip_y-posy;
		yl-=ys;
		posy=clip_y;

		xc+=sinas*ys;
		yc+=cosas*ys;
	}

	d=(quad*) dest->data+posx+posy*dest->pitch;
	for (y=0; y<yl; y++)
	{
		srcx=xc;
		srcy=yc;

		for (x=0; x<xl; x++)
		{
			tempx=(srcx>>16);
			tempy=(srcy>>16);

			if (tempx>=0 && tempx<width && tempy>=0 && tempy<height)
			{
				int sofs=tempx+tempy*width;
				int sc = source[sofs];
				if (sc != transColor)
				{
					sc=(sc & tmask) + (d[x] & tmask);
					d[x] = (sc >> 1);
				}
			}

			srcx+=cosas;
			srcy-=sinas;
		}
		d+=dest->pitch;
		xc+=sinas;
		yc+=cosas;
	}
}


void dd32_RotScale_lucent(int posx, int posy, float angle, float scale, image* src, image *dest)
{
	int xs,ys,xl,yl;
	int srcx,srcy,x,y,tempx,tempy;

	quad *source = (quad*) src->data, *d;
	int width=src->width;
	int height=src->height;
	int clip_x = dest->cx1;
	int clip_y = dest->cy1;
	int clip_xend = dest->cx2;
	int clip_yend = dest->cy2;
	float ft=atan2((float)width,(float)height);
	int T_WIDTH_CENTER=width>>1;
	int T_HEIGHT_CENTER=height>>1;
	int W_WIDTH=(int)((float)width/scale*sin(ft) + (float)height/scale*cos(ft));
	int W_HEIGHT=W_WIDTH;
	int W_HEIGHT_CENTER=W_HEIGHT>>1;
	int W_WIDTH_CENTER=W_HEIGHT_CENTER;

	int sinas = (int)(sin(-angle)*65536*scale);
	int cosas = (int)(cos(-angle)*65536*scale);

	int xc = T_WIDTH_CENTER*65536 - (W_HEIGHT_CENTER*(cosas+sinas));
	int yc = T_HEIGHT_CENTER*65536 - (W_WIDTH_CENTER*(cosas-sinas));
	posx -= W_WIDTH_CENTER;
	posy -= W_HEIGHT_CENTER;

	// clipping
	if (W_WIDTH<2 || W_HEIGHT<2) return;
	xl=W_WIDTH;
	yl=W_HEIGHT;
	xs=ys=0;
	if (posx>clip_xend || posy>clip_yend || posx+xl<clip_x || posy+yl<clip_y)
		return;
	if (posx+xl > clip_xend) xl=clip_xend-posx+1;
	if (posy+yl > clip_yend) yl=clip_yend-posy+1;
	if (posx<clip_x)
	{
		xs=clip_x-posx;
		xl-=xs;
		posx=clip_x;

		xc+=cosas*xs;
		yc-=sinas*xs;
	}
	if (posy<clip_y)
	{
		ys=clip_y-posy;
		yl-=ys;
		posy=clip_y;

		xc+=sinas*ys;
		yc+=cosas*ys;
	}

	d=(quad*) dest->data+posx+posy*dest->pitch;
	for (y=0; y<yl; y++)
	{
		srcx=xc;
		srcy=yc;

		for (x=0; x<xl; x++)
		{
			tempx=(srcx>>16);
			tempy=(srcy>>16);

			if (tempx>=0 && tempx<width && tempy>=0 && tempy<height)
			{
				int sofs=tempx+tempy*width;
				if (source[sofs] != (quad)transColor)
				{
					quad r1, g1, b1;
					quad r2, g2, b2;
					quad dp;

					int sc = source[sofs];
					dp = d[x];
					if (sc == transColor) continue;

					r1 = (sc >> 16) & 0xff,
					g1 = (sc >> 8) & 0xff,
					b1 = (sc & 0xff);

					r2 = (dp >> 16) & 0xff,
					g2 = (dp >> 8) & 0xff,
					b2 = (dp & 0xff);

					dp = ((((r1 * ialpha) + (r2 * alpha)) / 100) << 16) |
						 ((((g1 * ialpha) + (g2 * alpha)) / 100) << 8) |
						 ((((b1 * ialpha) + (b2 * alpha)) / 100));

					d[x] = dp;
				}
			}

			srcx+=cosas;
			srcy-=sinas;
		}
		d+=dest->pitch;
		xc+=sinas;
		yc+=cosas;
	}
}


void dd32_Silhouette(int x, int y, int c, image *src, image *dest)
{
	int *s=(int *)src->data,
		*d=(int *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
			if (s[x] != transColor) d[x]=c;
		s+=spitch;
		d+=dpitch;
	}
}


void dd32_Silhouette_50lucent(int x, int y, int c, image *src, image *dest)
{
	int *s=(int *)src->data,
		*d=(int *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			quad sc = s[x];
			if (sc == (quad)transColor) continue;
	        sc=(c & tmask) + (d[x] & tmask);
			d[x] = (sc >> 1);
		}
		s+=spitch;
		d+=dpitch;
	}
}


void dd32_Silhouette_lucent(int x, int y, int c, image *src, image *dest)
{
	int *s=(int *)src->data,
		*d=(int *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			quad r1, g1, b1;
			quad r2, g2, b2;
			quad dp;

			int sc = s[x];
			if (sc == transColor) continue;
			dp = d[x];

			r1 = (c >> 16) & 0xff,
			g1 = (c >> 8) & 0xff,
			b1 = (c & 0xff);

			r2 = (dp >> 16) & 0xff,
			g2 = (dp >> 8) & 0xff,
			b2 = (dp & 0xff);

			d[x] = ((((r1 * ialpha) + (r2 * alpha)) / 100) << 16) |
		           ((((g1 * ialpha) + (g2 * alpha)) / 100) << 8) |
				   ((((b1 * ialpha) + (b2 * alpha)) / 100));
		}
		s+=spitch;
		d+=dpitch;
	}
}

void dd15_Mosaic(int xf, int yf, image *src)
{
}

void dd32_Mosaic(int xf, int yf, image *src)
{
	int x,y,x2,y2,xc,yc,xr,yr;
	int x2max,y2max,xmax,ymax;
	int r,g,b;
	int i,j,c;
	int w,h;
	quad *data;
	quad color;
	int add,topleft;

	if(!(xf&&yf))
		return;

 	data=(quad *)src->data;
	w=src->cx2-src->cx1+1;
	h=src->cy2-src->cy1+1;
	xc=w/xf;
	yc=h/yf;
	xr=w%xf;
	yr=h%yf;
	add=src->pitch;

	if(xr)
		xmax=xc+1;
	else
		xmax=xc;
	if(yr)
		ymax=yc+1;
	else
		ymax=yc;

	for(x=0;x<xmax;x++)
	{
		topleft=x*xf+src->cx1+src->cy1*src->pitch;
		for(y=0;y<ymax;y++)
		{
			r=g=b=c=0;
			j=topleft;
			if(y<yc)
				y2max=yf;
			else
				y2max=yr;
			if(x<xc)
				x2max=xf;
			else
				x2max=xr;
			for(y2=0;y2<y2max;y2++)
			{
				i=j;
				for(x2=0;x2<x2max;x2++)
				{
  					b+=data[i]&0x000000FF;
					g+=(data[i]&0x0000FF00)>>8;
					r+=(data[i]&0x00FF0000)>>16;
					i++;
				}
				j+=add;
			}
			c=x2max*y2max;
			j=topleft;
			r/=c;
			g/=c;
			b/=c;
			color=dd32_MakeColor(r,g,b);
			for(y2=0;y2<y2max;y2++)
			{
				i=j;
				for(x2=0;x2<x2max;x2++)
					data[i++]=color;
				j+=add;
			}
			topleft=j;
		}
	}
}

#ifndef NOTIMELESS
void dd32_Timeless(int x, int y1, int y, image *src, image *dest)
{
	quad xofs = x < 0 ? (256-x) % 256 : x % 256;
	quad rot = x < 0 ? (320-x) % 320 : x % 320;
	quad yofs = (y1)%256;
	word ofs = 0;
/*
for (int i=0; i<32000; i++) timeless[i] = 256;
for (int i=0; i<32000; i+=100) timeless[i] = i/100;
//for (int i=0; i<100; i++) skewlines[i] = -5;

FILE *f = fopen ("gnbgtbl.m", "w");
for (int i=0; i<320; i++)
{
	fprintf(f, "dw ");
	for (int j=0; j<99; j++)
	{
		fprintf(f, "%d, ", timeless[(i*100)+j]);
	}
	fprintf(f, "%d \n", timeless[(i*100)+99]);
}

fclose(f);
err("done");*/

	quad *s=(quad *)src->data,
		 *d=(quad *)dest->data+(y*dest->pitch);

	quad chunkindex;
	quad lutindex;
	quad startofs = (yofs<<8) | xofs;
	for(int i=0;i<4;i++)
	{
		chunkindex = rot+i;
		for(int xx=0;xx<80;xx++)
		{
			lutindex = (chunkindex%320)*100;
			ofs = startofs;
			for(int yy=0;yy<100;yy++)
			{
				ofs += timeless[lutindex]+skewlines[yy];
				lutindex++;
				d[yy*dest->pitch+xx*4+i] = s[ofs];
				d[(199-yy)*dest->pitch+xx*4+i] = s[ofs];
			}
			chunkindex += 4;
		}
	}
}
/*
#define PI 3.14159265
void dd32_Timeless(int x, int y1, int y, image *src, image *dest)
{
    quad xofs = x < 0 ? (256-x) % 256 : x % 256;
    quad rot = x < 0 ? (640-x) % 640 : x % 640;
    quad yofs = (y1)%256;
    word ofs = 0;

    quad *s=(quad *)src->data,
         *d=(quad *)dest->data+(y*dest->pitch);

    float curx,cury;

    for(int x=0;x<640;x++)
    {

        curx = (float)xofs;
        cury = (float)yofs;
        for(int y=0;y<240;y++)
        {
            float _y = (float)y/240.0*100.0;
            float factor =
14.35694074889504*exp(-3.78506616195507*(float)_y) +
0.00001183042409*exp(0.14663881138575*(float)_y) + 0.64339851673117;

            curx += sin((float)x/640.0*2.0*PI) * factor;
            cury += sin((float)x/640.0*2.0*PI+0.5*PI) * factor;

            int nx = (int)curx;
            if(nx>=256)             { curx-=256.0; }
            if(nx<0)             { curx+=256.0; }
            int ny = (int)cury;
            if(ny>=256)             { cury-=256.0;}
            if(ny<0)             {cury+=256.0;}

            nx = (int)curx;
            ny = (int)cury;

            d[y*dest->pitch+x] = s[ny*256+nx];
            d[(479-y)*dest->pitch+x] = s[ny*256+nx];
        }
    }
} */
#endif

void dd32_BlitWrap(int x, int y, image *src, image *dest)
{
	quad *s=(quad *)src->data,c,
		 *d=(quad *)dest->data;
	int dpitch=dest->pitch;
	int dwidth=dest->width;
	int dheight=dest->height;

	for (int yy=0; yy < src->height; yy++)
	{
		for (int xx=0; xx < src->width; xx++)
		{
			c=s[(yy*src->pitch)+xx];
			if (c != (quad)transColor) d[(((yy+y)%dheight)*dpitch)+((xx+x)%dwidth)]=c;
		}
	}
}

void dd32_BlitWrap_lucent(int x, int y, image *src, image *dest)
{
	quad *s=(quad *)src->data,c;
	int dwidth=dest->width;
	int dheight=dest->height;

	for (int yy=0; yy < src->height; yy++)
	{
		for (int xx=0; xx < src->width; xx++)
		{
			c=s[(yy*src->pitch)+xx];
			if (c != (quad)transColor) PutPixel((xx+x)%dwidth, (yy+y)%dheight, c, dest);
		}
	}
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

void dd32_FlipBlit(int x, int y, int fx, int fy, image *src, image *dest)
{
	if(!fx && !fy) dd32_Blit(x,y,src,dest);

	int *s=(int *)src->data,
		*d=(int *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen>cx2) xlen = cx2-x+1;
	if (y+ylen>cy2) ylen = cy2-y+1;
	if (x<cx1) 	{
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d += (y * dpitch) + x;

	if(fx && !fy)
		for (; ylen--; s+=spitch, d+=dpitch)
			for(int i=0;i<xlen;i++)
				d[i]=s[xlen-i-1];

	else if(!fx && fy)
	{
		s+=spitch*(ylen-1);
		for (xlen *= 4; ylen--; s-=spitch, d+=dpitch)
			memcpy(d, s, xlen);
	}
	else if(fx && fy)
	{
		s+=spitch*(ylen-1);
		for (; ylen--; s-=spitch, d+=dpitch)
			for(int i=0;i<xlen;i++)
				d[i]=s[xlen-i-1];

	}
}

image *dd32_ImageFrom8bpp(byte *src, int width, int height, byte *pal)
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

image *dd32_ImageFrom24bpp(byte *src, int width, int height)
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

image *dd32_ImageFrom32bpp(byte *src, int width, int height)
{
	quad *dest;
	image *img;
	int i;
	byte r, g, b;

	img = new image(width, height);
	dest = (quad *) img->data;
	for (i=0; i<width*height; i++)
	{
		b = *src++;
		g = *src++;
		r = *src++;
		src++; //alpha
		dest[i] = MakeColor(r,g,b);
	}
	return img;
}

#endif //BLITTER_32BPP
/*********************** blitter managment **********************/

int SetLucent(int percent)
{
	if (percent < 0) percent = 0;
	if (percent > 100) percent = 100;
	int oldalpha = alpha;
	alpha = percent;
	ialpha = 100 - alpha;

	if(alpha == oldalpha) return oldalpha;

	switch (vid_bpp)
	{
		#ifdef BLITTER_15BPP
		case 15:
			log("WARNING: 15bpp blitters initialized");
			if (percent == 0)
			{
				PutPixel		= dd16_PutPixel;
				HLine			= dd16_HLine;
				VLine			= dd16_VLine;
				Blit            = dd16_Blit;
				TBlit           = dd16_TBlit;
				BlitTile        = dd16_BlitTile;
				TBlitTile		= dd16_TBlitTile;
				ScaleBlit       = dd16_ScaleBlit;
				TScaleBlit      = dd16_TScaleBlit;
				WrapBlit        = dd16_WrapBlit;
				TWrapBlit		= dd16_TWrapBlit;
				RotScale		= dd16_RotScale;
				Silhouette		= dd16_Silhouette;
				AdditiveBlit	= dd16_AddBlit;
				TAdditiveBlit	= dd16_TAddBlit;
				SubtractiveBlit	= dd16_SubtractBlit;
				TSubtractiveBlit= dd16_TSubtractBlit;
				BlitWrap		= dd16_BlitWrap;
				return oldalpha;
			}
			else if (percent == 50)
			{
				PutPixel		= dd16_PutPixel_50lucent;
				HLine			= dd16_HLine_50lucent;
				VLine			= dd16_VLine_50lucent;
				Blit            = dd16_Blit_50lucent;
				TBlit			= dd16_TBlit_50lucent;
				ScaleBlit		= dd16_ScaleBlit_50lucent;
				TScaleBlit      = dd16_TScaleBlit_50lucent;
				WrapBlit		= dd16_WrapBlit_50lucent;
				TWrapBlit		= dd16_TWrapBlit_50lucent;
				RotScale		= dd16_RotScale_50lucent;
				Silhouette		= dd16_Silhouette_50lucent;
				AdditiveBlit	= dd16_AddBlit_50lucent;
				TAdditiveBlit	= dd16_TAddBlit_50lucent;
				SubtractiveBlit	= dd16_SubtractBlit_50lucent;
				TSubtractiveBlit= dd16_TSubtractBlit_50lucent;
				BlitWrap		= dd16_BlitWrap_lucent;
				return oldalpha;
			}
			else
			{
				PutPixel		= dd15_PutPixel_lucent;
				Blit			= dd15_Blit_lucent;
				TBlit			= dd15_TBlit_lucent;
				HLine			= dd16_HLine_lucent;
				VLine			= dd16_VLine_lucent;
				ScaleBlit		= dd15_ScaleBlit_lucent;
				TScaleBlit      = dd15_TScaleBlit_lucent;
				WrapBlit		= dd15_WrapBlit_lucent;
				TWrapBlit		= dd15_TWrapBlit_lucent;
				RotScale		= dd15_RotScale_lucent;
				Silhouette		= dd15_Silhouette_lucent;
				AdditiveBlit	= dd16_AddBlit_lucent;
				TAdditiveBlit	= dd16_TAddBlit_lucent;
				SubtractiveBlit	= dd16_SubtractBlit_lucent;
				TSubtractiveBlit= dd16_TSubtractBlit_lucent;
				BlitWrap		= dd16_BlitWrap_lucent;
				return oldalpha;
			}
			break;
		#endif //BLITTER_15BPP
		#ifdef BLITTER_16BPP
		case 16:
			if (percent == 0)
			{
				PutPixel		= dd16_PutPixel;
				HLine			= dd16_HLine;
				VLine			= dd16_VLine;
				Blit            = dd16_Blit;
				TBlit           = dd16_TBlit;;
				BlitTile        = dd16_BlitTile;
				TBlitTile		= dd16_TBlitTile;
				ScaleBlit       = dd16_ScaleBlit;
				TScaleBlit      = dd16_TScaleBlit;
				WrapBlit        = dd16_WrapBlit;
				TWrapBlit		= dd16_TWrapBlit;
				RotScale		= dd16_RotScale;
				Silhouette		= dd16_Silhouette;
				AdditiveBlit	= dd16_AddBlit;
				TAdditiveBlit	= dd16_TAddBlit;
				SubtractiveBlit	= dd16_SubtractBlit;
				TSubtractiveBlit= dd16_TSubtractBlit;
				BlitWrap		= dd16_BlitWrap;
				return oldalpha;
			}
			else if (percent == 50)
			{
				PutPixel		= dd16_PutPixel_50lucent;
				HLine			= dd16_HLine_50lucent;
				VLine			= dd16_VLine_50lucent;
				Blit            = dd16_Blit_50lucent;
				TBlit			= dd16_TBlit_50lucent;
				TBlitTile		= dd16_TBlitTile_lucent;
				ScaleBlit		= dd16_ScaleBlit_50lucent;
				TScaleBlit      = dd16_TScaleBlit_50lucent;
				WrapBlit		= dd16_WrapBlit_50lucent;
				TWrapBlit		= dd16_TWrapBlit_50lucent;
				RotScale		= dd16_RotScale_50lucent;
				Silhouette		= dd16_Silhouette_50lucent;
				AdditiveBlit	= dd16_AddBlit_50lucent;
				TAdditiveBlit	= dd16_TAddBlit_50lucent;
				SubtractiveBlit	= dd16_SubtractBlit_50lucent;
				TSubtractiveBlit= dd16_TSubtractBlit_50lucent;
				BlitWrap		= dd16_BlitWrap_lucent;
				return oldalpha;
			}
			else
			{
				PutPixel		= dd16_PutPixel_lucent;
				HLine			= dd16_HLine_lucent;
				VLine			= dd16_VLine_lucent;
				Blit			= dd16_Blit_lucent;
				TBlit			= dd16_TBlit_lucent;
				TBlitTile		= dd16_TBlitTile_lucent;
				ScaleBlit		= dd16_ScaleBlit_lucent;
				TScaleBlit      = dd16_TScaleBlit_lucent;
				WrapBlit		= dd16_WrapBlit_lucent;
				TWrapBlit		= dd16_TWrapBlit_lucent;
				RotScale		= dd16_RotScale_lucent;
				Silhouette		= dd16_Silhouette_lucent;
				AdditiveBlit	= dd16_AddBlit_lucent;
				TAdditiveBlit	= dd16_TAddBlit_lucent;
				SubtractiveBlit	= dd16_SubtractBlit_lucent;
				TSubtractiveBlit= dd16_TSubtractBlit_lucent;
				BlitWrap		= dd16_BlitWrap_lucent;
				return oldalpha;
			}
			break;
		#endif //BLITTER_16BPP
		#ifdef BLITTER_61BPP
		case 61:
			if (percent == 0)
			{
				PutPixel		= dd61_PutPixel;
				HLine			= dd61_HLine;
				VLine			= dd61_VLine;
				Blit            = dd61_Blit;
				TBlit           = dd61_TBlit;;
				BlitTile        = dd61_BlitTile;
				TBlitTile		= dd61_TBlitTile;
				ScaleBlit       = dd61_ScaleBlit;
				TScaleBlit      = dd61_TScaleBlit;
				WrapBlit        = dd61_WrapBlit;
				TWrapBlit		= dd61_TWrapBlit;
				RotScale		= dd61_RotScale;
				Silhouette		= dd61_Silhouette;
				AdditiveBlit	= dd61_AddBlit;
				TAdditiveBlit	= dd61_TAddBlit;
				SubtractiveBlit	= dd61_SubtractBlit;
				TSubtractiveBlit= dd61_TSubtractBlit;
				BlitWrap		= dd61_BlitWrap;
				return oldalpha;
			}
			else if (percent == 50)
			{
				PutPixel		= dd61_PutPixel_50lucent;
				HLine			= dd61_HLine_50lucent;
				VLine			= dd61_VLine_50lucent;
				Blit            = dd61_Blit_50lucent;
				TBlit			= dd61_TBlit_50lucent;
				TBlitTile		= dd61_TBlitTile_lucent;
				ScaleBlit		= dd61_ScaleBlit_50lucent;
				TScaleBlit      = dd61_TScaleBlit_50lucent;
				WrapBlit		= dd61_WrapBlit_50lucent;
				TWrapBlit		= dd61_TWrapBlit_50lucent;
				RotScale		= dd61_RotScale_50lucent;
				Silhouette		= dd61_Silhouette_50lucent;
				AdditiveBlit	= dd61_AddBlit_50lucent;
				TAdditiveBlit	= dd61_TAddBlit_50lucent;
				SubtractiveBlit	= dd61_SubtractBlit_50lucent;
				TSubtractiveBlit= dd61_TSubtractBlit_50lucent;
				BlitWrap		= dd61_BlitWrap_lucent;
				return oldalpha;
			}
			else
			{
				PutPixel		= dd61_PutPixel_lucent;
				HLine			= dd61_HLine_lucent;
				VLine			= dd61_VLine_lucent;
				Blit			= dd61_Blit_lucent;
				TBlit			= dd61_TBlit_lucent;
				TBlitTile		= dd61_TBlitTile_lucent;
				ScaleBlit		= dd61_ScaleBlit_lucent;
				TScaleBlit      = dd61_TScaleBlit_lucent;
				WrapBlit		= dd61_WrapBlit_lucent;
				TWrapBlit		= dd61_TWrapBlit_lucent;
				RotScale		= dd61_RotScale_lucent;
				Silhouette		= dd61_Silhouette_lucent;
				AdditiveBlit	= dd61_AddBlit_lucent;
				TAdditiveBlit	= dd61_TAddBlit_lucent;
				SubtractiveBlit	= dd61_SubtractBlit_lucent;
				TSubtractiveBlit= dd61_TSubtractBlit_lucent;
				BlitWrap		= dd61_BlitWrap_lucent;
				return oldalpha;
			}
			break;
		#endif //BLITTER_61BPP
		#ifdef BLITTER_32BPP
		case 32:
			if (percent == 0)
			{
				PutPixel		= dd32_PutPixel;
				HLine			= dd32_HLine;
				VLine			= dd32_VLine;
				Blit            = dd32_Blit;
				TBlit           = dd32_TBlit;
				BlitTile        = dd32_BlitTile;
				TBlitTile		= dd32_TBlitTile;
				ScaleBlit       = dd32_ScaleBlit;
				TScaleBlit      = dd32_TScaleBlit;
				WrapBlit        = dd32_WrapBlit;
				TWrapBlit       = dd32_TWrapBlit;
				RotScale		= dd32_RotScale;
				Silhouette		= dd32_Silhouette;
				AdditiveBlit	= dd32_AddBlit;
				TAdditiveBlit	= dd32_TAddBlit;
				SubtractiveBlit	= dd32_SubtractBlit;
				TSubtractiveBlit= dd32_TSubtractBlit;
				BlitWrap		= dd32_BlitWrap;
				return oldalpha;
			}
			else if (percent == 50)
			{
				PutPixel		= dd32_PutPixel_50lucent;
				HLine			= dd32_HLine_50lucent;
				VLine			= dd32_VLine_50lucent;
				Blit            = dd32_Blit_50lucent;
				TBlit			= dd32_TBlit_50lucent;
				TBlitTile       = dd32_TBlitTile_lucent;
				ScaleBlit		= dd32_ScaleBlit_50lucent;
				TScaleBlit      = dd32_TScaleBlit_50lucent;
				WrapBlit		= dd32_WrapBlit_50lucent;
				TWrapBlit       = dd32_TWrapBlit_50lucent;
				RotScale		= dd32_RotScale_50lucent;
				Silhouette		= dd32_Silhouette_50lucent;
				AdditiveBlit	= dd32_AddBlit_50lucent;
				TAdditiveBlit	= dd32_TAddBlit_50lucent;
				SubtractiveBlit	= dd32_SubtractBlit_50lucent;
				TSubtractiveBlit= dd32_TSubtractBlit_50lucent;
				BlitWrap		= dd32_BlitWrap_lucent;
				return oldalpha;
			}
			else
			{
				PutPixel		= dd32_PutPixel_lucent;
				HLine			= dd32_HLine_lucent;
				VLine			= dd32_VLine_lucent;
				Blit			= dd32_Blit_lucent;
				TBlit			= dd32_TBlit_lucent;
				TBlitTile       = dd32_TBlitTile_lucent;
				ScaleBlit		= dd32_ScaleBlit_lucent;
				TScaleBlit      = dd32_TScaleBlit_lucent;
				WrapBlit		= dd32_WrapBlit_lucent;
				TWrapBlit       = dd32_TWrapBlit_lucent;
				RotScale		= dd32_RotScale_lucent;
				Silhouette		= dd32_Silhouette_lucent;
				AdditiveBlit	= dd32_AddBlit_lucent;
				TAdditiveBlit	= dd32_TAddBlit_lucent;
				SubtractiveBlit	= dd32_SubtractBlit_lucent;
				TSubtractiveBlit= dd32_TSubtractBlit_lucent;
				BlitWrap		= dd32_BlitWrap_lucent;
				return oldalpha;
			}
			break;
		#endif //BLITTER_32BPP
		default:
			err("vid_bpp (%d) not a standard value", vid_bpp);
	}
	return percent;
}


void dd_RegisterBlitters()
{
	switch (vid_bpp)
	{
		#ifdef BLITTER_15BPP
		case 15:
			MakeColor		= dd15_MakeColor;
			GetColor		= dd15_GetColor;
			Clear			= dd16_Clear;
			ReadPixel       = dd16_ReadPixel;
			PutPixel		= dd16_PutPixel;
			HLine			= dd16_HLine;
			VLine			= dd16_VLine;
			Line			= dd_Line;
			Box				= dd_Box;
			Rect            = dd_Rect;
			Sphere          = dd_Sphere;
			Circle          = dd_Circle;
			Blit            = dd16_Blit;
			TBlit           = dd16_TBlit;
			AdditiveBlit	= dd16_AddBlit;
			TAdditiveBlit	= dd16_TAddBlit;
			SubtractiveBlit = dd16_SubtractBlit;
			TSubtractiveBlit= dd16_TSubtractBlit;
			BlitTile        = dd16_BlitTile;
			TBlitTile		= dd16_TBlitTile;
			ScaleBlit       = dd16_ScaleBlit;
			TScaleBlit      = dd16_TScaleBlit;
			WrapBlit        = dd16_WrapBlit;
			TWrapBlit		= dd16_TWrapBlit;
			RotScale		= dd16_RotScale;
			Silhouette		= dd16_Silhouette;
			Mosaic			= dd15_Mosaic;
			#ifndef NOTIMELESS
			Timeless        = dd16_Timeless;
			#endif
			BlitWrap		= dd16_BlitWrap;
			ColorFilter     = dd_ColorFilter;
			Triangle		= dd_Triangle;
			AlphaBlit		= dd15_AlphaBlit;
			FlipBlit        = dd16_FlipBlit;
			ImageFrom8bpp	= dd16_ImageFrom8bpp;
			ImageFrom24bpp	= dd16_ImageFrom24bpp;
			ImageFrom32bpp	= dd16_ImageFrom32bpp;
			tmask  			= 0x7BDE;
			break;
		#endif //BLITTER_15BPP
		#ifdef BLITTER_16BPP
		case 16:
			MakeColor		= dd16_MakeColor;
			GetColor		= dd16_GetColor;
			Clear			= dd16_Clear;
			ReadPixel       = dd16_ReadPixel;
			PutPixel		= dd16_PutPixel;
			HLine			= dd16_HLine;
			VLine			= dd16_VLine;
			Line			= dd_Line;
			Box				= dd_Box;
			Rect            = dd_Rect;
			Sphere          = dd_Sphere;
			Circle          = dd_Circle;
			Blit            = dd16_Blit;
			TBlit           = dd16_TBlit;
			AdditiveBlit	= dd16_AddBlit;
			TAdditiveBlit	= dd16_TAddBlit;
			SubtractiveBlit = dd16_SubtractBlit;
			TSubtractiveBlit= dd16_TSubtractBlit;
			BlitTile        = dd16_BlitTile;
			TBlitTile		= dd16_TBlitTile;
			TScaleBlit		= dd16_TScaleBlit;
			ScaleBlit       = dd16_ScaleBlit;
			WrapBlit        = dd16_WrapBlit;
			TWrapBlit		= dd16_TWrapBlit;
			RotScale		= dd16_RotScale;
			Silhouette		= dd16_Silhouette;
			Mosaic			= dd16_Mosaic;
			#ifndef NOTIMELESS
			Timeless        = dd16_Timeless;
			#endif //NOTIMELESS
			BlitWrap		= dd16_BlitWrap;
			ColorFilter     = dd_ColorFilter;
			AlphaBlit       = dd16_AlphaBlit;
			Triangle		= dd_Triangle;
			FlipBlit        = dd16_FlipBlit;
			ImageFrom8bpp	= dd16_ImageFrom8bpp;
			ImageFrom24bpp	= dd16_ImageFrom24bpp;
			ImageFrom32bpp	= dd16_ImageFrom32bpp;
			tmask           = 0xF7DE;
			break;
		#endif //BLITTER_16BPP
		#ifdef BLITTER_61BPP
		case 61:
			MakeColor		= dd61_MakeColor;
			GetColor		= dd61_GetColor;
			Clear			= dd61_Clear;
			ReadPixel       = dd61_ReadPixel;
			PutPixel		= dd61_PutPixel;
			HLine			= dd61_HLine;
			VLine			= dd61_VLine;
			Line			= dd_Line;
			Box				= dd_Box;
			Rect            = dd_Rect;
			Sphere          = dd_Sphere;
			Circle          = dd_Circle;
			Blit            = dd61_Blit;
			TBlit           = dd61_TBlit;
			AdditiveBlit	= dd61_AddBlit;
			TAdditiveBlit	= dd61_TAddBlit;
			SubtractiveBlit = dd61_SubtractBlit;
			TSubtractiveBlit= dd61_TSubtractBlit;
			BlitTile        = dd61_BlitTile;
			TBlitTile		= dd61_TBlitTile;
			TScaleBlit		= dd61_TScaleBlit;
			ScaleBlit       = dd61_ScaleBlit;
			WrapBlit        = dd61_WrapBlit;
			TWrapBlit		= dd61_TWrapBlit;
			RotScale		= dd61_RotScale;
			Silhouette		= dd61_Silhouette;
			Mosaic			= dd61_Mosaic;
#ifndef NOTIMELESS
			Timeless        = dd61_Timeless;
#endif //NOTIMELESS
			BlitWrap		= dd61_BlitWrap;
			ColorFilter     = dd_ColorFilter;
			AlphaBlit       = dd61_AlphaBlit;
			Triangle		= dd_Triangle;
			FlipBlit        = dd61_FlipBlit;
			ImageFrom8bpp	= dd61_ImageFrom8bpp;
			ImageFrom24bpp	= dd61_ImageFrom24bpp;
			ImageFrom32bpp	= dd61_ImageFrom32bpp;
			tmask           = 0xF7DE;
			break;
		#endif //BLITTER_61BPP
		#ifdef BLITTER_32BPP
		case 32:
			MakeColor		= dd32_MakeColor;
			GetColor		= dd32_GetColor;
			Clear			= dd32_Clear;
			ReadPixel       = dd32_ReadPixel;
			PutPixel		= dd32_PutPixel;
			HLine			= dd32_HLine;
			VLine			= dd32_VLine;
			Line			= dd_Line;
			Box				= dd_Box;
			Rect            = dd_Rect;
			Sphere          = dd_Sphere;
			Circle          = dd_Circle;
			Blit            = dd32_Blit;
			TBlit           = dd32_TBlit;
			AdditiveBlit	= dd32_AddBlit;
			TAdditiveBlit	= dd32_TAddBlit;
			SubtractiveBlit = dd32_SubtractBlit;
			TSubtractiveBlit= dd32_TSubtractBlit;
			BlitTile        = dd32_BlitTile;
			TBlitTile		= dd32_TBlitTile;
			ScaleBlit       = dd32_ScaleBlit;
			TScaleBlit		= dd32_TScaleBlit;
			WrapBlit        = dd32_WrapBlit;
			TWrapBlit		= dd32_TWrapBlit;
			RotScale		= dd32_RotScale;
			Silhouette		= dd32_Silhouette;
			Mosaic			= dd32_Mosaic;
#ifndef NOTIMELESS
			Timeless        = dd32_Timeless;
#endif NOTIMELESS
			BlitWrap		= dd32_BlitWrap;
			ColorFilter     = dd_ColorFilter;
			AlphaBlit       = dd32_AlphaBlit;
			Triangle		= dd_Triangle;
			FlipBlit        = dd32_FlipBlit;
			ImageFrom8bpp	= dd32_ImageFrom8bpp;
			ImageFrom24bpp	= dd32_ImageFrom24bpp;
			ImageFrom32bpp	= dd32_ImageFrom32bpp;
			tmask           = 0xFEFEFEFE;
			break;
	#endif

		default:
			err("vid_bpp (%d) not a standard value", vid_bpp);
	}
	RectVGrad = dd_RectVGrad;
	RectHGrad = dd_RectHGrad;
	RectRGrad = dd_RectRGrad;
	Rect4Grad = dd_Rect4Grad;
	HSVtoColor = dd_HSVtoColor;
	GetHSV = dd_GetHSV;
	HueReplace = dd_HueReplace;
	ColorReplace = dd_ColorReplace;
	Init_2xSAI(vid_bpp);
	transColor = MakeColor(255, 0, 255);
}

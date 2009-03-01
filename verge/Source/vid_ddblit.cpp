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


template<bool CLIP>
void dd32_PutPixel(int x, int y, int color, image *dest)
{
	int *ptr = (int *)dest->data;
	if(CLIP)
		if (x<dest->cx1 || x>dest->cx2 || y<dest->cy1 || y>dest->cy2)
			return;
	ptr[(y * dest->pitch) + x] = color;
}

template<bool CLIP>
void dd32_PutPixel_50lucent(int x, int y, int color, image *dest)
{
	quad s, c;
	int *d=(int *)dest->data;

	if(CLIP)
		if (x<dest->cx1 || x>dest->cx2 || y<dest->cy1 || y>dest->cy2)
			return;

	s=d[(y * dest->pitch) + x];
	c=(s & tmask) + (color & tmask);
	d[(y * dest->pitch) + x] = (int) (c >> 1);
}

template<bool CLIP>
void dd32_PutPixel_lucent(int x, int y, int color, image *dest)
{
	byte *d, *c;

	if(CLIP)
		if (x<dest->cx1 || x>dest->cx2 || y<dest->cy1 || y>dest->cy2)
			return;

	c = (byte *) &color;
	d = (byte *) dest->data;
	d += ((y*dest->pitch)+x)<<2;
	//MBG TODO - THIS LOOKS LIKE A HACK THAT WILL BREAK ALPHA CHANNEL
#ifdef __BIG_ENDIAN__
	d++;
	c++;
#endif
	*d = ((*d * alpha) + (*c * ialpha)) / 100; d++; c++;
	*d = ((*d * alpha) + (*c * ialpha)) / 100; d++; c++;
	*d = ((*d * alpha) + (*c * ialpha)) / 100;
}




template<LUCENT_TYPE LT, bool CLIP> 
static void T_PutPixel(int x, int y, int color, image *dest)
{
	switch(LT) {
		case NONE: dd32_PutPixel<CLIP>(x,y,color,dest); break;
		case HALF: dd32_PutPixel_50lucent<CLIP>(x,y,color,dest); break;
		case ANY:  dd32_PutPixel_lucent<CLIP>(x,y,color,dest); break;
	}
}

template<LUCENT_TYPE LT, bool CLIP>
void T_HLine(int x, int y, int xe, int color, image *dest)
{
	int *d = (int *) dest->data;
	
	if(CLIP) {
		int cx1=0, cy1=0, cx2=0, cy2=0;
		if (xe<x) SWAP(x,xe);
		dest->GetClip(cx1, cy1, cx2, cy2);
		if (x>cx2 || y>cy2 || xe<cx1 || y<cy1)
			return;
		if (xe>cx2) xe=cx2;
		if (x<cx1)  x =cx1;
	}

	d += (y * dest->pitch) + x;
	for (; x<=xe; x++) {
		switch(LT) {
			case NONE: *d++ = color; break;
			case HALF: {
				int s=*d;
				s=(s & tmask) + (color & tmask);
				*d++ = (s >> 1);
				break;
			}
			case ANY:
				T_PutPixel<ANY,false>(x, y, color, dest);
				break;
		}
	}
}


template<LUCENT_TYPE LT>
void T_VLine(int x, int y, int ye, int color, image *dest)
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
	for (; y<=ye; y++, d+=dest->pitch) {
		switch(LT) {
			case NONE: *d = color; break;
			case HALF: {
				int s=*d;
				s=(s & tmask) + (color & tmask);
				*d = (s >> 1);
				break;
			}
			case ANY:
				T_PutPixel<ANY,false>(x, y, color, dest);
				break;
		}
	}
}

template<LUCENT_TYPE LT>
void T_Line(int x, int y, int xe, int ye, int color, image *dest)
{
	if (x == xe) { T_VLine<LT>(x,y,ye,color,dest); return; }
	if (y == ye) { T_HLine<LT,true>(x,y,xe,color,dest); return; }

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
		
		T_PutPixel<LT,true>(cx,cy,color,dest);
		
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

template<LUCENT_TYPE LT>
void T_Triangle(int x1, int y1, int x2, int y2, int x3, int y3, int color, image *dest)
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
				T_HLine<LT,true>(xval>>16, yon, xval2>>16, color, dest);
			xval+=xstep;
			xval2+=xstep2;
		}
	}
}

template<LUCENT_TYPE LT>
void T_Box(int x, int y, int x2, int y2, int color, image *dest)
{
	if (x2<x) SWAP(x,x2);
	if (y2<y) SWAP(y,y2);
	T_HLine<LT,true>(x, y, x2, color, dest);
	T_HLine<LT,true>(x, y2, x2, color, dest);
	T_VLine<LT>(x, y+1, y2-1, color, dest);
	T_VLine<LT>(x2, y+1, y2-1, color, dest);
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

template<LUCENT_TYPE LT>
void T_Oval(int x, int y, int xe, int ye, int color, int Fill, image *dest)
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
		T_HLine<LT,true>(x, y, x+xx-1, color, dest);
	else {
		T_PutPixel<LT,true>(x, y, color, dest);
		T_PutPixel<LT,true>(x+xx, y, color, dest);
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
			T_HLine<LT,true>(xe, ye, xe+xx-1, color, dest);
		else {
			T_PutPixel<LT,true>(xe, ye, color, dest);
			T_PutPixel<LT,true>(xe+xx, ye, color, dest);
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
			T_HLine<LT,true>(x, y, x+xx-1, color, dest);
			T_HLine<LT,true>(xe, ye, xe+xx-1, color, dest);
		}
		else {
			T_PutPixel<LT,true>(x, y, color, dest);
			T_PutPixel<LT,true>(x+xx, y, color, dest);
			T_PutPixel<LT,true>(xe, ye, color, dest);
			T_PutPixel<LT,true>(xe+xx, ye, color, dest);
		}
		lasty = y;
	}
}

template<LUCENT_TYPE LT>
void T_Sphere(int x, int y, int xradius, int yradius, int color, image *dest)
{
	T_Oval<LT>(x-xradius, y-yradius, x+xradius-1, y+yradius-1, color, 1, dest);
}

template<LUCENT_TYPE LT>
void T_Circle(int x, int y, int xradius, int yradius, int color, image *dest)
{
	T_Oval<LT>(x-xradius, y-yradius, x+xradius-1, y+yradius-1, color, 0, dest);
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
	int *d = (int *)dest->data;
	int bytes = dest->pitch * dest->height;
	while (bytes--)
		*d++ = color;
}

template<LUCENT_TYPE LT, int FILTER>
static void _T_ColorFilter(image *img) {
	int rr, gg, bb, z, c;

	int x1,x2,y1,y2;
	img->GetClip(x1,y1,x2,y2);

	quad *ptr = (quad*)img->data;
	int *data = (int*)&ptr[(y1 * img->pitch) + x1];

	for (int y=y1; y<=y2; y++)
	{
		//int* data_end = data+x2+1;
		for(int x=x1;x<=x2;x++,data++) {
			int col = *data;
			if (col == transColor) continue; // Overkill (2006-07-27): Ignore trans pixels
			GetColor(col, rr, gg, bb);
			switch (FILTER)
			{
				case 0: 
				case 1: z = (rr+gg+bb)/3; c = MakeColor(z,z,z); break;
				case 2: z = 255-((rr+gg+bb)/3); c = MakeColor(z,z,z); break;
				case 3: c = MakeColor(255-rr, 255-gg, 255-bb); break;
				case 4: z = (rr+gg+bb)/3; c = MakeColor(z, 0, 0); break;
				case 5: z = (rr+gg+bb)/3; c = MakeColor(0, z, 0); break;
				case 6: z = (rr+gg+bb)/3; c = MakeColor(0, 0, z); break;
				case 7: z = (rr+gg+bb)/3; c = MakeColor(cf_r1+((cf_rr*z)>>8), cf_g1+((cf_gr*z)>>8), cf_b1+((cf_br*z)>>8)); break;
			}
			
			T_PutPixel<LT,false>(x, y, c, img);

			*data = c;
		}
		data += img->pitch - (x2-x1+1);
	}
}

template<LUCENT_TYPE LT>
static void T_ColorFilter(int filter, image *img)
{
	switch (filter)
	{
		case 0: _T_ColorFilter<LT,0>(img); break;
		case 1: _T_ColorFilter<LT,1>(img); break;
		case 2: _T_ColorFilter<LT,2>(img); break;
		case 3: _T_ColorFilter<LT,3>(img); break;
		case 4: _T_ColorFilter<LT,4>(img); break;
		case 5: _T_ColorFilter<LT,5>(img); break;
		case 6: _T_ColorFilter<LT,6>(img); break;
		case 7: _T_ColorFilter<LT,7>(img); break;
		default: break;
	}
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

inline void COPY_PIXELS(quad* dst, quad* src, const int num) {
	for(int i=0;i<num;i++)
		*dst++=*src++;
}

static void create_tables() {
	static bool created = false;
	if(created) return;
	created = true;
}

inline void COPY_PIXELS_BLEND(int * dst, int* src, const int num) {
	for(int i=0;i<num;i++)
	{
		const Color& sc = ((Color*)src)[i];
		Color& dc = ((Color*)dst)[i];

		if(sc.a == 0) continue;
		if (sc.a == 255)
		{
			dc = sc;
			continue;
		}

		int pa = sc.a;
		int ipa = 255-pa;

		dc.r = ((sc.r*pa)+(dc.r*ipa))>>8;
		dc.g = ((sc.g*pa)+(dc.g*ipa))>>8;
		dc.b = ((sc.b*pa)+(dc.b*ipa))>>8;
	}
}

inline void COPY_PIXELS(int* dst, int* src, const int num) {
	COPY_PIXELS((quad*)dst,(quad*)src,num);
}

inline void COPY_PIXELS_320_480(int* dst, int* src) {
	memcpy(dst,src,320*480*4);
}

inline void COPY_PIXELS_320(int* dst, int* src) {
	memcpy(dst,src,320*4);
}


template<LUCENT_TYPE LT>
void T_Blit(int x, int y, image *src, image *dest)
{
	int *s=(int *)src->data,
		*d=(int *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1, cy1, cx2, cy2;

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
	switch(LT) {
		case NONE: 
			if(src->alpha) {
				for (; ylen--; s+=spitch, d+=dpitch)
					COPY_PIXELS_BLEND(d,s,xlen);
			}
			else {
				//iphone optimization
				if(xlen == 320) {
					if(ylen == 480 && dpitch == 320 && spitch == 320)
						COPY_PIXELS_320_480(d,s);
					else
						for (; ylen--; s+=spitch, d+=dpitch)
							COPY_PIXELS_320(d,s);
				}
				else
					for (; ylen--; s+=spitch, d+=dpitch)
						COPY_PIXELS(d,s,xlen);
			}
			break;
		case HALF: {
			for (; ylen; ylen--) {
				for (x=0; x<xlen; x++) {
					int sc=s[x];
					sc=(sc & tmask) + (d[x] & tmask);
					d[x] = (sc >> 1);
				}
				s+=spitch;
				d+=dpitch;
			}
			break;
		}
		case ANY: {
			for (; ylen; ylen--) {
				for (x=0; x<xlen; x++) {
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
			break;
		}
	}
		
}


template<LUCENT_TYPE LT>
void T_TBlit(int x, int y, image *src, image *dest)
{
	int *s=(int *)src->data,c,
		*d=(int *)dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1, cy1, cx2, cy2;

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
	for (; ylen; ylen--) {
		for (x=0; x<xlen; x++) {
			switch(LT) {
				case NONE:
					c=s[x];
					if (c != transColor) d[x]=c;
					break;
				case HALF: {
					int sc=s[x]; if (sc == transColor) continue;
					sc=(sc & tmask) + (d[x] & tmask);
					d[x] = (sc >> 1);
					break;
				}
				case ANY: {
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
				break;
			}
		}
		s+=spitch;
		d+=dpitch;
	}
}

template<LUCENT_TYPE LT>
void T_BlitTile(int x, int y, quad *src, image *dest)
{
	quad *s=src,
		 *d=dest->data;
	int spitch=16,
		dpitch=dest->pitch;
	int xlen=16,
		ylen=16;
	int cx1, cy1, cx2, cy2;

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

	switch(LT) {
		case NONE:
		case HALF:
		case ANY:
			for (;ylen--; s+=spitch,d+=dpitch)
				COPY_PIXELS(d,s,xlen);
			break;
	}

}

template<LUCENT_TYPE LT>
void T_TBlitTile(int x, int y, quad *src, image *dest)
{
	quad *s=src,
		 *d=dest->data;
	int spitch=16,
		dpitch=dest->pitch;
	int xlen=16,
		ylen=16;
	int cx1, cy1, cx2, cy2;
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
			switch(LT) {
			case NONE:
				c=s[x];
				if (c != (quad)transColor) d[x]=c;
				break;
			case ANY:
			case HALF: {
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
				break;
				}
			}
		}
		s+=spitch;
		d+=dpitch;
	}
}


template<LUCENT_TYPE LT, bool TRANSPARENT>
void T_ScaleBlit(int x, int y, int dw, int dh, image *src, image *dest)
{
	int i, j;
	int xerr, yerr;
	int xerr_start, yerr_start;
	int xadj, yadj;
	quad *d, *s;
	int xl, yl, xs, ys;
	int cx1, cy1, cx2, cy2;

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

	for (i = 0; i < yl; i += 1) {
		xerr = xerr_start;
		for (j = 0; j < xl; j += 1) {
			int c = s[(xerr >> 16)];
			switch(LT) {
				case NONE:
					if(TRANSPARENT)
						if (c != transColor)
							d[j] = c;
					break;
				case HALF:
					if(TRANSPARENT)
						if (c != transColor) {
							c = (c & tmask) + (d[j] & tmask);
							d[j] = (c >> 1);
						}
					break;
				case ANY: {
					if(TRANSPARENT)
						if (c != transColor) {

						quad r1, g1, b1;
						quad r2, g2, b2;
						quad dp;

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
					}
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
			color=MakeColor(r,g,b);
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

template<LUCENT_TYPE LT>
void T_FlipBlit(int x, int y, int fx, int fy, image *src, image *dest)
{
	if(!fx && !fy) T_Blit<LT>(x,y,src,dest);

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

	img = new image(width, height);
	img->alpha = true;
	dest = (quad *)img->data;
	memcpy(dest,src,width*height*4);
	return img;
}

/*********************** blitter managment **********************/

template<LUCENT_TYPE LT>
static void T_Rect(int x, int y, int x2, int y2, int color, image *dest)
{
	int cx1, cy1, cx2, cy2;
	if (x2<x) SWAP(x,x2);
	if (y2<y) SWAP(y,y2);
	dest->GetClip(cx1, cy1, cx2, cy2);
	if (x>cx2 || y>cy2 || x2<cx1 || y2<cy1)
		return;
	if (x2>cx2) x2=cx2;
	if (x<cx1)  x =cx1;
	if (y2>cy2) y2=cy2;
	if (y<cy1)  y =cy1;

	//speed optimization for entire-buffer clearing
	/*if(y==0&&y2==dest->height-1
		&&x==0&&x2==dest->width-1
		&&dest->pitch == dest->width) {
			int todo = dest->width*dest->height;
			int* d = (int*)dest->data;
			for(int i=0;i<todo;i++)
				*d++ = color;
			return;
	}*/

	for (; y<=y2; y++)
		T_HLine<LT,false>(x, y, x2, color, dest);
}


int SetLucent(int percent)
{
	if (percent < 0) percent = 0;
	if (percent > 100) percent = 100;
	int oldalpha = alpha;
	alpha = percent;
	ialpha = 100 - alpha;

	if(alpha == oldalpha) return oldalpha;

	if (percent == 0)
	{
		Sphere          = T_Sphere<NONE>;
		Circle          = T_Circle<NONE>;
		Box				= T_Box<NONE>;
		Line			= T_Line<NONE>;
		ColorFilter     = T_ColorFilter<NONE>;
		PutPixel		= T_PutPixel<NONE,true>;
		DrawRect        = T_Rect<NONE>;
		HLine			= T_HLine<NONE,true>;
		VLine			= T_VLine<NONE>;
		Blit            = T_Blit<NONE>;
		TBlit           = T_TBlit<NONE>;
		BlitTile        = T_BlitTile<NONE>;
		TBlitTile       = T_TBlitTile<NONE>;
		ScaleBlit		= T_ScaleBlit<NONE,false>;
		TScaleBlit      = T_ScaleBlit<NONE,true>;
		Triangle		= T_Triangle<NONE>;
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
		Sphere          = T_Sphere<HALF>;
		Circle          = T_Circle<HALF>;
		Box				= T_Box<HALF>;
		Line			= T_Line<HALF>;
		ColorFilter     = T_ColorFilter<HALF>;
		PutPixel		= T_PutPixel<HALF,true>;
		DrawRect        = T_Rect<HALF>;
		HLine			= T_HLine<HALF,true>;
		VLine			= T_VLine<HALF>;
		Blit            = T_Blit<HALF>;
		TBlit			= T_TBlit<HALF>;
		BlitTile        = T_BlitTile<HALF>;
		TBlitTile       = T_TBlitTile<HALF>;
		ScaleBlit		= T_ScaleBlit<HALF,false>;
		TScaleBlit      = T_ScaleBlit<HALF,true>;
		Triangle		= T_Triangle<HALF>;
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
		Sphere          = T_Sphere<ANY>;
		Circle          = T_Circle<ANY>;
		Box				= T_Box<ANY>;
		Line			= T_Line<ANY>;
		ColorFilter     = T_ColorFilter<ANY>;
		PutPixel		= T_PutPixel<ANY,true>;
		DrawRect        = T_Rect<ANY>;
		HLine			= T_HLine<ANY,true>;
		VLine			= T_VLine<ANY>;
		Blit            = T_Blit<ANY>;
		TBlit			= T_TBlit<ANY>;
		BlitTile        = T_BlitTile<ANY>;
		TBlitTile       = T_TBlitTile<ANY>;
		ScaleBlit		= T_ScaleBlit<ANY,false>;
		TScaleBlit      = T_ScaleBlit<ANY,true>;
		Triangle		= T_Triangle<ANY>;
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
	return percent;
}


void dd_RegisterBlitters()
{
	if(vid_bpp != 32) 
		err("Sorry! temporarily, support for non-32bpp is removed. Itll come back soon.");

	AdditiveBlit	= dd32_AddBlit;
	TAdditiveBlit	= dd32_TAddBlit;
	SubtractiveBlit = dd32_SubtractBlit;
	TSubtractiveBlit= dd32_TSubtractBlit;
	WrapBlit        = dd32_WrapBlit;
	TWrapBlit		= dd32_TWrapBlit;
	RotScale		= dd32_RotScale;
	Silhouette		= dd32_Silhouette;
	Mosaic			= dd32_Mosaic;
	BlitWrap		= dd32_BlitWrap;
	AlphaBlit       = dd32_AlphaBlit;
	FlipBlit        = T_FlipBlit<NONE>;
	ImageFrom8bpp	= dd32_ImageFrom8bpp;
	ImageFrom24bpp	= dd32_ImageFrom24bpp;
	ImageFrom32bpp	= dd32_ImageFrom32bpp;
	
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



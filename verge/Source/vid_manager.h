/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


#ifndef VIDMANAGER_H
#define VIDMANAGER_H

#define MODE_NULL 0
#define MODE_SOFTWARE 1
#define MODE_D3D 2

class image
{
public:
	int width, height, pitch;
	int cx1, cy1, cx2, cy2;
	int bpp, shell;
	void *data;

	image();
	image(int xres, int yres);
	~image();
	void delete_data();
	void SetClip(int x1, int y1, int x2, int y2);
	void GetClip(int &x1, int &y1, int &x2, int &y2);
};


class AuxWindow
{
public:
	bool bMouseInside;
	AuxWindow() { bMouseInside = false; }
	virtual ~AuxWindow() {}
public:
	virtual void dispose() = 0;
	virtual int getHandle() = 0;
	virtual int getImageHandle() = 0;
	virtual image* getImage() = 0;
	virtual int getXres() = 0;
	virtual int getYres() = 0;
	virtual int getWidth() = 0;
	virtual int getHeight() = 0;
	virtual void setPosition(int x, int y) = 0;
	virtual void setResolution(int w, int h) = 0;
	virtual void setSize(int w, int h) = 0;
	virtual void setVisibility(bool vis) = 0;
	virtual void setTitle(const char *title) = 0;
	virtual void positionCommand(int command, int arg1, int arg2) =0;
};

extern int vid_bpp, vid_xres, vid_yres, vid_bytesperpixel, transColor;
extern bool vid_window;
extern image *screen;
extern AuxWindow *gameWindow;
extern int alpha, ialpha;
extern int skewlines[];

int dd_SetMode(int xres, int yres, int bpp, bool windowflag);
int vid_SetMode(int xres, int yres, int bpp, int window, int mode);
int SetLucent(int percent);

void run2xSAI(image *src, image *dest);

extern int    (*MakeColor) (int r, int g, int b);
extern bool   (*GetColor) (int c, int &r, int &g, int &b);
extern void   (*Flip) (void);
extern void   (*Blit) (int x, int y, image *src, image *dest);
extern void   (*TBlit) (int x, int y, image *src, image *dest);
extern void   (*AlphaBlit) (int x, int y, image *src, image *alpha, image *dest);
extern void   (*AdditiveBlit) (int x, int y, image *src, image *dest);
extern void   (*TAdditiveBlit) (int x, int y, image *src, image *dest);
extern void   (*SubtractiveBlit) (int x, int y, image *src, image *dest);
extern void   (*TSubtractiveBlit) (int x, int y, image *src, image *dest);
extern void   (*BlitTile) (int x, int y, char *src, image *dest);
extern void   (*TBlitTile) (int x, int y, char *src, image *dest);
extern void   (*Clear) (int color, image *dest);
extern void   (*PutPixel) (int x, int y, int color, image *dest);
extern int    (*ReadPixel) (int x, int y, image *dest);
extern void   (*Line) (int x, int y, int xe, int ye, int color, image *dest);
extern void   (*VLine) (int x, int y, int ye, int color, image *dest);
extern void   (*HLine) (int x, int y, int xe, int color, image *dest);
extern void   (*Box) (int x, int y, int xe, int ye, int color, image *dest);
extern void   (*Rect) (int x, int y, int xe, int ye, int color, image *dest);
extern void   (*Sphere) (int x, int y, int xradius, int yradius, int color, image *dest);
extern void   (*Circle) (int x, int y, int xradius, int yradius, int color, image *dest);
extern void   (*ScaleBlit) (int x, int y, int dw, int dh, image *src, image *dest);
extern void   (*TScaleBlit) (int x, int y, int dw, int dh, image *src, image *dest);
extern void   (*WrapBlit) (int x, int y, image *src, image *dst);
extern void   (*TWrapBlit) (int x, int y, image *src, image *dst);
extern void   (*Silhouette) (int x, int y, int c, image *src, image *dst);
extern void   (*RotScale) (int x, int y, float angle, float scale, image *src, image *dest);
extern void   (*Mosaic) (int xf, int yf, image *src);
extern void   (*Timeless) (int x, int y1, int y, image *src, image *dest);
extern void   (*BlitWrap) (int x, int y, image *src, image *dest);
extern void   (*ColorFilter) (int filter, image *img);
extern void   (*Triangle) (int x1, int y1, int x2, int y2, int x3, int y3, int c, image *dest);
extern void   (*FlipBlit) (int x, int y, int fx, int fy, image *src, image *dest);
extern image* (*ImageFrom8bpp) (byte *src, int width, int height, byte *pal);
extern image* (*ImageFrom24bpp) (byte *src, int width, int height);
extern image* (*ImageFrom32bpp) (byte *src, int width, int height);
extern void   (*vid_Close) (void);
extern AuxWindow * (*vid_createAuxWindow)(void);
extern AuxWindow * (*vid_findAuxWindow)(int handle);
extern void   GrabRegion(int sx1, int sy1, int sx2, int sy2, int dx, int dy, image *s, image *d);
// Overkill 2006-02-04
extern void   (*RectVGrad) (int x, int y, int xe, int ye, int color, int color2, image *dest);
// Overkill 2006-02-04
extern void   (*RectHGrad) (int x, int y, int xe, int ye, int color, int color2, image *dest);
// janus 2006-07-22
extern void   (*RectRGrad) (int x1, int y1, int x2, int y2, int color1, int color2, image *dest);
// janus 2006-07-22
extern void   (*Rect4Grad) (int x1, int y1, int x2, int y2, int color1, int color2, int color3, int color4, image *dest);
// Overkill (2007-05-04)
extern int    (*HSVtoColor) (int h, int s, int v);
// Overkill (2007-05-04)
extern void    (*GetHSV) (int color, int &h, int &s, int &v);
// Overkill (2007-05-04)
extern void    (*HueReplace) (int hue_find, int hue_tolerance, int hue_replace, image *img);
// Overkill (2007-05-04)
extern void	(*ColorReplace) (int color_find, int color_replace, image *img);

#endif

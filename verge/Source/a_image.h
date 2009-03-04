/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


#ifndef IMAGE_H
#define IMAGE_H

class image
{
public:
	int width, height, pitch;
	int cx1, cy1, cx2, cy2;
	bool shell, alpha;
	void *data;
	byte *alphaChannel;
	int bpp;


	image(int bpp=-1);
	image(int xres, int yres, int bpp=-1);
	void delete_data();
	void alloc_data();
	void alloc_alpha();
	~image();

	void SetClip(int x1, int y1, int x2, int y2) {
		cx1 = x1 >= 0 ? x1 : 0;
		cy1 = y1 >= 0 ? y1 : 0;
		cx1 = cx1 < width ? cx1 : width-1;
		cy1 = cy1 < height ? cy1 : height-1;
		cx2 = x2 >= 0 ? x2 : 0;
		cy2 = y2 >= 0 ? y2 : 0;
		cx2 = cx2 < width ? cx2 : width-1;
		cy2 = cy2 < height ? cy2 : height-1;
	}

	void GetClip(int &x1, int &y1, int &x2, int &y2)
	{
		x1 = cx1;
		y1 = cy1;
		x2 = cx2;
		y2 = cy2;
	}


};

image *xLoadImage(const char *fname);
image *xLoadImage0(const char *name);
byte *xLoadImage8bpp(const char *fname);
image *xLoadImage8(const char *fname);

#endif

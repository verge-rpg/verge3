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
	a_image.cpp
 ****************************************************************/

#include "xerxes.h"

image* xLoadImage_int_respect8bitTransparency(const char* fname)
{
	image* newimage;
	
	corona::Image* img;

	log("loading image %s", fname);
	if (Exist(fname))
	{
		img = corona::OpenImage(fname, corona::FF_AUTODETECT, corona::PF_DONTCARE);
	}
	else
	{
		VFILE* vf = vopen(fname);
		if (!vf)
		{
			err("loadimage: couldn't load image %s; couldnt find a file or a vfile", fname);
		}
		FILE* f = fopen("v3img_temp.$$$", "wb");
		if (!f)
		{
			// TODO: implement
		}
		int l = filesize(vf);
		char* buffer = new char[l];
		vread(buffer, l, vf);
		fwrite(buffer, 1, l, f);
		vclose(vf);
		fclose(f);
		img = corona::OpenImage("v3img_temp.$$$", corona::FF_AUTODETECT, corona::PF_DONTCARE);
		remove("v3img_temp.$$$");
	}

	if (!img)
	{
		err("loadimage: couldn't load image %s; corona just bombed.", fname);
	}

	if (img->getFormat() == corona::PF_I8)
	{
		int transparency_index = 0;
		if (img->getPaletteFormat() != corona::PF_R8G8B8)
		{
			//we can convert this one. we get it with new corona 1.0.2
			//8bpp gif loader
			if (img->getPaletteFormat() == corona::PF_R8G8B8A8)
			{
				unsigned char* p = (unsigned char*) img->getPalette();
				int c = img->getPaletteSize();
				unsigned char* ps = p + 4;
				unsigned char* pd = p + 3;
				for (int i = 1; i < c; i++)
				{
					*pd++ = *ps++;
					*pd++ = *ps++;
					*pd++ = *ps++;
					if (!*ps++)
					{
						transparency_index = i;
					}
				}
			}
			else
			{
				err("loadimage: couldnt load image %s; unexpected pixel format", fname);
			}
		}
			
		unsigned char* pal2 = (unsigned char*) img->getPalette();
		unsigned char pal[768];
		memcpy(pal, pal2, img->getPaletteSize()*3);		

		int ofs = 3*transparency_index;
		pal[ofs++] = 255;
		pal[ofs++] = 0;
		pal[ofs++] = 255;

		newimage = ImageFrom8bpp((unsigned char*) img->getPixels(), img->getWidth(), img->getHeight(), pal);
		delete img;
		return newimage;
	}
	else
	{
		img = corona::ConvertImage(img,corona::PF_R8G8B8);
		newimage=ImageFrom24bpp((unsigned char *)img->getPixels(),img->getWidth(),img->getHeight());
		delete img;
        return newimage;
	}
}

image *xLoadImage_int(const char *fname,int tflag)
{
	image *newimage;

	corona::Image *img;

	log("loading image %s",fname);
	if (Exist(fname))
		img = corona::OpenImage(fname,corona::FF_AUTODETECT,corona::PF_DONTCARE);
	else
	{
		VFILE *vf = vopen(fname);
		if (!vf)
			err("loadimage: couldn't load image %s; couldnt find a file or a vfile",fname);
		FILE *f = fopen("v3img_temp.$$$","wb");
		int l = filesize(vf);
		char *buffer = new char[l];
		vread(buffer, l, vf);
		fwrite(buffer, 1, l, f);
		vclose(vf);
		fclose(f);
		img = corona::OpenImage("v3img_temp.$$$",corona::FF_AUTODETECT,corona::PF_DONTCARE);
		remove("v3img_temp.$$$");
	}
	if(!img)
		err("loadimage: couldn't load image %s; corona just bombed.",fname);

	switch(img->getFormat())
	{
	case corona::PF_I8:
	{
		if(img->getPaletteFormat()!=corona::PF_R8G8B8) {

			//we can convert this one. we get it with new corona 1.0.2
			//8bpp gif loader
			if(img->getPaletteFormat() == corona::PF_R8G8B8A8) {
				unsigned char *p=(unsigned char *)img->getPalette();
				int c = img->getPaletteSize();
				unsigned char *ps = p+4, *pd = p+3;
				for(int i=1;i<c;i++) {
					*pd++ = *ps++;
					*pd++ = *ps++;
					*pd++ = *ps++;
					ps++;
				}
			}
			else err("loadimage: couldnt load image %s; unexpected pixel format",fname);
		}

		unsigned char *pal2=(byte*)img->getPalette();
		unsigned char pal[768];
		memcpy(pal, pal2, img->getPaletteSize()*3);
		if(tflag)
		{
			pal[0]=255; pal[1]=0; pal[2]=255;
		}
		newimage=ImageFrom8bpp((byte*)img->getPixels(), img->getWidth(), img->getHeight(), pal);
		delete img;
		return newimage;
	}
	break;

	case corona::PF_R8G8B8A8:
		img = corona::ConvertImage(img,corona::PF_B8G8R8A8);
		newimage=ImageFrom32bpp((byte*)img->getPixels(),img->getWidth(),img->getHeight());
		delete img;
        return newimage;

	default:
	{
		img = corona::ConvertImage(img,corona::PF_R8G8B8);
		newimage=ImageFrom24bpp((byte*)img->getPixels(),img->getWidth(),img->getHeight());
		delete img;
        return newimage;
	}
	}
}


image *xLoadImage(const char *fname)
{
	return xLoadImage_int(fname, 0);
}

image *xLoadImage0(const char *fname)
{
	return xLoadImage_int(fname, 1);
}

image* xLoadImage8(const char* fname)
{
	return xLoadImage_int_respect8bitTransparency(fname);
}

image::image(int bpp)
:shell(false)
,bpp(bpp==-1?vid_bpp:bpp)
,alpha(false)
,alphaChannel(0)
{
}

image::image(int xres, int yres, int bpp)
:width(xres)
,height(yres)
,bpp(bpp==-1?vid_bpp:bpp)
,pitch(xres)
,cx1(0),cy1(0),cx2(xres-1),cy2(yres-1)
,shell(false)
,alpha(false)
,alphaChannel(0)
{
	alloc_data();
}

void image::delete_data() {
	if(shell) return;

	delete[] (byte*)data;
	delete[] alphaChannel;
	data = 0;
	alphaChannel = 0;
}

void image::alloc_alpha()
{
	alpha = true;
	alphaChannel = new byte[width*height];
	assert(alphaChannel);

	//we are going to assume that pixels are 4byte aligned.
	assert((((int)alphaChannel) & 3) == 0);
}

void image::alloc_data()
{
	shell = false;

	data = new byte[width*height*BytesPerPixel(bpp)];
	assert(data);

	//we are going to assume that pixels are 4byte aligned.
	assert((((int)data) & 3) == 0);

	pitch = width;
}

image::~image()
{
	delete_data();
}
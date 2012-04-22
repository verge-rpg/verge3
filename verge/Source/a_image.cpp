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
#include <cassert>
#include <memory>

corona::Image* load_image_from_packfile(const char* filename)
{
	VFILE* vf = vopen(filename);
	if (!vf)
	{
		err("loadimage: couldn't load image %s; couldnt find a file or a vfile", filename);
	}
	int l = filesize(vf);
	std::auto_ptr<char> buffer(new char[l]);
	vread(buffer.get(), l, vf);
	vclose(vf);
	std::auto_ptr<corona::File> memfile(corona::CreateMemoryFile(buffer.get(), l));
	return corona::OpenImage(memfile.get(), corona::FF_AUTODETECT, corona::PF_DONTCARE);
}

corona::Image* load_image_from_disk_or_packfile(const char* filename)
{
	corona::Image* img;
	//log("loading image %s", filename);
	if (Exist(filename))
	{
		img = corona::OpenImage(filename, corona::FF_AUTODETECT, corona::PF_DONTCARE);
	}
	else
	{
		img = load_image_from_packfile(filename);
	}

	if (!img)
	{
		err("loadimage: couldn't load image %s; corona just bombed.", filename);
	}
	return img;
}

image* create_image_from_24bit_corona(corona::Image* img)
{
	std::auto_ptr<corona::Image> converted_img(corona::ConvertImage(img, corona::PF_R8G8B8));
	return ImageFrom24bpp((byte*)converted_img->getPixels(), converted_img->getWidth(), converted_img->getHeight());
}

int convert_quad_palette_to_triplets(corona::Image* img)
{
	int transparency_index = 0;
	unsigned char* p = (unsigned char*)img->getPalette();
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
	return transparency_index;
}

image* create_image_from_8bit_corona(corona::Image* img, int transparency_index, int tflag)
{
	unsigned char *pal2 = (byte*)img->getPalette();
	unsigned char pal[768];
	memcpy(pal, pal2, img->getPaletteSize()*3);
	if (tflag)
	{
		int ofs = 3*transparency_index;
		pal[ofs++] = 255;
		pal[ofs++] = 0;
		pal[ofs++] = 255;
	}
	image* result = ImageFrom8bpp((byte*)img->getPixels(), img->getWidth(), img->getHeight(), pal);
	delete img; /* corona::ConvertImage in the 24-bit path also does this */
	return result;
}

image* load_image(const char* fname, bool use_transparency_index, int tflag)
{
	corona::Image* img = load_image_from_disk_or_packfile(fname);
	if (img->getFormat() == corona::PF_I8)
	{
		int transparency_index = 0;
		if (img->getPaletteFormat() != corona::PF_R8G8B8)
		{
			//we can convert this one. we get it with new corona 1.0.2
			//8bpp gif loader
			if (img->getPaletteFormat() == corona::PF_R8G8B8A8)
			{
				transparency_index = convert_quad_palette_to_triplets(img);
			}
			else
			{
				err("loadimage: couldnt load image %s; unexpected pixel format", fname);
			}
		}

		return create_image_from_8bit_corona(img, use_transparency_index ? transparency_index : 0, tflag);
	}

    return create_image_from_24bit_corona(img);
}

image* xLoadImage_int_respect8bitTransparency(const char* fname)
{
	return load_image(fname, /*use_transparency_index*/true, /*tflag*/1);
}

image *xLoadImage_int(const char *fname,int tflag)
{
	return load_image(fname, /*use_transparency_index*/false, tflag);
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

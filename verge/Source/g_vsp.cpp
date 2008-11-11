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
 * verge3: g_vsp.cpp                                              *
 * copyright (c) 2001 vecna                                       *
 ******************************************************************/

#include "xerxes.h"

/****************************** data ******************************/

#define VSP_SIGNATURE		5264214
#define VSP_VERSION			6

#define VSP_FORMAT_24BPP	1
#define VSP_FORMAT_32BPP	2

#define VSP_NO_COMPRESSION  0
#define VSP_ZLIB			1

#define ANIM_MODE_FORWARD   0
#define ANIM_MODE_BACKWARD  1
#define ANIM_MODE_RANDOM	2
#define ANIM_MODE_PINGPONG	3

/****************************** code ******************************/

VSP::VSP(char *fname)
{
	VFILE *f = vopen(fname);
	if (!f)
		err("VSP::VSP() - couldn't find VSP file %s", fname);
	LoadVSP(f);
	vclose(f);
}

VSP::VSP(VFILE *f)
{
	LoadVSP(f);
}

VSP::~VSP()
{
	delete[] tileidx;
	delete[] flipped;
	delete[] vadelay;
	delete[] anims;
	container->data = 0;
	delete container;
	delete vspdata;
	delete obs;
}

void VSP::LoadVSP(VFILE *f)
{
	int signature;
	vread(&signature, 4, f);
	if (signature != VSP_SIGNATURE)
		err("VSP::LoadVSP() - Invalid VSP signature!");

	int version;
	vread(&version, 4, f);
	if (version != VSP_VERSION)
		err("VSP::LoadVSP() - Invalid version (%d)", version);

	int tilesize;
	vread(&tilesize, 4, f);
	if (tilesize != 16)
		err("VSP::LoadVSP() - bzzzzzzzzttt. Try a 16x16 vsp! %d",tilesize);

	int format;
	vread(&format, 4, f);
	vread(&numtiles, 4, f);

	int compression;
	vread(&compression, 4, f);
	if (compression != VSP_ZLIB) err("huhhruhoeijfaoijrf..fdpok no compression? tell vecna!!!");
	byte *tiledatabuf = new byte[16*16*3*numtiles];
	cvread(tiledatabuf, 16*16*3*numtiles, f);

	container = new image(16, 16);
	container->delete_data();

	vspdata = ImageFrom24bpp(tiledatabuf, 16, 16*numtiles);
	delete[] tiledatabuf;

	vread(&numanims, 4, f);
	anims = new vspanim_r[numanims];
	for(int i = 0; i < numanims; i++)
	{
		vread(anims[i].name, 256, f);
		vread(&anims[i].start, 4, f);
		vread(&anims[i].finish, 4, f);
		vread(&anims[i].delay, 4, f);
		vread(&anims[i].mode, 4, f);
	}

	ValidateAnimations();

	vread(&numobs, 4, f);
	obs = new char[numobs*256];
	cvread(obs, numobs*256, f);

	// initialize tile anim stuff
	tileidx = new int[numtiles];
	flipped = new int[numtiles];
	vadelay = new int[numanims];

	int i;
	for (i=0; i<numanims; i++)
		vadelay[i]=0;
	for (i=0; i<numtiles; i++)
	{
		flipped[i] = 0;
		tileidx[i] = i;
	}
	mytimer = systemtime;

	// Overkill (2006-07-20): And added back in.
	SetHandleImage(2, vspdata);
}

// Overkill (2006-07-20): Saver functions!
void VSP::save(FILE *f)
{
	// Signature
	int signature = VSP_SIGNATURE;
	fwrite(&signature, 1, 4, f);
	// Version
	int version = VSP_VERSION;
	fwrite(&version, 1, 4, f);
	// Tilesize - always 16x16
	int tilesize = 16;
	fwrite(&tilesize, 1, 4, f);
	// Format??? always 0, I assume.
	int format = 0;
	fwrite(&format, 1, 4, f);
	// Number of tiles.
	fwrite(&numtiles, 1, 4, f);

	// Compression - always ZLIB
	int compression = VSP_ZLIB;
	fwrite(&compression, 1, 4, f);

	// Write our tiledata.
	byte *tiledatabuf = ImageTo24bpp(vspdata);
	cfwrite(tiledatabuf, 1, 16*16*3*numtiles, f);
	// Animation!
	fwrite(&numanims, 1, 4, f);
	for(int i = 0; i < numanims; i++)
	{
		fwrite(anims[i].name, 1, 256, f);
		fwrite(&anims[i].start, 1, 4, f);
		fwrite(&anims[i].finish, 1, 4, f);
		fwrite(&anims[i].delay, 1, 4, f);
		fwrite(&anims[i].mode, 1, 4, f);
	}
	// Obstruction tiles
	fwrite(&numobs, 1, 4, f);
	cfwrite(obs, 1, numobs*256, f);
}

void VSP::UpdateAnimations()
{
	while (mytimer < systemtime)
	{
		AnimateTiles();
		mytimer++;
	}
}

void VSP::Blit(int x, int y, int index, image *dest)
{

	//if (index >= numtiles) err("VSP::BlitTile(), tile %d exceeds %d", index, numtiles);
	if (index >= numtiles) return;
	index = tileidx[index];
	if (index >= numtiles) return;
	//if (index >= numtiles) err("VSP::BlitTile(), tile %d exceeds %d", index, numtiles);
	quad* tile = vspdata->data + (index<<8);
	BlitTile(x, y, tile, dest);
}

void VSP::TBlit(int x, int y, int index, image *dest)
{
	while (mytimer < systemtime)
	{
		AnimateTiles();
		mytimer++;
	}
	//if (index >= numtiles) err("VSP::BlitTile(), tile %d exceeds %d", index, numtiles);
	if (index >= numtiles) return;
	index = tileidx[index];
	if (index >= numtiles) return;
	//if (index >= numtiles) err("VSP::BlitTile(), tile %d exceeds %d", index, numtiles);
	quad* tile = vspdata->data + (index<<8);
	TBlitTile(x, y, tile, dest);
}

void VSP::BlitObs(int x, int y, int index, image *dest)
{
	if (index >= numobs) return;
	char *c = (char *) obs + (index * 256);
	int white = MakeColor(255,255,255);
	for (int yy=0; yy<16; yy++)
		for (int xx=0; xx<16; xx++)
			if (c[(yy*16)+xx]) PutPixel(x+xx, y+yy, white, dest);
}

void VSP::AnimateTile(int i, int l)
{
	switch (anims[i].mode)
	{
	    case ANIM_MODE_FORWARD:
			if (tileidx[l]<anims[i].finish) tileidx[l]++;
            else tileidx[l]=anims[i].start;
            break;
		case ANIM_MODE_BACKWARD:
			if (tileidx[l]>anims[i].start) tileidx[l]--;
            else tileidx[l]=anims[i].finish;
            break;
		case ANIM_MODE_RANDOM:
			tileidx[l]=rnd(anims[i].start, anims[i].finish);
            break;
		case ANIM_MODE_PINGPONG:
			if (flipped[l])
            {
				if (tileidx[l]!=anims[i].start) tileidx[l]--;
				else { tileidx[l]++; flipped[l]=0; }
            }
            else
            {
				if (tileidx[l]!=anims[i].finish) tileidx[l]++;
				else { tileidx[l]--; flipped[l]=1; }
            }
			break;
	}
}

void VSP::AnimateTiles()
{
	for (int i=0; i<numanims; i++)
	{
		if ((anims[i].delay) && (anims[i].delay<vadelay[i]))
		{
			vadelay[i]=0;
			for (int l=anims[i].start; l<=anims[i].finish; l++)
				AnimateTile(i,l);
		}
		vadelay[i]++;
	}
}

void VSP::ValidateAnimations()
{
	for (int i=0; i<numanims; i++)
		if (anims[i].start<0 || anims[i].start>=numtiles || anims[i].finish<0 || anims[i].finish>=numtiles)
			err("VSP::ValidateAnimations() - animation %d references out of index tiles", i);
}


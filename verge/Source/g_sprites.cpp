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
 * verge3: g_sprites.cpp                                          *
 * copyright (c) 2004 vecna                                       *
 ******************************************************************/

#include "xerxes.h"

/****************************** data ******************************/

sprite sprites[256];

/****************************** code ******************************/

void ResetSprites()
{
	memset(sprites, 0, sizeof (sprites));
	for (int i=0; i<256; i++)
	{
		sprites[i].ent = -1;
	}
}

int GetSprite()
{
	for (int i=0; i<256; i++)
	{
		if (!sprites[i].image)
		{
			// Reset the element.
			memset(&sprites[i], 0,
					sizeof (sprite));	// <-- Important! Only initializing size of a struct (sprite).
										// Not array of structs (sprites).
			sprites[i].ent = -1;
			return i;
		}
	}
	return -1;
}

void RenderSprite(int i)
{
		int zx, zy;
		if (!sprites[i].image) return;
		if (sprites[i].wait) return;
		zx = sprites[i].x;
		zy = sprites[i].y;
		if (!sprites[i].sc || sprites[i].ent >= 0)
		{
			zx -= xwin;
			zy -= ywin;
		}
		if (sprites[i].ent >= 0)
		{
			zx += entity[sprites[i].ent]->getx() - entity[sprites[i].ent]->chr->hx;
			zy += entity[sprites[i].ent]->gety() - entity[sprites[i].ent]->chr->hy;
		}
		image *spr = ImageForHandle(sprites[i].image);
		if (sprites[i].alphamap)
		{
			image *alphamap = ImageForHandle(sprites[i].alphamap);
			AlphaBlit(zx, zy, spr, alphamap, screen);
			return;
		}
		if (sprites[i].addsub)
		{
			SetLucent(sprites[i].lucent);
			if (sprites[i].addsub<0)
				TSubtractiveBlit(zx, zy, spr, screen);
			else
				TAdditiveBlit(zx, zy, spr, screen);
			return;
		}
		if (sprites[i].silhouette)
		{
			SetLucent(sprites[i].lucent);
			Silhouette(zx, zy, sprites[i].color, spr, screen);
			return;
		}
		SetLucent(sprites[i].lucent);
		TBlit(zx, zy, spr, screen);
}

void RenderSprites()
{
	for (int i=0; i<256; i++)
	{
		if (sprites[i].onmap) continue;
		RenderSprite(i);
	}
	SetLucent(0);
}

void RenderLayerSprites(int layer)
{
	for (int i=0; i<256; i++)
	{
		if (!sprites[i].onmap) continue;
		if (sprites[i].ent >= 0) continue;
		if (sprites[i].layer != layer) continue;
		RenderSprite(i);
	}
	SetLucent(0);
}

void RenderSpritesBelowEntity(int ent)
{
	for (int i=0; i<256; i++)
	{
		if (!sprites[i].onmap) continue;
		if (sprites[i].ent != ent) continue;
		if (sprites[i].layer > 0) continue;
		RenderSprite(i);
	}
	SetLucent(0);
}


void RenderSpritesAboveEntity(int ent)
{
	for (int i=0; i<256; i++)
	{
		if (!sprites[i].onmap) continue;
		if (sprites[i].ent != ent) continue;
		if (sprites[i].layer < 1) continue;
		RenderSprite(i);
	}
	SetLucent(0);
}

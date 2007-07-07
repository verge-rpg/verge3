/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


class sprite
{
public:
	int x, y;
	int sc;
	int ent;
	int image, alphamap;
	int xflip, yflip;
	int addsub, lucent;
	int thinkrate;
	int thinkctr;
	int silhouette;
	int color;
	int wait;	// delay before processed.
	int timer;	// A timer of how long the sprite has been active.
	int layer;
	int onmap;
	std::string thinkproc;
};

extern sprite sprites[256];

void RenderSprites();
void ResetSprites();
void RenderLayerSprites(int layer);
void RenderSpritesBelowEntity (int ent);
void RenderSpritesAboveEntity (int ent);
int  GetSprite();

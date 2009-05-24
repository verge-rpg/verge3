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
	// we can no longer memset this struct, since it contains a std::string
	// so we set up a constructor to do the initialization for us
	sprite()
		: x(0), y(0)
		, sc(0)
		, ent(-1)
		, image(0), alphamap(0)
		, xflip(0), yflip(0)
		, addsub(0), lucent(0)
		, thinkrate(0), thinkctr(0)
		, silhouette(0)
		, color(0)
		, wait(0)
		, timer(0)
		, layer(0)
		, onmap(0)
	{}
	
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
	StringRef thinkproc;
};

extern std::vector<sprite> sprites;

void RenderSprites();
void ResetSprites();
void RenderLayerSprites(int layer);
void RenderSpritesBelowEntity (int ent);
void RenderSpritesAboveEntity (int ent);
int  GetSprite();

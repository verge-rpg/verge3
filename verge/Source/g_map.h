/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.

#include <vector>

struct Zone
{
	char name[256];
	char script[256];
	int method;
	int percent;
	int delay;
};

class Layer
{
public:
	Layer(VFILE *f);
	Layer(int width, int height);
	~Layer();
	void save(FILE *f); // Overkill (2006-07-20): Saver functions!
	int GetTile(int x, int y);
	void SetTile(int x, int y, int t);
    void SetParallaxX(double p);
    void SetParallaxY(double p);
	

	char layername[256];
	int lucent;
	int width, height;
    int x_offset, y_offset; // used to account for changing parallax
	double parallax_x, parallax_y;
	word *tiledata;
};

class MAP
{
public:
	char mapfname[256];
	int startx, starty;
	int mapwidth, mapheight;
	char mapname[256];
	char vspname[256];
	// Overkill 2006-05-21 - for specifying filename of vsp that will
	// be used with whatever map was just saved.
	char savevspname[256]; 
	char musicname[256];
	char renderstring[256];
	char startupscript[256];

	VSP *tileset;

	int numlayers;
	std::vector<Layer*> layers;
	byte *obslayer;
	word *zonelayer;

	int numzones;
	//Zone *zones;
	std::vector<Zone*> zones;

	int mapentities;

	MAP();
	MAP(char *fname);
	~MAP();
	void save(FILE *f); // Overkill (2006-07-20): Saver functions!

	int addLayer(int width, int height); // Overkill (2006-07-20): Add a layer.
	int zone(int x, int y);
	int obstruct(int x, int y);
	int obstructpixel(int x, int y);
	void SetZone(int x, int y, int t);
	void SetObs(int x, int y, int t);

	void BlitLayer(int l, int tx, int ty, int xwin, int ywin, image *dest);
	void TBlitLayer(int l, int tx, int ty, int xwin, int ywin, image *dest);
	void render(int x, int y, image *dest);
	void BlitObs(int tx, int ty, int xwin, int ywin, image *dest);
};

/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.

#include "xerxes.h"

struct vspanim_r
{
	char name[256];					// strand name
	int start;                      // strand start
	int finish;                     // strand end
	int delay;                      // tile-switch delay
	int mode;                       // tile-animation mode
};

class VSP
{
public:
	int numtiles;
	image *vspdata, *container;
	vspanim_r *anims;
	int numanims;
	int *vadelay, *tileidx, *flipped;
	quad mytimer;

	int numobs;
	char *obs;

	VSP(char *fname);
	VSP(VFILE *f);
	~VSP();
	void LoadVSP(VFILE *f);
	void save(FILE *f);	// Overkill (2006-07-20): Saver functions!
	void Blit(int x, int y, int index, image *dest);
	void TBlit(int x, int y, int index, image *dest);
	void BlitObs(int x, int y, int index, image *dest);
	void AnimateTile(int i, int l);
	void AnimateTiles();
	void ValidateAnimations();
	void UpdateAnimations();
	
	int GetObs(int t, int x, int y)
	{
		if (!t) return 0;
		if (t>=numobs || t<0) return 1;
		if (x<0 || y<0 || x>=G_TILESIZE || y>=G_TILESIZE) return 1;
		return obs[(t*G_TILESIZE*G_TILESIZE)+(y*G_TILESIZE)+x];
	}

};

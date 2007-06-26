/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


class CHR
{
public:
	image *rawdata, *container;

	int fxsize, fysize;					// frame x/y dimensions
	int hx, hy;							// x/y obstruction hotspot
	int hw, hh;							// hotspot width/hieght
	int totalframes;					// total # of frames.
    int idle[5];						// idle frames

	int animsize[9];
	int *anims[9];

	string name;                        // the filename this was loaded from

	CHR(char *fname);
	~CHR();
	void save(FILE *f);	// Overkill (2006-07-20): Saver functions!
	void render(int x, int y, int frame, image *dest);
	int GetFrame(int d, int &framect);
	int GetFrameConst(int d, int framect);

	char *parsestr;
	void ParseAnimation(int d, char *anim);
	int GetAnimLength(char *anim);
	int GetArg();
};

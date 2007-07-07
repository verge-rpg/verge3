/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


class Font
{
public:
	image *container, *rawdata;
	int width, height;
	int subsets, selected;
	int totalframes;
	char fwidth[100];
	bool incolor;

	Font(const char *fname, int xsize, int ysize);
	Font(const char *fname);
	~Font();
	void EnableVariableWidth();
	bool ColumnEmpty(int cell, int column);
	void PrintChar(char c, int x, int y, image *dest);
	void PrintString(char *str, int x, int y, image *dest...);
	void PrintCenter(char *str, int x, int y, image *dest...);
	void PrintRight(char *str, int x, int y, image *dest ...);
	void SetCharacterWidth(int character, int width);
	int  Pixels(const char *str);
};

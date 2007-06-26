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
 * verge3: vc_debug.cpp                                           *
 * copyright (c) 2002 vecna                                       *
 ******************************************************************/

#include "xerxes.h"

/****************************** data ******************************/

bool engine_paused = false;

/****************************** code ******************************/

class Window
{
public:
	int x1, y1, x2, y2;
	char *wintitle;

	Window() {}
	Window(int wx1, int wy1, int wx2, int wy2, char *title)
	{
		x1 = wx1;
		y1 = wy1;
		x2 = wx2;
		y2 = wy2;
		wintitle = title;
	}

	int mix(int ofs, int max)
	{
		int r = 10 + (156 * ofs / max);
		int g = 36 + (166 * ofs / max);
		int b = 106 + (134 * ofs / max);
		return MakeColor(r, g, b);
	}

	virtual void rendercontent(int xadj, int yadj, int width, int height)=0;

	void render()
	{
		Box(x1, y1, x2, y2, 0, screen);

		for (int x=x1+1; x<=x2-1; x++)
			VLine(x, y1+1, y1+9, mix(x-x1, x2-x1), screen);
		GotoXY(x1+2, y1+2); PrintString(wintitle, screen);

		screen->SetClip(x1+1, y1+10, x2-1,y2-1);
		rendercontent(x1, y1, x2-x1, y2-y1);
		screen->SetClip(0, 0, screen->width, screen->height);
	}
};


class StackTrace : public Window
{
public:
	StackTrace(int wx1, int wy1, int wx2, int wy2, char *title)	: Window(wx1,wy1,wx2,wy2,title) {}
	void rendercontent(int xadj, int yadj, int width, int height)
	{
		SetLucent(50);
		Rect(xadj, yadj, xadj+width, yadj+height, 0xffffffff, screen);
		SetLucent(0);
	}
};


void DebugBreakpoint(char *str)
{
	engine_paused = true;
	int mytime = systemtime;
	SetLucent(0);
	image *scr = new image(screen->width, screen->height);
	Blit(0, 0, screen, scr);

	StackTrace *stacktrace = new StackTrace(3, 3, 180, 80, "Stack Trace");

	while (!keys[SCAN_ESC])
	{
		UpdateControls();
		Blit(0, 0, scr, screen);
		stacktrace->render();
		ShowPage();
	}
	delete scr;
	delete stacktrace;
	keys[SCAN_ESC]=0;
	systemtime = mytime;
	engine_paused = false;
}

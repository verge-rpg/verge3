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
	win_mouse.cpp
 ****************************************************************/

#include "xerxes.h"

/***************************** data *****************************/

int mouse_x, mouse_y, mouse_l, mouse_r, mouse_m;
float mwheel;
bool WinCursor = true;

/***************************** code *****************************/

void mouse_Init()
{
	mouse_l = 0;
	mouse_r = 0;
	mouse_m = 0;
	mwheel = 0.0;
}

void mouse_set(int x, int y)
{
	float _x = (float)x/(float)vid_xres;
	float _y = (float)y/(float)vid_yres;

	RECT r;
	GetClientRect(hMainWnd, &r);

	POINT pt;
	pt.x = (int)(_x*(float)(r.right-r.left));
	pt.y = (int)(_y*(float)(r.bottom-r.top));
	ClientToScreen(hMainWnd,&pt);
	SetCursorPos(pt.x,pt.y);

	mouse_Update();
}

void mouse_Update()
{
	RECT dr;
	POINT pt;

	if (vid_window)
	{
		GetClientRect(hMainWnd, &dr);
		ClientToScreen(hMainWnd, (LPPOINT) &dr);
		ClientToScreen(hMainWnd, (LPPOINT) &dr + 1);
		if (!GetCursorPos(&pt)) return;
		mouse_x = (int)((float)(pt.x-dr.left)/((float)(dr.right-dr.left)/(float)vid_xres));
		mouse_y = (int)((float)(pt.y-dr.top)/((float)(dr.bottom-dr.top)/(float)vid_yres));
	}
	else
	{
		if (!GetCursorPos(&pt)) return;
		mouse_x = pt.x;
		mouse_y = pt.y;
		if (WinCursor && AppIsForeground)
		{
			ShowCursor(0);
			WinCursor = false;
		}
		if (!WinCursor && !AppIsForeground)
		{
			ShowCursor(1);
			WinCursor = true;
		}
	}
}
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
	mac_mouse.cpp
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
	float xRatio = (float)x / (float)vid_xres;
	float yRatio = (float)y / (float)vid_yres;

	int scaledX = (int)(xRatio * (float)(gameWindow->getWidth()));
	int scaledY = (int)(yRatio * (float)(gameWindow->getHeight()));

	sdl_Window* window = static_cast<sdl_Window*>(gameWindow);
	SDL_WarpMouseInWindow(window->window, scaledX, scaledY);

	mouse_Update();
}

void mouse_Update()
{
	int x, y;

	SDL_GetMouseState(&x, &y);

	sdl_Window* window = static_cast<sdl_Window*>(gameWindow);
	
	// the displayed area within the window
	int win_w, win_h;
	int win_x, win_y;
	
	if(window) {
		// an sdl_Window - we can get the actual displayed area
		window->get_displayed_area(win_w, win_h, win_x, win_y);
	} else {
		// fill in with best-guess (window completely taken up)
		win_w = gameWindow->getWidth();
		win_h = gameWindow->getHeight();
		win_x = win_y = 0;
	}
	
	mouse_x = (int)((float)(x - win_x) / ((float)(win_w) / (float)vid_xres));
	mouse_y = (int)((float)(y - win_y) / ((float)(win_h) / (float)vid_yres));
}
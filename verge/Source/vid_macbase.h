/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


#ifndef DDBASE_H
#define DDBASE_H

class sdl_Window : public AuxWindow
{
public:
	image *img;
	int imgHandle;
	SDL_Surface *screen_surface;
	SDL_Surface *back_surface;
	char *back_buffer;
	bool bGameWindow;
	bool bActive;
	bool shouldclear;
	int handle;
	int xres,yres;
	bool bVisible;
	
	int winx,winy;
	int winw,winh;
	
	
	sdl_Window(bool bGameWindow);
	virtual ~sdl_Window();
	void dispose();
	void flip_win();
	int set_win(int w, int h, int bpp);
	void shutdown_win();
	void deactivate();
	void activate();
	void createWindow();
	void setupDummyImage();
	void adjust(int w, int h);
	void get_displayed_area(int &out_w, int &out_h, int &out_x, int &out_y);
	
	//
	//specifically for AuxWindow
	int getHandle();
	int getImageHandle();
	image* getImage();
	int getXres();
	int getYres();
	int getWidth();
	int getHeight();
	void setResolution(int w, int h);
	void setPosition(int x, int y);
	void setSize(int w, int h);
	void setVisibility(bool vis);
	void setTitle(char *title);
	void positionCommand(int command, int arg1, int arg2);
};


void handleResize(SDL_ResizeEvent e);
void sdl_Flip();
void sdl_Fallback();
int sdl_SetMode(int xres, int yres, int bpp, bool windowflag);
void sdl_Close();
void dd_RegisterBlitters();
void sdl_video_init();
void sdl_toggleFullscreen();
#endif
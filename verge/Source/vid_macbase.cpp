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
	vid_macbase.cpp
 ****************************************************************/

#include "xerxes.h"
#include <stack>

/*
 * AuxWindows are not supported in Mac (SDL) video.
 * Much of the AuxWindow code remains below to make
 * adding such a feature easier at a later date.
 */


sdl_Window *sdl_gameWindow;

//forward decl
AuxWindow *sdl_createAuxWindow();
AuxWindow *sdl_findAuxWindow(int handle);

/***************************** data *****************************/

bool sdl_initd = false;
bool sdl_bGameWindowRectInitialized = false;

std::vector<sdl_Window*> sdl_windows;
std::stack<int> sdl_handles;
int sdl_handleCount;

/***************************** code *****************************/

void handleResize(SDL_ResizeEvent e)
{
	sdl_gameWindow->adjust(e.w, e.h);
	// Uncomment the following to have it redraw immediately
	// after we resize the window. This could result in a
	// partial image being drawn.
	// sdl_gameWindow->flip_win();
}


int sdl_handlePop()
{
	if(sdl_handles.empty())
	{
		for(int i=1;i<=4;i++)
			sdl_handles.push(sdl_handleCount+i);
		sdl_handleCount += 4;
	}

	int handle = sdl_handles.top();
	sdl_handles.pop();
	return handle;
}

void sdl_removeWindow(sdl_Window *window)
{
	for(std::vector<sdl_Window*>::iterator it = sdl_windows.begin(); it != sdl_windows.end(); it++)
		if(*it == window)
		{
			sdl_windows.erase(it);
			return;
		}
}

void sdl_video_init()
{
	for(int i=16;i>0;i--)
		sdl_handles.push(i);
	sdl_handleCount=16;

	sdl_gameWindow = new sdl_Window(true);
	gameWindow = dynamic_cast<AuxWindow*>(sdl_gameWindow);
	sdl_gameWindow->setVisibility(true);

	vid_Close = sdl_Close;
	Flip = sdl_Flip;
	vid_createAuxWindow = sdl_createAuxWindow;
	vid_findAuxWindow = sdl_findAuxWindow;
	sdl_initd = true;
}

int sdl_SetMode(int xres, int yres, int bpp, bool windowflag)
{
	if (!sdl_initd)
		sdl_video_init();

	int ret = sdl_gameWindow->set_win(xres,yres,bpp);
	if(!ret)
		return 0;

	sdl_gameWindow->xres = xres;
	sdl_gameWindow->yres = yres;

	vid_xres = xres;
	vid_yres = yres;
	vid_window = windowflag;

	dd_RegisterBlitters();

	//do this now for the gamewindow, because this is the first time we know what size to make the gamewindow
	//TODO: might want to check to see if we're moving up to a higher res than the window can display,
	//      and switch to the higher res if we are: xres > sdl_gameWindow->winw || yres > sdl_gameWindow->winh
	if(!sdl_bGameWindowRectInitialized)
	{
		sdl_gameWindow->adjust(xres,yres); // this sets the winw/winh correctly

		sdl_bGameWindowRectInitialized = true;
	}

	return 1;
}

void sdl_Flip()
{
	if(vid_window)
	{
		for(std::vector<sdl_Window*>::iterator it = sdl_windows.begin(); it != sdl_windows.end(); it++)
			(*it)->flip_win();
	}
	else
		sdl_gameWindow->flip_win();
}

void sdl_Close()
{
	std::vector<sdl_Window*> deletes;
	for(std::vector<sdl_Window*>::iterator it = sdl_windows.begin(); it != sdl_windows.end(); it++)
		deletes.push_back(*it);

	for(std::vector<sdl_Window*>::iterator it = deletes.begin(); it != deletes.end(); it++)
		(*it)->dispose();
}

AuxWindow *sdl_createAuxWindow()
{
	/* TODO Aux window create */
	err("Creating new windows is not implemented.");
	return 0;
}

AuxWindow *sdl_findAuxWindow(int handle)
{
	for(std::vector<sdl_Window*>::iterator it = sdl_windows.begin(); it != sdl_windows.end(); it++)
		if((*it)->handle == handle)
			return dynamic_cast<AuxWindow *>(*it);
	return 0;
}

void sdl_toggleFullscreen()
{
	vid_window = ! vid_window;
	sdl_gameWindow->adjust(vid_xres, vid_yres);
}

//////////////////////////////
/////// sdl_Window implementation
///////////////////////////////


int sdl_Window::getHandle() { return handle; }
int sdl_Window::getImageHandle() { return imgHandle; }
image *sdl_Window::getImage() { return img; }

int sdl_Window::getXres()
{
	return this->xres;
}
int sdl_Window::getYres()
{
	return this->yres;
}
int sdl_Window::getWidth()
{
	return this->winw;
}
int sdl_Window::getHeight()
{
	return this->winh;
}


void sdl_Window::setPosition(int x, int y)
{
	/* TODO set position */
}

void sdl_Window::positionCommand(int command, int arg1, int arg2)
{
	/* TODO set position */
}

void sdl_Window::setupDummyImage()
{
	if(!img)
	{
		img = new image(xres,yres);
		SetHandleImage(imgHandle,img);
		return;
	}

	image *newimg = new image(xres,yres);

	//copy current image to the new dummy image
	Blit(0,0,img,newimg);

	if(img)
		delete img;

	img = newimg;
	SetHandleImage(imgHandle,img);
}


void sdl_Window::setResolution(int w, int h)
{
	xres = w;
	yres = h;
	if(bActive)
	{
		if(bGameWindow)
			sdl_SetMode(w,h,vid_bpp,vid_window);
		else
			set_win(w,h,vid_bpp);
	}
	else
		setupDummyImage();
}

void sdl_Window::setSize(int w, int h)
{
	adjust(w,h);

	/* TODO Window placement */
}

void sdl_Window::setVisibility(bool vis)
{
	if(bActive)
	{
		if(!bVisible && vis)
		{
			/* TODO re-show window */
		}
		if(bVisible && !vis)
			SDL_WM_IconifyWindow();
	}
	bVisible = vis;
}

void sdl_Window::setTitle(const char *title)
{
	string titleConverted = title;
	for(int i = 0; i < titleConverted.length(); i++) {
		// check for high-ascii characters,
		// remove them and give warning message
		if(titleConverted[i] & 0x80) {
			// special case often used >= character
			if(titleConverted[i] == -77) {
				titleConverted[i] = '3';
			} else {
				titleConverted[i] = ' ';
			}
			log("Modified non 7-bit-ascii title.\n");
		}
	}
	SDL_WM_SetCaption(titleConverted.c_str(), 0);
}

void sdl_Window::createWindow()
{
}

void sdl_Window::shutdown_win()
{
}

void sdl_Window::dispose()
{
}

void sdl_Window::deactivate()
{
}

void sdl_Window::activate()
{
}

void sdl_Window::get_displayed_area(int &out_w, int &out_h, int &out_x, int &out_y) 
{
	const int dst_w = winw;
	const int dst_h = winh;
	const int src_w = xres;
	const int src_h = yres;	
	
	// find which is proportionally larger (are we
	// going to take up the full width or the full
	// height when scaling?)
	double heightMultiple = dst_h / (double)src_h;
	double widthMultiple = dst_w / (double)src_w;
	
	// check which (height or width) is more limiting
	if(heightMultiple < widthMultiple)
	{
		// height is more limiting, we scale
		// width to keep aspect ratio and use
		// full height of destination
		out_h = dst_h;
		out_w = (int)(heightMultiple * src_w);
		
		// ensure nothing funny happened with floats
		if(out_w > dst_w) {
			out_w = dst_w;
		}
		
		// then we center the image in the destination
		// by creating a width offset of half the
		// difference
		out_y = 0;
		out_x = (dst_w - out_w) / 2;
	}
	else
	{
		// same deal, but width is limiter
		out_w = dst_w;
		out_h = (int)(widthMultiple * src_h);
		
		// ensure nothing funny happened with floats
		if(out_h > dst_h) {
			out_h = dst_h;
		}			
		
		out_x = 0;
		out_y = (dst_h - out_h) / 2;
	}
}

void sdl_Window::flip_win()
{
	if(!bVisible)
		return;

	if(!img)
		return;

	SDL_UnlockSurface(screen_surface);
	SDL_Flip(screen_surface);	
	SDL_LockSurface(screen_surface);
}


// sets virtual window attributes (ie the buffer
// verge draws into, not the window on the screen)
int sdl_Window::set_win(int w, int h, int bpp)
{
	//vid_bytesperpixel = bpp / 8;

	//if I use this, then SDL tries to do some 565 -> 8888 conversion. oddly enough. or maybe its vice-versa
	//SDL_GL_SetAttribute(SDL_GL_RED_SIZE,8);
	//SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,8);
	//SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,0);

	screen_surface = SDL_SetVideoMode(512, 512, 0, (vid_window ? SDL_RESIZABLE : SDL_FULLSCREEN) | SDL_SWSURFACE); // SWSURFACE because we may need to scale into it in flip_win
	SDL_LockSurface(screen_surface);

	w = 320;
	h = 480;
	bpp = 32;

	vid_bpp = bpp;

	delete img;
	img = new image();
	img->shell = true;
	SetHandleImage(imgHandle,img);

	if(bGameWindow)
	{
		SetHandleImage(1,img);
		screen = img;
	}

	img->data = (quad*)screen_surface->pixels;
//	img->alphamap = 0;
//	img->bpp = bpp;
	img->width = w;
	img->height = h;
	img->pitch = 512; //TODO - change pitch to a constant for iphone, that should speed up some blitting. maybe have to code special cases
	img->cx1 = 0;
	img->cx2 = w-1;
	img->cy1 = 0;
	img->cy2 = h-1;

	return 1;
}

sdl_Window::sdl_Window(bool bGameWindow) : AuxWindow()
{
	img = 0;
	bVisible = false;
	shouldclear = true;
	back_buffer = 0;
	back_surface = 0;
	screen_surface = 0;
	xres = yres = 0;
    handle = sdl_handles.top();

	sdl_handles.pop();
	sdl_windows.push_back(this);

	//get an image handle to use from now on. we will fill in the imagebank slot for that handle
	//when we have an image to put there
	if(bGameWindow)
		imgHandle = 1;
	else
		imgHandle = HandleForImage(0);

	if(bGameWindow)
		bActive = true;
	else
	{
		if(vid_window)
			bActive = true;
		else
			bActive = false;
	}

	this->bGameWindow = bGameWindow;

	//always create the window now
	createWindow();
}

sdl_Window::~sdl_Window()
{

}

// Sets actual window dimensions & attrs
void sdl_Window::adjust(int w, int h)
{

}
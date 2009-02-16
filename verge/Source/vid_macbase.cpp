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

	// gather information about the back-buffer and
	// front buffer sizes
	const int dst_w = winw;
	const int dst_h = winh;
	const int src_w = xres;
	const int src_h = yres;
	
	if(shouldclear) {
		// fill it with black
		SDL_FillRect(screen_surface,NULL,SDL_MapRGB(screen_surface->format, 0, 0, 0));
		shouldclear = false;
	}
	
	if(dst_w == src_w && dst_h == src_h) {
		// same size - no scaling needed
		SDL_BlitSurface(back_surface, NULL, screen_surface, NULL);
	} else {
		
		int out_h, out_w; // the eventual size of the image
		int off_h, off_w; // the eventual placement of the image

		get_displayed_area(out_w, out_h, off_w, off_h);
		
		// run the actual scaling, using algorithm from vid_ddblit's dd32_ScaleBlit
		// with some parts removed because we know the whole image is blitted
		// (ie no clipping)
		int xadj = (src_w << 16) / out_w;
		int yadj = (src_h << 16) / out_h;
		int xerr;
		int yerr = 0;
		
		// these pitches are in pixels, instead of bytes as SDL
		int src_pitch = back_surface->pitch / 4;
		int dst_pitch = screen_surface->pitch / 4;
		quad *d, *s;
		
		SDL_LockSurface(screen_surface);
		SDL_LockSurface(back_surface);
		
		s = (quad *) back_surface->pixels;
		d = ((quad *) screen_surface->pixels) + (off_h * dst_pitch) + off_w;
		
		for(int i = 0; i < out_h; i++) {
			xerr = 0;
			for(int j = 0; j < out_w; j++) {
				d[j] = s[(xerr >> 16)];
				xerr += xadj;
			}
			d    += dst_pitch;
			yerr += yadj;
			s    += (yerr >> 16) * src_pitch;
			yerr &= 0xffff;
		}
		
		SDL_UnlockSurface(back_surface);
		SDL_UnlockSurface(screen_surface);
	}
	
	SDL_Flip(screen_surface);	
}


// sets virtual window attributes (ie the buffer
// verge draws into, not the window on the screen)
int sdl_Window::set_win(int w, int h, int bpp)
{
	if(back_buffer) {
		if(w == xres &&
		   h == yres &&
		   bpp == vid_bpp)
			return 1; // no changes to be made
	}
	if(back_buffer) {
		delete back_buffer;
		SDL_FreeSurface(back_surface);
	}

	try {
		back_buffer = new char[w * h * bpp/8];
	} catch(std::bad_alloc&) {
		err("Couldn't make back buffer of %dx%d at %d bpp", w, h, bpp);
	}

	back_surface = SDL_CreateRGBSurfaceFrom(back_buffer, w, h, 32, w * 4, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000);

	vid_bpp = bpp;
	//vid_bytesperpixel = bpp / 8;

	if(img) delete img;
	img = new image();
	img->shell = true;
	SetHandleImage(imgHandle,img);

	if(bGameWindow)
	{
		SetHandleImage(1,img);
		screen = img;
	}

	img->data = (quad*)back_buffer;
//	img->alphamap = 0;
//	img->bpp = bpp;
	img->width = w;
	img->height = h;
	img->pitch = w;
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
	if(back_buffer) {
		delete back_buffer;		
		SDL_FreeSurface(back_surface);
	}

}

// Sets actual window dimensions & attrs
void sdl_Window::adjust(int w, int h)
{
	// we're never going to get a 320x240 window back from SDL,
	// so just ask for a 640x480 one and we'll scale it up when we flip
	if(!vid_window && w == 320 && h == 240 )
	{
		w = 640;
		h = 480;
	}
	
	screen_surface = SDL_SetVideoMode(w, h, vid_bpp, (vid_window ? SDL_RESIZABLE : SDL_FULLSCREEN) | SDL_SWSURFACE); // SWSURFACE because we may need to scale into it in flip_win
	shouldclear = true;

	if(!screen_surface) {
		err("Could not get SDL window: %s.", SDL_GetError());
	}
	// set to resulting video height/width, not requested
	// so that blitting knows the actual dimensions
	this->winw = screen_surface->w;
	this->winh = screen_surface->h;
}
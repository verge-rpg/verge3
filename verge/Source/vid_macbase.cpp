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
#include <cassert>

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

std::vector<sdl_Window*> sdl_windows;
std::stack<int> sdl_handles;
int sdl_handleCount;

/***************************** code *****************************/

void handleResize(const SDL_WindowEvent& e)
{
	assert(e.event == SDL_WINDOWEVENT_RESIZED);
	log("resize event %d %d", e.data1, e.data2);
	sdl_gameWindow->adjust(e.data1, e.data2);
	// Uncomment the following to have it redraw immediately
	// after we resize the window. This could result in a
	// partial image being drawn.
	// sdl_gameWindow->flip_win();
}

int sdl_handlePop()
{
	if(sdl_handles.empty())
	{
		for(int i = 1; i <= 4; i++)
		{
			sdl_handles.push(++sdl_handleCount);
		}
	}

	int handle = sdl_handles.top();
	sdl_handles.pop();
	return handle;
}

void sdl_removeWindow(sdl_Window *window)
{
	for (auto it = sdl_windows.begin(); it != sdl_windows.end(); ++it)
	{
		if (*it == window)
		{
			sdl_windows.erase(it);
			return;
		}
	}
}

void sdl_video_init()
{
    for(int i = 16; i > 0; i--)
		sdl_handles.push(i);
	sdl_handleCount = 16;

	int winw = v3_xres;
	int winh = v3_yres;
	int base_win_x_res = getInitialWindowXres();
	int base_win_y_res = getInitialWindowYres();

	/// this is for the windowsize verge.cfg vars.
	if (base_win_x_res > 0 && base_win_y_res > 0)
	{
		winw = base_win_x_res;
		winh = base_win_y_res;
	}

	sdl_gameWindow = new sdl_Window(true, winw, winh);
	gameWindow = sdl_gameWindow;
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

	int ret = sdl_gameWindow->set_win(xres, yres, bpp);
	if(!ret)
		return 0;

	sdl_gameWindow->xres = xres;
	sdl_gameWindow->yres = yres;

	vid_xres = xres;
	vid_yres = yres;
	vid_window = windowflag;

	dd_RegisterBlitters();

	if (xres > sdl_gameWindow->winw || yres > sdl_gameWindow->winh)
	{
		log("expanding to fit resolution %d %d", xres, yres);
		sdl_gameWindow->adjust(xres, yres);
	}

	return 1;
}

void sdl_Flip()
{
	if(vid_window)
	{
		for (auto& win : sdl_windows)
		{
			win->flip_win();
		}
	}
	else
	{
		sdl_gameWindow->flip_win();
	}
}

void sdl_Close()
{
	for (auto& win : sdl_windows)
	{
		win->dispose();
	}
}

AuxWindow *sdl_createAuxWindow()
{
	return new sdl_Window(false, 320, 240);
}

AuxWindow *sdl_findAuxWindow(int handle)
{
	for (auto& win : sdl_windows)
	{
		if (win->handle == handle)
		{
			return win;
		}
	}
	return nullptr;
}

void sdl_toggleFullscreen()
{
	log("toggle fullscreen %d %d", vid_xres, vid_yres);

	vid_window = !vid_window;
	sdl_gameWindow->adjust(vid_xres, vid_yres);
}

//////////////////////////////
/////// sdl_Window implementation
///////////////////////////////


sdl_Window::sdl_Window(bool bGameWindow, int w, int h)
: AuxWindow(),
	img(0),
	imgHandle(0),
	window(nullptr),
	back_surface(nullptr),
	back_buffer(nullptr),
	bGameWindow(bGameWindow),
	bActive(false),
    handle(0),
	xres(0), yres(0),
	bVisible(false),
	winw(w), winh(h)
{
	handle = sdl_handlePop();

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
	//always create the window now
	createWindow();
}

sdl_Window::~sdl_Window()
{
	if(back_buffer)
	{
		delete back_buffer;		
		SDL_FreeSurface(back_surface);
	}
}

void sdl_Window::dispose()
{
	if (bGameWindow)
		return;

	sdl_removeWindow(this);
	sdl_handles.push(handle);

	if(!bGameWindow)
		FreeImageHandle(this->imgHandle);

	if (window != nullptr)
	{
		SDL_DestroyWindow(window);
		window = nullptr;
	}
}

void sdl_Window::flip_win()
{
	if (!bVisible)
		return;

	if (!img)
		return;

	// gather information about the back-buffer and
	// front buffer sizes
	SDL_Surface* screen_surface = SDL_GetWindowSurface(window);
	winw = screen_surface->w;
	winh = screen_surface->h;	
	const int dst_w = screen_surface->w;
	const int dst_h = screen_surface->h;
	const int src_w = xres;
	const int src_h = yres;
	
	if (dst_w == src_w && dst_h == src_h)
	{
#ifdef __EMSCRIPTEN__
        SDL_LockSurface(screen_surface);

        int size = dst_w * dst_h;
        quad* s = (quad*) back_buffer;
        quad* d = (quad*) screen_surface->pixels;

        // same size - no scaling needed
        for (int i = 0; i != size; i++)
		{
            quad p = *s++;
            byte r = p & 0xFF;
            byte g = (p >> 8) & 0xFF;
            byte b = (p >> 16) & 0xFF;
            *d++ = (r << 16) | (g << 8) | b | 0xFF000000;
        }

        SDL_UnlockSurface(screen_surface);
#else		
		// same size - no scaling needed
		SDL_BlitSurface(back_surface, NULL, screen_surface, NULL);
#endif		
	}
	else
	{
    	ScaleFormat sf = vid_window ? v3_scale_win : v3_scale_full;

		int iw = 0;
		int ih = 0;
		if (sf == SCALE_FORMAT_STRETCH)
		{
			iw = dst_w;
			ih = dst_h;
		}
		else
		{
			int factor = dst_w / xres;
			if (sf == SCALE_FORMAT_ASPECT && factor > dst_h / yres)
			{
				factor = dst_h / yres;
			}
			// Window can fit entire screen. Factor is upscaling, multiply by integer ratio.
			if (sf == SCALE_FORMAT_ASPECT && factor >= 1)
			{
				iw = xres * factor;
				ih = yres * factor;
			}
			// Window can't fit entire screen. Need to downscale this, fallback on lossy floating point scaling.
			else
			{
				float ratio = (float)dst_w / xres;
				float ph = ratio * (float)yres;
				if((int)ph > dst_h)
				{
					ratio = (float)dst_h / (float)yres;
					ih = dst_h;
					iw = (int)(ratio * (float)xres);
				}
				else
				{
					ih = (int)ph;
					iw = dst_w;
				}
			}
		}		

		// TODO: look at vid_gdibase to see how to handle ScaleFormat stuff

		int out_w = iw;
		int out_h = ih;
		int off_w = (dst_w - iw) / 2;
		int off_h = (dst_h - ih) / 2;

		// run the actual scaling, using algorithm from vid_ddblit's dd32_ScaleBlit
		// with some parts removed because we know the whole image is blitted
		// (ie no clipping)
		int xadj = (src_w << 16) / out_w;
		int yadj = (src_h << 16) / out_h;
		int yerr = 0;
		
		// these pitches are in pixels, instead of bytes as SDL
		int src_pitch = back_surface->pitch / 4;
		int dst_pitch = screen_surface->pitch / 4;
		
		SDL_LockSurface(screen_surface);
		SDL_LockSurface(back_surface);

		//memset(screen_surface->pixels, 0, screen_surface->pitch * screen->width);
		
		quad* s = (quad*) back_surface->pixels;
		quad* d = ((quad*) screen_surface->pixels) + (off_h * dst_pitch) + off_w;
		
		for (int i = 0; i < out_h; i++)
		{
			int xerr = 0;
			for (int j = 0; j < out_w; j++)
			{
#ifdef __EMSCRIPTEN__
	            quad p = s[(xerr >> 16)];
	            byte r = p & 0xFF;
	            byte g = (p >> 8) & 0xFF;
	            byte b = (p >> 16) & 0xFF;
				d[j] = (r << 16) | (g << 8) | b | 0xFF000000;
#else

				d[j] = s[(xerr >> 16)];
#endif				
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
	
	SDL_UpdateWindowSurface(window);
}

// sets virtual window attributes (ie the buffer
// verge draws into, not the window on the screen)
int sdl_Window::set_win(int w, int h, int bpp)
{
	if(back_buffer)
	{
		if(w == xres
		&& h == yres
		&& bpp == vid_bpp)
		{
			return 1; // no changes to be made
		}
	}
	if(back_buffer)
	{
		delete back_buffer;
		SDL_FreeSurface(back_surface);
	}

	try
	{
		back_buffer = new char[w * h * bpp/8];
	}
	catch(std::bad_alloc&)
	{
		err("Couldn't make back buffer of %dx%d at %d bpp", w, h, bpp);
	}

	back_surface = SDL_CreateRGBSurfaceFrom(back_buffer, w, h, 32, w * 4, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000);

	vid_bpp = bpp;
	vid_bytesperpixel = bpp / 8;

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
	img->bpp = bpp;
	img->width = w;
	img->height = h;
	img->pitch = w;
	img->cx1 = 0;
	img->cx2 = w-1;
	img->cy1 = 0;
	img->cy2 = h-1;

	return 1;
}

void sdl_Window::shutdown_win()
{
}

void sdl_Window::deactivate()
{
	if (!bActive)
		return;
	bActive = false;
	setupDummyImage();
	shutdown_win();
	SDL_HideWindow(window);
}

void sdl_Window::activate()
{
	image* tempimg = new image(xres,yres);
	Blit(0,0,img,tempimg);
	if (bActive)
		return;
	bActive = true;
	set_win(xres,yres,vid_bpp);
	Blit(0,0,tempimg,img);
	if (bVisible)
	{
		SDL_ShowWindow(window);
	}
}

void sdl_Window::createWindow()
{
	window = SDL_CreateWindow("verge3",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		winw,
		winh,
		(vid_window || !bGameWindow ? SDL_WINDOW_RESIZABLE : SDL_WINDOW_FULLSCREEN_DESKTOP)
			| (bGameWindow ? 0 : SDL_WINDOW_HIDDEN));
	SDL_GetWindowSize(window, &winw, &winh);

#ifdef __EMSCRIPTEN__
	EM_ASM(
    	window.onWindowResize();
	);
#endif
}

void sdl_Window::setupDummyImage()
{
	if(!img)
	{
		img = new image(xres, yres);
		SetHandleImage(imgHandle, img);
		return;
	}

	image *newimg = new image(xres,yres);

	//copy current image to the new dummy image
	Blit(0, 0, img, newimg);

	if(img)
		delete img;

	img = newimg;
	SetHandleImage(imgHandle, img);
}

// Sets actual window dimensions & attrs
void sdl_Window::adjust(int w, int h)
{
	log("adjust %d %d %d %d", xres, yres, w, h);		
	SDL_SetWindowSize(window, w, h);
	if (bGameWindow)
	{
		SDL_SetWindowFullscreen(window, vid_window ? SDL_WINDOW_RESIZABLE : SDL_WINDOW_FULLSCREEN_DESKTOP);
	}
	SDL_GetWindowSize(window, &winw, &winh);

#ifdef __EMSCRIPTEN__
	EM_ASM(
    	window.onWindowResize();
	);
#endif	
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

void sdl_Window::setResolution(int w, int h)
{
	xres = w;
	yres = h;
	if (bActive)
	{
		log("setResolution %d %d", w, h);		

		if(bGameWindow)
			sdl_SetMode(w, h, vid_bpp, vid_window);
		else
			set_win(w, h, vid_bpp);
	}
	else
		setupDummyImage();
}

void sdl_Window::setPosition(int x, int y)
{
	SDL_SetWindowPosition(window, x, y);
}

void sdl_Window::setSize(int w, int h)
{
	log("set size %d %d", w, h);		

	adjust(w, h);
}

void sdl_Window::setVisibility(bool vis)
{
	if(bActive)
	{
		if (vis)
		{
			SDL_ShowWindow(window);
		}
		else
		{
			SDL_HideWindow(window);
		}
	}

	bVisible = vis;
}

void sdl_Window::setTitle(const char *title)
{
	string titleConverted = title;
	for(int i = 0; i < titleConverted.length(); i++)
	{
		// check for high-ascii characters,
		// remove them and give warning message
		if(titleConverted[i] & 0x80)
		{
			// special case often used >= character
			if(titleConverted[i] == -77)
			{
				titleConverted[i] = '3';
			}
			else
			{
				titleConverted[i] = ' ';
			}
			log("Modified non 7-bit-ascii title.\n");
		}
	}

	SDL_SetWindowTitle(window, titleConverted.c_str());
}

void sdl_Window::positionCommand(int command, int arg1, int arg2)
{
	switch (command)
	{
		case 0:
		{
			auto destwin = sdl_findAuxWindow(arg1);

			int srcX, srcY;
			int srcW, srcH;
			SDL_GetWindowPosition(window, &srcX, &srcY);
			SDL_GetWindowSize(window, &srcW, &srcH);
			
			int destX, destY;
			int destW, destH;
			SDL_GetWindowPosition(window, &destX, &destY);
			SDL_GetWindowSize(window, &destW, &destH);

			srcX = destX - srcW;
			srcY = destY;
			setPosition(srcX, srcY);
		}
		break;
	}
}
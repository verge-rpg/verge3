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
	vid_ddbase.cpp
 ****************************************************************/

#include "xerxes.h"
#include "ddraw.h"
#include <crtdbg.h>
#include <stack>

//todo

//when you start off in fullscreen mode, a good window position is not being selected
//for when you alt-enter back into windowmode

class dd_Window : public AuxWindow
{
public:
	LPDIRECTDRAWSURFACE dx_bs,dx_os,dx_win_bs;
	DDSURFACEDESC dx_bsd,dx_osd,dx_win_bsd;
	LPDIRECTDRAWCLIPPER clipper;

	image *img;
	int imgHandle;
	HWND hwnd;
	bool bGameWindow;
	bool bActive;
	int handle;
	int xres,yres;
	bool bVisible;

	int winx,winy;
	int winw,winh;


	dd_Window(bool bGameWindow);
	void dispose();
	void flip_win();
	void flip_fullscreen();
	int set_win(int w, int h, int bpp);
	void shutdown_win();
	int set_fullscreen(int w, int h, int bpp);
	void deactivate();
	void activate();
	void createWindow();
	void setupDummyImage();
	void adjust(int w, int h, RECT *r);

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
	void setTitle(const char *title);
	void positionCommand(int command, int arg1, int arg2);
};

dd_Window *dd_gameWindow;

//forward decl
AuxWindow *dd_createAuxWindow();
AuxWindow *dd_findAuxWindow(int handle);

/***************************** data *****************************/

LPDIRECTDRAW dx_dd;
LPDIRECTDRAWSURFACE dx_ps,dx_win_ps;
DDSURFACEDESC dx_psd,dx_win_psd;

bool dd_initd = false;
bool dd_bHasBeenFullscreen = false;
bool dd_bGameWindowRectInitialized = false;
bool dd_bWasMaximized = false;

//dd_Window *gameWindow;

std::vector<dd_Window*> dd_windows;
std::stack<int> dd_handles;
int dd_handleCount;

/***************************** code *****************************/

#define DX_RELEASE(x) { if (x) { x->Release(); x=0; } }

int dd_handlePop()
{
	if(dd_handles.empty())
	{
		for(int i=1;i<=4;i++)
			dd_handles.push(dd_handleCount+i);
		dd_handleCount += 4;
	}

	int handle = dd_handles.top();
	dd_handles.pop();
	return handle;
}

void dd_removeWindow(dd_Window *window)
{
	for(std::vector<dd_Window*>::iterator it = dd_windows.begin(); it != dd_windows.end(); it++)
		if(*it == window)
		{
			dd_windows.erase(it);
			return;
		}
}

void dd_init()
{
	HRESULT hr = DirectDrawCreate(NULL, &dx_dd, NULL);
	if (hr != DD_OK)
		err("Could not create DirectDraw object.");

	for(int i=16;i>0;i--)
		dd_handles.push(i);
	dd_handleCount=16;

	WNDCLASS WndClass;
	memset(&WndClass, 0, sizeof(WNDCLASS));
	WndClass.hCursor = LoadCursor(0, IDC_ARROW);
	WndClass.hIcon = LoadIcon(hMainInst, MAKEINTRESOURCE(1));
	WndClass.lpszClassName = "xerxes-dd-gamewindow";
	WndClass.hInstance = hMainInst;
	WndClass.lpfnWndProc = win_gameWindowProc;
	RegisterClass(&WndClass);

	memset(&WndClass, 0, sizeof(WNDCLASS));
	WndClass.hCursor = LoadCursor(0, IDC_ARROW);
	WndClass.lpszClassName = "xerxes-dd-auxwindow";
	WndClass.hInstance = hMainInst;
	WndClass.lpfnWndProc = win_auxWindowProc;
	RegisterClass(&WndClass);

	dd_gameWindow = new dd_Window(true);
	gameWindow = dynamic_cast<AuxWindow*>(dd_gameWindow);
	hMainWnd = dd_gameWindow->hwnd;
	dd_gameWindow->setVisibility(true);

	if(automax)
	{
		ShowWindow(dd_gameWindow->hwnd,SW_SHOWMAXIMIZED);
	}

	vid_Close = dd_Close;
	Flip = dd_Flip;
	vid_createAuxWindow = dd_createAuxWindow;
	vid_findAuxWindow = dd_findAuxWindow;
	dd_initd = true;
}

int dd_SetMode(int xres, int yres, int bpp, bool windowflag)
{
	HRESULT hr;

	if (!dd_initd)
		dd_init();

	//do this now for the gamewindow, because this is the first time we know what size to make the gamewindow
	if(!dd_bGameWindowRectInitialized)
	{
		RECT r;
		dd_gameWindow->adjust(xres,yres,&r);

		int base_win_x_res = getInitialWindowXres();
		int base_win_y_res = getInitialWindowYres();

		/// this is for the windowsize verge.cfg vars.
		if( base_win_x_res > 0 && base_win_y_res > 0 ) {
			int win_offset_x = (r.right-r.left) - xres;
			int win_offset_y = (r.bottom-r.top) - yres;

			dd_gameWindow->winw = win_offset_x+base_win_x_res;
			dd_gameWindow->winh = win_offset_y+base_win_y_res;
		
		} else {
			dd_gameWindow->winw = r.right-r.left;
			dd_gameWindow->winh = r.bottom-r.top;
		}

		WINDOWPLACEMENT wp;
		wp.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(dd_gameWindow->hwnd,&wp);
		wp.rcNormalPosition.left = GetSystemMetrics(SM_CXSCREEN)/2-xres/2;
		wp.rcNormalPosition.top = GetSystemMetrics(SM_CYSCREEN)/2-yres/2;
		wp.rcNormalPosition.right = wp.rcNormalPosition.left + dd_gameWindow->winw;
		wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + dd_gameWindow->winh;
		SetWindowPlacement(dd_gameWindow->hwnd,&wp);

		dd_bGameWindowRectInitialized = true;
	}

	//must deactivate all auxwindows
	if(vid_window && !windowflag)
		for(std::vector<dd_Window*>::iterator it = dd_windows.begin(); it != dd_windows.end(); it++)
		{
			if(!(*it)->bGameWindow)
				(*it)->deactivate();
		}

	if (!windowflag)
	{
		//if we are switching into fullscreen, we are going to lose these sizes and positions
		//save them now so we can restore them when we flip back to windowmode
		if(vid_window)
		{
			WINDOWPLACEMENT wp;
			wp.length = sizeof(WINDOWPLACEMENT);
			GetWindowPlacement(dd_gameWindow->hwnd,&wp);
			dd_gameWindow->winx = wp.rcNormalPosition.left;
			dd_gameWindow->winy = wp.rcNormalPosition.top;
			dd_gameWindow->winw = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
			dd_gameWindow->winh = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
			dd_bWasMaximized = (wp.showCmd == SW_SHOWMAXIMIZED);
		}

		//ShowWindow(dd_gameWindow->hwnd,SW_SHOWMAXIMIZED);
		ShowWindow(dd_gameWindow->hwnd,SW_HIDE);

		int ret = dd_gameWindow->set_fullscreen(xres,yres,bpp);
		if(!ret)
			return 0;

		ShowWindow(dd_gameWindow->hwnd,SW_SHOW);

		dd_gameWindow->xres = xres;
		dd_gameWindow->yres = yres;

		vid_xres = xres;
		vid_yres = yres;
		vid_window = false;
		dd_bHasBeenFullscreen = true;

		dd_RegisterBlitters();
		return ret;
	}
	else
	{
		DX_RELEASE(dx_ps);
		DX_RELEASE(dx_win_ps);

		if (bpp != DesktopBPP) return 0;
		if (!vid_window)
			dx_dd->RestoreDisplayMode();
		dx_dd->SetCooperativeLevel(dd_gameWindow->hwnd, DDSCL_NORMAL);

		hr = dx_dd->CreateSurface(&dx_win_psd, &dx_win_ps, NULL);
		if (hr != DD_OK)
		{
			return 0;
		}

		int ret = dd_gameWindow->set_win(xres,yres,bpp);
		if(!ret)
			return 0;


		if(dd_bHasBeenFullscreen)
		{
			WINDOWPLACEMENT wp;
			wp.length = sizeof(WINDOWPLACEMENT);
			GetWindowPlacement(dd_gameWindow->hwnd,&wp);
			wp.rcNormalPosition.left = dd_gameWindow->winx;
			wp.rcNormalPosition.top = dd_gameWindow->winy;
			wp.rcNormalPosition.right = wp.rcNormalPosition.left + dd_gameWindow->winw;
			wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + dd_gameWindow->winh;
			SetWindowPlacement(dd_gameWindow->hwnd,&wp);

			if(dd_bWasMaximized)
				ShowWindow(dd_gameWindow->hwnd,SW_SHOWMAXIMIZED);
		}

		//must activate all auxwindows
		if(!vid_window)
			for(std::vector<dd_Window*>::iterator it = dd_windows.begin(); it != dd_windows.end(); it++)
				if(!(*it)->bGameWindow)
					(*it)->activate();

		//bring the gamewindow back to the front
		SetWindowPos(dd_gameWindow->hwnd,HWND_TOP,0,0,0,0,SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);

		dd_gameWindow->xres = xres;
		dd_gameWindow->yres = yres;

		vid_xres = xres;
		vid_yres = yres;
		vid_window = true;

		dd_RegisterBlitters();
		return 1;
	}
	return 0;
}

void dd_Flip()
{
	if(vid_window)
	{
		for(std::vector<dd_Window*>::iterator it = dd_windows.begin(); it != dd_windows.end(); it++)
			(*it)->flip_win();
	}
	else
		dd_gameWindow->flip_fullscreen();
}

void dd_Close()
{
	std::vector<dd_Window*> deletes;
	for(std::vector<dd_Window*>::iterator it = dd_windows.begin(); it != dd_windows.end(); it++)
		deletes.push_back(*it);

	for(std::vector<dd_Window*>::iterator it = deletes.begin(); it != deletes.end(); it++)
		(*it)->dispose();

	DX_RELEASE(dx_ps);
	DX_RELEASE(dx_win_ps);
	DX_RELEASE(dx_dd);
}

AuxWindow *dd_createAuxWindow()
{
	return dynamic_cast<AuxWindow *>(new dd_Window(false));
}

AuxWindow *dd_findAuxWindow(int handle)
{
	for(std::vector<dd_Window*>::iterator it = dd_windows.begin(); it != dd_windows.end(); it++)
		if((*it)->handle == handle)
			return dynamic_cast<AuxWindow *>(*it);
	return 0;
}


//////////////////////////////
/////// dd_Window implementation
///////////////////////////////


int dd_Window::getHandle() { return handle; }
int dd_Window::getImageHandle() { return imgHandle; }
image *dd_Window::getImage() { return img; }

int dd_Window::getXres()
{
	return this->xres;
}
int dd_Window::getYres()
{
	return this->yres;
}
int dd_Window::getWidth()
{
	RECT r;
	GetClientRect(hwnd,&r);
	return r.right - r.left;
}
int dd_Window::getHeight()
{
	RECT r;
	GetClientRect(hwnd,&r);
	return r.bottom - r.top;
}


void dd_Window::setPosition(int x, int y)
{
	//SetWindowPos(hwnd,0,x,y,0,0,SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
	HWND hw = GetActiveWindow();
	WINDOWPLACEMENT wp;
	wp.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(hwnd,&wp);
	int w = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
	int h = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
	wp.rcNormalPosition.left = x;
	wp.rcNormalPosition.top = y;
	wp.rcNormalPosition.right = wp.rcNormalPosition.left + w;
	wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + h;
	SetWindowPlacement(hwnd,&wp);
	SetActiveWindow(hw);
}

void dd_Window::positionCommand(int command, int arg1, int arg2)
{
	//todo: validate window args
	switch(command)
	{
	case 0:
		{
			RECT rDest;
			RECT rSrc;
			int w,h;

			dd_Window *destwin = (dd_Window *)(dd_findAuxWindow(arg1));
			GetWindowRect(destwin->hwnd,&rDest);
			GetWindowRect(hwnd,&rSrc);
			w = rSrc.right - rSrc.left;
			h = rSrc.bottom - rSrc.top;
			rSrc.left = rDest.left - w;
			rSrc.top = rDest.top;
			//SetWindowPos(hwnd,0,rSrc.left,rSrc.top,0,0,SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
			setPosition(rSrc.left,rSrc.top);
		}
		break;
	}
}

void dd_Window::setupDummyImage()
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


void dd_Window::setResolution(int w, int h)
{
	xres = w;
	yres = h;
	if(bActive)
	{
		if(bGameWindow)
			dd_SetMode(w,h,vid_bpp,vid_window);
		else
			set_win(w,h,vid_bpp);
	}
	else
		setupDummyImage();
}

void dd_Window::setSize(int w, int h)
{
	RECT r;
	adjust(w,h,&r);

	WINDOWPLACEMENT wp;
	wp.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(hwnd,&wp);
	wp.rcNormalPosition.right = wp.rcNormalPosition.left + (r.right-r.left);
	wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + (r.bottom-r.top);
	SetWindowPlacement(hwnd,&wp);
}

void dd_Window::setVisibility(bool vis)
{
	if(bActive)
	{
		if(!bVisible && vis)
		{
			ShowWindow(hwnd,SW_SHOWNA);
			if(!bGameWindow)
				SetWindowPos(hwnd,hMainWnd,0,0,0,0,SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
		}
		if(bVisible && !vis)
			ShowWindow(hwnd,SW_HIDE);
	}
	bVisible = vis;
}
void dd_Window::setTitle(const char *title)
{
	SetWindowText(hwnd,title);
}


void dd_Window::createWindow()
{
	if(bGameWindow)
	{
		hwnd = CreateWindowEx(0, "xerxes-dd-gamewindow", "xerxes", WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_THICKFRAME | WS_CAPTION, 0,0,0,0, NULL, NULL, hMainInst, NULL);
		SetWindowLong(hwnd,GWL_USERDATA,(long)dynamic_cast<AuxWindow*>(this));
		win_addWindow(hwnd);
	}
	else
	{
		hwnd = CreateWindowEx(0, "xerxes-dd-auxwindow", "xerxes", WS_CAPTION | WS_THICKFRAME, 0,0,0,0,0,0,hMainInst,0);
		SetWindowLong(hwnd,GWL_USERDATA,(long)dynamic_cast<AuxWindow*>(this));
		win_addWindow(hwnd);
	}
}

void dd_Window::shutdown_win()
{
	DX_RELEASE(dx_win_bs);
}

void dd_Window::dispose()
{
	if(bGameWindow)
		return;

	dd_removeWindow(this);
	dd_handles.push(handle);

	if(!bGameWindow)
		FreeImageHandle(this->imgHandle);

	shutdown_win();

	win_removeWindow(hwnd);
	DestroyWindow(hwnd);
}

void dd_Window::deactivate()
{
	if(!bActive)
		return;
	bActive = false;
	setupDummyImage();
	shutdown_win();
	ShowWindow(hwnd,SW_HIDE);
}

void dd_Window::activate()
{
	image *tempimg = new image(xres,yres);
	Blit(0,0,img,tempimg);
	if(bActive)
		return;
	bActive = true;
	set_win(xres,yres,vid_bpp);
	Blit(0,0,tempimg,img);
	if(bVisible)
		ShowWindow(hwnd,SW_SHOWNA);
}

void dd_Window::flip_win()
{
	RECT r;

	if(!bVisible)
		return;

	if(!img)
		return;

	//convert the image format if necessary
	if(DesktopBPP != vid_bpp) {
		image* temp = ImageAdapt(img, vid_bpp, DesktopBPP);
		memcpy((quad*)dx_win_bsd.lpSurface,temp->data,img->width*img->height*DesktopBPP/8);
		delete temp;
	}

	GetClientRect(hwnd, &r);

	int xres = img->width;
	int yres = img->height;

	//disable letterboxing
	if(0)
	{
		ClientToScreen(hwnd, (POINT *)&r);
		ClientToScreen(hwnd, (POINT *)&r + 1);
		dx_win_ps->Blt(&r, dx_win_bs, 0, 0, 0);
		return;
	}

	//-----
	//grump calculations
	int w = r.right - r.left;
	int h = r.bottom - r.top;

	//will contain w/h to render at
	int iw,ih;

	float ratio = (float)w / xres;
	float ph = ratio * (float)yres;

	if((int)ph>h)
	{
		ratio = (float)h / (float)yres;
		ih = h;
		iw = (int)(ratio * (float)xres);
	}
	else
	{
		ih = (int)ph;
		iw = w;
	}

	//create a client rectangle with the render destination properly set, possibly in the middle of the client area
	r.left += (w-iw)/2;
	r.top += (h-ih)/2;

	r.right = r.left + iw;
	r.bottom = r.top + ih;

	ClientToScreen(hwnd, (POINT *)&r);
	ClientToScreen(hwnd, (POINT *)&r + 1);

	//render the screen
	dx_win_ps->SetClipper(clipper);
	dx_win_ps->Blt(&r, dx_win_bs, 0, DDBLT_WAIT, 0);

	//draw black letterbox bars
	if(iw != w || ih != h)
	{

		RECT rClient;
		GetClientRect(hwnd, &rClient);
		ClientToScreen(hwnd, (POINT *)&rClient);
		ClientToScreen(hwnd, (POINT *)&rClient + 1);

		DDBLTFX ddbltfx;
		memset(&ddbltfx, 0, sizeof(DDBLTFX));
		ddbltfx.dwSize = sizeof(DDBLTFX);
		ddbltfx.dwFillColor = 0;

		if(iw < w)
		{
			RECT rBlit;
			rBlit.left = rClient.left;
			rBlit.right = r.left;
			rBlit.top = r.top;
			rBlit.bottom = r.bottom;
			dx_win_ps->Blt(&rBlit, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);

			rBlit.left = r.right;
			rBlit.right = rClient.right;
			dx_win_ps->Blt(&rBlit, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
		}
		else
		{
			RECT rBlit;
			rBlit.left = r.left;
			rBlit.right = r.right;
			rBlit.top = rClient.top;
			rBlit.bottom = r.top;
			dx_win_ps->Blt(&rBlit, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);

			rBlit.top = r.bottom;
			rBlit.bottom = rClient.bottom;
			dx_win_ps->Blt(&rBlit, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
		}
	}
}

void dd_Window::flip_fullscreen()
{
	int i=0;
	HRESULT hr;

	//hack for 320x200 letterboxing
	if(xres == 320 && yres == 200)
	{
		hr = dx_bs->BltFast(0,20,dx_os,NULL,DDBLTFAST_WAIT | DDBLTFAST_NOCOLORKEY);
		DDBLTFX ddbltfx;
		memset(&ddbltfx, 0, sizeof(DDBLTFX));
		ddbltfx.dwSize = sizeof(DDBLTFX);
		ddbltfx.dwFillColor = 0;
		RECT rBlit;
		rBlit.left = 0; rBlit.top = 0; rBlit.right = 320; rBlit.bottom = 20;
		dx_bs->Blt(&rBlit, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
		rBlit.left = 0; rBlit.top = 220; rBlit.right = 320; rBlit.bottom = 240;
		dx_bs->Blt(&rBlit, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
	}
	//regular case
	else
		hr = dx_bs->BltFast(0,0,dx_os,NULL,DDBLTFAST_WAIT | DDBLTFAST_NOCOLORKEY);


	if(hr==DDERR_SURFACELOST)
	{
		dx_bs->Restore();
		dx_os->Restore();
		dx_bs->BltFast(0,0,dx_os,NULL,DDBLTFAST_WAIT | DDBLTFAST_NOCOLORKEY);
	}

	hr=dx_ps->Flip(0,DDFLIP_WAIT | DDFLIP_NOVSYNC);
	//dx_ps->Flip(0,0);

	if(hr==DDERR_SURFACELOST)
	{
		dx_ps->Restore();
		hr=dx_ps->Flip(0,DDFLIP_WAIT | DDFLIP_NOVSYNC);
	}


	hr=dx_os->Lock(0,&dx_osd,DDLOCK_SURFACEMEMORYPTR | DDLOCK_WRITEONLY | DDLOCK_WAIT,0);
	if(hr==DDERR_SURFACELOST)
	{
		dx_os->Restore();
		hr=dx_os->Lock(0,&dx_osd,DDLOCK_SURFACEMEMORYPTR | DDLOCK_WRITEONLY | DDLOCK_WAIT,0);
	}
	dx_os->Unlock(0);

	img->data=(quad*)dx_osd.lpSurface;
	img->pitch=dx_osd.lPitch/vid_bytesperpixel;
}

int dd_Window::set_win(int w, int h, int bpp)
{
//	RECT r, r2;
	HRESULT hr;

	shutdown_win();

	dx_win_bsd.dwWidth = w;
	dx_win_bsd.dwHeight = h;
	hr = dx_dd->CreateSurface(&dx_win_bsd, &dx_win_bs, NULL);
	if (hr != DD_OK)
	{
		return 0;
	}
	hr = dx_win_bs->Lock(0, &dx_win_bsd, 0, 0);
	if (hr != DD_OK)
	{
		DX_RELEASE(dx_win_bs);
		return 0;
	}
	dx_win_bs->Unlock(0);
	hr = dx_dd->CreateClipper(0, &clipper, 0);
	if (hr != DD_OK)
	{
		DX_RELEASE(dx_win_bs);
		return 0;
	}
	hr = clipper->SetHWnd(0, hwnd);
	if (hr != DD_OK)
	{
		DX_RELEASE(dx_win_bs);
		DX_RELEASE(clipper);
		return 0;
	}

	if(bGameWindow)
	{
		int ws = GetWindowLong(hwnd,GWL_STYLE);
		ws &= ~WS_POPUP;
		ws |= WS_OVERLAPPEDWINDOW | WS_THICKFRAME;
		SetWindowLong(hwnd, GWL_STYLE, ws);
	}

	//set pixelformat parameters
	//if(bGameWindow)
	//{
	//	DDPIXELFORMAT ddpf;
	//	ddpf.dwSize = sizeof(ddpf);
	//	ddpf.dwFlags = DDPF_RGB;
	//	hr = dx_win_ps->GetPixelFormat(&ddpf);
	//	if (hr != DD_OK) err("Could not get pixel format!");
	//	//if (ddpf.dwRBitMask == 0x7C00 && bpp == 16)
	//	//	vid_bpp = 15, vid_bytesperpixel = 2;
	//	//else
	//	//	vid_bpp = bpp, vid_bytesperpixel = bpp / 8;
	//}

	delete img;
	img = new image();
	img->width = w;
	img->height = h;
	img->cx1 = 0;
	img->cx2 = w-1;
	img->cy1 = 0;
	img->cy2 = h-1;
	
	if(vid_bpp == DesktopBPP) {
		img->shell = true;
		img->data = (quad*)dx_win_bsd.lpSurface;
		img->pitch = dx_win_bsd.lPitch / vid_bytesperpixel;
	} else {
		img->alloc_data();
	}

	SetHandleImage(imgHandle,img);
	if(bGameWindow)
	{
		SetHandleImage(1,img);
		screen = img;
	}
	

	return 1;
}

//must be the game window
int dd_Window::set_fullscreen(int w, int h, int bpp)
{
	assert(bGameWindow);

	quad ws;
	HRESULT hr;


	DX_RELEASE(dx_os);
	DX_RELEASE(dx_ps);
	DX_RELEASE(dx_win_ps);

	hr = dx_dd->SetCooperativeLevel(hMainWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
	if (hr != DD_OK)
	{
		return 0;
	}

	//hack for 320x200 letterboxing
	if(w == 320 && h == 200)
		dx_dd->SetDisplayMode(320, 240, bpp);
	else
		dx_dd->SetDisplayMode(w, h, bpp);


	hr = dx_dd->CreateSurface(&dx_psd, &dx_ps, NULL);
	if (hr != DD_OK)
	{
		return 0;
	}


	dx_osd.dwWidth=w;
	dx_osd.dwHeight=h;
	hr=dx_dd->CreateSurface(&dx_osd,&dx_os,NULL);
	if(hr!=DD_OK)
	{
		return 0;
	}
	hr = dx_ps->GetAttachedSurface(&dx_bsd.ddsCaps, &dx_bs);
	if (hr != DD_OK)
	{
		DX_RELEASE(dx_os);
		return 0;
	}

	ws = GetWindowLong(hwnd, GWL_STYLE);
	ws &= ~WS_OVERLAPPEDWINDOW;
	ws |= WS_POPUP;
	SetWindowLong(hwnd, GWL_STYLE, ws);
	SetWindowPos(hwnd,0,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_FRAMECHANGED | SWP_NOZORDER);

	//make window take up entire screen
	//choose one or the other
	//SetWindowPos(hwnd,0,0,0,w,h,SWP_NOACTIVATE | SWP_NOZORDER);
	ShowWindow(hwnd,SW_SHOWMAXIMIZED);

	//set pixelformat parameters
	{
		DDPIXELFORMAT ddpf;
		ddpf.dwSize = sizeof(ddpf);
		ddpf.dwFlags = DDPF_RGB;
		hr = dx_ps->GetPixelFormat(&ddpf);
		if (hr != DD_OK) err("Could not get pixel format!");
		//if (ddpf.dwRBitMask == 0x7C00 && bpp == 16)
		//	vid_bpp = 15, vid_bytesperpixel = 2;
		//else
		//	vid_bpp = bpp, vid_bytesperpixel = bpp / 8;
		vid_bpp = 32;
	}


	if(img) delete img;
	img = new image();
	img->shell = true;

	SetHandleImage(1,img);
	screen = img;

//	img->alphamap = 0;
	img->width = w;
	img->height = h;
	img->cx1 = 0;
	img->cx2 = w-1;
	img->cy1 = 0;
	img->cy2 = h-1;
	img->data=0;

	flip_fullscreen();

	return 1;
}

dd_Window::dd_Window(bool bGameWindow) : AuxWindow()
{
	img = 0;
	dx_bs = 0;
	dx_os = 0;
	dx_win_bs = 0;
	bVisible = false;
    handle = dd_handlePop();

	dd_windows.push_back(this);

	//get an image handle to use from now on. we will fill in the imagebank slot for that handle
	//when we have an image to put there
	if(bGameWindow)
		imgHandle = 1;
	else
		imgHandle = HandleForImage(0);

	hwnd = 0;

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

	/* Make suface descriptions */

	memset(&dx_psd, 0, sizeof(DDSURFACEDESC));
	dx_psd.dwSize = sizeof(DDSURFACEDESC);
	dx_psd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	dx_psd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
	dx_psd.dwBackBufferCount = 1;

	memset(&dx_bsd, 0, sizeof(DDSURFACEDESC));
	dx_bsd.dwSize = sizeof(DDSURFACEDESC);
	dx_bsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;// | DDSCAPS_FLIP | DDSCAPS_COMPLEX;

	memset(&dx_osd,0,sizeof DDSURFACEDESC);
	dx_osd.dwSize=sizeof DDSURFACEDESC;
	dx_osd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	dx_osd.ddsCaps.dwCaps=DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;

	memset(&dx_win_psd, 0, sizeof(DDSURFACEDESC));
	dx_win_psd.dwSize = sizeof(DDSURFACEDESC);
	dx_win_psd.dwFlags = DDSD_CAPS;
	dx_win_psd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	memset(&dx_win_bsd, 0, sizeof(DDSURFACEDESC));
	dx_win_bsd.dwSize = sizeof(DDSURFACEDESC);
	dx_win_bsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	dx_win_bsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;

	//always create the window now
	createWindow();
}


void dd_Window::adjust(int w, int h, RECT *r)
{
	int ws = GetWindowLong(hwnd,GWL_STYLE);
	r->left = r->top = 0;
	r->right = w;
	r->bottom = h;
	AdjustWindowRectEx(r, GetWindowLong(hwnd,GWL_STYLE), GetMenu(hwnd) != 0, GetWindowLong(hwnd, GWL_EXSTYLE));
}
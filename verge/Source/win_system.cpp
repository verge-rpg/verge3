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
	win_system.cpp
 ****************************************************************/

#include "xerxes.h"
#include <io.h>
/***************************** data *****************************/

HWND hMainWnd;
HINSTANCE hMainInst;
std::vector<HWND> win_activeWindows;

/****************************************************************/

bool AppIsForeground;
int DesktopBPP;

/***************************** code *****************************/

LRESULT APIENTRY WndProc(HWND hWnd, UINT message,WPARAM wParam, LPARAM lParam);
void HandleMessages();

void LoadConfig();
void dd_init();
int APIENTRY WinMain(HINSTANCE hCurrentInst, HINSTANCE zwhocares, LPSTR szCommandline, int nCmdShow)
{
	hMainInst = hCurrentInst;
	DesktopBPP = GetDeviceCaps(GetDC(NULL), BITSPIXEL);
	//dd_init();
	setWindowTitle(APPNAME);

	srand(timeGetTime());
	log_Init(true);


	xmain(__argc,__argv);
	err("");
	return 0;
}

void setWindowTitle(char *str) {
	SetWindowText(hMainWnd, str);
}

char *clipboard_getText()
{
	static char buf[4096];
	if(!IsClipboardFormatAvailable(CF_TEXT))
		return 0;

	OpenClipboard(0);
	HANDLE h = GetClipboardData(CF_TEXT);
	char *cp = (char *)GlobalLock(h);
	strncpy(buf, cp, 4096);
	GlobalUnlock(h);
	CloseClipboard();
	return buf;
}

void clipboard_setText(char *text)
{
	HANDLE h = GlobalAlloc(GMEM_MOVEABLE,strlen(text)+1);
	char *cp = (char *)GlobalLock(h);
	strcpy(cp,text);
	GlobalUnlock(h);
	OpenClipboard(0);
	EmptyClipboard();
	SetClipboardData(CF_TEXT,h);
	CloseClipboard();
}

image *clipboard_getImage()
{
	//make sure we can get a DIB, the only clipboard format we understand
	if (!IsClipboardFormatAvailable(CF_DIB))
		return 0;

	if (!OpenClipboard(0))
		return 0;

	HGLOBAL mem;
	BITMAPINFOHEADER *bih;

	mem = (HGLOBAL) GetClipboardData(CF_DIB);
	if (!mem)
	{
		CloseClipboard();
		return 0;
	}

	bih = (BITMAPINFOHEADER *) GlobalLock(mem);
	if (!bih)
	{
		CloseClipboard();
		return 0;
	}

	//apparently this has to be here even though windows says it doesnt need it
	bih->biSizeImage = (((bih->biWidth * bih->biBitCount + 31)&~31)>>3)*bih->biHeight;

	BITMAPINFOHEADER newbih;
	newbih.biSize=sizeof BITMAPINFOHEADER;
	newbih.biWidth=bih->biWidth;
	newbih.biHeight=-bih->biHeight;
	newbih.biPlanes=1;
	newbih.biBitCount=32;
	newbih.biCompression=BI_RGB;
	newbih.biSizeImage=newbih.biXPelsPerMeter=newbih.biYPelsPerMeter=newbih.biClrUsed=newbih.biClrImportant=0;

	VOID *vp;
	HDC tempdc=CreateCompatibleDC(0);
	HBITMAP hbmpnew=CreateDIBSection(0,(BITMAPINFO *)bih,DIB_RGB_COLORS,&vp,0,0);

	memcpy(vp,(void *)((char *)bih+bih->biSize+bih->biClrUsed*4),bih->biSizeImage);
	byte *tempData = new byte[bih->biWidth*bih->biHeight*4];
	GetDIBits(tempdc,hbmpnew,0,bih->biHeight,tempData,(BITMAPINFO *)&newbih,DIB_RGB_COLORS);
	image *img = ImageFrom32bpp(tempData,bih->biWidth,bih->biHeight);
	delete[] tempData;

	DeleteObject(hbmpnew);
	DeleteDC(tempdc);

	GlobalUnlock(mem);
	CloseClipboard();

	return img;
}

void clipboard_putImage(image *img)
{
	HGLOBAL mem;
	BITMAPINFOHEADER bih;
	HBITMAP bmp;
	HDC tempdc;
	void *memptr;
	void *vp;

	if (!OpenClipboard(0))
		return;
	EmptyClipboard();

	//set up a dibsection bitmap to hold our 32bpp bitmap
	bih.biSize=sizeof BITMAPINFOHEADER;
	bih.biWidth=img->width;
	bih.biHeight=-img->height;
	bih.biPlanes=1;
	bih.biBitCount=img->bpp;
	bih.biCompression=BI_RGB;
	bih.biSizeImage=bih.biXPelsPerMeter=bih.biYPelsPerMeter=bih.biClrUsed=bih.biClrImportant=0;
	bmp=CreateDIBSection(0,(BITMAPINFO *)&bih,DIB_RGB_COLORS,&vp,0,0);
	memcpy(vp,img->data,img->width*img->height*vid_bytesperpixel);

	//allocate our memory object
	mem=GlobalAlloc(GMEM_MOVEABLE,sizeof (BITMAPINFOHEADER)+img->width*img->height*3);
	memptr=GlobalLock(mem);

	//convert the dibsection to a 24bpp dib
	bih.biHeight=img->height;
	bih.biBitCount=24;
	tempdc=CreateCompatibleDC(0);
	GetDIBits(tempdc,bmp,0,img->height,(char *)memptr+sizeof (BITMAPINFOHEADER),(BITMAPINFO *)&bih,DIB_RGB_COLORS);
	DeleteDC(tempdc);

	//copy in the dib header
	memcpy(memptr,&bih,sizeof BITMAPINFOHEADER);

	GlobalUnlock(mem);

	SetClipboardData(CF_DIB,mem);
	CloseClipboard();
}

void HandleMessages(void)
{

	for(std::vector<HWND>::iterator it = win_activeWindows.begin(); it != win_activeWindows.end(); it++)
	{
		HWND hwnd = *it;
		MSG msg;
		while (PeekMessage(&msg, hwnd, (int) NULL, (int) NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

void win_addWindow(HWND window)
{
	win_activeWindows.push_back(window);
}

void win_removeWindow(HWND window)
{
	for(std::vector<HWND>::iterator it = win_activeWindows.begin(); it != win_activeWindows.end(); it++)
		if(*it == window)
		{
			win_activeWindows.erase(it);
			break;
		}

}

LRESULT APIENTRY win_auxWindowProc(HWND hWnd, UINT message,WPARAM wParam, LPARAM lParam)
{
	AuxWindow *auxwin = (AuxWindow *)GetWindowLong(hWnd,GWL_USERDATA);
	switch (message)
	{
		case WM_MOUSEMOVE:
			{
				if(!auxwin->bMouseInside)
				{
					auxwin->bMouseInside = true;
					ShowCursor(0);
					TRACKMOUSEEVENT tme = { sizeof(tme) };
					tme.dwFlags = TME_LEAVE;
					tme.hwndTrack = hWnd;
					TrackMouseEvent(&tme);
				}
			}

			break;
		case WM_MOUSELEAVE:
			{
				auxwin->bMouseInside = false;
				ShowCursor(1);
			}
			break;
		case WM_SIZING:
			{
				if(!vid_window)
					return false;

				const int snap = 12;
				RECT *dr = (RECT*)lParam;
				RECT r;
				RECT wndr;
				GetClientRect(hWnd,&r);
				GetWindowRect(hWnd,&wndr);
				int framew = (wndr.right-wndr.left)-(r.right-r.left);
				int frameh = (wndr.bottom-wndr.top)-(r.bottom-r.top);

				int xres = auxwin->getImage()->width;
				int yres = auxwin->getImage()->height;

				int w = dr->right-dr->left;
				int h = dr->bottom-dr->top;
				w += snap;
				h += snap;
				w -= framew;
				h -= frameh;
				int wf = w/xres;
				int hf = h/yres;
				if(wf==0 || hf == 0)
					return 0;

				w -= snap;
				h -= snap;
				if(abs(w-xres*wf)<=snap)
				{
					if(wParam==WMSZ_LEFT || wParam==WMSZ_TOPLEFT || wParam==WMSZ_BOTTOMLEFT)
						dr->left = dr->right - wf*xres - framew;
					else
						dr->right = dr->left + wf*xres + framew;
				}
				if(abs(h-yres*wf)<=snap)
				{
					if(wParam==WMSZ_TOP || wParam==WMSZ_TOPLEFT || wParam==WMSZ_TOPRIGHT)
						dr->top = dr->bottom - hf*yres - frameh;
					else
						dr->bottom = dr->top + hf*yres + frameh;
				}

				return true;
			}
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT APIENTRY win_gameWindowProc(HWND hWnd, UINT message,WPARAM wParam, LPARAM lParam)
{
	AuxWindow *auxwin = (AuxWindow *)GetWindowLong(hWnd,GWL_USERDATA);
	switch (message)
	{

		case WM_SIZING:
			{
				if(!vid_window)
					return false;

				const int snap = 12;
				RECT *dr = (RECT*)lParam;
				RECT r;
				RECT wndr;
				GetClientRect(hWnd,&r);
				GetWindowRect(hWnd,&wndr);
				int framew = (wndr.right-wndr.left)-(r.right-r.left);
				int frameh = (wndr.bottom-wndr.top)-(r.bottom-r.top);

				int xres = auxwin->getImage()->width;
				int yres = auxwin->getImage()->height;

				int w = dr->right-dr->left;
				int h = dr->bottom-dr->top;
				w += snap;
				h += snap;
				w -= framew;
				h -= frameh;
				int wf = w/xres;
				int hf = h/yres;
				if(wf==0 || hf == 0)
					return 0;

				w -= snap;
				h -= snap;
				if(abs(w-xres*wf)<=snap)
				{
					if(wParam==WMSZ_LEFT || wParam==WMSZ_TOPLEFT || wParam==WMSZ_BOTTOMLEFT)
						dr->left = dr->right - wf*xres - framew;
					else
						dr->right = dr->left + wf*xres + framew;
				}
				if(abs(h-yres*wf)<=snap)
				{
					if(wParam==WMSZ_TOP || wParam==WMSZ_TOPLEFT || wParam==WMSZ_TOPRIGHT)
						dr->top = dr->bottom - hf*yres - frameh;
					else
						dr->bottom = dr->top + hf*yres + frameh;
				}

				return true;
			}

		case WM_CLOSE:
			err("");
			break;
		case WM_ACTIVATE:
			if (LOWORD(wParam) == WA_INACTIVE)
				AppIsForeground = false;
			else
				AppIsForeground = true;
			break;
		case WM_ACTIVATEAPP:
			if (!(BOOL)wParam)
				AppIsForeground = false;
			else
				AppIsForeground = true;
			return 0;
		case WM_SYSCOMMAND:
			if (wParam == SC_CLOSE)
				err("");
			break;
		case WM_SYSKEYDOWN:
			if (wParam == 'X')
				err("");
			if(wParam==VK_RETURN&&!(lParam&0x40000000))
			{
				if(!vid_SetMode(vid_xres,vid_yres,vid_bpp,!vid_window,MODE_SOFTWARE))
					err("could not execute alt-enter");
				return 0;
			}
			return 0;

		case WM_KEYDOWN: return 0;
		case WM_KEYUP: return 0;
		case WM_MOUSEMOVE:
			{
				if(!auxwin->bMouseInside)
				{
					auxwin->bMouseInside = true;
					ShowCursor(0);
					TRACKMOUSEEVENT tme = { sizeof(tme) };
					tme.dwFlags = TME_LEAVE;
					tme.hwndTrack = hWnd;
					TrackMouseEvent(&tme);
				}
			}

			break;
		case WM_MOUSELEAVE:
			{
				auxwin->bMouseInside = false;
				ShowCursor(1);
			}
			break;
		case WM_LBUTTONDOWN: mouse_l = true; break;
		case WM_LBUTTONUP:   mouse_l = false; break;
		case WM_RBUTTONDOWN: mouse_r = true; break;
		case WM_RBUTTONUP:   mouse_r = false; break;
		case WM_MBUTTONDOWN: mouse_m = true; break;
		case WM_MBUTTONUP:   mouse_m = false; break;
		case WM_MOUSEWHEEL:
			mwheel += (short) HIWORD(wParam);
			break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}


void err(char *str, ...)
{
	va_list argptr;
	char msg[256];

	va_start(argptr, str);
	vsprintf(msg, str, argptr);
	va_end(argptr);

	win_movie_shutdown();
	snd_Shutdown();


	if(vid_Close)
 	{
 		vid_Close();
 		DestroyWindow(hMainWnd);
 	}

	if (strlen(msg))
	{
		MessageBox(GetDesktopWindow(), msg, APPNAME, MB_OK | MB_TASKMODAL);
		log("Exiting: %s", msg);
	}
	delete systimer;
	PostQuitMessage(0);
	exit(strlen(msg)==0?0:-1);
}

void initConsole()
{
	AllocConsole();
}

void writeToConsole(char *str)
{
	DWORD crap;
	WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), str, strlen(str), &crap, 0);
}

int getYear()
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	return time.wYear;
}

int getMonth()
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	return time.wMonth;
}

int getDay()
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	return time.wDay;
}

int getDayOfWeek()
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	return time.wDayOfWeek;
}

int getHour()
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	return time.wHour;
}

int getMinute()
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	return time.wMinute;
}

int getSecond()
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	return time.wSecond	;
}

std::vector<string> listFilePattern(string pattern)
{
	std::vector<string> res;
	_finddata_t rec;
	int handle = _findfirst(pattern.c_str(), &rec);
	int result = handle;
	while (result != -1)
	{
		res.push_back(rec.name);
		result = _findnext(handle, &rec);
	}
	_findclose(handle);
	return res;
}


void showMessageBox(string message)
{
	MessageBox(GetDesktopWindow(), message.c_str(), APPNAME, MB_OK | MB_TASKMODAL);
}

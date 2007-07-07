/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


// A header! :O
#include "xerxes.h"


#define BTN_EVAL		100
#define BTN_RELOADVC	101
#define BTN_RELOADMAP	102
#define EDIT_EVAL		200
#define LIST_VC			300

HWND hButtonEval, hEditEval;
HWND hButtonReloadVC, hButtonReloadMap, hListboxVC;


LRESULT APIENTRY editcode_WindowProc(HWND hWnd, UINT message,WPARAM wParam, LPARAM lParam)
{
	HLOCAL linein;
	LRESULT index;
	char buf[BUFSIZ];

	switch (message)
	{
	case WM_COMMAND:
		if(HIWORD(wParam) == BN_CLICKED) {
			switch(LOWORD(wParam)) {
			case BTN_EVAL:
				linein = (HLOCAL)SendMessage(hEditEval,EM_GETHANDLE,0,0);
				runEval((char *)LocalLock(linein));
				LocalUnlock(linein);
				return 0;
			case BTN_RELOADVC:
				index = SendMessage(hListboxVC,LB_GETCURSEL,0,0);
				if(index == LB_ERR) {
					MessageBox(hWnd,"Please choose a VC file from the list to the right.","Error",MB_OK);
					return 0;
				}
				SendMessage(hListboxVC,LB_GETTEXT,index,(LPARAM)buf);
				runReload(buf);
				return 0;
			case BTN_RELOADMAP:
				reloadMap();
				return 0;
			}
		}
		break;
	default:
		break;
	}


	return DefWindowProc(hWnd, message, wParam, lParam);
}

void AddSourceFile(std::string fname)
{
	if(SendMessage(hListboxVC,LB_FINDSTRINGEXACT,-1,(LPARAM)fname.c_str()) == LB_ERR)
		SendMessage(hListboxVC,LB_ADDSTRING,0,(LPARAM)fname.c_str());
}

void InitEditCode()
{
	WNDCLASS WndClass;

	memset(&WndClass, 0, sizeof(WNDCLASS));
	WndClass.hCursor = LoadCursor(0, IDC_ARROW);
	WndClass.lpszClassName = "verge3-editcodewindow";
	WndClass.hInstance = hMainInst;
	WndClass.lpfnWndProc = editcode_WindowProc;
	WndClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
	RegisterClass(&WndClass);

	HWND hWnd = CreateWindowEx(
		0,"verge3-editcodewindow","Edit Code",WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,CW_USEDEFAULT,550,350,
		(HWND) NULL,(HMENU) NULL,hMainInst,NULL
	);
	ShowWindow(hWnd,SW_SHOWMINNOACTIVE);
	win_addWindow(hWnd);

	HWND hTemp;

	HFONT fArial = CreateFont(10,10,0,0,FW_NORMAL,0,0,0,0,0,0,0,0,"Arial");
	SelectObject(GetDC(hWnd),fArial);

	hTemp = CreateWindowEx(0,"Static","Source Reload",WS_CHILD|WS_VISIBLE,5,5,431,20,hWnd,(HMENU) NULL,hMainInst,NULL);
	hTemp = CreateWindowEx(0,"Static","",WS_CHILD|WS_VISIBLE|SS_SUNKEN,5,25,531,150,hWnd,(HMENU) NULL,hMainInst,NULL);
	hTemp = CreateWindowEx(0,"Static",
		"Only new or changed functions are reloaded. Global variables, #defines and struct definitions are ignored. Reloading maps causes them to restart."
		,WS_CHILD|WS_VISIBLE,10,28,190,100,hWnd,(HMENU) NULL,hMainInst,NULL);
	hTemp = CreateWindowEx(0,"Static","Run Code",WS_CHILD|WS_VISIBLE,5,178,431,20,hWnd,(HMENU) NULL,hMainInst,NULL);
	hTemp = CreateWindowEx(0,"Static","",WS_CHILD|WS_VISIBLE|SS_SUNKEN,5,198,531,115,hWnd,(HMENU) NULL,hMainInst,NULL);
	hTemp = CreateWindowEx(0,"Static",
		"Code is evaluated as if it were a part of this map."
		,WS_CHILD|WS_VISIBLE,10,201,190,80,hWnd,(HMENU) NULL,hMainInst,NULL);
	SelectObject(GetDC(hTemp),fArial);



	hButtonEval =
		CreateWindowEx(0,"Button","Eval",WS_CHILD|WS_TABSTOP|WS_VISIBLE,110,283,90,25,hWnd,(HMENU) NULL,hMainInst,NULL);
	SetWindowLong(hButtonEval,GWL_ID,BTN_EVAL);
	hButtonReloadVC =
		CreateWindowEx(0,"Button","Reload VC", WS_CHILD|WS_TABSTOP|WS_VISIBLE,110,143,90,25,hWnd,(HMENU) NULL,hMainInst,NULL);
	SetWindowLong(hButtonReloadVC,GWL_ID,BTN_RELOADVC);
	hButtonReloadMap =
		CreateWindowEx(0,"Button","Reload Map", WS_CHILD|WS_TABSTOP|WS_VISIBLE,10,143,90,25,hWnd,(HMENU) NULL,hMainInst,NULL);
	SetWindowLong(hButtonReloadMap,GWL_ID,BTN_RELOADMAP);

	hEditEval =
		CreateWindowEx(0,"Edit",NULL,WS_CHILD|WS_TABSTOP|WS_VISIBLE|ES_AUTOVSCROLL|ES_MULTILINE|ES_LEFT|WS_VSCROLL|WS_HSCROLL,210,200,323,109,hWnd,(HMENU) NULL,hMainInst,NULL);
	SetWindowLong(hEditEval,GWL_ID,EDIT_EVAL);

	hListboxVC =
		CreateWindowEx(0,"ListBox",NULL,WS_CHILD|WS_TABSTOP|WS_VISIBLE|LBS_HASSTRINGS|WS_VSCROLL|WS_HSCROLL,210,28,323,148,hWnd,(HMENU) NULL,hMainInst,NULL);
	SetWindowLong(hListboxVC,GWL_ID,LIST_VC);
	SendMessage(hListboxVC,LB_ADDSTRING,0,(LPARAM)"system.vc");
	SendMessage(hListboxVC,LB_ADDSTRING,0,(LPARAM)"(Current Map VC)");

}
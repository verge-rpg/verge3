
#include <windows.h>
int APIENTRY _WinMain(HINSTANCE hCurrentInst, HINSTANCE zwhocares, LPSTR szCommandline, int nCmdShow);

int APIENTRY WinMain(HINSTANCE hCurrentInst, HINSTANCE zwhocares, LPSTR szCommandline, int nCmdShow)
{
    return _WinMain(hCurrentInst, zwhocares, szCommandline, nCmdShow);
}
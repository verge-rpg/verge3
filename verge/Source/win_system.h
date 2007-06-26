/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


#ifndef SYSTEM_H
#define SYSTEM_H

#include <vector>

LRESULT APIENTRY win_gameWindowProc(HWND hWnd, UINT message,WPARAM wParam, LPARAM lParam);
LRESULT APIENTRY win_auxWindowProc(HWND hWnd, UINT message,WPARAM wParam, LPARAM lParam);

void win_addWindow(HWND window);
void win_removeWindow(HWND window);

void setWindowTitle(char *str);

char *clipboard_getText();
void clipboard_setText(char *text);
image *clipboard_getImage();
void clipboard_putImage(image *img);

void initConsole();
void writeToConsole(char *text);

extern HWND hMainWnd;
extern HINSTANCE hMainInst;
extern bool AppIsForeground;
extern int DesktopBPP;

int getYear();
int getMonth();
int getDay();
int getDayOfWeek();
int getHour();
int getMinute();
int getSecond();

std::vector<string> listFilePattern(string pattern);


void showMessageBox(string message);

#endif
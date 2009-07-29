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

//void win_addWindow(HWND window);
//void win_removeWindow(HWND window);

void setWindowTitle(const char *str);

char *clipboard_getText();
void clipboard_setText(const char *text);
image *clipboard_getImage();
void clipboard_putImage(image *img);

void writeToConsole(char *str);
void initConsole();

extern bool AppIsForeground;
extern int DesktopBPP;

int getYear();
int getMonth();
int getDay();
int getDayOfWeek();
int getHour();
int getMinute();
int getSecond();

void strupr(char *c);
void strlwr(char *c);

void listFilePattern(std::vector<std::string> &res, CStringRef pattern);

void showMessageBox(CStringRef message);

unsigned int timeGetTime();

void Sleep(unsigned int msec);

void InitEditCode(); // impl in mac_cocoa_util.mm
void AddSourceFile(std::string s); // impl in mac_cocoa_util.mm

StringRef GetSystemSaveDirectory(CStringRef name); // impl in mac_cocoa_util.mm 
void ChangeToRootDirectory(); // impl in mac_cocoa_util.mm

#endif

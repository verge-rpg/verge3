#include "xerxes.h"

extern bool AppIsForeground;
extern int DesktopBPP;

void initConsole();
void writeToConsole(char *text);
void InitEditCode();
void AddSourceFile(string s);
std::vector<string> listFilePattern(string pattern);

unsigned int timeGetTime();
void Sleep(unsigned int msec);

int getYear();
int getMonth();
int getDay();
int getDayOfWeek();
int getHour();
int getMinute();
int getSecond();
void setWindowTitle(char *str);

char *clipboard_getText();
void clipboard_setText(char *text);
image *clipboard_getImage();
void clipboard_putImage(image *img);
void showMessageBox(string message);

int psp_SetMode(int xres, int yres, int bpp, bool windowflag);

//improve me or map me to psp function!
#define _ASSERTE(x)

#ifndef COMMON_H
#define COMMON_H

void log_Init(bool on);
void log(char *text, ...);
char *va(char* format, ...);
int strcasecmp(char *s1, char *s2);
bool isletter(char c);
void strclean(char *s);
int sgn(int a);
int rnd(int lo, int hi);

#endif
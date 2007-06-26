#ifndef MOUSE_H
#define MOUSE_H

extern int mouse_x, mouse_y, mouse_l, mouse_r, mouse_m;
extern float mwheel;

void mouse_set(int x, int y);

void mouse_Init();
void mouse_Update();

#endif

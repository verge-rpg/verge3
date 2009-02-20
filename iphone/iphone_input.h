#ifndef _INPUT_H
#define _INPUT_H

struct stick
{
	bool active;
	int analog_x, analog_y;
	bool up, down, left, right;
	quad button[32];

	int range_left, range_right, range_up, range_down;
	int xmin, ymin, xmax, ymax, xrange, yrange;
};
extern stick sticks[4];

int joy_NumSticks();
int joy_Init();
int joy_Init(int j1, int j2, int j3, int j4);
void joy_Close();
void joy_Update();

extern int mouse_x, mouse_y, mouse_l, mouse_r, mouse_m;
extern float mwheel;

void mouse_set(int x, int y);

void mouse_Init();
void mouse_Update();

#endif

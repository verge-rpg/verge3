#ifndef JOYSTICK_H
#define JOYSTICK_H

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

#endif

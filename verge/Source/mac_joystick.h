/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


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
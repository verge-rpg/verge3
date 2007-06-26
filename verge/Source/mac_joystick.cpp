/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


/****************************************************************
	xerxes engine
	mac_joystick.cpp
 ****************************************************************/

#include "xerxes.h"

/***************************** data *****************************/

#define MAX_DEVS 16
#define DEADZONE 30
#define NOJOY 0

bool joy_initd = false;
SDL_Joystick * joy_sticks[4];
int joy_numsticks;

stick sticks[4];

/***************************** code *****************************/

int joy_NumSticks()
{
	joy_numsticks = SDL_NumJoysticks();
	return joy_numsticks;
}


int joy_Init()
{
	SDL_JoystickEventState(SDL_ENABLE);

	joy_NumSticks(); /* Get number of attached joysticks */
	if (!joy_numsticks) return 0;  /* No joysticks */

	int numsticks = 0;

	/* Initialize all joysticks */

	for (int i=0; i<4; i++)
	{
		if(i < joy_numsticks)
		{
			joy_sticks[i] = SDL_JoystickOpen(i);
		}
		else
		{
			joy_sticks[i] = NOJOY;
		}

		if (joy_sticks[i] == NOJOY)
		{
			memset(&sticks[i], 0, sizeof (stick));
			continue;
		}
		sticks[i].active = true;
		numsticks++;
		sticks[i].xmin = -32768; // SDL-defined mins and maxes
		sticks[i].xmax = 32767;
		sticks[i].ymin = -32768;
		sticks[i].ymax = 32767;
		sticks[i].xrange = sticks[i].xmax - sticks[i].xmin;
		sticks[i].yrange = sticks[i].ymax - sticks[i].ymin;
		sticks[i].range_left = (sticks[i].xrange * DEADZONE / 100) + sticks[i].xmin;
		sticks[i].range_right = (sticks[i].xrange * (100 - DEADZONE) / 100) + sticks[i].xmin;
		sticks[i].range_up = (sticks[i].yrange * DEADZONE / 100) + sticks[i].ymin;
		sticks[i].range_down = (sticks[i].yrange * (100 - DEADZONE) / 100) + sticks[i].ymin;

		sticks[i].analog_x = 0;
		sticks[i].analog_y = 0;
		sticks[i].up = false;
		sticks[i].down = false;
		sticks[i].left = false;
		sticks[i].right = false;
		memset(sticks[i].button, 0, sizeof (sticks[i].button));
		//log("x1 %d x2 %d y1 %d y2 %d xr %d yr %d", sticks[i].xmin, sticks[i].xmax, sticks[i].ymin, sticks[i].ymax, sticks[i].xrange, sticks[i].yrange);
		//log("left %d right %d up %d down %d", sticks[i].range_left, sticks[i].range_right, sticks[i].range_up, sticks[i].range_down);
	}

	if (numsticks) joy_initd = true;
	return numsticks;
}


void joy_Close()
{
	/* You don't need to call this unless you want to turn off joystick
	   input after it's been initialized to save cycles. */

	joy_initd = false;
	for(int i = 0; i < 4; i++)
	{
		if(joy_sticks[i] != NOJOY)
			SDL_JoystickClose(joy_sticks[i]);
		joy_sticks[i] = NOJOY;
	}
	SDL_JoystickEventState(SDL_IGNORE);
	joy_numsticks = 0;
}


void joy_Update()
{
	if (!joy_initd) return;
	for (int i=0; i<4; i++)
	{
		Sint16 xpos, ypos;
		xpos = SDL_JoystickGetAxis(joy_sticks[i], 0);
		ypos = SDL_JoystickGetAxis(joy_sticks[i], 1);

		if (!sticks[i].active) continue;

		sticks[i].left  = (xpos < sticks[i].range_left);
		sticks[i].right = (xpos > sticks[i].range_right);
		sticks[i].up    = (ypos < sticks[i].range_up);
		sticks[i].down  = (ypos > sticks[i].range_down);

		// convert to (-1000,1000) range for verge
		sticks[i].analog_x = (xpos * 2000 / sticks[i].xrange) - 1000;
		sticks[i].analog_y = (ypos * 2000 / sticks[i].yrange) - 1000;

		for (int b=0; b<32; b++)
			sticks[i].button[b] = SDL_JoystickGetButton(joy_sticks[i], b);
	}
}

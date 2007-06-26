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
	win_joystick.cpp
 ****************************************************************/

#include "xerxes.h"

/***************************** data *****************************/

#define MAX_DEVS 16
#define DEADZONE 30
#define NOJOY -1

bool joy_initd = false;
bool joy_present[MAX_DEVS];
int joy_sticks[4];
int joy_numsticks;

stick sticks[4];

quad buttonmask[32] =
{
	JOY_BUTTON1,  JOY_BUTTON2,  JOY_BUTTON3,  JOY_BUTTON4,  JOY_BUTTON5,  JOY_BUTTON6,
	JOY_BUTTON7,  JOY_BUTTON8,  JOY_BUTTON9,  JOY_BUTTON10, JOY_BUTTON11, JOY_BUTTON12,
	JOY_BUTTON13, JOY_BUTTON14, JOY_BUTTON15, JOY_BUTTON16, JOY_BUTTON17, JOY_BUTTON18,
	JOY_BUTTON19, JOY_BUTTON20, JOY_BUTTON21, JOY_BUTTON22, JOY_BUTTON23, JOY_BUTTON24,
	JOY_BUTTON25, JOY_BUTTON26, JOY_BUTTON27, JOY_BUTTON28, JOY_BUTTON29, JOY_BUTTON30,
	JOY_BUTTON31, JOY_BUTTON32
};

/***************************** code *****************************/

int joy_NumSticks()
{
	JOYINFO joyinfo;
	int NumDevs = joyGetNumDevs();
	if (NumDevs > MAX_DEVS) NumDevs = MAX_DEVS;
	joy_numsticks = 0;
	memset(joy_present, 0, sizeof joy_present);

	/* joyGetNumDevs() simply returns the number of joystick slots, not actual
	   number of devices attached. On my win2k system, it always returns 16. So we
	   have to poll each joystick slot to see if a device is actually attached. */

	for (int i = 0; i < NumDevs; i++)
	{
		joy_present[i] = (joyGetPos(i, &joyinfo) == JOYERR_NOERROR);
		if (joy_present[i]) joy_numsticks++;
	}
	return joy_numsticks;
}


int joy_Init()
{
	int j1, j2;

	joy_NumSticks(); /* Get number of attached joysticks */
	if (!joy_numsticks) return 0;  /* No joysticks */

	j1 = 0; j2 = NOJOY;
	while (j1 < MAX_DEVS) /* Grab first active joystick for J1 */
	{
		if (joy_present[j1]) break;
		j1++;
	}
	if (joy_numsticks > 1) /* If 2 or more sticks, grab next one for J2 */
	{
		j2 = j1 + 1;
		while (j2 < MAX_DEVS)
		{
			if (joy_present[j2]) break;
			j2++;
		}
	}
	return joy_Init(j1, j2, NOJOY, NOJOY); /* Initialize calibrated stuffs */
}


int joy_Init(int j1, int j2, int j3, int j4)
{
	JOYINFO joyinfo;
	JOYCAPS jc;
	int numsticks = 0;

	/* Verify all specified joysticks */

	joy_sticks[0] = (joyGetPos(j1, &joyinfo) == JOYERR_NOERROR) ? j1 : NOJOY;
	joy_sticks[1] = (joyGetPos(j2, &joyinfo) == JOYERR_NOERROR) ? j2 : NOJOY;
	joy_sticks[2] = (joyGetPos(j3, &joyinfo) == JOYERR_NOERROR) ? j3 : NOJOY;
	joy_sticks[3] = (joyGetPos(j4, &joyinfo) == JOYERR_NOERROR) ? j4 : NOJOY;

	/* Initialize all joysticks */

	for (int i=0; i<4; i++)
	{
		if (joy_sticks[i] == NOJOY)
		{
			memset(&sticks[i], 0, sizeof stick);
			continue;
		}
		sticks[i].active = true;
		numsticks++;
		joyGetDevCaps(joy_sticks[i], &jc, sizeof JOYCAPS);
		sticks[i].xmin = jc.wXmin;
		sticks[i].xmax = jc.wXmax;
		sticks[i].ymin = jc.wYmin;
		sticks[i].ymax = jc.wYmax;
		sticks[i].xrange = jc.wXmax - jc.wXmin;
		sticks[i].yrange = jc.wYmax - jc.wYmin;
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
		memset(sticks[i].button, 0, sizeof sticks[i].button);
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
	memset(joy_present, 0, sizeof joy_present);
	joy_numsticks = 0;
}


void joy_Update()
{
	JOYINFO ji;
	int result;

	if (!joy_initd) return;
	for (int i=0; i<4; i++)
	{
		if (!sticks[i].active) continue;
		if (result = joyGetPos(joy_sticks[i], &ji) != JOYERR_NOERROR)
			return; // some error,

		sticks[i].left  = (ji.wXpos < sticks[i].range_left);
		sticks[i].right = (ji.wXpos > sticks[i].range_right);
		sticks[i].up    = (ji.wYpos < sticks[i].range_up);
		sticks[i].down  = (ji.wYpos > sticks[i].range_down);

		sticks[i].analog_x = (ji.wXpos * 2000 / sticks[i].xrange) - 1000;
		sticks[i].analog_y = (ji.wYpos * 2000 / sticks[i].yrange) - 1000;

		for (int b=0; b<32; b++)
			sticks[i].button[b] = (ji.wButtons & buttonmask[b]);
	}
}

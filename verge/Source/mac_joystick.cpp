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

static constexpr int DEADZONE = 30;

bool joy_initd = false;
SDL_Joystick* sdl_joysticks[4];

stick sticks[4];

/***************************** code *****************************/

int joy_Init()
{
	SDL_JoystickUpdate();

	int joysticks_detected = SDL_NumJoysticks();

	log("joy_Init: Detected %d devices from SDL", joysticks_detected);
	
	int joysticks_opened = 0;

	// Initialize all joysticks
	for (int i = 0; i < 4; i++)
	{
		if (i < joysticks_detected)
		{
			joy_Add(i);
		}

		if (sdl_joysticks[i] != nullptr)
		{
			joysticks_opened++;
		}
	}

	log("joy_Init: Opened %d / %d joysticks (joystick system %s)", joysticks_opened, joysticks_detected, joysticks_opened > 0 ? "initialized" : "disabled");

	joy_initd = true;
	return true;
}

void joy_Close()
{
	joy_initd = false;

	for (int i = 0; i < 4; i++)
	{
		if (sdl_joysticks[i] != nullptr)
		{
			SDL_JoystickClose(sdl_joysticks[i]);
		}

		sdl_joysticks[i] = nullptr;
	}
}

void joy_Add(int i)
{
	auto joy = SDL_JoystickOpen(i);

	if (joy == nullptr)
	{
		log("joy_Init: Couldn't open joystick %d", i);
		return;
	}

	sdl_joysticks[i] = joy;

	auto& stick = sticks[i];
	stick = {};
	stick.active = true;
	stick.xmin = -32768; // SDL-defined mins and maxes
	stick.xmax = 32767;
	stick.ymin = -32768;
	stick.ymax = 32767;
	stick.xrange = stick.xmax - stick.xmin;
	stick.yrange = stick.ymax - stick.ymin;
	stick.range_left = (stick.xrange * DEADZONE / 100) + stick.xmin;
	stick.range_right = (stick.xrange * (100 - DEADZONE) / 100) + stick.xmin;
	stick.range_up = (stick.yrange * DEADZONE / 100) + stick.ymin;
	stick.range_down = (stick.yrange * (100 - DEADZONE) / 100) + stick.ymin;
	stick.analog_x = 0;
	stick.analog_y = 0;
	stick.up = false;
	stick.down = false;
	stick.left = false;
	stick.right = false;

	//log("x1 %d x2 %d y1 %d y2 %d xr %d yr %d", stick.xmin, stick.xmax, stick.ymin, stick.ymax, stick.xrange, stick.yrange);
	//log("left %d right %d up %d down %d", stick.range_left, stick.range_right, stick.range_up, stick.range_down);
	log("joy_Add: Opened joystick %d", i);
}

void joy_Remove(int instance_id)
{	
	for (int i = 3; i >= 0; i--)
	{
		auto joy = sdl_joysticks[i];

		if (joy != nullptr
		&& SDL_JoystickInstanceID(joy) == instance_id)
		{
			SDL_JoystickClose(joy);
			sticks[i] = {};
			sdl_joysticks[i] = nullptr;
			log("joy_Remove: Closed joystick %d", i);
			break;
		}
	}
}

void joy_Update()
{
	if (!joy_initd)
	{
		return;
	}

	for (int i = 0; i < 4; i++)
	{
		auto& stick = sticks[i];
		if (!stick.active)
		{
			continue;
		}

		Sint16 xpos, ypos;
		xpos = SDL_JoystickGetAxis(sdl_joysticks[i], 0);
		ypos = SDL_JoystickGetAxis(sdl_joysticks[i], 1);		

		stick.left = xpos < stick.range_left;
		stick.right = xpos > stick.range_right;
		stick.up = ypos < stick.range_up;
		stick.down = ypos > stick.range_down;

		// convert to (-1000,1000) range for verge
		stick.analog_x = (xpos * 2000 / stick.xrange) - 1000;
		stick.analog_y = (ypos * 2000 / stick.yrange) - 1000;

		for (int b = 0; b < 32; b++)
		{
			stick.button[b] = SDL_JoystickGetButton(sdl_joysticks[i], b);
		}
	}
}

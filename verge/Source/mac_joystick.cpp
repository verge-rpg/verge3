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
SDL_GameController* sdl_controllers[4];

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

		if (sdl_controllers[i] != nullptr)
		{
			joysticks_opened++;
		}
	}

	log("joy_Init: Opened %d / %d joysticks at startup", joysticks_opened, joysticks_detected);

	joy_initd = true;
	return true;
}

void joy_Close()
{
	joy_initd = false;

	for (int i = 0; i < 4; i++)
	{
		if (sdl_controllers[i] != nullptr)
		{
			SDL_GameControllerClose(sdl_controllers[i]);
			sdl_controllers[i] = nullptr;
		}

		sticks[i] = {};
	}
}

void joy_Add(int i)
{
	auto controller = SDL_GameControllerOpen(i);
	if (controller == nullptr)
	{
		log("joy_Init: Couldn't open joystick %d", i);
		return;
	}

	sdl_controllers[i] = controller;

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
		auto controller = sdl_controllers[i];

		if (controller != nullptr)
		{
			auto joy = SDL_GameControllerGetJoystick(controller);

			if (joy != nullptr)
			{
				if (SDL_JoystickInstanceID(joy) == instance_id)
				{
					SDL_GameControllerClose(controller);
					sticks[i] = {};
					sdl_controllers[i] = nullptr;
					log("joy_Remove: Closed joystick %d", i);
					break;
				}
			}
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

		auto controller = sdl_controllers[i];

		Sint16 xpos, ypos;
		xpos = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
		ypos = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);

		stick.left = xpos < stick.range_left || SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
		stick.right = xpos > stick.range_right || SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
		stick.up = ypos < stick.range_up || SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_UP);
		stick.down = ypos > stick.range_down || SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN);

		// convert to (-1000,1000) range for verge
		stick.analog_x = (xpos * 2000 / stick.xrange) - 1000;
		stick.analog_y = (ypos * 2000 / stick.yrange) - 1000;

		for (int b = 0; b < SDL_CONTROLLER_BUTTON_MAX; b++)
		{
			stick.button[b] = SDL_GameControllerGetButton(controller, static_cast<SDL_GameControllerButton>(b));
		}
	}
}

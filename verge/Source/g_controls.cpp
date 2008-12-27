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
	g_controls.cpp
 ****************************************************************/

#include "xerxes.h"

/***************************** data *****************************/

byte up, down, left, right;
byte b1, b2, b3, b4;
byte kill_up, kill_down, kill_left, kill_right;
byte kill_b1, kill_b2, kill_b3, kill_b4;

StringRef bindbutton[4];

byte k_b1 = SCAN_ENTER,
     k_b2 = SCAN_ALT,
	 k_b3 = SCAN_ESC,
	 k_b4 = SCAN_SPACE;

// Overkill (2006-06-25): Customizable directionals on the keyboard.
byte k_up = SCAN_UP,
	 k_down = SCAN_DOWN,
	 k_left = SCAN_LEFT,
	 k_right = SCAN_RIGHT;

byte j_b1=0, j_b2=1, j_b3=2, j_b4=3;

/***************************** code *****************************/
extern int _input_killswitch;
void UpdateControls()
{
	HandleMessages();

	if( _input_killswitch ) {
		b4 = b3 = b2 = b1 = right = left = down = up = false;
		return;
	}

	joy_Update();
	mouse_Update();
	UpdateKeyboard();

	byte oldb1 = b1,
		 oldb2 = b2,
	     oldb3 = b3,
		 oldb4 = b4;

	// Overkill (2006-06-25):
	// The following four ifs have been altered to allow custom directional keys.
	if (keys[k_up] || sticks[0].up) up = true; else up = false;
	if (keys[k_left] || sticks[0].left) left = true; else left = false;
	if (keys[k_down] || sticks[0].down) down = true; else down = false;
	if (keys[k_right] || sticks[0].right) right = true; else right = false;

	if (keys[k_b1] || sticks[0].button[j_b1]) b1 = true; else b1 = false;
	if (keys[k_b2] || sticks[0].button[j_b2]) b2 = true; else b2 = false;
	if (keys[k_b3] || sticks[0].button[j_b3]) b3 = true; else b3 = false;
	if (keys[k_b4] || sticks[0].button[j_b4]) b4 = true; else b4 = false;

	if (!up && kill_up) kill_up = false;
	if (!down && kill_down) kill_down = false;
	if (!left && kill_left) kill_left = false;
	if (!right && kill_right) kill_right = false;

	if (!b1 && kill_b1) kill_b1 = false;
	if (!b2 && kill_b2) kill_b2 = false;
	if (!b3 && kill_b3) kill_b3 = false;
	if (!b4 && kill_b4) kill_b4 = false;

	if (up && kill_up) up = false;
	if (down && kill_down) down = false;
	if (left && kill_left) left = false;
	if (right && kill_right) right = false;

	if (b1 && kill_b1) b1 = false;
	if (b2 && kill_b2) b2 = false;
	if (b3 && kill_b3) b3 = false;
	if (b4 && kill_b4) b4 = false;

	//mbg 9/5/05 todo removed for psp
	//TODO LUA
	if (b1 && !oldb1) se->ExecuteFunctionString(bindbutton[0].c_str());
	if (b2 && !oldb2) se->ExecuteFunctionString(bindbutton[1].c_str());
	if (b3 && !oldb3) se->ExecuteFunctionString(bindbutton[2].c_str());
	if (b4 && !oldb4) se->ExecuteFunctionString(bindbutton[3].c_str());
}

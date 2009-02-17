/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


/******************************************************************
 * verge3: mac_keyboard.cpp                                       *
 * copyright (c) 2001 vecna                                       *
 ******************************************************************/

#include "xerxes.h"
#include "SDL_scancode.h"

/****************************** data ******************************/

char keys[256];
byte lastpressed;
byte lastkey;
StringRef bindarray[256];

byte key_ascii_tbl[128] =
{
   0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,   9,
   'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0,  0,   'a', 's',
   'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', 39,  0,   0,   92,  'z', 'x', 'c', 'v',
   'b', 'n', 'm', ',', '.', '/', 0,   '*', 0,   ' ', 0,   3,   3,   3,   3,   8,
   3,   3,   3,   3,   3,   0,   0,   0,   0,   0,   '-', 0,   0,   0,   '+', 0,
   0,   0,   0,   127, 0,   0,   92,  3,   3,   0,   0,   0,   0,   0,   0,   0,
   13,  0,   '/', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   127,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   '/', 0,   0,   0,   0,   0
};

byte key_shift_tbl[128] =
{
   0,   0,   '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0, 126,
   'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0,   0,   'A', 'S',
   'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', 34,  0,   0,   '|', 'Z', 'X', 'C', 'V',
   'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   1,   1,   1,   1,   1,
   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   '-', 0,   0,   0,   '+', 0,
   0,   0,   1,   127, 0,   0,   0,   1,   1,   0,   0,   0,   0,   0,   0,   0,
   13,  0,   '/', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   127,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   '/', 0,   0,   0,   0,   0
};

byte SdlKeyToScan(SDLKey key)
{
	// Converts from SDLkeys to keyboard scan codes
	// some keys have no mapping, the return 0
	/* TODO Keypad might be arrows instead of numbers */
	switch (key)
	{
    case SDL_SCANCODE_BACKSPACE:
        return SCAN_BACKSP;
    case SDL_SCANCODE_TAB:
        return SCAN_TAB;
    case SDL_SCANCODE_CLEAR:
        return 0;
    case SDL_SCANCODE_RETURN:
        return SCAN_ENTER;
    case SDL_SCANCODE_PAUSE:
        return 0;
    case SDL_SCANCODE_ESCAPE:
        return SCAN_ESC;
    case SDL_SCANCODE_SPACE:
        return SCAN_SPACE;
    case SDL_SCANCODE_APOSTROPHE:
        return SCAN_QUOTA;
    case SDL_SCANCODE_COMMA:
        return SCAN_COMA;
    case SDL_SCANCODE_MINUS:
        return SCAN_MINUS;
    case SDL_SCANCODE_PERIOD:
        return SCAN_DOT;
    case SDL_SCANCODE_SLASH:
        return SCAN_SLASH;
    case SDL_SCANCODE_0:
        return SCAN_0;
    case SDL_SCANCODE_1:
        return SCAN_1;
    case SDL_SCANCODE_2:
        return SCAN_2;
    case SDL_SCANCODE_3:
        return SCAN_3;
    case SDL_SCANCODE_4:
        return SCAN_4;
    case SDL_SCANCODE_5:
        return SCAN_5;
    case SDL_SCANCODE_6:
        return SCAN_6;
    case SDL_SCANCODE_7:
        return SCAN_7;
    case SDL_SCANCODE_8:
        return SCAN_8;
    case SDL_SCANCODE_9:
        return SCAN_9;
    case SDL_SCANCODE_SEMICOLON:
        return SCAN_SCOLON;
    case SDL_SCANCODE_EQUALS:
        return SCAN_EQUALS;
    case SDL_SCANCODE_LEFTBRACKET:
        return SCAN_LANGLE;
    case SDL_SCANCODE_BACKSLASH:
        return SCAN_BSLASH;
    case SDL_SCANCODE_RIGHTBRACKET:
        return SCAN_RANGLE;
    case SDL_SCANCODE_A:
        return SCAN_A;
    case SDL_SCANCODE_B:
        return SCAN_B;
    case SDL_SCANCODE_C:
        return SCAN_C;
    case SDL_SCANCODE_D:
        return SCAN_D;
    case SDL_SCANCODE_E:
        return SCAN_E;
    case SDL_SCANCODE_F:
        return SCAN_F;
    case SDL_SCANCODE_G:
        return SCAN_G;
    case SDL_SCANCODE_H:
        return SCAN_H;
    case SDL_SCANCODE_I:
        return SCAN_I;
    case SDL_SCANCODE_J:
        return SCAN_J;
    case SDL_SCANCODE_K:
        return SCAN_K;
    case SDL_SCANCODE_L:
        return SCAN_L;
    case SDL_SCANCODE_M:
        return SCAN_M;
    case SDL_SCANCODE_N:
        return SCAN_N;
    case SDL_SCANCODE_O:
        return SCAN_O;
    case SDL_SCANCODE_P:
        return SCAN_P;
    case SDL_SCANCODE_Q:
        return SCAN_Q;
    case SDL_SCANCODE_R:
        return SCAN_R;
    case SDL_SCANCODE_S:
        return SCAN_S;
    case SDL_SCANCODE_T:
        return SCAN_T;
    case SDL_SCANCODE_U:
        return SCAN_U;
    case SDL_SCANCODE_V:
        return SCAN_V;
    case SDL_SCANCODE_W:
        return SCAN_W;
    case SDL_SCANCODE_X:
        return SCAN_X;
    case SDL_SCANCODE_Y:
        return SCAN_Y;
    case SDL_SCANCODE_Z:
        return SCAN_Z;
    case SDL_SCANCODE_DELETE:
        return SCAN_DEL;
    case SDL_SCANCODE_KP_0:
        return SCAN_0;
    case SDL_SCANCODE_KP_1:
        return SCAN_1;
    case SDL_SCANCODE_KP_2:
        return SCAN_2;
    case SDL_SCANCODE_KP_3:
        return SCAN_3;
    case SDL_SCANCODE_KP_4:
        return SCAN_4;
    case SDL_SCANCODE_KP_5:
        return SCAN_5;
    case SDL_SCANCODE_KP_6:
        return SCAN_6;
    case SDL_SCANCODE_KP_7:
        return SCAN_7;
    case SDL_SCANCODE_KP_8:
        return SCAN_8;
    case SDL_SCANCODE_KP_9:
        return SCAN_9;
    case SDL_SCANCODE_KP_PERIOD:
        return SCAN_DOT;
    case SDL_SCANCODE_KP_DIVIDE:
        return SCAN_BSLASH;
    case SDL_SCANCODE_KP_MULTIPLY:
        return SCAN_8;
    case SDL_SCANCODE_KP_MINUS:
        return SCAN_MINUS;
    case SDL_SCANCODE_KP_PLUS:
        return SCAN_EQUALS;
    case SDL_SCANCODE_KP_ENTER:
        return SCAN_ENTER;
    case SDL_SCANCODE_KP_EQUALS:
        return SCAN_EQUALS;
    case SDL_SCANCODE_UP:
        return SCAN_UP;
    case SDL_SCANCODE_DOWN:
        return SCAN_DOWN;
    case SDL_SCANCODE_RIGHT:
        return SCAN_RIGHT;
    case SDL_SCANCODE_LEFT:
        return SCAN_LEFT;
    case SDL_SCANCODE_INSERT:
        return SCAN_INSERT;
    case SDL_SCANCODE_HOME:
        return SCAN_HOME;
    case SDL_SCANCODE_END:
        return SCAN_END;
    case SDL_SCANCODE_PAGEUP:
        return SCAN_PGUP;
    case SDL_SCANCODE_PAGEDOWN:
        return SCAN_PGDN;
    case SDL_SCANCODE_F1:
        return SCAN_F1;
    case SDL_SCANCODE_F2:
        return SCAN_F2;
    case SDL_SCANCODE_F3:
        return SCAN_F3;
    case SDL_SCANCODE_F4:
        return SCAN_F4;
    case SDL_SCANCODE_F5:
        return SCAN_F5;
    case SDL_SCANCODE_F6:
        return SCAN_F6;
    case SDL_SCANCODE_F7:
        return SCAN_F7;
    case SDL_SCANCODE_F8:
        return SCAN_F8;
    case SDL_SCANCODE_F9:
        return SCAN_F9;
    case SDL_SCANCODE_F10:
        return SCAN_F10;
    case SDL_SCANCODE_F11:
        return SCAN_F11;
    case SDL_SCANCODE_F12:
        return SCAN_F12;
    case SDL_SCANCODE_F13:
        return 0;
    case SDL_SCANCODE_F14:
        return 0;
    case SDL_SCANCODE_F15:
        return 0;
    case SDL_SCANCODE_NUMLOCKCLEAR:
        return SCAN_NUMLOCK;
    case SDL_SCANCODE_CAPSLOCK:
        return SCAN_CAPS;
    case SDL_SCANCODE_SCROLLLOCK:
        return SCAN_SCRLOCK;
    case SDL_SCANCODE_RSHIFT:
        return SCAN_RSHIFT;
    case SDL_SCANCODE_LSHIFT:
        return SCAN_LSHIFT;
    case SDL_SCANCODE_RCTRL:
        return SCAN_CTRL;
    case SDL_SCANCODE_LCTRL:
        return SCAN_CTRL;
    case SDL_SCANCODE_RALT:
        return SCAN_ALT;
    case SDL_SCANCODE_LALT:
        return SCAN_ALT;
	default:
		return 0;
	}
}

/****************************** code ******************************/

void ParseKeyEvent(SDL_KeyboardEvent keyEvent)
{
	byte scanCode = SdlKeyToScan(keyEvent.keysym.sym);
	bool key_pressed = false;

	if(keyEvent.type == SDL_KEYDOWN)
	{
		lastpressed = scanCode;
		keys[scanCode]=(char) 0x80;
		key_pressed = true;

		if (keys[SCAN_LSHIFT] || keys[SCAN_RSHIFT])
			lastkey = key_shift_tbl[lastpressed];
		else
			lastkey = key_ascii_tbl[lastpressed];
	}
	else
	{
		keys[scanCode]=0;
	}
#ifdef __APPLE__
	// check for command-q or command-enter
	if(keys[SCAN_ALT] && keys[SCAN_Q]) err("");
#elif __LINUX__
	// alt-x quits
	if(keys[SCAN_ALT] && keys[SCAN_X]) err("");
#endif

	if(keys[SCAN_ALT] && keys[SCAN_ENTER])
	{
		keys[SCAN_M]=0;
		keys[SCAN_ALT]=0;
		keys[184]=0;
		if(lastpressed==SCAN_ENTER) lastpressed=0;
		sdl_toggleFullscreen();
	}

	if (lastpressed == SCAN_LEFT && keys[SCAN_RIGHT]) keys[SCAN_RIGHT]=0;
	if (lastpressed == SCAN_RIGHT && keys[SCAN_LEFT]) keys[SCAN_LEFT]=0;
	if (lastpressed == SCAN_UP && keys[SCAN_DOWN]) keys[SCAN_DOWN]=0;
	if (lastpressed == SCAN_DOWN && keys[SCAN_UP]) keys[SCAN_UP]=0;

	if (key_pressed && bindarray[lastpressed].length())
		se->ExecuteFunctionString(bindarray[lastpressed].c_str());
	return ;
}

void UpdateKeyboard()
{
	HandleMessages();
}

void ShutdownKeyboard()
{
}

void InitKeyboard()
{
	int i;
	for(i=0; i < sizeof(keys); i++)
		keys[i] = 0;
}

// check to see if a raw SDL key is pressed -
// for keys not covered by normal keycodes
bool isSDLKeyPressed(int key)
{
	int numkeys;
	Uint8 *keys = SDL_GetKeyboardState(&numkeys);
	if(key < numkeys)
		return keys[key];
	return false;
}

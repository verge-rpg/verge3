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
    case SDLK_BACKSPACE:
        return SCAN_BACKSP;
    case SDLK_TAB:
        return SCAN_TAB;
    case SDLK_CLEAR:
        return 0;
    case SDLK_RETURN:
        return SCAN_ENTER;
    case SDLK_PAUSE:
        return 0;
    case SDLK_ESCAPE:
        return SCAN_ESC;
    case SDLK_SPACE:
        return SCAN_SPACE;
    case SDLK_EXCLAIM:
        return SCAN_1;
    case SDLK_QUOTEDBL:
        return SCAN_QUOTA;
    case SDLK_HASH:
        return SCAN_3;
    case SDLK_DOLLAR:
        return SCAN_4;
    case SDLK_AMPERSAND:
        return SCAN_7;
    case SDLK_QUOTE:
        return SCAN_QUOTA;
    case SDLK_LEFTPAREN:
        return SCAN_9;
    case SDLK_RIGHTPAREN:
        return SCAN_0;
    case SDLK_ASTERISK:
        return SCAN_8;
    case SDLK_PLUS:
        return SCAN_EQUALS;
    case SDLK_COMMA:
        return SCAN_COMA;
    case SDLK_MINUS:
        return SCAN_MINUS;
    case SDLK_PERIOD:
        return SCAN_DOT;
    case SDLK_SLASH:
        return SCAN_SLASH;
    case SDLK_0:
        return SCAN_0;
    case SDLK_1:
        return SCAN_1;
    case SDLK_2:
        return SCAN_2;
    case SDLK_3:
        return SCAN_3;
    case SDLK_4:
        return SCAN_4;
    case SDLK_5:
        return SCAN_5;
    case SDLK_6:
        return SCAN_6;
    case SDLK_7:
        return SCAN_7;
    case SDLK_8:
        return SCAN_8;
    case SDLK_9:
        return SCAN_9;
    case SDLK_COLON:
        return SCAN_SCOLON;
    case SDLK_SEMICOLON:
        return SCAN_SCOLON;
    case SDLK_LESS:
        return SCAN_COMA;
    case SDLK_EQUALS:
        return SCAN_EQUALS;
    case SDLK_GREATER:
        return SCAN_DOT;
    case SDLK_QUESTION:
        return SCAN_SLASH;
    case SDLK_AT:
        return SCAN_2;
    case SDLK_LEFTBRACKET:
        return SCAN_LANGLE;
    case SDLK_BACKSLASH:
        return SCAN_BSLASH;
    case SDLK_RIGHTBRACKET:
        return SCAN_RANGLE;
    case SDLK_CARET:
        return SCAN_6;
    case SDLK_UNDERSCORE:
        return SCAN_MINUS;
    case SDLK_BACKQUOTE:
        return 0;
    case SDLK_a:
        return SCAN_A;
    case SDLK_b:
        return SCAN_B;
    case SDLK_c:
        return SCAN_C;
    case SDLK_d:
        return SCAN_D;
    case SDLK_e:
        return SCAN_E;
    case SDLK_f:
        return SCAN_F;
    case SDLK_g:
        return SCAN_G;
    case SDLK_h:
        return SCAN_H;
    case SDLK_i:
        return SCAN_I;
    case SDLK_j:
        return SCAN_J;
    case SDLK_k:
        return SCAN_K;
    case SDLK_l:
        return SCAN_L;
    case SDLK_m:
        return SCAN_M;
    case SDLK_n:
        return SCAN_N;
    case SDLK_o:
        return SCAN_O;
    case SDLK_p:
        return SCAN_P;
    case SDLK_q:
        return SCAN_Q;
    case SDLK_r:
        return SCAN_R;
    case SDLK_s:
        return SCAN_S;
    case SDLK_t:
        return SCAN_T;
    case SDLK_u:
        return SCAN_U;
    case SDLK_v:
        return SCAN_V;
    case SDLK_w:
        return SCAN_W;
    case SDLK_x:
        return SCAN_X;
    case SDLK_y:
        return SCAN_Y;
    case SDLK_z:
        return SCAN_Z;
    case SDLK_DELETE:
        return SCAN_DEL;
    case SDLK_KP0:
        return SCAN_0;
    case SDLK_KP1:
        return SCAN_1;
    case SDLK_KP2:
        return SCAN_2;
    case SDLK_KP3:
        return SCAN_3;
    case SDLK_KP4:
        return SCAN_4;
    case SDLK_KP5:
        return SCAN_5;
    case SDLK_KP6:
        return SCAN_6;
    case SDLK_KP7:
        return SCAN_7;
    case SDLK_KP8:
        return SCAN_8;
    case SDLK_KP9:
        return SCAN_9;
    case SDLK_KP_PERIOD:
        return SCAN_DOT;
    case SDLK_KP_DIVIDE:
        return SCAN_BSLASH;
    case SDLK_KP_MULTIPLY:
        return SCAN_8;
    case SDLK_KP_MINUS:
        return SCAN_MINUS;
    case SDLK_KP_PLUS:
        return SCAN_EQUALS;
    case SDLK_KP_ENTER:
        return SCAN_ENTER;
    case SDLK_KP_EQUALS:
        return SCAN_EQUALS;
    case SDLK_UP:
        return SCAN_UP;
    case SDLK_DOWN:
        return SCAN_DOWN;
    case SDLK_RIGHT:
        return SCAN_RIGHT;
    case SDLK_LEFT:
        return SCAN_LEFT;
    case SDLK_INSERT:
        return SCAN_INSERT;
    case SDLK_HOME:
        return SCAN_HOME;
    case SDLK_END:
        return SCAN_END;
    case SDLK_PAGEUP:
        return SCAN_PGUP;
    case SDLK_PAGEDOWN:
        return SCAN_PGDN;
    case SDLK_F1:
        return SCAN_F1;
    case SDLK_F2:
        return SCAN_F2;
    case SDLK_F3:
        return SCAN_F3;
    case SDLK_F4:
        return SCAN_F4;
    case SDLK_F5:
        return SCAN_F5;
    case SDLK_F6:
        return SCAN_F6;
    case SDLK_F7:
        return SCAN_F7;
    case SDLK_F8:
        return SCAN_F8;
    case SDLK_F9:
        return SCAN_F9;
    case SDLK_F10:
        return SCAN_F10;
    case SDLK_F11:
        return SCAN_F11;
    case SDLK_F12:
        return SCAN_F12;
    case SDLK_F13:
        return 0;
    case SDLK_F14:
        return 0;
    case SDLK_F15:
        return 0;
    case SDLK_NUMLOCK:
        return SCAN_NUMLOCK;
    case SDLK_CAPSLOCK:
        return SCAN_CAPS;
    case SDLK_SCROLLOCK:
        return SCAN_SCRLOCK;
    case SDLK_RSHIFT:
        return SCAN_RSHIFT;
    case SDLK_LSHIFT:
        return SCAN_LSHIFT;
    case SDLK_RCTRL:
        return SCAN_CTRL;
    case SDLK_LCTRL:
        return SCAN_CTRL;
    case SDLK_RALT:
        return SCAN_ALT;
    case SDLK_LALT:
        return SCAN_ALT;
    case SDLK_RMETA:
        return SCAN_ALT;
    case SDLK_LMETA:
        return SCAN_ALT;
    case SDLK_LSUPER:
        return SCAN_ALT;
    case SDLK_RSUPER:
        return SCAN_ALT;
    case SDLK_MODE:
        return 0;
    case SDLK_HELP:
        return 0;
    case SDLK_PRINT:
        return 0;
    case SDLK_SYSREQ:
        return 0;
    case SDLK_BREAK:
        return 0;
    case SDLK_MENU:
        return 0;
    case SDLK_POWER:
        return 0;
    case SDLK_EURO:
        return 0;
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

	// check for command-q or command-enter
	if(keys[SCAN_ALT] && keys[SCAN_Q]) err("");
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
	Uint8 *keys = SDL_GetKeyState(&numkeys);
	if(key < numkeys)
		return keys[key];
	return false;
}

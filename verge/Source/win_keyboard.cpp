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
 * verge3: win_keyboard.cpp                                       *
 * copyright (c) 2001 vecna                                       *
 ******************************************************************/

#include "xerxes.h"
#define DIRECTINPUT_VERSION 0x0300
#include "dinput.h"

/****************************** data ******************************/

// Overkill (2006-06-30): Key buffer - stores ASCII keystrokes,
// since verge sucks at reading input fast enough.
std::string keybuffer;
// Overkill (2006-06-30): The delay before keys repeat. (Defaults to 0 - no repeat).
int key_input_delay;

LPDIRECTINPUT dinput;
DIPROPDWORD dipdw;
LPDIRECTINPUTDEVICE  di_keyb;
LPDIRECTINPUTDEVICE di_joy;
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

/****************************** code ******************************/

int ParseKeyEvent()
{
	HRESULT hr;
	int y=0;
	DIDEVICEOBJECTDATA rgod[64];
	DWORD cod=64, iod;
	bool key_pressed = false;

	HandleMessages();
	hr = di_keyb -> GetDeviceData(sizeof(DIDEVICEOBJECTDATA),rgod, &cod, 0);	//retrive data
	if(hr!=DI_OK&&hr!=DI_BUFFEROVERFLOW)
	{
		hr = di_keyb -> Acquire();
		if(!SUCCEEDED(hr))
			//if(hr==DIERR_OTHERAPPHASPRIO) return 1;
			if(hr==DIERR_OTHERAPPHASPRIO) return 0;
	}
	else if(cod>0&&cod<=64)
	{
		for(iod=0;iod<cod;iod++)
		{
			if(rgod[iod].dwData==0x80)
			{
				keys[rgod[iod].dwOfs]=(char) 0x80;
				lastpressed = (char) rgod[iod].dwOfs;
				key_pressed = true;
				if (keys[SCAN_LSHIFT] || keys[SCAN_RSHIFT])
					lastkey = key_shift_tbl[lastpressed];
				else
					lastkey = key_ascii_tbl[lastpressed];

				// Overkill (2006-06-30): Add input to a key buffer.
				// Backspace.
				if (lastpressed == SCAN_BACKSP)
				{
					keybuffer = keybuffer + (char)8;
				}
				// Enter.
				else if (lastpressed == SCAN_ENTER)
				{
					keybuffer = keybuffer + '\n';
				}
				else if (lastkey)
				{
					keybuffer = keybuffer + (char)lastkey;
				}

				if (lastpressed != repeatedkey)
				{
					key_timer = systemtime;
					key_repeater = 0;
					repeatedkey = lastpressed;
				}
			}
			if(rgod[iod].dwData==0)
			{
				keys[rgod[iod].dwOfs]=0;
				if (rgod[iod].dwOfs == (unsigned) repeatedkey)
				{
					key_timer = 0;
					key_repeater = 0;
					repeatedkey = 0;
				}
				switch(rgod[iod].dwOfs)
				{
				case 0xCB: keys[SCAN_LEFT]=0;
					key_timer=0; key_repeater=0; repeatedkey=0;
					break;
				case 0xCD: keys[SCAN_RIGHT]=0;
					key_timer=0; key_repeater=0; repeatedkey=0;
					break;
				case 0xD0: keys[SCAN_DOWN]=0;
					key_timer=0; key_repeater=0; repeatedkey=0;
					break;
				case 0xC8: keys[SCAN_UP]=0;
					key_timer=0; key_repeater=0; repeatedkey=0;
					break;
				case 184: keys[SCAN_ALT]=0;
					key_timer=0; key_repeater=0; repeatedkey=0;
					break;
				case 156: keys[SCAN_ENTER]=0;
					key_timer=0; key_repeater=0; repeatedkey=0;
					break;
				}
			}
		}
	}
	if(keys[SCAN_ALT] && keys[SCAN_X]) err("");
	if(keys[SCAN_ALT] && keys[SCAN_TAB])
	{
		keys[SCAN_TAB]=0;
		keys[SCAN_ALT]=0;
		repeatedkey=0;
		keys[184]=0;
		if(lastpressed==SCAN_TAB) lastpressed=0;
	}
	if(keys[0xCB]) { keys[SCAN_LEFT]=(char)128; keys[0xCB]=0; lastpressed=SCAN_LEFT; key_timer=systemtime; repeatedkey=SCAN_LEFT; key_repeater=0;}
	if(keys[0xCD]) { keys[SCAN_RIGHT]=(char)128; keys[0xCD]=0; lastpressed=SCAN_RIGHT; key_timer=systemtime; repeatedkey=SCAN_RIGHT; key_repeater=0;}
	if(keys[0xD0]) { keys[SCAN_DOWN]=(char)128; keys[0xD0]=0; lastpressed=SCAN_DOWN; key_timer=systemtime;repeatedkey=SCAN_DOWN; key_repeater=0;}
	if(keys[0xC8]) { keys[SCAN_UP]=(char)128; keys[0xC8]=0; lastpressed=SCAN_UP; key_timer=systemtime; repeatedkey=SCAN_UP; key_repeater=0;}
	if(keys[184]) { keys[SCAN_ALT]=(char)128; keys[184]=0; lastpressed=SCAN_ALT; key_timer=systemtime; repeatedkey=SCAN_ALT; key_repeater=0;}
	if(keys[156]) { keys[SCAN_ENTER]=(char)128; keys[156]=0; lastpressed=SCAN_ENTER; key_timer=systemtime; repeatedkey=SCAN_ENTER; key_repeater=0;}

	if (lastpressed == SCAN_LEFT && keys[SCAN_RIGHT]) keys[SCAN_RIGHT]=0;
	if (lastpressed == SCAN_RIGHT && keys[SCAN_LEFT]) keys[SCAN_LEFT]=0;
	if (lastpressed == SCAN_UP && keys[SCAN_DOWN]) keys[SCAN_DOWN]=0;
	if (lastpressed == SCAN_DOWN && keys[SCAN_UP]) keys[SCAN_UP]=0;

	// Overkill (2006-06-30):
	// Allowing for key repeat as long as the user wants key repeat (defaults to off).
	if (systemtime - key_timer > key_input_delay
			&& repeatedkey > 0 && key_input_delay > 0)
	{
		// Overkill (2006-06-30): Add input to a key buffer.
		byte mykey;
		if (keys[SCAN_LSHIFT] || keys[SCAN_RSHIFT])
			mykey = key_shift_tbl[repeatedkey];
		else
			mykey = key_ascii_tbl[repeatedkey];
		// Backspace.
		if (repeatedkey == SCAN_BACKSP)
		{
			keybuffer = keybuffer + (char)8;
		}
		// Enter.
		else if (repeatedkey == SCAN_ENTER)
		{
			keybuffer = keybuffer + '\n';
		}
		else if (mykey)
		{
				keybuffer = keybuffer + (char)mykey;
		}
		// Update the timestamp, so it'll repeat at a sane rate again.
		key_timer = systemtime;
	}

	if (key_pressed && bindarray[lastpressed].length())
		se->ExecuteFunctionString(bindarray[lastpressed]);
	if (key_pressed)
		return 1;
	return 0;
}

void UpdateKeyboard()
{
	int result;
	do
	{
		result = ParseKeyEvent();
		// Overkill (2006-06-30): 
		// If the string gets too long, shorten it to the 255 most recent characters.
		if (keybuffer.length() > 255)
		{
			keybuffer = vc_strright(keybuffer,255).c_str();
		}
	}
	while (result);
}

// Overkill (2006-06-30):
// Clears the contents of the key buffer.
void FlushKeyBuffer()
{
	keybuffer = "";
}

void ShutdownKeyboard()
{
   if (di_keyb)
   {
      di_keyb -> Unacquire();
      di_keyb -> Release();
      di_keyb = NULL;
   }
   if (dinput)
   {
      dinput -> Release();
      dinput = NULL;
   }
}

void InitKeyboard()
{
	HRESULT hr;

	// Overkill (2006-06-30):
	// Initialize key buffer to an empty string.
	keybuffer = "";

	hr = DirectInputCreate(hMainInst, DIRECTINPUT_VERSION, &dinput, NULL);
	if (FAILED(hr))
		err("InitKeyboard: DirectInputCreate");

	hr = dinput -> CreateDevice(GUID_SysKeyboard, &di_keyb, NULL);
	if (FAILED(hr))
        err("InitKeyboard: dinput CreateDevice");

	hr = di_keyb -> SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr))
        err("InitKeyboard: dinput SetDataFormat");

	hr = di_keyb -> SetCooperativeLevel(hMainWnd,DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	if (FAILED(hr))
        err("InitKeyboard: sinput SetCooperativeLevel");

	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = 64;

	hr = di_keyb -> SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);

	if (FAILED(hr))
        err("InitKeyboard: Set buffer size");
	atexit(ShutdownKeyboard);
	return;
}

//all script engine common api functionality (between lua and vc) is spammed into here

#include "xerxes.h"
#include "opcodes.h"
#include "g_script.h"
#include "sincos.h"
#include <algorithm>

#define VCFILES				51
#define VC_READ				1
#define VC_WRITE			2
#define VC_WRITE_APPEND		3 // Overkill (2006-07-05): Append mode added.

//------------------- script engine state variables ----------------
int cur_stick = 0;
StringRef renderfunc,timerfunc;

int event_tx;
int event_ty;
int event_zone;
int event_entity;
int event_param;
int event_sprite;

int event_entity_hit;

int __grue_actor_index;


StringRef event_str;

StringRef _trigger_onStep, _trigger_afterStep;
StringRef _trigger_beforeEntityScript, _trigger_afterEntityScript;
StringRef _trigger_onEntityCollide;
StringRef _trigger_afterPlayerMove;

int _input_killswitch = 0;
//--------------------------------

struct FileRecord
{
	bool active;
	FILE *fptr;
	VFILE *vfptr;
	int mode;
};

FileRecord vcfiles[VCFILES];

ScriptEngine::ScriptEngine() {}


void InitScriptEngine() {
	memset(vcfiles, 0, sizeof (vcfiles));
	// allocate one dummy handle for dicts (0, null handle)
	Handle::forceAlloc(HANDLE_TYPE_DICT,1);

	cur_stick = 0;
	_input_killswitch = 0;
}


int vc_GetYear()
{
	return getYear();
}

int vc_GetMonth()
{
	return getMonth();
}

int vc_GetDay()
{
	return getDay();
}

int vc_GetDayOfWeek()
{
	return getDayOfWeek();
}

int vc_GetHour()
{
	return getHour();
}

int vc_GetMinute()
{
	return getMinute();
}

int vc_GetSecond()
{
	return getSecond();
}


ScriptEngine *se;

void ScriptEngine::WriteHvar(int category, int loc, int ofs, int value)
{
	switch (category)
	{
		case intHVAR0:    // _WRITEINT
			switch (loc)
			{
				case 1: vctimer = value; break;
				case 3: lastpressed = value; break;
				case 4: mouse_set(value,mouse_y); break;
				case 5: mouse_set(mouse_x,value); break;
				case 6: mouse_l = value; break;
				case 7: mouse_r = value; break;
				case 8: mouse_m = value; break;
				case 9: mwheel = (float) value; break;
				case 17: cur_stick = value<4 ? value : 0; break;
				case 26: up = value; break;
				case 27: down = value; break;
				case 28: left = value; break;
				case 29: right = value; break;
				case 30: b1 = value; break;
				case 31: b2 = value; break;
				case 32: b3 = value; break;
				case 33: b4 = value; break;
				case 39: xwin = value; break;
				case 40: ywin = value; break;
				case 41: cameratracking = value; break;
				case 51: transColor = value; break;
				case 60: lastkey = value; break;
				case 96: cameratracker = value; break;
				case 101: playerstep = value >= 1 ? value : 1; break;
				case 102: playerdiagonals = value ? 1 : 0; break;
				default: vcerr("Unknown HVAR0 (%d) (set %d)", loc, value);
			}
			break;
		case intHVAR1:
			switch (loc)
			{
				case 2: if(ofs<0 || ofs>255) vcerr("Invalid key number: %d ; 0..255 inclusive are valid"); else keys[ofs] = value; break;
				case 43: if (ofs>=0 && ofs<entities) entity[ofs]->setxy(value, entity[ofs]->gety()); break;
				case 44: if (ofs>=0 && ofs<entities) entity[ofs]->setxy(entity[ofs]->getx(), value); break;
				case 45: if (ofs>=0 && ofs<entities) entity[ofs]->specframe = value; break;
				#ifndef NOTIMELESS
				case 52: skewlines[ofs] = value; break;
				#endif
				case 53: (*(byte *)ofs)=(byte) value; return;
				case 54: (*(word *)ofs)=(word) value; return;
				case 55: (*(quad *)ofs)=(quad) value; return;
				case 56: (*(char *)ofs)=(byte) value; return;
				case 57: (*(short*)ofs)=(word) value; return;
				case 58: (*(int  *)ofs)=(quad) value; return;
				case 62: if (ofs>=0 && ofs<entities) entity[ofs]->setface(value); break;
				case 63: if (ofs>=0 && ofs<entities) entity[ofs]->setspeed(value); break;
				case 64: if (ofs>=0 && ofs<entities) entity[ofs]->visible = value ? true : false; break;
				case 66: if (ofs>=0 && ofs<sprites.size()) sprites[ofs].x = value; return;
				case 67: if (ofs>=0 && ofs<sprites.size()) sprites[ofs].y = value; return;
				case 68: if (ofs>=0 && ofs<sprites.size()) sprites[ofs].sc = value; return;
				case 69: if (ofs>=0 && ofs<sprites.size()) sprites[ofs].image = value; return;
				case 70: if (ofs>=0 && ofs<sprites.size()) sprites[ofs].lucent = value; return;
				case 71: if (ofs>=0 && ofs<sprites.size()) sprites[ofs].addsub = value; return;
				case 72: if (ofs>=0 && ofs<sprites.size()) sprites[ofs].alphamap = value; return;
				case 73: if (ofs>=0 && ofs<sprites.size()) sprites[ofs].thinkrate = value; return; // Overkill (2006-07-28)
				case 75: if (ofs>=0 && ofs<sprites.size()) sprites[ofs].xflip = value; return;
				case 76: if (ofs>=0 && ofs<sprites.size()) sprites[ofs].yflip = value; return;
				case 78: if (ofs>=0 && ofs<entities) entity[ofs]->obstruction = (value!=0); return;
				case 79: if (ofs>=0 && ofs<entities) entity[ofs]->obstructable = (value!=0); return;
				case 94: if (current_map && ofs>=0 && ofs<current_map->numlayers) current_map->layers[ofs]->lucent = value; return;
                case 97: if (current_map && ofs>=0 && ofs<current_map->numlayers) current_map->layers[ofs]->SetParallaxX(value / 65536.0); return;
                case 98: if (current_map && ofs>=0 && ofs<current_map->numlayers) current_map->layers[ofs]->SetParallaxY(value / 65536.0); return;
				case 105: if (ofs>=0 && ofs<sprites.size()) sprites[ofs].ent = value; return; // Overkill (2006-07-28)
				case 106: if (ofs>=0 && ofs<sprites.size()) sprites[ofs].silhouette = value; return; // Overkill (2006-07-28)
				case 107: if (ofs>=0 && ofs<sprites.size()) sprites[ofs].color = value; return; // Overkill (2006-07-28)
				case 108: if (ofs>=0 && ofs<sprites.size()) sprites[ofs].wait = value; return; // Overkill (2006-07-28)
				case 109: if (ofs>=0 && ofs<sprites.size()) sprites[ofs].onmap = value; return; // Overkill (2006-07-28)
				case 110: if (ofs>=0 && ofs<sprites.size()) sprites[ofs].layer = value; return; // Overkill (2006-07-28)
				case 111: if (ofs>=0 && ofs<entities) entity[ofs]->lucent = value; return; // Overkill (2006-07-28)
				case 112: if (ofs>=0 && ofs<sprites.size()) sprites[ofs].timer = value; return; // Overkill (2006-07-28)
				//case 113: ent.framew
				//case 114: ent.frameh

				default:
					WriteHvar_derived(category,loc,ofs,value);
			}
			break;
		default:
			vcerr("Fatal Error Code Razmataz"); 
	}
}

void ScriptEngine::WriteHvar_str(int category, int loc, int arg, CStringRef value)
{
	switch(category)
	{
	case strHSTR0:
		switch (loc)
		{
			case 85: if (current_map) strcpy(current_map->renderstring, to_upper_copy(value.str()).c_str());
					 break;
			case 95: clipboard_setText(value.c_str()); break;
			case 104: if (current_map) strcpy(current_map->savevspname, value.c_str());
					 break;

			case 121: //trigger.onStep
					_trigger_onStep = value;
					break;
			case 122: //trigger.afterStep
					_trigger_afterStep = value;
					break;
			case 123: //trigger.beforeEntityScript
					_trigger_beforeEntityScript = value;
					break;
			case 124: //trigger.afterEntityScript
					_trigger_afterEntityScript = value;
					break;
			case 125: //trigger.onEntityCollide
					_trigger_onEntityCollide = value;
					break;
			case 127: //trigger.afterPlayerMove
					_trigger_afterPlayerMove = value;
					break;
					
			default: WriteHvar_str_derived(category,loc,arg,value);
		}
		break;
	case strHSTR1:
		switch (loc)
		{
			case 65:
				if(arg >= 0 && arg < entities)
					entity[arg]->script = value; break;
				break;
			case 74: 
				if(arg >= 0 && arg < sprites.size())
					sprites[arg].thinkproc = value; break;
				break;
			case 100: //Entity.Chr
				ScriptEngine::Set_EntityChr(arg,value);
				break;
			case 115: // Overkill (2006-07-30): Entity description
				ScriptEngine::Set_EntityDescription(arg,value);
				break;
			default: WriteHvar_str_derived(category,loc,arg,value);
		}
		break;
	default:
		vcerr("Fatal Error Code Hangnail\n");
	}
}

void ScriptEngine::WriteHvar_str_derived(int category, int loc, int ofs, CStringRef value)
{
	switch(category)
	{
		case strHSTR0:
			vcerr("Unknown HSTR0 (%d)", loc); break;
		case strHSTR1:
			vcerr("Unknown HSTR1 (%d, %d)", loc, ofs); break;
		default:
			vcerr("Fatal Error Code Interstellar"); break;
	}
}


StringRef ScriptEngine::ReadHvar_str(int category, int loc, int arg)
{
	switch(category)
	{
		case strHSTR0:
		switch (loc)
		{
			case 84: return current_map ? current_map->mapname : empty_string;
			case 85: return current_map ? current_map->renderstring : empty_string; break;
			case 86: return current_map ? current_map->musicname : empty_string; break;
			case 95: return clipboard_getText(); break;
            case 99: return current_map ? current_map->mapfname : empty_string; break;
			case 104: return current_map ? current_map->savevspname : empty_string; break;
			case 121: return _trigger_onStep;
			case 122: return _trigger_afterStep;
			case 123: return _trigger_beforeEntityScript;
			case 124: return _trigger_afterEntityScript;
			case 125: return _trigger_onEntityCollide;
			case 127: return _trigger_afterPlayerMove;
			default: return ReadHvar_str_derived(category,loc,arg);
		}
		case strHSTR1:
			switch (loc)
			{
				case 65:
					if(arg >= 0 && arg < entities)
						return entity[arg]->script;
					else
						return empty_string;
				case 74:
					if(arg >= 0 && arg < sprites.size())
						return sprites[arg].thinkproc; // Overkill (2006-07-28): No more HSTR error 4 u.
					else
						return empty_string;
					break;
				case 88: // Overkill (2006-06-25): Now this actually has a use!
						if (current_map)
							if (arg >= 0 && arg < current_map->numzones)
								return current_map->zones[arg]->name;
						return empty_string;
				case 89: // Overkill (2006-06-25): Now this actually has a use!
						if (current_map)
							if (arg >= 0 && arg < current_map->numzones)
								return current_map->zones[arg]->script;
						return empty_string;
				case 100: //entity.chr
					return ScriptEngine::Get_EntityChr(arg);
				case 115:
					// Overkill (2006-07-30): Entity description
					return ScriptEngine::Get_EntityDescription(arg);
				default:
					return ReadHvar_str_derived(category,loc,arg);
			}
			break;
		default:
			vcerr("Fatal Error Code Boomerang"); 
			return empty_string;
	}
}

StringRef ScriptEngine::ReadHvar_str_derived(int category, int loc, int ofs)
{
	switch(category)
	{
		case strHSTR0:
			vcerr("Unknown HSTR0 (%d)", loc); break;
		case strHSTR1:
			vcerr("Unknown HSTR1 (%d, %d)", loc, ofs); break;
		default:
			vcerr("Fatal Error Code Finagle"); break;
	}
	return empty_string;
}

int ScriptEngine::ReadHvar(int category, int loc, int ofs)
{
	switch(category)
	{
		case intHVAR0:
			switch (loc)
			{
				case 0: return systemtime;
				case 1: return vctimer;
				case 3: return lastpressed;
				case 4: return mouse_x;
				case 5: return mouse_y;
				case 6: return mouse_l;
				case 7: return mouse_r;
				case 8: return mouse_m;
				case 9: return (int) mwheel;
				case 10: return vc_GetYear();
				case 11: return vc_GetMonth();
				case 12: return vc_GetDay();
				case 13: return vc_GetDayOfWeek();
				case 14: return vc_GetHour();
				case 15: return vc_GetMinute();
				case 16: return vc_GetSecond();
				case 17: return cur_stick;
				case 18: return sticks[cur_stick].active;
				case 19: return sticks[cur_stick].up;
				case 20: return sticks[cur_stick].down;
				case 21: return sticks[cur_stick].left;
				case 22: return sticks[cur_stick].right;
				case 23: return sticks[cur_stick].analog_x;
				case 24: return sticks[cur_stick].analog_y;
				case 26: return up;
				case 27: return down;
				case 28: return left;
				case 29: return right;
				case 30: return b1;
				case 31: return b2;
				case 32: return b3;
				case 33: return b4;
				case 34: return event_tx;
				case 35: return event_ty;
				case 36: return event_zone;
				case 37: return event_entity;
				case 38: return event_param;
				case 39: return xwin;
				case 40: return ywin;
				case 41: return cameratracking;
				case 42: return entities;
				case 51: return transColor;
				case 59: return gameWindow->getHandle();
				case 60: return lastkey;
				case 80: return current_map ? current_map->mapwidth : 0;
				case 81: return current_map ? current_map->mapheight : 0;
				case 82: return current_map ? current_map->startx : 0;
				case 83: return current_map ? current_map->starty : 0;
				case 87: return current_map ? 2 : 0;
				case 90: return event_sprite; // Overkill (2006-07-28): No more HVAR error.
				case 96: return cameratracker;
				case 101: return playerstep;
				case 102: return playerdiagonals;
				case 103: return AppIsForeground;
				case 116: return ReadHvar_derived(category,loc,ofs);
				case 126: return event_entity_hit;

				default: vcerr("Unknown HVAR0 (%d)", loc);
			}
			return -1;
		case intHVAR1:
			switch (loc)
			{
				case 2: if(ofs<0 || ofs>255) vcerr("Invalid key number: %d ; 0..255 inclusive are valid"); else return keys[ofs];
				case 25: if(ofs<0 || ofs>31) vcerr("Invalid button number: 0..31 inclusive are valid."); else return sticks[cur_stick].button[ofs];
				case 43: if (ofs>=0 && ofs<entities) return entity[ofs]->getx(); return 0;
				case 44: if (ofs>=0 && ofs<entities) return entity[ofs]->gety(); return 0;
				case 45: if (ofs>=0 && ofs<entities) return entity[ofs]->specframe; return 0;
				case 46: if (ofs>=0 && ofs<entities) return entity[ofs]->frame; return 0;
				case 47: if (ofs>=0 && ofs<entities) return entity[ofs]->chr->hx; return 0;
				case 48: if (ofs>=0 && ofs<entities) return entity[ofs]->chr->hy; return 0;
				case 49: if (ofs>=0 && ofs<entities) return entity[ofs]->chr->hw; return 0;
				case 50: if (ofs>=0 && ofs<entities) return entity[ofs]->chr->hh; return 0;
				#ifndef NOTIMELESS
				case 52: return skewlines[ofs];
				#endif
				case 53: return (int) (*(byte *)ofs);
				case 54: return (int) (*(word *)ofs);
				case 55: return (int) (*(quad *)ofs);
				case 56: return (int) (*(char *)ofs);
				case 57: return (int) (*(short*)ofs);
				case 58: return (int) (*(int  *)ofs);
				case 59: vcerr("Cannot write to gameWindow!");
				case 61: if (ofs>=0 && ofs<entities) return entity[ofs]->movecode; return 0;
				case 62: if (ofs>=0 && ofs<entities) return entity[ofs]->face; return 0;
				case 63: if (ofs>=0 && ofs<entities) return entity[ofs]->speed; return 0;
				case 64: if (ofs>=0 && ofs<entities) return entity[ofs]->visible; return 0;
				case 66: if (ofs>=0 && ofs<sprites.size()) return sprites[ofs].x; return 0;
				case 67: if (ofs>=0 && ofs<sprites.size()) return sprites[ofs].y; return 0;
				case 68: if (ofs>=0 && ofs<sprites.size()) return sprites[ofs].sc; return 0;
				case 69: if (ofs>=0 && ofs<sprites.size()) return sprites[ofs].image; return 0;
				case 70: if (ofs>=0 && ofs<sprites.size()) return sprites[ofs].lucent; return 0;
				case 71: if (ofs>=0 && ofs<sprites.size()) return sprites[ofs].addsub; return 0;
				case 72: if (ofs>=0 && ofs<sprites.size()) return sprites[ofs].alphamap; return 0;
				case 73: if (ofs>=0 && ofs<sprites.size()) return sprites[ofs].thinkrate; return 0; // Overkill (2006-07-28)
				case 75: if (ofs>=0 && ofs<sprites.size()) return sprites[ofs].xflip; return 0;
				case 76: if (ofs>=0 && ofs<sprites.size()) return sprites[ofs].yflip; return 0;
				case 78: if (ofs>=0 && ofs<entities) return entity[ofs]->obstruction; return 0;
				case 79: if (ofs>=0 && ofs<entities) return entity[ofs]->obstructable; return 0;
				case 94: if (current_map && ofs>=0 && ofs<current_map->numlayers) return current_map->layers[ofs]->lucent;
                case 97: if (current_map && ofs>=0 && ofs<current_map->numlayers) return (int)(current_map->layers[ofs]->parallax_x * 65536);
                case 98: if (current_map && ofs>=0 && ofs<current_map->numlayers) return (int)(current_map->layers[ofs]->parallax_y * 65536);
				case 105: if (ofs>=0 && ofs<sprites.size()) return sprites[ofs].ent; return 0; // Overkill (2006-07-28)
				case 106: if (ofs>=0 && ofs<sprites.size()) return sprites[ofs].silhouette; return 0; // Overkill (2006-07-28)
				case 107: if (ofs>=0 && ofs<sprites.size()) return sprites[ofs].color; return 0; // Overkill (2006-07-28)
				case 108: if (ofs>=0 && ofs<sprites.size()) return sprites[ofs].wait; return 0; // Overkill (2006-07-28)
				case 109: if (ofs>=0 && ofs<sprites.size()) return sprites[ofs].onmap; return 0; // Overkill (2006-07-28)
				case 110: if (ofs>=0 && ofs<sprites.size()) return sprites[ofs].layer; return 0; // Overkill (2006-07-28)
				case 111: if (ofs>=0 && ofs<entities) return entity[ofs]->lucent; return 0; // Overkill (2006-07-28)
				case 112: if (ofs>=0 && ofs<sprites.size()) return sprites[ofs].timer; return 0; // Overkill (2006-07-28)
				case 113: return ScriptEngine::Get_EntityFrameW(ofs); // Overkill (2006-07-28)
				case 114: return ScriptEngine::Get_EntityFrameH(ofs); // Overkill (2006-07-28)
				default:
					return ReadHvar_derived(category,loc,ofs);
			
			}
			return -1;
		
		default:
			vcerr("Fatal Error Code Earache"); 
			return -1;
	}
}

int ScriptEngine::ReadHvar_derived(int category, int loc, int ofs)
{
	switch(category)
	{
		case intHVAR0:
			vcerr("Unknown HVAR0 (%d)", loc); break;
		case intHVAR1:
			vcerr("Unknown HVAR1 (%d, %d)", loc, ofs); break;
		default:
			vcerr("Fatal Error Code Mongoose"); break;
	}
	return -1;
}

void ScriptEngine::WriteHvar_derived(int category, int loc, int ofs, int value)
{
	switch(category)
	{
		case intHVAR0:
			vcerr("Unknown HVAR0 (%d)", loc); break;
		case intHVAR1:
			vcerr("Unknown HVAR1 (%d, %d)", loc, ofs); break;
		default:
			vcerr("Fatal Error Code Sapience"); break;
	}
}

void ScriptEngine::Error(const char *str, ...) { 
  	va_list argptr;
	char msg[256];

	va_start(argptr, str);
	vsprintf(msg, str, argptr);
	va_end(argptr);
	DisplayError(msg);
}

void ScriptEngine::ArgumentPassAddInt(int value)
{
	argument_t arg;
	arg.type_id = t_INT;
	arg.int_value = value;
	arg.string_value = empty_string;
	argument_pass_list.push_back(arg);
}

void ScriptEngine::ArgumentPassAddString(StringRef value)
{
	argument_t arg;
	arg.type_id = t_STRING;
	arg.int_value = 0;
	arg.string_value = value;
	argument_pass_list.push_back(arg);
}

void ScriptEngine::ArgumentPassClear()
{
	argument_pass_list.clear();
}

void EnforceNoDirectories(StringRef s)
{
	int	n = 0;
	if (!s.length()) return;

    if (s[0]=='/' || s[0]=='\\')
		se->Error("vc does not allow accessing dir: %s", s.c_str());

    if (s[1]==':')
		se->Error("vc does not allow accessing dir: %s", s.c_str());

    n=0;
    while (n<s.length()-1)
    {
		if (s[n]=='.' && s[n+1]=='.')
			se->Error("vc does not allow accessing dir: %s", s.c_str());
		n++;
    }
}


void HookTimer()
{
	if (!timerfunc.empty())
	{
		while (hooktimer)
		{
			se->ExecuteFunctionString(timerfunc);
			hooktimer--;
		}
	}
}

void HookRetrace()
{
	if (!renderfunc.empty())
		se->ExecuteFunctionString(renderfunc);
}

image *ImageForHandle(int handle)
{
	if (handle == 0)
		se->Error("ImageForHandle() - Null image reference (0), probably an uninitialized image handle");

	if (!Handle::isValid(HANDLE_TYPE_IMAGE, handle) )
		se->Error("ImageForHandle() - Image reference is bogus! (%d)", handle);

	image* ptr = (image*) Handle::getPointer(HANDLE_TYPE_IMAGE,handle);

	if (ptr == NULL)
		se->Error("ImageForHandle() - Image reference (%d) is valid but no image is allocated for this handle. You may have mistakenly freed it and continued to use it.", handle);
	return ptr;
}


void FreeImageHandle(int handle)
{
	Handle::free(HANDLE_TYPE_IMAGE,handle);
}

void SetHandleImage(int handle, image *img)
{
	Handle::setPointer(HANDLE_TYPE_IMAGE, handle, (void*)img);
}

int HandleForImage(image *img)
{
	return Handle::alloc(HANDLE_TYPE_IMAGE, img);
}

//-------------

void ScriptEngine::Exit(CStringRef message) { err("%s",message.c_str()); }

void ScriptEngine::SetButtonJB(int b, int jb) {
	switch (b)
	{
		case 1: j_b1 = jb; break;
		case 2: j_b2 = jb; break;
		case 3: j_b3 = jb; break;
		case 4: j_b4 = jb; break;
	}
}

// Overkill (2007-08-25): HookButton is supposed to start at 1, not 0.
// It's meant to be consistent with Unpress().
void ScriptEngine::HookButton(int b, CStringRef s) {
	if (b<1 || b>4) return;
	bindbutton[b-1] = s;
}

void ScriptEngine::HookKey(int k, CStringRef s) {
	if (k<0 || k>127) return;
	bindarray[k] = s;
}

void ScriptEngine::HookTimer(CStringRef s) {
	hooktimer = 0;
	timerfunc = s;
}

void ScriptEngine::HookRetrace(CStringRef s) {
	renderfunc = s;
}

void ScriptEngine::Log(CStringRef s) { log(s.c_str()); }
void ScriptEngine::MessageBox(CStringRef msg) { showMessageBox(msg); }
int ScriptEngine::Random(int min, int max) { return rnd(min, max); }
void ScriptEngine::SetAppName(CStringRef s) { setWindowTitle(s.c_str()); }

void ScriptEngine::SetButtonKey(int b, int k) {
	switch (b)
	{
		case 1: k_b1 = k; break;
		case 2: k_b2 = k; break;
		case 3: k_b3 = k; break;
		case 4: k_b4 = k; break;
		// Overkill (2006-06-25): Can set the directionals as well, now.
		case 5: k_up = k; break;
		case 6: k_down = k; break;
		case 7: k_left = k; break;
		case 8: k_right = k; break;
	}
}

void ScriptEngine::SetRandSeed(int seed) { arandseed(seed); }
void ScriptEngine::SetResolution(int v3_xres, int v3_yres) { vid_SetMode(v3_xres, v3_yres, vid_bpp, vid_window, MODE_SOFTWARE); }

void ScriptEngine::Unpress(int n) {
	switch (n)
	{
		case 0: 
			if (b1) UnB1(); 
			if (b2) UnB2(); 
			if (b3) UnB3(); 
			if (b4) UnB4(); 	
			break;
		case 1: if (b1) UnB1(); break;
		case 2: if (b2) UnB2(); break;
		case 3: if (b3) UnB3(); break;
		case 4: if (b4) UnB4(); break;
		case 5: if (up) UnUp(); break;
		case 6: if (down) UnDown(); break;
		case 7: if (left) UnLeft(); break;
		case 8: if (right) UnRight(); break;
		case 9: 
			if (b1) UnB1(); 
			if (b2) UnB2(); 
			if (b3) UnB3(); 
			if (b4) UnB4();
			if (up) UnUp();
			if (down) UnDown();
			if (left) UnLeft();
			if (right) UnRight();
			break;
	}
}

void ScriptEngine::UpdateControls() { ::UpdateControls(); }


int ScriptEngine::Asc(CStringRef s) { if(s.length() == 0) return 0; else return (int)s[0]; }
StringRef ScriptEngine::Chr(int c) { 
	char buf[2] = {(char)c,0};
	return buf;
}
StringRef ScriptEngine::GetToken(CStringRef s, CStringRef d, int i) {
	int n = 0;
	int tokenindex = 0;
	while (n < s.length())
	{
		while (n < s.length() && isdelim(s[n], d))
			n++;
		int len = 0, start = n;
		while (n < s.length() && !isdelim(s[n], d))
		{
			len++;
			n++;
		}
		if (i == tokenindex)
			return s.substr(start,len);
		tokenindex++;
	}

	return empty_string;
}
StringRef ScriptEngine::Left(CStringRef str, int len) { return vc_strleft(str,len); }
int ScriptEngine::Len(CStringRef s) { return s.length(); }
StringRef ScriptEngine::Mid(CStringRef str, int pos, int len) { return vc_strmid(str,pos,len); }
StringRef ScriptEngine::Right(CStringRef str, int len) { return vc_strright(str,len); }
StringRef ScriptEngine::Str(int d) { return va("%d", d); }
int ScriptEngine::Strcmp(CStringRef s1, CStringRef s2) { return strcmp(s1.c_str(), s2.c_str()); }
StringRef ScriptEngine::Strdup(CStringRef s, int times) {
	std::string ret;
	int slen = s.length();
	ret.reserve(s.size()*times);
	for (int i=0; i<times; i++)
		memcpy(&ret[slen*i],&s[0],slen);
	return ret;
}
int ScriptEngine::TokenCount(CStringRef s, CStringRef d) {
	int n = 0;
	int tokenindex = 0;
	while (n < s.length())
	{
		bool tp = false;

		while (n < s.length() && isdelim(s[n], d))
			n++;
		while (n < s.length() && !isdelim(s[n], d))
		{
			tp = true;
			n++;
		}
		if (tp) tokenindex++;
	}
	return tokenindex;
}
StringRef ScriptEngine::ToLower(CStringRef str) { 
	std::string temp = str.str();
	to_lower(temp); 
	return temp;
}

StringRef ScriptEngine::ToUpper(CStringRef str)
{
	StringRef temp = str.str();
	to_upper(temp.dangerous_peek()); 
	return temp;
}

int ScriptEngine::Val(CStringRef s) { return atoi(s.c_str()); }


//VI.d. Map Functions
int ScriptEngine::GetObs(int x, int y) { if(!current_map) return 0; else return current_map->obstruct(x, y); }
int ScriptEngine::GetObsPixel(int x, int y) { if(!current_map) return 0; else return current_map->obstructpixel(x, y); }
int ScriptEngine::GetTile(int x, int y, int i) { if(!current_map) return 0; if(i>=current_map->numlayers) return 0; return current_map->layers[i]->GetTile(x,y); }
int ScriptEngine::GetZone(int x, int y) { if(!current_map) return 0; else return current_map->zone(x,y); }
void ScriptEngine::Map(CStringRef map) {
	strcpy(mapname, map.c_str());
	die = 1;
	done = 1;
}
void ScriptEngine::Render() {
	TimedProcessEntities(); 
	::Render();
}
void ScriptEngine::RenderMap(int x, int y, int d) {
	if (!current_map) return;
	image *dest = ImageForHandle(d);
	current_map->render(x, y, dest);
}
void ScriptEngine::SetObs(int x, int y, int c) { if(!current_map) return; else current_map->SetObs(x,y,c); }
void ScriptEngine::SetTile(int x, int y, int i, int z) { if(!current_map) return; else if(i>=current_map->numlayers) return; else current_map->layers[i]->SetTile(x,y,z); }
void ScriptEngine::SetZone(int x, int y, int z) { if(!current_map) return; else if(z>=current_map->numzones) return; else current_map->SetZone(x,y,z); }

//VI.e. Entity Functions
void ScriptEngine::ChangeCHR(int e, CStringRef c) {
	if (e<0 || e >= entities) return;
	else entity[e]->set_chr(c);
}
void ScriptEngine::EntityMove(int e, CStringRef s) {
	if (e<0 || e >= entities) return;
	else entity[e]->SetMoveScript(s.c_str());
}
void ScriptEngine::EntitySetWanderDelay(int e, int d) {
	if (e<0 || e >= entities) return;
	else entity[e]->SetWanderDelay(d);
}
void ScriptEngine::EntitySetWanderRect(int e, int x1, int y1, int x2, int y2) {
	if (e<0 || e >= entities) return;
	else entity[e]->SetWanderBox(x1, y1, x2, y2);
}
void ScriptEngine::EntitySetWanderZone(int e) {
	if (e<0 || e >= entities) return;
	else entity[e]->SetWanderZone();
}
int ScriptEngine::EntitySpawn(int x, int y, CStringRef s) { return AllocateEntity(x*16,y*16,s.c_str()); }
void ScriptEngine::EntityStalk(int stalker, int stalkee) {
	if (stalker<0 || stalker>=entities)
		return;
	if (stalkee<0 || stalkee>=entities)
	{
		entity[stalker]->clear_stalk();
		return;
	}
	entity[stalker]->stalk(entity[stalkee]);
}
void ScriptEngine::EntityStop(int e) {
	if (e<0 || e >= entities) return;
	else entity[e]->SetMotionless();
}
void ScriptEngine::HookEntityRender(int i, CStringRef s) {
	if (i<0 || i>=entities) err("vc_HookEntityRender() - no such entity %d", i);
	entity[i]->hookrender = s;
}
void ScriptEngine::PlayerMove(CStringRef s) {
	if (!myself) return;
	myself->SetMoveScript( s.c_str() );

	while( myself->movecode )
	{
		::TimedProcessEntities();
		::Render();
		::ShowPage();
	}

	PlayerEntityMoveCleanup();
}

void ScriptEngine::PlayerEntityMoveCleanup() {
	if (!myself) return;

	myself->movecode = 0;
	afterPlayerMove();
}

void ScriptEngine::SetEntitiesPaused(int i) {
	entitiespaused = i ? true : false;
	if (!entitiespaused)
		lastentitythink = systemtime;
}
void ScriptEngine::SetPlayer(int e) {
	if (e<0 || e>=entities)
	{
		player = -1;
		myself = 0;
		return;
	}
	myself = entity[e];
	player = e;
	myself->SetMotionless();
	myself->obstructable = true;
}

int ScriptEngine::GetPlayer()
{
	return player;
}

//VI.f. Graphics Functions
void ScriptEngine::AdditiveBlit(int x, int y, int src, int dst) {
	image *s = ImageForHandle(src);
	image *d = ImageForHandle(dst);
	::AdditiveBlit(x, y, s, d);
}
void ScriptEngine::AlphaBlit(int x, int y, int src, int alpha, int dst) {
	image *s = ImageForHandle(src);
	image *a = ImageForHandle(alpha);
	image *d = ImageForHandle(dst);
	::AlphaBlit(x, y, s, a, d);
}
void ScriptEngine::Blit(int x, int y, int src, int dst) {
	image *s = ImageForHandle(src);
	image *d = ImageForHandle(dst);
	::Blit(x, y, s, d);
}
void ScriptEngine::BlitEntityFrame(int x, int y, int e, int f, int dst) {
	image *d = ImageForHandle(dst);
	if (!current_map || e<0 || e >= entities) return;
	entity[e]->chr->render(x, y, f, d);
}

// Overkill (2007-08-25): src and dest were backwards. Whoops!
void ScriptEngine::BlitLucent(int x, int y, int lucent, int src, int dst) {
	image *s = ImageForHandle(src);
	image *d = ImageForHandle(dst);
	int oldalpha = alpha;
	::SetLucent(lucent);
	::Blit(x, y, s, d);
	::SetLucent(oldalpha);
}
void ScriptEngine::BlitTile(int x, int y, int t, int dst) {
	image *d = ImageForHandle(dst);
	if (current_map) {
		current_map->tileset->UpdateAnimations();
		current_map->tileset->Blit(x, y, t, d);
	}
}
void ScriptEngine::BlitWrap(int x, int y, int src, int dst) {
	image *s = ImageForHandle(src);
	image *d = ImageForHandle(dst);
	::BlitWrap(x, y, s, d);
}
void ScriptEngine::Circle(int x1, int y1, int xr, int yr, int c, int dst) {
	image *d = ImageForHandle(dst);
	::Circle(x1, y1, xr, yr, c, d);
}
void ScriptEngine::CircleFill(int x1, int y1, int xr, int yr, int c, int dst) {
	image *d = ImageForHandle(dst);
	::Sphere(x1, y1, xr, yr, c, d);
}
void ScriptEngine::ColorFilter(int filter, int dst) {
	image *d = ImageForHandle(dst);
	::ColorFilter(filter, d);
}
void ScriptEngine::CopyImageToClipboard(int s) {
	image *src = ImageForHandle(s);
	::clipboard_putImage(src);
}
int ScriptEngine::DuplicateImage(int s) {
	image *src = ImageForHandle(s);
	image *img = new image(src->width, src->height);
	::Blit(0, 0, src, img);
	return HandleForImage(img);
}
void ScriptEngine::FlipBlit(int x, int y, bool fx, bool fy, int src, int dst) {
	image *s = ImageForHandle(src);
	image *d = ImageForHandle(dst);
	::FlipBlit(x, y, fx, fy, s, d);
}
void ScriptEngine::FreeImage(int handle) {
	// Overkill (2006-07-28):
	// Fixed the bug where FreeImage() didn't do what its name implied.
	if (handle == 0)
	{
 		se->Error("vc_FreeImage() - cannot free a null image reference!");
	}
	if (handle == 1)
	{
 		se->Error("vc_FreeImage() - cannot free the screen reference");
	}

	if( !ScriptEngine::ImageValid(handle) )
	{
 		se->Error("vc_FreeImage() - cannot free an invalid image reference (try using ImageValid() first?)");		
	}

	delete ImageForHandle(handle);
	FreeImageHandle(handle);
}
int ScriptEngine::GetB(int c) {
	int r, g, b;
	::GetColor(c, r, g, b);
	return b;
}
int ScriptEngine::GetG(int c) {
	int r, g, b;
	::GetColor(c, r, g, b);
	return g;
}
int ScriptEngine::GetImageFromClipboard() {
	image *t = clipboard_getImage();
	if (!t) return 0;
	else return HandleForImage(t);
}
int ScriptEngine::GetPixel(int x, int y, int src) {
	image *s = ImageForHandle(src);
	return ::ReadPixel(x, y, s);
}
int ScriptEngine::GetR(int c) {
	int r, g, b;
	::GetColor(c, r, g, b);
	return r;
}
void ScriptEngine::GrabRegion(int sx1, int sy1, int sx2, int sy2, int dx, int dy, int src, int dst) {
	image *s = ImageForHandle(src);
	image *d = ImageForHandle(dst);

	int dcx1, dcy1, dcx2, dcy2;
	d->GetClip(dcx1, dcy1, dcx2, dcy2);

	if (sx1>sx2) SWAP(sx1, sx2);
	if (sy1>sy2) SWAP(sy1, sy2);
	int grabwidth = sx2 - sx1;
	int grabheight = sy2 - sy1;
	if (dx+grabwidth<0 || dy+grabheight<0) return;
	d->SetClip(dx, dy, dx+grabwidth, dy+grabheight);
	::Blit(dx-sx1, dy-sy1, s, d);

	d->SetClip(dcx1, dcy1, dcx2, dcy2);
}
int ScriptEngine::ImageHeight(int src) { return ((image*)ImageForHandle(src))->height; }
int ScriptEngine::ImageShell(int x, int y, int w, int h, int src) {
	image *s = ImageForHandle(src);
	if (w+x > s->width || y+h > s->height)
		err(
			"ImageShell() - Bad arguements. x/y+w/h greater than original image dimensions\n\nx:%d,w:%d (%d),y:%d,h:%d (%d), orig_x:%d, orig_y:%d",
			x,w,x+w,y,h,y+h,s->width,s->height
		);

	image *d = new image(w, h);
	d->delete_data();
	d->shell = true;

	d->data = ((quad *)s->data + (y*s->pitch)+x);
	d->pitch = s->pitch;
	
	return HandleForImage(d);
}
int ScriptEngine::ImageValid(int handle) {
	if (handle <= 0 || handle >= Handle::getHandleCount(HANDLE_TYPE_IMAGE) || (Handle::getPointer(HANDLE_TYPE_IMAGE,handle) == NULL) )
		return 0;
	else return 1;
}
int ScriptEngine::ImageWidth(int src) { return ((image*)ImageForHandle(src))->width; }
void ScriptEngine::Line(int x1, int y1, int x2, int y2, int c, int dst) {
	image *d = ImageForHandle(dst);
	::Line(x1, y1, x2, y2, c, d);
}
int ScriptEngine::LoadImage(CStringRef fn) { return HandleForImage(::xLoadImage(fn.c_str())); }
int ScriptEngine::LoadImage0(CStringRef fn) { return HandleForImage(::xLoadImage0(fn.c_str())); }
int ScriptEngine::LoadImage8(CStringRef fn) { return HandleForImage(::xLoadImage8(fn.c_str())); }
int ScriptEngine::MakeColor(int r, int g, int b) { return ::MakeColor(r,g,b); }
int ScriptEngine::MixColor(int c1, int c2, int p) {
	if (p>255) p=255;
	if (p<0) p=0;

	int r1, g1, b1;
	int r2, g2, b2;
	::GetColor(c1, r1, g1, b1);
	::GetColor(c2, r2, g2, b2);

	return ::MakeColor((r1*(255-p)/255)+(r2*p/255), (g1*(255-p)/255)+(g2*p/255), (b1*(255-p)/255)+(b2*p/255));
}
void ScriptEngine::Mosaic(int xgran, int ygran, int dst) {
	image *dest = ImageForHandle(dst);
	::Mosaic(xgran, ygran, dest);
}
int ScriptEngine::NewImage(int xsize, int ysize) {
	image *n = new image(xsize, ysize);
	return HandleForImage(n);
}
void ScriptEngine::Rect(int x1, int y1, int x2, int y2, int c, int dst) {
	image *d = ImageForHandle(dst);
	::Box(x1, y1, x2, y2, c, d);
}
void ScriptEngine::RectFill(int x1, int y1, int x2, int y2, int c, int dst) {
	image *d = ImageForHandle(dst);
	::DrawRect(x1, y1, x2, y2, c, d);
}
void ScriptEngine::RotScale(int x, int y, int angle, int scale, int src, int dst) {
	image *s = ImageForHandle(src);
	image *d = ImageForHandle(dst);
	::RotScale(x, y,  angle*(float)3.14159/(float)180.0, scale/(float)1000.0, s, d);
}
void ScriptEngine::ScaleBlit(int x, int y, int dw, int dh, int src, int dst) {
	image *s = ImageForHandle(src);
	image *d = ImageForHandle(dst);
	::ScaleBlit(x, y, dw, dh, s, d);
}
void ScriptEngine::SetClip(int x1, int y1, int x2, int y2, int img) {
	image *i = ImageForHandle(img);
	i->SetClip(x1, y1, x2, y2);
}
void ScriptEngine::SetCustomColorFilter(int c1, int c2) {
	GetColor(c1, cf_r1, cf_g1, cf_b1);
	GetColor(c2, cf_r2, cf_g2, cf_b2);
	cf_rr = cf_r2 - cf_r1;
	cf_gr = cf_g2 - cf_g1;
	cf_br = cf_b2 - cf_b1;
}
void ScriptEngine::SetLucent(int p) { ::SetLucent(p); }
void ScriptEngine::SetPixel(int x, int y, int c, int dst) {
	image *d = ImageForHandle(dst);
	::PutPixel(x, y, c, d);
}
void ScriptEngine::ShowPage() {
	::UpdateControls();
	::ShowPage();
}
void ScriptEngine::Silhouette(int x, int y, int c, int src, int dst) {
	image *s = ImageForHandle(src);
	image *d = ImageForHandle(dst);
	::Silhouette(x, y, c, s, d);
}
void ScriptEngine::SubtractiveBlit(int x, int y, int src, int dst) {
	image *s = ImageForHandle(src);
	image *d = ImageForHandle(dst);
	::SubtractiveBlit(x, y, s, d);
}
void ScriptEngine::SuperSecretThingy(int xskew, int yofs, int y, int src, int dst) {
	image *s = ImageForHandle(src);
	image *d = ImageForHandle(dst);

	if (s->width != 256 || s->height != 256)
		err("SuperSecretThingy() - Source image MUST be 256x256!!");

	::Timeless(xskew, yofs, y, s, d);
}
void ScriptEngine::TAdditiveBlit(int x, int y, int src ,int dst) {
	image *s = ImageForHandle(src);
	image *d = ImageForHandle(dst);
	::TAdditiveBlit(x, y, s, d);
}
void ScriptEngine::TBlit(int x, int y, int src, int dst) {
	image *s = ImageForHandle(src);
	image *d = ImageForHandle(dst);
	::TBlit(x, y, s, d);
}
// Overkill (2007-08-25): src and dest were backwards. Whoops!
void ScriptEngine::TBlitLucent(int x, int y, int lucent, int src, int dst) {
	image *s = ImageForHandle(src);
	image *d = ImageForHandle(dst);
	int oldalpha = alpha;
	::SetLucent(lucent);
	::TBlit(x, y, s, d);
	::SetLucent(oldalpha);
}
void ScriptEngine::TBlitTile(int x, int y, int t, int dst) {
	image *d = ImageForHandle(dst);
	if (current_map) current_map->tileset->TBlit(x, y, t, d);
}
void ScriptEngine::TGrabRegion(int sx1, int sy1, int sx2, int sy2, int dx, int dy, int src, int dst) {
	image *s = ImageForHandle(src);
	image *d = ImageForHandle(dst);

	int dcx1, dcy1, dcx2, dcy2;
	d->GetClip(dcx1, dcy1, dcx2, dcy2);

	if (sx1>sx2) SWAP(sx1, sx2);
	if (sy1>sy2) SWAP(sy1, sy2);
	int grabwidth = sx2 - sx1;
	int grabheight = sy2 - sy1;
	if (dx+grabwidth<0 || dy+grabheight<0) return;
	d->SetClip(dx, dy, dx+grabwidth, dy+grabheight);
	::TBlit(dx-sx1, dy-sy1, s, d);

	d->SetClip(dcx1, dcy1, dcx2, dcy2);
}
void ScriptEngine::Triangle(int x1, int y1, int x2, int y2, int x3, int y3, int c, int dst) {
	image *dest = ImageForHandle(dst);
	::Triangle(x1, y1, x2, y2, x3, y3, c, dest);
}
void ScriptEngine::TScaleBlit(int x, int y, int dw, int dh, int src, int dst) {
	image *s = ImageForHandle(src);
	image *d = ImageForHandle(dst);
	::TScaleBlit(x, y, dw, dh, s, d);
}
void ScriptEngine::TSubtractiveBlit(int x, int y, int src, int dst) {
	image *s = ImageForHandle(src);
	image *d = ImageForHandle(dst);
	::TSubtractiveBlit(x, y, s, d);
}
void ScriptEngine::TWrapBlit(int x, int y, int src, int dst) {
	image *s = ImageForHandle(src);
	image *d = ImageForHandle(dst);
	::TWrapBlit(x, y, s, d);
}
void ScriptEngine::WrapBlit(int x, int y, int src, int dst) {
	image *s = ImageForHandle(src);
	image *d = ImageForHandle(dst);
	::WrapBlit(x, y, s, d);
}

//VI.g. Sprite Functions
int ScriptEngine::GetSprite() { return ::GetSprite(); }
void ScriptEngine::ResetSprites() { return ::ResetSprites(); }

//VI.h. Sound/Music Functions
void ScriptEngine::FreeSong(int handle) { ::FreeSong(handle); }
void ScriptEngine::FreeSound(int slot) { ::FreeSample((void*)slot); }
int ScriptEngine::GetSongPos(int handle) { return ::GetSongPos(handle); }
int ScriptEngine::GetSongVolume(int handle) { return ::GetSongVol(handle); }
int ScriptEngine::LoadSong(CStringRef fn) { return ::LoadSong(fn.c_str()); }
int ScriptEngine::LoadSound(CStringRef fn) { return (int)LoadSample(fn.c_str()); }
void ScriptEngine::PlayMusic(CStringRef fn) { ::PlayMusic(fn.c_str()); }
void ScriptEngine::PlaySong(int handle) { ::PlaySong(handle); }
int ScriptEngine::PlaySound(int slot, int volume) { return ::PlaySample((void*) slot, volume * 255 / 100); }
void ScriptEngine::SetMusicVolume(int v) { ::SetMusicVolume(v); }
void ScriptEngine::SetSongPaused(int h, int p) { ::SetPaused(h,p); }
void ScriptEngine::SetSongPos(int h, int p) { ::SetSongPos(h,p); }
void ScriptEngine::SetSongVolume(int h, int v) { ::SetSongVol(h,v); }
void ScriptEngine::StopMusic() { ::StopMusic(); }
void ScriptEngine::StopSong(int handle) { ::StopSong(handle); }
void ScriptEngine::StopSound(int chan) { ::StopSound(chan); }

//VI.i. Font Functions
void ScriptEngine::EnableVariableWidth(int fh) {
	Font *font = (Font*)fh;
	font->EnableVariableWidth();
}
int ScriptEngine::FontHeight(int f) {
	if (!f) return 7;
	else return ((Font *) f)->height;
}
void ScriptEngine::FreeFont(int f) {
	Font *font = (Font*) f;
	if (font) delete font;
}
int ScriptEngine::LoadFont(CStringRef filename, int width, int height) {
	return (int) new Font(filename.c_str(), width, height);
}
int ScriptEngine::LoadFontEx(CStringRef filename) { return (int) new Font(filename.c_str()); }
//helper:
static void print(int x, int y, image *dest, Font *font, CStringRef text, int which) {
	switch(which) {
		case 0:	
			if (font == 0) {
				::GotoXY(x, y);
				::PrintString(text.c_str(), dest);
			} else font->PrintString("%s", x, y, dest, text.c_str());
			break;
		case 1:	
			if (font == 0) ::PrintCenter(x, y, text.c_str(), dest);
			else font->PrintCenter("%s", x, y, dest, text.c_str());
			break;
		case 2:	
			if (font == 0) ::PrintRight(x, y, text.c_str(), dest);
			else font->PrintRight("%s", x, y, dest, text.c_str());
			break;
	}
}
void ScriptEngine::PrintCenter(int x, int y, int d, int fh, CStringRef text) { print(x,y,ImageForHandle(d),(Font*)fh,text,1); }
void ScriptEngine::PrintRight(int x, int y, int d, int fh, CStringRef text) { print(x,y,ImageForHandle(d),(Font*)fh,text,2); }
void ScriptEngine::PrintString(int x, int y, int d, int fh, CStringRef text) { print(x,y,ImageForHandle(d),(Font*)fh,text,0); }
int ScriptEngine::TextWidth(int fh, CStringRef text) {
	Font *font = (Font*)fh;
	if (font == 0) return ::pixels(text.c_str());
	else return font->Pixels(text.c_str());
}

//VI.j. Math Functions
//helper:
static int mydtoi(double d) { return (int)floor(d + 0.5); }
int ScriptEngine::acos(int val) {
	double dv = (double) val / 65535;
	double ac = ::acos(dv);
	ac = ac * 180 / 3.14159265358979; // convert radians to degrees
	return ::mydtoi(ac);
}
int ScriptEngine::facos(int val) {
	double dv = (double) val / 65535;
	double ac = ::acos(dv);
	ac *= 65536; // Convert to 16.16 fixed point
	return mydtoi(ac);
}
int ScriptEngine::asin(int val) {
	double dv = (double) val / 65535;
	double as = ::asin(dv);
	as = as * 180 / 3.14159265358979; // convert radians to degrees
	return mydtoi(as);
}
int ScriptEngine::fasin(int val) {
	double dv = (double) val / 65535;
	double as = ::asin(dv);
	as *= 65536; // Convert to 16.16 fixed point
	return mydtoi(as);
}
int ScriptEngine::atan(int val) {
	double dv = (double) val / 65535;
	double at = ::atan(dv);
	at = at * 180 / 3.14159265358979; // convert radians to degrees
	return mydtoi(at);
}
int ScriptEngine::fatan(int val) {
	double dv = (double) val / 65535;
	double at = ::atan(dv);
	at *= 65536; // Convert to 16.16 fixed point
	return mydtoi(at);
}
int ScriptEngine::atan2(int y, int x) {
	float f = ::atan2((float)y,(float)x);
	return (int)(f/2.0/3.14159265358979*360.0);
}
int ScriptEngine::fatan2(int y, int x) {
	double theta = ::atan2((double) y, (double) x);
	return mydtoi(theta * 65536);
}
int ScriptEngine::sin(int n) {
    while (n < 0) n += 360;
    while (n >= 360) n -= 360;
	return sintbl[n];
}
int ScriptEngine::cos(int n) {
    while (n < 0) n += 360;
    while (n >= 360) n -= 360;
	return costbl[n];
}
int ScriptEngine::tan(int n) {
    while (n < 0) n += 360;
    while (n >= 360) n -= 360;
	return tantbl[n];
}
int ScriptEngine::fsin(int val) {
	double magnitude = ::sin((double) val / 65536);
	return mydtoi(magnitude * 65536);
}
int ScriptEngine::fcos(int val) {
	double magnitude = ::cos((double) val / 65536);
	return mydtoi(magnitude * 65536);
}
int ScriptEngine::ftan(int val) {
	double magnitude = ::tan((double) val / 65536);
	return mydtoi(magnitude * 65536);
}
int ScriptEngine::pow(int a, int b) {
	return (int) ::pow((double)a, (double)b);
}
int ScriptEngine::sqrt(int val) {
	return (int) (float) ::sqrt((float) val);
}

//VI.k. File Functions
void ScriptEngine::FileClose(int handle) {
	if (!handle) return;
	if (handle > VCFILES)
		se->Error("FileClose() - uhh, given file handle is not a valid file handle.");
	if (!vcfiles[handle].active)
		se->Error("FileClose() - given file handle is not open.");

	switch (vcfiles[handle].mode)
	{
		case VC_READ:
			vclose(vcfiles[handle].vfptr);
			vcfiles[handle].vfptr = 0;
			vcfiles[handle].mode = 0;
			vcfiles[handle].active = false;
			break;
		case VC_WRITE:
			fclose(vcfiles[handle].fptr);
			vcfiles[handle].fptr = 0;
			vcfiles[handle].mode = 0;
			vcfiles[handle].active = false;
			break;
		default:
			se->Error("FileClose() - uhhh. file mode is not valid?? you did something very bad!");
	}
}

int ScriptEngine::FileCurrentPos(int handle) {
	if (!handle || handle > VCFILES || !vcfiles[handle].active)
		se->Error("FileCurrentPos() - file handle is either invalid or file is not open.");

	switch (vcfiles[handle].mode)
	{
		case VC_READ:
			return vtell(vcfiles[handle].vfptr);
			break;
		case VC_WRITE:
			return ftell(vcfiles[handle].fptr);
			break;
		default:
			se->Error("FileCurentPos() - File mode not valid! That's bad!");
	}
	return 0;
}

bool ScriptEngine::FileEOF(int handle) {
	if (!handle) se->Error("FileEOF() - File is not open.");
	if (handle > VCFILES) se->Error("FileEOF() - given file handle is not a valid file handle.");
	if (!vcfiles[handle].active) se->Error("FileEOF() - given file handle is not open.");
	if (vcfiles[handle].mode != VC_READ) se->Error("FileEOF() - given file handle is a write-mode file.");

	return veof(vcfiles[handle].vfptr)!=0;
}

int ScriptEngine::FileOpen(CStringRef fname, int filemode) {
	int index;

	for (index=1; index<VCFILES; index++)
		if (!vcfiles[index].active)
			break;
	if (index == VCFILES)
		se->Error("FileOpen() - Out of file handles! \nTry closing files you're done with, or if you really need more, \nemail vecna@verge-rpg.com and pester me!");
	
	const char* cpfname = fname.c_str();

	#ifdef __APPLE__
	// swap backslashes in path for forward slashes
	// (windows -> unix/max)
	string converted = fname.str();
	boost::algorithm::replace_all(converted, "\\", "/");
	cpfname = converted.c_str();
	#endif

	switch (filemode)
	{
		case VC_READ:
			vcfiles[index].vfptr = vopen(cpfname);
			if (!vcfiles[index].vfptr)
			{
				//log("opening of %s for reading failed.", cpfname);
				return 0;
			}
			vcfiles[index].active = true;
			vcfiles[index].mode = VC_READ;
			break;
		case VC_WRITE:
		case VC_WRITE_APPEND: // Overkill (2006-07-05): Append mode added.
			if(filemode == VC_WRITE)
				vcfiles[index].fptr = fopen(cpfname, "wb");
			else vcfiles[index].fptr = fopen(cpfname, "ab");
			if (!vcfiles[index].fptr)
			{
				//::log("opening of %s for writing/appending failed.", cpfname);
				return 0;
			}
			vcfiles[index].active = true;
			vcfiles[index].mode = VC_WRITE;
			break;
		default:
			se->Error("FileOpen() - not a valid file mode!");
	}
	return index;
}

int ScriptEngine::FileReadByte(int handle) {
	if (!handle || handle > VCFILES || !vcfiles[handle].active)
		se->Error("FileReadByte() - file handle is either invalid or file is not open.");
	if (vcfiles[handle].mode != VC_READ)
		se->Error("FileReadByte() - given file handle is a write-mode file.");

	int ret=0;
	vread(&ret, 1, vcfiles[handle].vfptr);
	return ret;
}

StringRef ScriptEngine::FileReadln(int handle) {
	if (!handle) se->Error("FileReadln() - File is not open.");
	if (handle > VCFILES) se->Error("FileReadln() - given file handle is not a valid file handle.");
	if (!vcfiles[handle].active) se->Error("FileReadln() - given file handle is not open.");
	if (vcfiles[handle].mode != VC_READ) se->Error("FileReadln() - given file handle is a write-mode file.");


	char buffer[255]; 	// buffer for each read
	std::string result = ""; // all the text so far
	int eol = 0;        // flag for when we've hit the end of a line
	do {
		vgets(buffer, 255, vcfiles[handle].vfptr); // read it

		if(buffer[0] == '\0')  {
			eol = 1; // we didn't read anything, this is eof
		} else if(buffer[strlen(buffer)-1] == 10 || buffer[strlen(buffer)-1] == 13) {
			// last character is a EOL character, so it's the end of a line
			eol = 1;
		}

		strclean(buffer);
		result += buffer;
	} while(!eol);

	return result;
}
int ScriptEngine::FileReadQuad(int handle) {
	if (!handle || handle > VCFILES || !vcfiles[handle].active)
		se->Error("FileReadQuad() - file handle is either invalid or file is not open.");
	if (vcfiles[handle].mode != VC_READ)
		se->Error("FileReadQuad() - given file handle is a write-mode file.");

	int ret=0;
	vread(&ret, 4, vcfiles[handle].vfptr);
	return ret;
}
StringRef ScriptEngine::FileReadString(int handle){
	int len = 0;
	char *buffer;

	if (!handle || handle > VCFILES || !vcfiles[handle].active)
		se->Error("FileReadString() - file handle is either invalid or file is not open.");
	if (vcfiles[handle].mode != VC_READ)
		se->Error("FileReadString() - given file handle is a write-mode file.");

	vread(&len, 2, vcfiles[handle].vfptr);
	buffer = new char[len+1];
	vread(buffer, len, vcfiles[handle].vfptr);
	buffer[len]=0;
	StringRef ret = buffer;
	delete[] buffer;
	return ret;
}
StringRef ScriptEngine::FileReadToken(int handle) {
	if (!handle) se->Error("FileReadToken() - File is not open.");
	if (handle > VCFILES) se->Error("FileReadToken() - given file handle is not a valid file handle.");
	if (!vcfiles[handle].active) se->Error("FileReadToken() - given file handle is not open.");
	if (vcfiles[handle].mode != VC_READ) se->Error("FileReadToken() - given file handle is a write-mode file.");

	char buffer[255];
	buffer[0] = '\0'; // ensure sending back "" on error
	vscanf(vcfiles[handle].vfptr, "%s", buffer);
	strclean(buffer);
	return buffer;
}
int ScriptEngine::FileReadWord(int handle) {
	if (!handle || handle > VCFILES || !vcfiles[handle].active)
		se->Error("FileReadWord() - file handle is either invalid or file is not open.");
	if (vcfiles[handle].mode != VC_READ)
		se->Error("FileReadWord() - given file handle is a write-mode file.");

	int ret = 0;
	vread(&ret, 2, vcfiles[handle].vfptr);
	return ret;
}
void ScriptEngine::FileSeekLine(int handle, int line) {
	if (!handle) se->Error("FileSeekLine() - File is not open.");
	if (handle > VCFILES) se->Error("FileSeekLine() - given file handle is not a valid file handle.");
	if (!vcfiles[handle].active) se->Error("FileSeekLine() - given file handle is not open.");
	if (vcfiles[handle].mode != VC_READ) se->Error("FileSeekLine() - given file handle is a write-mode file.");

	vseek(vcfiles[handle].vfptr, 0, SEEK_SET);
	char temp[256+1];
	while (line-->0)
        vgets(temp, 256, vcfiles[handle].vfptr);
}
void ScriptEngine::FileSeekPos(int handle, int offset, int mode) {
	if (!handle || handle > VCFILES || !vcfiles[handle].active)
		se->Error("FileSeekPos() - file handle is either invalid or file is not open.");

	switch (vcfiles[handle].mode)
	{
		case VC_READ:
			vseek(vcfiles[handle].vfptr, offset, mode);
			break;
		case VC_WRITE:
			fseek(vcfiles[handle].fptr, offset, mode);
			break;
		default:
			se->Error("SFileeekPos() - File mode not valid! That's bad!");
	}
}
void ScriptEngine::FileWrite(int handle, CStringRef s) {
	if (!handle) se->Error("FileWrite() - Yo, you be writin' to a file that aint open, foo.");
	if (handle > VCFILES) se->Error("FileWrite() - given file handle is not a valid file handle.");
	if (!vcfiles[handle].active) se->Error("FileWrite() - given file handle is not open.");
	if (vcfiles[handle].mode != VC_WRITE) se->Error("FileWrite() - given file handle is a read-mode file.");

	fwrite(s.c_str(), 1, s.length(), vcfiles[handle].fptr);
}
void ScriptEngine::FileWriteByte(int handle, int var) {
	if (!handle || handle > VCFILES || !vcfiles[handle].active)
		se->Error("FileWriteByte() - file handle is either invalid or file is not open.");
	if (vcfiles[handle].mode != VC_WRITE)
		se->Error("FileWriteByte() - given file handle is a read-mode file.");
	flip(&var, sizeof(var)); // ensure little-endian writing
	fwrite(&var, 1, 1, vcfiles[handle].fptr);
}
void ScriptEngine::FileWriteln(int handle, CStringRef s) {
	if (!handle) se->Error("FileWriteln() - Yo, you be writin' to a file that aint open, foo.");
	if (handle > VCFILES) se->Error("FileWriteln() - given file handle is not a valid file handle.");
	if (!vcfiles[handle].active) se->Error("FileWriteln() - given file handle is not open.");
	if (vcfiles[handle].mode != VC_WRITE) se->Error("FileWriteln() - given file handle is a read-mode file.");

	fwrite(s.c_str(), 1, s.length(), vcfiles[handle].fptr);
	fwrite("\r\n", 1, 2, vcfiles[handle].fptr);
}
void ScriptEngine::FileWriteQuad(int handle, int var) {
	if (!handle || handle > VCFILES || !vcfiles[handle].active)
		se->Error("FileWriteQuad() - file handle is either invalid or file is not open.");
	if (vcfiles[handle].mode != VC_WRITE)
		se->Error("FileWriteQuad() - given file handle is a read-mode file.");
	flip(&var, sizeof(var)); // ensure little-endian writing
	fwrite(&var, 1, 4, vcfiles[handle].fptr);
}
void ScriptEngine::FileWriteString(int handle, CStringRef s) {
	if (!handle || handle > VCFILES || !vcfiles[handle].active)
		se->Error("FileWriteString() - file handle is either invalid or file is not open.");
	if (vcfiles[handle].mode != VC_WRITE)
		se->Error("FileWriteString() - given file handle is a read-mode file.");

	int l = s.length();
	int writeLength = l;

	flip(&writeLength, sizeof(writeLength)); // ensure little-endian writing
	fwrite(&writeLength, 1, 2, vcfiles[handle].fptr);
	fwrite(s.c_str(), 1, l, vcfiles[handle].fptr);
}
void ScriptEngine::FileWriteWord(int handle, int var) {
	if (!handle || handle > VCFILES || !vcfiles[handle].active)
		se->Error("vc_FileWriteWord() - file handle is either invalid or file is not open.");
	if (vcfiles[handle].mode != VC_WRITE)
		se->Error("vc_FileWriteWord() - given file handle is a read-mode file.");
	flip(&var, sizeof(var)); // ensure little-endian writing
	fwrite(&var, 1, 2, vcfiles[handle].fptr);
}
StringRef ScriptEngine::ListFilePattern(CStringRef pattern) {
	std::vector<std::string> result;
	listFilePattern(result, pattern);
	std::string ret;

	for(std::vector<std::string>::iterator i = result.begin();
		i != result.end();
		i++)
			ret += *i + "|";
	return ret;
}

// Overkill (2006-07-20):
// Saves a CHR file, using an open file handle, saving the specified entity.
void ScriptEngine::FileWriteCHR(int handle, int ent) {
	if (!handle || handle > VCFILES || !vcfiles[handle].active)
		se->Error("FileWriteCHR() - file handle is either invalid or file is not open.");
	if (vcfiles[handle].mode != VC_WRITE)
		se->Error("FileWriteCHR() - given file handle is a read-mode file.");
	if (ent < 0 || ent >= entities)
		se->Error("Tried saving an invalid or inactive ent index (%d).", ent);

	entity[ent]->chr->save(vcfiles[handle].fptr);	
}

// Overkill (2006-07-20):
// Saves a MAP file, using an open file handle, saving the current map.
void ScriptEngine::FileWriteMAP(int handle) {
	if (!handle || handle > VCFILES || !vcfiles[handle].active)
		se->Error("FileWriteMAP() - file handle is either invalid or file is not open.");
	if (vcfiles[handle].mode != VC_WRITE)
		se->Error("FileWriteMAP() - given file handle is a read-mode file.");
	if (!current_map)
		se->Error("FileWriteMAP() - There is no active map, therefore making it not possible to save this map.");

	current_map->save(vcfiles[handle].fptr);	
}
// Overkill (2006-07-20):
// Saves a VSP file, using an open file handle, saving the current map's VSP.
void ScriptEngine::FileWriteVSP(int handle) {
	if (!handle || handle > VCFILES || !vcfiles[handle].active)
		se->Error("FileWriteVSP() - file handle is either invalid or file is not open.");
	if (vcfiles[handle].mode != VC_WRITE)
		se->Error("FileWriteVSP() - given file handle is a read-mode file.");
	if (!current_map)
		se->Error("FileWriteVSP() - There is no active map, therefore making it not possible to save the map's vsp.");

	current_map->tileset->save(vcfiles[handle].fptr);	
}

//VI.l. Window Managment Functions
//helper"
static void checkhandle(char *func, int handle, AuxWindow *auxwin) {
	if(!handle)
		se->Error("%s() - cannot access a null window handle!",func);
	if(!auxwin)
		se->Error("%s() - invalid window handle!",func);
}
void ScriptEngine::WindowClose(int win) {
	if(win == 1) se->Error("WindowClose() - cannot close gameWindow");
	AuxWindow *auxwin = vid_findAuxWindow(win);
	checkhandle("WindowClose",win,auxwin);
	auxwin->dispose();
}
int ScriptEngine::WindowCreate(int x, int y, int w, int h, CStringRef s) {
	AuxWindow *auxwin = vid_createAuxWindow();
	auxwin->setTitle(s.c_str());
	auxwin->setPosition(x,y);
	auxwin->setResolution(w,h);
	auxwin->setSize(w,h);
	auxwin->setVisibility(true);
	return auxwin->getHandle();
}
int ScriptEngine::WindowGetHeight(int win) {
	AuxWindow *auxwin = vid_findAuxWindow(win);
	checkhandle("WindowGetHeight",win,auxwin);
	return auxwin->getHeight();
}
int ScriptEngine::WindowGetImage(int win) {
	AuxWindow *auxwin = vid_findAuxWindow(win);
	checkhandle("WindowGetImage",win,auxwin);
	return auxwin->getImageHandle();
}
int ScriptEngine::WindowGetWidth(int win) {
	AuxWindow *auxwin = vid_findAuxWindow(win);
	checkhandle("WindowGetWidth",win,auxwin);
	return auxwin->getWidth();
}
int ScriptEngine::WindowGetXRes(int win) {
	AuxWindow *auxwin = vid_findAuxWindow(win);
	checkhandle("WindowGetXRes",win,auxwin);
	return auxwin->getXres();
}
int ScriptEngine::WindowGetYRes(int win) {
	AuxWindow *auxwin = vid_findAuxWindow(win);
	checkhandle("WindowGetYRes",win,auxwin);
	return auxwin->getYres();
}
void ScriptEngine::WindowHide(int win) {
	AuxWindow *auxwin = vid_findAuxWindow(win);
	checkhandle("WindowHide",win,auxwin);
	auxwin->setVisibility(false);
}
void ScriptEngine::WindowPositionCommand(int win, int command, int arg1, int arg2) {
	AuxWindow *auxwin = vid_findAuxWindow(win);
	checkhandle("WindowPositionCommand",win,auxwin);
	auxwin->positionCommand(command,arg1,arg2);
}
void ScriptEngine::WindowSetPosition(int win, int x, int y) {
	AuxWindow *auxwin = vid_findAuxWindow(win);
	checkhandle("WindowSetPosition",win,auxwin);
	auxwin->setPosition(x,y);
}
void ScriptEngine::WindowSetResolution(int win, int w, int h) {
	AuxWindow *auxwin = vid_findAuxWindow(win);
	checkhandle("WindowSetResolution",win,auxwin);
	auxwin->setResolution(w,h);
	auxwin->setSize(w,h);
}
void ScriptEngine::WindowSetSize(int win, int w, int h) {
	AuxWindow *auxwin = vid_findAuxWindow(win);
	checkhandle("WindowSetSize",win,auxwin);
	auxwin->setSize(w,h);
}
void ScriptEngine::WindowSetTitle(int win, CStringRef s) {
	AuxWindow *auxwin = vid_findAuxWindow(win);
	checkhandle("WindowSetTitle",win,auxwin);
	auxwin->setTitle(s.c_str());
}
void ScriptEngine::WindowShow(int win) {
	AuxWindow *auxwin = vid_findAuxWindow(win);
	checkhandle("WindowShow",win,auxwin);
	auxwin->setVisibility(true);
}
//VI.m. Movie Playback Functions
void ScriptEngine::AbortMovie() { win_movie_abortSimple(); }
void ScriptEngine::MovieClose(int m) { win_movie_close(m); }
int ScriptEngine::MovieGetCurrFrame(int m) { return win_movie_getCurrFrame(m); }
int ScriptEngine::MovieGetFramerate(int m) { return win_movie_getFramerate(m); }
int ScriptEngine::MovieGetImage(int m) { return win_movie_getImage(m); }
int ScriptEngine::MovieLoad(CStringRef s, bool mute) { return win_movie_load(s.c_str(), mute); }
void ScriptEngine::MovieNextFrame(int m) { win_movie_nextFrame(m); }
void ScriptEngine::MoviePlay(int m, bool loop) { win_movie_play(m, loop?1:0); }
void ScriptEngine::MovieRender(int m) { win_movie_render(m); }
void ScriptEngine::MovieSetFrame(int m, int f) { win_movie_setFrame(m,f); }
int ScriptEngine::PlayMovie(CStringRef s){ return win_movie_playSimple(s.c_str()); }

//VI.n. Netcode Functions
ServerSocket *vcserver = 0;

// Overkill (2008-04-17): Socket port can be switched to something besides 45150.
int vcsockport = 45150;
void ScriptEngine::SetConnectionPort(int port)
{
	vcsockport = port;
}

// Overkill (2008-04-17): Socket port can be switched to something besides 45150.
int ScriptEngine::Connect(CStringRef ip) {
	Socket *s;
	try
	{
		s = new Socket(ip.c_str(), vcsockport);
	}
	catch (NetworkException ne) {
		return 0;
	}
	return (int) s;
}

// Overkill (2008-04-17): Socket port can be switched to something besides 45150.
// Caveat: The server currently may not switch listen ports once instantiated.
int ScriptEngine::GetConnection() {
	try {
		if (!vcserver)
			vcserver = new ServerSocket(vcsockport);
		Socket *s = vcserver->accept();
		return (int) s;
	}
	catch(NetworkException e) {
		return 0;
    }
}

int ScriptEngine::GetUrlImage(CStringRef url) { return ::getUrlImage(url); }
StringRef ScriptEngine::GetUrlText(CStringRef url) { return ::getUrlText(url); }
void ScriptEngine::SocketClose(int sh) { delete ((Socket *)sh); }
bool ScriptEngine::SocketConnected(int sh) { return ((Socket*)sh)->connected()!=0; }
StringRef ScriptEngine::SocketGetFile(int sh, CStringRef override) {
	static char stbuf[4096];
	Socket *s = (Socket *) sh;
	StringRef retstr;

	EnforceNoDirectories(override);

	int stlen = 0, ret;
	ret = s->blockread(2, &stlen);
	if (!ret)
		return StringRef();

	ret = s->blockread(stlen, stbuf);
	stbuf[stlen] = 0;

	StringRef fn = stbuf;
	EnforceNoDirectories(fn);

	int fl;
	s->blockread(4, &fl);

	char *buf = new char[fl];
	s->blockread(fl, buf);

	FILE *f;
	if (override.length())
	{
		retstr = override;
		f = fopen(override.c_str(), "wb");
	}
	else
	{
		retstr = fn;
		f = fopen(fn.c_str(), "wb");
	}
	if (!f)
		err("SocketGetFile: couldn't open file for writing!");
	fwrite(buf, 1, fl, f);
	fclose(f);
	delete[] buf;

	return retstr;
}
int ScriptEngine::SocketGetInt(int sh) {
	Socket *s = (Socket *) sh;
	int ret;
	char t;
	ret = s->blockread(1, &t);
	if (t != '1')
		err("SocketGetInt() - packet being received is not an int");
	int temp;
	ret = s->blockread(4, &temp);
	return temp;
}
StringRef ScriptEngine::SocketGetString(int sh) {
	static char buf[4096];
	Socket *s = (Socket *) sh;
	int stlen = 0, ret;
	char t;
	ret = s->blockread(1, &t);
	if (t != '3')
		err("SocketGetString() - packet being received is not a string");
	ret = s->blockread(2, &stlen);
	if (!ret)
		return StringRef();

#ifdef __BIG_ENDIAN__
	stlen >>= 16;
#endif

	if (stlen>4095) err("yeah uh dont send such big strings thru the network plz0r");
	ret = s->blockread(stlen, buf);
	buf[stlen] = 0;
	return buf;
}
bool ScriptEngine::SocketHasData(int sh) { return ((Socket*)sh)->dataready()!=0; }
void ScriptEngine::SocketSendFile(int sh, CStringRef fn) {
	Socket *s = (Socket *) sh;

	EnforceNoDirectories(fn);

	VFILE *f = vopen(fn.c_str());
	if (!f)
		err("ehhhhhh here's a tip. SocketSendFile can't send a file that doesnt exist (you tried to send %s)", fn.c_str());

	int i = fn.length();
	s->write(2, &i);
	s->write(i, fn.c_str());

	int l = filesize(f);
	s->write(4, &l);
	char *buf = new char[l];
	vread(buf, l, f);
	s->write(l, buf);
	delete[] buf;
	vclose(f);
}
void ScriptEngine::SocketSendInt(int sh, int i) {
	Socket *s = (Socket *) sh;
	char t = '1';
	s->write(1, &t);
	s->write(4, &i);
}
void ScriptEngine::SocketSendString(int sh, CStringRef str) {
	Socket *s = (Socket *) sh;
	int len = str.length();
	if (len>4095) err("yeah uh dont send such big strings thru the network plz0r");
	char t = '3';
	s->write(1, &t);

#ifdef __BIG_ENDIAN__
	len <<= 16;
#endif

	s->write(2, &len);

#ifdef __BIG_ENDIAN__
	len >>= 16;
#endif

	s->write(len, str.c_str());
}

// Overkill (2008-04-17): Sockets can send and receive raw length-delimited strings
StringRef ScriptEngine::SocketGetRaw(int sh, int len)
{
	static char buf[4096];
	Socket *s = (Socket *) sh;
	if (len > 4095)
	{
		err("SocketGetRaw() - can only receive a maximum of 4095 characters at a time. You've tried to get %d", len);
	}
	int ret = s->nonblockread(len, buf);
	buf[ret] = 0;
	return buf;
}

// Overkill (2008-04-17): Sockets can send and receive raw length-delimited strings
void ScriptEngine::SocketSendRaw(int sh, CStringRef str)
{
	Socket *s = (Socket *) sh;
	int len = str.length();
	s->write(len, str.c_str());
}

// Overkill (2008-04-20): Peek at how many bytes are in buffer. Requested by ustor.
int ScriptEngine::SocketByteCount(int sh)
{
	Socket *s = (Socket *) sh;
	return s->byteCount();
}

//XX: unsorted functions and variables, mostly newly added and undocumented
CStringRef ScriptEngine::Get_EntityChr(int arg) {
	if(arg >= 0 && arg < entities && entity[arg]->chr != 0)
		return entity[arg]->chr->name;
	else
		return empty_string;
}
void ScriptEngine::Set_EntityChr(int arg, CStringRef chr) {
	if(arg >= 0 && arg < entities)
		entity[arg]->set_chr(chr);
}
int ScriptEngine::Get_EntityFrameW(int ofs) {
	if (ofs>=0 && ofs<entities) return entity[ofs]->chr->fxsize; else return 0;
}
int ScriptEngine::Get_EntityFrameH(int ofs) {
	if (ofs>=0 && ofs<entities) return entity[ofs]->chr->fysize; else return 0;
}

CStringRef ScriptEngine::Get_EntityDescription(int arg) {
	if(arg >= 0 && arg < entities)
		return entity[arg]->description;
	else
		return empty_string;
}
void ScriptEngine::Set_EntityDescription(int arg, CStringRef val) { 
	if(arg >= 0 && arg < entities)
		entity[arg]->description = val;
}

void ScriptEngine::Set_EntityActivateScript(int arg, CStringRef val)
{
	if(arg >= 0 && arg < entities)
		entity[arg]->script = val;
}


bool ScriptEngine::SoundIsPlaying(int chn) { return ::SoundIsPlaying(chn); }
void ScriptEngine::RectVGrad(int x1, int y1, int x2, int y2, int c, int c2, int d)
{
	image *id = ImageForHandle(d);
	::RectVGrad(x1, y1, x2, y2, c, c2, id);
}
void ScriptEngine::RectHGrad(int x1, int y1, int x2, int y2, int c, int c2, int d)
{
	image *id = ImageForHandle(d);
	::RectHGrad(x1, y1, x2, y2, c, c2, id);
}
void ScriptEngine::RectRGrad(int x1, int y1, int x2, int y2, int c, int c2, int d)
{
	image *id = ImageForHandle(d);
	::RectRGrad(x1, y1, x2, y2, c, c2, id);
}
void ScriptEngine::Rect4Grad(int x1, int y1, int x2, int y2, int c1, int c2, int c3, int c4, int d)
{
	image *id = ImageForHandle(d);
	::Rect4Grad(x1, y1, x2, y2, c1, c2, c3, c4, id);
}

// Overkill: 2005-12-28
// Helper function for WrapText.
static int TextWidth(int f, CStringRef text, int pos, int len)
{
	Font *font = (Font*) f;
	if (font == 0)
	{
		return pixels(text.c_str(),text.c_str()+len);
	}
	else
	{
		return font->Pixels(text.c_str()+pos,text.c_str()+pos+len);
	}
}

// Overkill: 2005-12-28
// Thank you, Zip.
StringRef ScriptEngine::strovr(CStringRef rep, CStringRef source, int offset)
{
	return ::strovr(source, rep, offset);
}

// Overkill: 2005-12-19
// Thank you, Zip.
// Rewritten, Kildorf: 2007-10-16
StringRef ScriptEngine::WrapText(int wt_font, CStringRef wt_s, int wt_linelen)
// Pass: The font to use, the string to wrap, the length in pixels to fit into
// Return: The passed string with \n characters inserted as breaks
// Assmes: The font is valid, and will overrun if a word is longer than linelen
// Note: Existing breaks will be respected, but adjacent \n characters will be
//     replaced with a single \n so add a space for multiple line breaks
{
	int lastbreak = -1; // beginning of the current line
	int lastws = -1; // last whitespace character
	int currloc = 0; // current character
	int len = wt_s.length(); // length of string

	StringRef temp = wt_s.str();

	while (currloc < len)
	{
		if (temp[currloc] == ' ')
		{
			lastws = currloc;
		}
		else if (temp[currloc] == '\n')
		{
			lastws = currloc;
			lastbreak = currloc;
		}
		else if (temp[currloc] == '\r')
		{
			if (temp[currloc+1] == '\n')
				currloc++;
			lastws = currloc;
			lastbreak = currloc;
		}
		else if (::TextWidth(wt_font, temp, lastbreak+1, currloc - (lastbreak)) > wt_linelen && lastws != lastbreak)
		{
			temp.dangerous_peek()[lastws] = '\n';
			lastbreak = lastws;
		}

		currloc++;
	}

	return temp;
}

int ScriptEngine::strpos(CStringRef sub, CStringRef source, int start) {
	return source.str().find(sub.str(), start);
}

int ScriptEngine::HSV(int h, int s, int v) { return ::HSVtoColor(h,s,v); }
int ScriptEngine::GetH(int col) {
	int h, s, v;
	::GetHSV(col, h, s, v);
	return h;
}
int ScriptEngine::GetS(int col) {
	int h, s, v;
	::GetHSV(col, h, s, v);
	return s;
}
int ScriptEngine::GetV(int col) {
	int h, s, v;
	::GetHSV(col, h, s, v);
	return v;
}
void ScriptEngine::HueReplace(int hue_find, int hue_tolerance, int hue_replace, int image) {
	::HueReplace(hue_find, hue_tolerance, hue_replace, ImageForHandle(image));
}
void ScriptEngine::ColorReplace(int find, int replace, int image)
{
	::ColorReplace(find, replace, ImageForHandle(image));
}

// Overkill (2006-06-30): Gets the contents of the key buffer.
// TODO: Implement for other platforms.
StringRef ScriptEngine::GetKeyBuffer()
{
	#ifdef __WIN32__
		return keybuffer;
	#else 
		err("The function GetKeyBuffer() is not defined for this platform.");
		return StringRef();
	#endif
}

// Overkill (2006-06-30): Clears the contents of the key buffer.
// TODO: Implement for other platforms.
void ScriptEngine::FlushKeyBuffer()
{
	#ifdef __WIN32__
		::FlushKeyBuffer();
	#else 
		err("The function FlushKeyBuffer() is not defined for this platform.");
	#endif
}

// Overkill (2006-06-30): Sets the delay in centiseconds before key repeat.
// TODO: Implement for other platforms.
void ScriptEngine::SetKeyDelay(int d)
{
	if (d < 0)
	{
		d = 0;
	}
	#ifdef __WIN32__
		key_input_delay = d;
	#else 
		err("The function SetKeyDelay() is not defined for this platform.");
	#endif
}
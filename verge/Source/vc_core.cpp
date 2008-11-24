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
 * verge3: vc_core.cpp                                            *
 * copyright (c) 2002 vecna                                       *
 ******************************************************************/

#include <stdarg.h>
#include <algorithm>
#include "xerxes.h"
#include "opcodes.h"

#include <sstream>

/****************************** data ******************************/

#undef max
#define max(a, b) (a > b ? b : a)

int vc_paranoid         =0;     // paranoid VC checking
int vc_arraycheck       =0;     // array range-checking

bool die				=false;
int invc				= 0;

//-------------------------VCPlugins statics-----------------------
int VCPlugins::_currId = 0;
std::map<std::string,VCPlugins::Function*> VCPlugins::functions;
std::map<std::string,VCPlugins::Variable*> VCPlugins::variables;
std::vector<VCPlugins::Plugin*> VCPlugins::plugins;

/******************* vc engine state variables ********************/

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

extern void VcBuildLibraryDispatchTable ();

/****************************** code ******************************/

VCCore::VCCore()
{
	userfuncMap[0] = userfuncMap[1] = userfuncMap[2] = 0;
	VcBuildLibraryDispatchTable();

	int_stack_base = 0;
	int_stack_ptr = 0;
	str_stack_base = 0;
	str_stack_ptr = 0;
	currentvc = &coreimages[CIMAGE_SYSTEM];
	current_cimage = CIMAGE_SYSTEM;
	LoadSystemXVC();
}

VCCore::~VCCore()
{
	int i, j;

	// Free ints.
	for (i=0; i<global_ints.size(); i++)
		delete global_ints[i];
	global_ints.clear();

	// Free strings.
	for (i=0; i<global_strings.size(); i++)
		delete global_strings[i];
	global_strings.clear();

	// Free struct instances
	for (i=0; i<struct_instances.size(); i++)
		delete struct_instances[i];
	struct_instances.clear();

	// Free functions
	for (i=0; i<NUM_CIMAGES; i++) {
		for (j=0; j<userfuncs[i].size(); j++)
			delete userfuncs[i][j];
		userfuncs[i].clear();
		delete[] userfuncMap[i];
		userfuncMap[i] = 0;
	}

	delete[] vcint;
	delete[] vcstring;
}

void VCCore::ExecAutoexec()
{
	ExecuteFunctionString("sysvc_global_initializers");
	ExecuteFunctionString("autoexec");
}

bool VCCore::ExecuteFunctionString(const StringRef &script)
{
	quad hash = FastHash(true,script.c_str());
	for(int i=0; i<NUM_CIMAGES; i++) {
		TUserFuncMap find; find.hash = hash;
		TUserFuncMap *end = userfuncMap[i]+userfuncs[i].size();
		TUserFuncMap *found = std::lower_bound( userfuncMap[i], end, find );
		if(found == end || found->hash != hash) continue;
	
		invc++;
		// Overkill (2007-05-02): Argument passing introduced.
		ExecuteUserFunc(i, found->index, true);
		invc--;

		return true;
	}

	return false;
}

//TODO - yuck I cant believe this looks at all the strings
bool VCCore::FunctionExists(const StringRef &script)
{
	quad hash = FastHash(true, script.c_str());
	for(int i=0; i<NUM_CIMAGES; i++) {
		TUserFuncMap find; find.hash = hash;
		TUserFuncMap *end = userfuncMap[i]+userfuncs[i].size();
		TUserFuncMap *found = std::lower_bound( userfuncMap[i], end, find );
		if(found == end || found->hash != hash) continue;
	
		return true;
	}

	return false;
}

/************************** private code **************************/

void VCCore::LoadSystemXVC()
{
	char xvc_sig[8] = "VERGE30", buf[8];
	VFILE *vf = vopen("system.xvc");

	if (!vf)
		err("VCCore::LoadSystemXVC() - Could not open system.xvc!");

	FILE *f = vf->fp;

	fread(buf, 1, 8, f);
	if (strcmp(xvc_sig, buf))
		err("VCCore::LoadSystemXVC() - system.xvc is missing signature!");

	userfuncs[CIMAGE_SYSTEM].clear();
	global_ints.clear();
	global_strings.clear();
	struct_instances.clear();

	int ver;
	fread_le(&ver, f);
	if (ver != 1)
		err("VCCore::LoadSystemXVC() - system.xvc has incorrect version marker");

	int size, i;
	fread_le(&size, f);
	for (i=0; i<size; i++)
		global_ints.push_back(new int_t(f));
	maxint = size ? global_ints[size-1]->ofs + global_ints[size-1]->len : 1;

	fread_le(&size, f);
	for (i=0; i<size; i++)
		global_strings.push_back(new string_t(f));
	maxstr = size ? global_strings[size-1]->ofs + global_strings[size-1]->len : 1;

	fread_le(&size, f);
	for (i=0; i<size; i++)
		struct_instances.push_back(new struct_instance(f));

	fread_le(&size, f);
	userfuncMap[CIMAGE_SYSTEM] = new TUserFuncMap[size];
	for (i=0; i<size; i++) {
		function_t* func = new function_t(f);
		userfuncs[CIMAGE_SYSTEM].push_back(func);
		userfuncMap[CIMAGE_SYSTEM][i].index = i;
		userfuncMap[CIMAGE_SYSTEM][i].hash = FastHash(func->name);
		//log("hash %08X function %s", userfuncMap[CIMAGE_SYSTEM][i].hash, func->name);
	}
	std::sort(userfuncMap[CIMAGE_SYSTEM],userfuncMap[CIMAGE_SYSTEM]+size);
	//for(int i=0;i<size;i++)
	//	log(":: id %06d hash %08X", i, userfuncMap[CIMAGE_SYSTEM][i].hash);

	// Allocate and initialize all vc global variables (to 0 or "")
	vcint = new int[maxint];
	vcstring = new StringRef[maxstr];
	memset(vcint, 0, maxint*4);

	coreimages[CIMAGE_SYSTEM].LoadChunk(f);
	vclose(vf);
}

void VCCore::LoadCore(VFILE *f, int cimage, bool append, bool patch_others)
{
	if(!append)
		UnloadCore(cimage);

	int newfuncs;
	fread_le(&newfuncs, f->fp);
	if (!newfuncs) return;

	userfuncMap[cimage] = new TUserFuncMap[newfuncs];
	for (int i=0; i<newfuncs; i++) {
		function_t *func =new function_t(f->fp);

		if(patch_others) {
			int j, k;
			for (j=0; j<NUM_CIMAGES; j++) {
				for(k=0; k<userfuncs[j].size();k++) {
					if(!strcasecmp(userfuncs[j][k]->name, func->name)) {
						// patch it from old image/func pair to new one
						funcpatch_t oldfunc(j, k);
						funcpatch_t newfunc(cimage, userfuncs[cimage].size());
						// only patch if not yet patched, so we only patch
						// the actual called ones. This means we can unload a
						// core image and functions "revert" to the previous
						// patching
						if(patch_table.find(oldfunc) == patch_table.end()) {
							patch_table[oldfunc] = newfunc;
						}
					}
				}
			}
		}

		userfuncs[cimage].push_back(func);
		userfuncMap[cimage][i].index = i;
		userfuncMap[cimage][i].hash = FastHash(func->name);

		#ifdef ALLOW_SCRIPT_COMPILATION
		if(vcc)
			vcc->PushFunction(cimage, func);
		#endif
	}

	std::sort(userfuncMap[cimage],userfuncMap[cimage]+newfuncs);

	if(append)
		coreimages[cimage].Append(f->fp, true);
	else
		coreimages[cimage].LoadChunk(f->fp);
}

void VCCore::LoadMapScript(VFILE *f)
{
	LoadCore(f, CIMAGE_MAP, false, false);
	
}

void VCCore::UnloadCore(int cimage)
{
	// remove patches to and from this image
	// we create a new patch table altogether
	// (easier than trying to erase while iterating)
	patch_table_t new_patch_table;

	patch_table_t::iterator it = patch_table.begin();
	while(it != patch_table.end()) {
		funcpatch_t oldfunc = it->first;
		funcpatch_t newfunc = it->second;

		if(oldfunc.first != cimage && newfunc.first != cimage) {
			// not concerting this image, copy over
			new_patch_table[oldfunc] = newfunc;
		}

		it++;
	}
	patch_table = new_patch_table;

	// remove all the functions from core and compiler
	for (int i=0; i<userfuncs[cimage].size(); i++)
	{
		delete userfuncs[cimage][i];
	}

	#ifdef ALLOW_SCRIPT_COMPILATION
	if(vcc)
		vcc->ClearFunctionList(cimage);
	#endif

	userfuncs[cimage].clear();
	delete[] userfuncMap[cimage];
	userfuncMap[cimage] = 0;
}

Chunk *VCCore::GetCore(int cimage)
{
	return &coreimages[cimage];
}

void VCCore::PushInt(int n)
{
	if (int_stack_ptr < 0 || int_stack_ptr >= 1024)
		err("VCCore::PushInt() Stack overflow");
	int_stack[int_stack_ptr++] = n;
}

int VCCore::PopInt()
{
	if (int_stack_ptr <= 0 || int_stack_ptr > 1024)
		err("VCCore::PopInt() Stack underflow");
	return int_stack[--int_stack_ptr];
}

void VCCore::PushString(CStringRef s)
{
	if (str_stack_ptr < 0 || str_stack_ptr > 1024)
		err("VCCore::PushString() Stack overflow!");
	str_stack[str_stack_ptr++] = s;
}

StringRef VCCore::PopString()
{
	if (str_stack_ptr<=0 || str_stack_ptr>1024)
		err("VCCore::PopString() Stack underflow!");
	return str_stack[--str_stack_ptr];
}

int vc_GetYear();
int vc_GetMonth();
int vc_GetDay();
int vc_GetDayOfWeek();
int vc_GetHour();
int vc_GetMinute();
int vc_GetSecond();


int VCCore::GetIntArgument(int index)
{
	if (index >= vararg_stack[vararg_stack.size() - 1].size())
	{
			return 0;
	}
	return int_stack[int_stack_base + in_func->numlocals + index];
}

StringRef VCCore::GetStringArgument(int index)
{
	if (index >= vararg_stack[vararg_stack.size() - 1].size())
	{
			return empty_string;
	}
	return str_stack[str_stack_base + in_func->numlocals + index];
}

void VCCore::SetIntArgument(int index, int value)
{
	if (index >= vararg_stack[vararg_stack.size() - 1].size())
	{
			return;
	}
	int_stack[int_stack_base + in_func->numlocals + index] = value;	
}

void VCCore::SetStringArgument(int index, CStringRef value)
{
	if (index >= vararg_stack[vararg_stack.size() - 1].size())
	{
			return;
	}
	str_stack[str_stack_base + in_func->numlocals + index] = value;	
}

int VCCore::ReadInt(int category, int loc, int ofs)
{
	switch (category)
	{
		case intGLOBAL:
			if (loc<0 || loc>=maxint)
				vcerr("ReadInt: bad offset to globalint (var)");
			return vcint[loc];
		case intARRAY:
			if (loc<0 || loc>=maxint)
				vcerr("ReadInt: bad offset to globalint (arr)");
			return vcint[loc];
		case intHVAR0:   // _READINT
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
				// Overkill (2007-05-02): Variable argument lists.
				case 116: return vararg_stack[vararg_stack.size() - 1].size();

				case 126: return event_entity_hit;

				default: vcerr("Unknown HVAR0 (%d)", loc);
			}
			break;
		case intHVAR1:
			switch (loc)
			{
				case 2: return keys[max(ofs,256)]; break;
				case 25: if(ofs<0 || ofs>31) vcerr("Invalid button number: 0..31 inclusive are valid."); else return sticks[cur_stick].button[max(ofs,32)];
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
				case 66: if (ofs>=0 && ofs<256) return sprites[ofs].x; return 0;
				case 67: if (ofs>=0 && ofs<256) return sprites[ofs].y; return 0;
				case 68: if (ofs>=0 && ofs<256) return sprites[ofs].sc; return 0;
				case 69: if (ofs>=0 &&	ofs<256) return sprites[ofs].image; return 0;
				case 70: if (ofs>=0 && ofs<256) return sprites[ofs].lucent; return 0;
				case 71: if (ofs>=0 && ofs<256) return sprites[ofs].addsub; return 0;
				case 72: if (ofs>=0 && ofs<256) return sprites[ofs].alphamap; return 0;
				case 73: if (ofs>=0 && ofs<256) return sprites[ofs].thinkrate; return 0; // Overkill (2006-07-28)
				case 78: if (ofs>=0 && ofs<entities) return entity[ofs]->obstruction; return 0;
				case 79: if (ofs>=0 && ofs<entities) return entity[ofs]->obstructable; return 0;
				case 94: if (current_map && ofs>=0 && ofs<current_map->numlayers) return current_map->layers[ofs]->lucent;
                case 97: if (current_map && ofs>=0 && ofs<current_map->numlayers) return (int)(current_map->layers[ofs]->parallax_x * 65536);
                case 98: if (current_map && ofs>=0 && ofs<current_map->numlayers) return (int)(current_map->layers[ofs]->parallax_y * 65536);
				case 105: if (ofs>=0 && ofs<256) return sprites[ofs].ent; return 0; // Overkill (2006-07-28)
				case 106: if (ofs>=0 && ofs<256) return sprites[ofs].silhouette; return 0; // Overkill (2006-07-28)
				case 107: if (ofs>=0 && ofs<256) return sprites[ofs].color; return 0; // Overkill (2006-07-28)
				case 108: if (ofs>=0 && ofs<256) return sprites[ofs].wait; return 0; // Overkill (2006-07-28)
				case 109: if (ofs>=0 && ofs<256) return sprites[ofs].onmap; return 0; // Overkill (2006-07-28)
				case 110: if (ofs>=0 && ofs<256) return sprites[ofs].layer; return 0; // Overkill (2006-07-28)
				case 111: if (ofs>=0 && ofs<entities) return entity[ofs]->lucent; return 0; // Overkill (2006-07-28)
				case 112: if (ofs>=0 && ofs<256) return sprites[ofs].timer; return 0; // Overkill (2006-07-28)
				case 113: return ScriptEngine::Get_EntityFrameW(ofs); // Overkill (2006-07-28)
				case 114: return ScriptEngine::Get_EntityFrameH(ofs); // Overkill (2006-07-28)
				// Overkill (2007-05-02): Variable argument lists.
				case 117: return GetIntArgument(ofs);
				case 118: if (ofs >= 0 && ofs < vararg_stack[vararg_stack.size() - 1].size()) { return vararg_stack[vararg_stack.size() - 1][ofs].type_id == t_INT; } return 0;
				case 119: if (ofs >= 0 && ofs < vararg_stack[vararg_stack.size() - 1].size()) { return vararg_stack[vararg_stack.size() - 1][ofs].type_id == t_STRING; } return 0;
                return -1;
				default: vcerr("Unknown HVAR1 (%d, %d)", loc, ofs);
			}
			break;
		case intLOCAL:
			if (loc<0 || loc>99)
				vcerr("VCCore::ReadInt() - bad offset to local ints: %d", loc);
			return int_stack[int_stack_base+loc];
		default:
			vcerr("VC Execution error: Invalid ReadInt category %d", (int) category);
	}
	return 0;
}

void VCCore::WriteInt(int category, int loc, int ofs, int value)
{
	switch (category)
	{
		case intGLOBAL:
			if (loc<0 || loc>=maxint)
				vcerr("WriteInt: bad offset to variable globalint (%d)", loc);
			vcint[loc]=value; break;
		case intARRAY:
			if (loc<0 || loc>=maxint)
				vcerr("WriteInt: bad offset to array globalint (%d)", loc);
			vcint[loc]=value; break;
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
				case 2: keys[ofs] = value; break;
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
				case 66: if (ofs>=0 && ofs<256) sprites[ofs].x = value; return;
				case 67: if (ofs>=0 && ofs<256) sprites[ofs].y = value; return;
				case 68: if (ofs>=0 && ofs<256) sprites[ofs].sc = value; return;
				case 69: if (ofs>=0 && ofs<256) sprites[ofs].image = value; return;
				case 70: if (ofs>=0 && ofs<256) sprites[ofs].lucent = value; return;
				case 71: if (ofs>=0 && ofs<256) sprites[ofs].addsub = value; return;
				case 72: if (ofs>=0 && ofs<256) sprites[ofs].alphamap = value; return;
				case 73: if (ofs>=0 && ofs<256) sprites[ofs].thinkrate = value; return; // Overkill (2006-07-28)
				case 78: if (ofs>=0 && ofs<entities) entity[ofs]->obstruction = (value!=0); return;
				case 79: if (ofs>=0 && ofs<entities) entity[ofs]->obstructable = (value!=0); return;
				case 94: if (current_map && ofs>=0 && ofs<current_map->numlayers) current_map->layers[ofs]->lucent = value; return;
                case 97: if (current_map && ofs>=0 && ofs<current_map->numlayers) current_map->layers[ofs]->SetParallaxX(value / 65536.0); return;
                case 98: if (current_map && ofs>=0 && ofs<current_map->numlayers) current_map->layers[ofs]->SetParallaxY(value / 65536.0); return;
				case 105: if (ofs>=0 && ofs<256) sprites[ofs].ent = value; return; // Overkill (2006-07-28)
				case 106: if (ofs>=0 && ofs<256) sprites[ofs].silhouette = value; return; // Overkill (2006-07-28)
				case 107: if (ofs>=0 && ofs<256) sprites[ofs].color = value; return; // Overkill (2006-07-28)
				case 108: if (ofs>=0 && ofs<256) sprites[ofs].wait = value; return; // Overkill (2006-07-28)
				case 109: if (ofs>=0 && ofs<256) sprites[ofs].onmap = value; return; // Overkill (2006-07-28)
				case 110: if (ofs>=0 && ofs<256) sprites[ofs].layer = value; return; // Overkill (2006-07-28)
				case 111: if (ofs>=0 && ofs<entities) entity[ofs]->lucent = value; return; // Overkill (2006-07-28)
				case 112: if (ofs>=0 && ofs<256) sprites[ofs].timer = value; return; // Overkill (2006-07-28)
				//case 113: ent.framew
				//case 114: ent.frameh
				// Overkill (2007-05-02): Variable argument lists.
				case 117: return SetIntArgument(ofs, value);
				default: vcerr("Unknown HVAR1 (%d, %d) (set %d)", loc, ofs, value);
			}
			break;
		case intLOCAL:
			if (loc<0 || loc>99)
				vcerr("VCCore::WriteInt() bad offset to local ints: %d", loc);
			int_stack[int_stack_base+loc] = value;
			return;
		default:
			vcerr("VC Execution error: Invalid WriteInt category %d", (int) category);
	}
}

int VCCore::ProcessOperand()
{
	byte c=0;
	quad d=0, ofs=0;

	byte op = currentvc->GrabC();
	switch (op)
	{
		case intPLUGINVAR:
			HandlePluginVarRead(currentvc->GrabD());
			return vcreturn;
			break;
		case intLITERAL:
			return currentvc->GrabD();
		case intHVAR0:
			d = currentvc->GrabD();
			return ReadInt(op, d, 0);
		case intHVAR1:
			d = currentvc->GrabD();
			ofs = ResolveOperand();
			return ReadInt(op, d, ofs);
		case intGLOBAL:
			d = currentvc->GrabD();
			return ReadInt(op, global_ints[d]->ofs, 0);
		case intARRAY:
		{
			int idx = currentvc->GrabD();
			d = global_ints[idx]->ofs;
			ofs = 0;
			for (int i=0; i<global_ints[idx]->dim; i++)
			{
				int dimofs = ResolveOperand();
				int old = dimofs;
				for (int j=i+1; j<global_ints[idx]->dim; j++)
					dimofs *= global_ints[idx]->dims[j];
				ofs += dimofs;
				if (dimofs >= global_ints[idx]->len)
					vcerr("Bad offset on reading array %s (%d/%d)", global_ints[idx]->name, ofs, global_ints[idx]->len);
			}
			return ReadInt(op, d+ofs, 0);
		}
		case intLOCAL:
			d=currentvc->GrabD();
			if (d>99) vcerr("ProcessOperand: bad offset to local ints %d", d);
			return int_stack[int_stack_base+d];
		case intLIBFUNC:
			currentvc->GrabC(); // skip opLIBFUNC
			HandleLibFunc();
			return vcreturn;
		case intUSERFUNC:
			currentvc->GrabC(); // skip opUSERFUNC
			c = currentvc->GrabC(); // to ensure correct order
			ExecuteUserFunc(c, currentvc->GrabD());
			return vcreturn;
		case intPLUGINFUNC:
			currentvc->GrabC(); //skip opPluginFunc
			HandlePluginFunc(currentvc->GrabD());
			return vcreturn;
			break;
		case intGROUP:
			return ResolveOperand();
		case ifZERO:
			return ProcessOperand() ? false : true;
		case iopNEGATE:
			return -1 * ProcessOperand();
		case iopNOT:
			return ~ProcessOperand();
		default:
			vcerr("VCCore::ProcessOperand() - Invalid intsource %d.", op);
			break;
	}
	return 0;
}

int VCCore::ResolveOperand()
{
	int i2, num = ProcessOperand();
	while (true)
	{
		byte c = currentvc->GrabC();
		switch (c)
		{
			case iopADD: num += ProcessOperand(); continue;
			case iopSUB: num -= ProcessOperand(); continue;
			case iopDIV:
				i2 = ProcessOperand();
				if (!i2) num = 0; else num /= i2;
				continue;
			case iopMULT: num = num * ProcessOperand(); continue;
			case iopMOD:
				i2 = ProcessOperand();
				if (!i2) num = 0; else num %= i2;
				continue;
			case iopSHL: num = num << ProcessOperand(); continue;
			case iopSHR: num = num >> ProcessOperand(); continue;
			case iopAND: num = num & ProcessOperand(); continue;
			case iopOR:  num = num | ProcessOperand(); continue;
			case iopXOR: num = num ^ ProcessOperand(); continue;

			case ifEQUAL: 			num = (num == ProcessOperand()) ? true : false; continue;
			case ifNOTEQUAL:		num = (num != ProcessOperand()) ? true : false; continue;
			case ifGREATER:			num = (num >  ProcessOperand()) ? true : false; continue;
			case ifGREATEROREQUAL: 	num = (num >= ProcessOperand()) ? true : false; continue;
			case ifLESS:			num = (num <  ProcessOperand()) ? true : false; continue;
			case ifLESSOREQUAL:		num = (num <= ProcessOperand()) ? true : false; continue;

			case ifAND:				num = (ProcessOperand() && num) ? true : false; continue;
			case ifOR:				num = (ProcessOperand() || num) ? true : false; continue;
			case iopEND: break;
			default:
				vcerr("VCCore::ResolveOperand() - unknown Operand combiner (%d)", (int) c);
		}
		break;
	}
	return num;
}

StringRef VCCore::ProcessString()
{
	StringRef ret;
	int d;
	byte c = currentvc->GrabC();
	byte temp;
	switch (c)
	{
		case strLITERAL:
		{
			ret = currentvc->GrabString();
			break;
		}
		case strGLOBAL:
		{
			int idx = currentvc->GrabD();
			d = global_strings[idx]->ofs;
			if (d >= 0 && d < maxstr)
				ret = vcstring[d];
			else
				vcerr("VCCore::ProcessString() - bad offset to vcstring[] (strGLOBAL) %d, valid range [0,%d) \n global string name: '%s' ", d, maxstr, global_strings[idx]->name);
			break;
		}
		case strARRAY:
		{
			int idx = currentvc->GrabD();
			d = global_strings[idx]->ofs;

			for (int i=0; i<global_strings[idx]->dim; i++)
			{
				int dimofs = ResolveOperand();
				for (int j=i+1; j<global_strings[idx]->dim; j++)
					dimofs *= global_strings[idx]->dims[j];
				d += dimofs;
			}
			if (d>=0 && d<maxstr)
				ret = vcstring[d];
			else {
				vcerr("VCCore::ProcessString() - bad offset to vcstring[] (strARRAY) %d, valid range [0,%d).\n global string name: '%s'", d, maxstr, global_strings[idx]->name);
			}
			break;
		}
		case strHSTR0:   // _READSTR
		{
			int idx = currentvc->GrabD();
			switch (idx)
			{
			    case 84: ret = current_map ? current_map->mapname : empty_string; break;
				case 85: ret = current_map ? current_map->renderstring : empty_string; break;
				case 86: ret = current_map ? current_map->musicname : empty_string; break;
				case 95: ret = clipboard_getText(); break;
                case 99: ret = current_map ? current_map->mapfname : empty_string; break;
				case 104: ret = current_map ? current_map->savevspname : empty_string; break;

				case 121: //trigger.onStep
						ret = _trigger_onStep;
						break;
				case 122: //trigger.afterStep
						ret = _trigger_afterStep;
						break;
				case 123: //trigger.beforeEntityScript
						ret = _trigger_beforeEntityScript;
						break;
				case 124: //trigger.afterEntityScript
						ret = _trigger_afterEntityScript;
						break;
				case 125: //trigger.onEntityCollide
						ret = _trigger_onEntityCollide;
						break;

				default: vcerr("VCCore::ProcessString() - bad HSTR0 (%d)", idx);
			}
			break;
		}
		case strHSTR1:
		{
			int idx = currentvc->GrabD();
			int arg = ResolveOperand();
			switch (idx)
			{
				case 65:
					if(arg >= 0 && arg < entities)
					{
						ret = entity[arg]->script;
					}
					else
					{
						ret = empty_string;
					}
					break;
				case 74:
					if(arg >= 0 && arg < 256)
					{
						ret = sprites[arg].thinkproc; // Overkill (2006-07-28): No more HSTR error 4 u.
					}
					else
					{
						ret = empty_string;
					}
					break;
				case 88: // Overkill (2006-06-25): Now this actually has a use!
						ret = empty_string;
						if (current_map)
						{
							if (arg >= 0 && arg < current_map->numzones)
							{
								ret = current_map->zones[arg]->name;
							}
						}
						break;
				case 89: // Overkill (2006-06-25): Now this actually has a use!
						ret = empty_string;
						if (current_map)
						{
							if (arg >= 0 && arg < current_map->numzones)
							{
								ret = current_map->zones[arg]->script;
							}
						}
						break;
				case 100: //entity.chr
					ret = ScriptEngine::Get_EntityChr(arg);
					break;
				case 115:
					// Overkill (2006-07-30): Entity description
					return ScriptEngine::Get_EntityDescription(arg);
				case 120: // Overkill (2007-02-05): Variable argument lists.
					ret = GetStringArgument(arg);
					break;
				default: vcerr("VCCore::ProcessString() - bad HSTR1 (%d, %d)", idx, arg);
			}
			break;
		}
		case strINT: {
			char buf[16];
			_itoa(ResolveOperand(),buf,10);
			ret = buf;
			break;
		}
		case strLEFT:
			ret = ResolveString();
			ret = vc_strleft(ret,ResolveOperand());
			break;
		case strRIGHT:
			ret = ResolveString();
			d = ResolveOperand();
			ret = vc_strright(ret,d);
			break;
		case strMID:
			ret = ResolveString();
			d = ResolveOperand();
			ret = vc_strmid(ret,d,ResolveOperand());
			break;
		case strLOCAL:
			d = currentvc->GrabD();
			if (d>=0 && d<40)
				ret = str_stack[str_stack_base+d];
			else
				vcerr("VCCore::ProcessString() - bad offset to local strings");
			break;
		case strLIBFUNC:
			currentvc->GrabC(); // skip opLIBFUNC
			HandleLibFunc();
			ret = vcretstr;
			break;
		case strUSERFUNC:
			currentvc->GrabC(); // skip opUSERFUNC
			temp = currentvc->GrabC(); // to ensure correct order
			ExecuteUserFunc(temp, currentvc->GrabD());
			ret = vcretstr;
			break;
		case strPLUGINFUNC:
			currentvc->GrabC(); //skip opPluginFunc
			HandlePluginFunc(currentvc->GrabD());
			ret = vcretstr;
			break;
		case strPLUGINVAR:
			HandlePluginVarRead(currentvc->GrabD());
			ret = vcretstr;
			break;
		default:
			vcerr("VCCore::ProcessString() - Unknown VC string operand %d", c);
	}
	return ret;
}

StringRef VCCore::ResolveString()
{
	byte c;
	StringRef ret = ProcessString();
	std::string temp;
	bool useTemp = false;
	do
	{
		c = currentvc->GrabC();
		if (c == sADD) {
			if(!useTemp) {
				temp = ret.str();
				useTemp = true;
			}
			temp += ProcessString().str();
		}
		else if (c != sEND)
			vcerr("VCCore::ResolveString() - Unknown string operator %d", (int) c);
	} while (c != sEND);

	if(useTemp)
		return temp;
	else
		return ret;
}

void VCCore::ExecuteBlock()
{
	bool done = false;
	while (!done)
	{
		if (die) break;
		byte opcode = currentvc->GrabC();
		byte temp = 0;
		switch (opcode)
		{
			case opRETURN:		done = true; break; //code=(char *) vcpop(); break;
			case opASSIGN:		HandleAssign(); break;
			case opIF:			HandleIf(); break;
			case opPLUGINFUNC:	HandlePluginFunc(currentvc->GrabD()); break;
			case opLIBFUNC:		HandleLibFunc(); break;
			case opUSERFUNC:	temp = currentvc->GrabC(); // to ensure correct order of eval
								ExecuteUserFunc(temp, currentvc->GrabD()); break;
			case opGOTO:		currentvc->setpos(currentvc->GrabD()); break;
			case opSWITCH:
				if(!HandleSwitch()) {
					done = true; // it returned abnormally, so we return to
				}
				break;

			case opRETVALUE:	vcreturn=ResolveOperand();  break;
			case opRETSTRING:	vcretstr=ResolveString(); break;
			case opVARARG_START:
				// Overkill (2007-05-02): Ignored vararg pass, catch it so we can continue.
				IgnoreVararg(); break;
			default:
				vcerr("VCCore::ExecuteBlock() - Invalid opcode! (%d: %d)", currentvc->curpos() - 1, opcode);
		}
	}
}

void VCCore::HandlePluginVarRead(int id) {	
	VCPlugins::Variable *var = VCPlugins::variableByIndex(id);

	VCPlugins::VariableContext context;
	context._tag = this;
	context._intret = _vcplugins_functioncontext_get_intret;
	context._strret = _vcplugins_functioncontext_get_strret;
		
	//resolve the indices
	for(int i=0;i<var->dimensions;i++)
		context.indices.push_back(ResolveOperand());

	VCPlugins::executeVariable(id,context,VCPlugins::VARIABLE_READ);
}


void VCCore::HandlePluginFunc(int id) {
	VCPlugins::FunctionContext context;
	context._tag = this;
	context._int = _vcplugins_functioncontext_get_int;
	context._str = _vcplugins_functioncontext_get_str;
	context._intret = _vcplugins_functioncontext_get_intret;
	context._strret = _vcplugins_functioncontext_get_strret;
	VCPlugins::execFunction(id,context);
}

void VCCore::ReadVararg(std::vector<argument_t>& vararg)
{
	char c;
	argument_t arg;
	while (true)
	{
		c = currentvc->GrabC();
		if (c == t_INT)
		{
			arg.type_id = c;
			arg.int_value = ResolveOperand();
			arg.string_value = empty_string;
			vararg.push_back(arg);
		}
		else if (c == t_STRING)
		{
			arg.type_id = c;
			arg.int_value = 0;
			arg.string_value = ResolveString();
			vararg.push_back(arg);
		}
		else if (c == t_VARARG)
		{
			std::vector<argument_t> va = vararg_stack[vararg_stack.size() - 1];
			std::vector<argument_t>::iterator it;
			
			for (it = va.begin(); it != va.end(); it++)
			{
				vararg.push_back(*it);
			}
		}
		else if (c == opVARARG_END)
		{
			break;
		}
		else
		{
			vcerr("ExecuteUserFunc: Invalid opcode found while retrieving variable arguments! (%d: %d)", currentvc->curpos() - 1, c);
		}
	}
}

void VCCore::IgnoreVararg()
{
	while (true)
	{
		if (currentvc->GrabC() == opVARARG_END)
		{
			break;
		}
	}
}

bool VCCore::CheckForVarargs()
{
	// Check for variable arguments.
	int pos = currentvc->curpos();
	byte c = currentvc->GrabC();
	if (c == opVARARG_START)
	{
		return true;
	}
	currentvc->setpos(pos);
	return false;
}

void VCCore::ArgumentPassAddInt(int value)
{
	argument_t arg;
	arg.type_id = t_INT;
	arg.int_value = value;
	arg.string_value = empty_string;
	argument_pass_list.push_back(arg);
}

void VCCore::ArgumentPassAddString(StringRef value)
{
	argument_t arg;
	arg.type_id = t_STRING;
	arg.int_value = 0;
	arg.string_value = value;
	argument_pass_list.push_back(arg);
}

void VCCore::ArgumentPassClear()
{
	argument_pass_list.clear();
}

void VCCore::ExecuteUserFunc(int cimage, int ufunc, bool argument_pass)
{
	bool check_for_patch = true;
	// apply patches until we arrive and an unpatched one
	while(check_for_patch) {
		funcpatch_t oldfunc(cimage, ufunc);
		patch_table_t::iterator patch = patch_table.find(oldfunc);

		if(patch != patch_table.end()) {
			funcpatch_t newfunc = patch->second;
			cimage = newfunc.first;
			ufunc = newfunc.second;
			check_for_patch = true;
		} else {
			check_for_patch = false; // we're done
		}
	}

	if (ufunc<0 || ufunc>=userfuncs[cimage].size())
		vcerr("ExecuteUserFunc: VC sys script out of bounds (%d/%d)", ufunc, userfuncs[cimage].size());

	function_t *func = userfuncs[cimage][ufunc];
	int save_intbase = int_stack_base;
	int save_strbase = str_stack_base;
	int isp = int_stack_ptr;
	int ssp = str_stack_ptr;

	if (func->numlocals)
	{
		int n;

		if (argument_pass)
		{
			if (argument_pass_list.size() < func->numargs)
			{
				vcerr("Passed %d arguments to callback %s() when %d are needed.", argument_pass_list.size(), func->name, func->numargs);
			}
		}

		for (n=0; n < func->numargs; n++)			// read in arguments
		{
			switch (func->argtype[n])
			{
				case t_INT:
					if (argument_pass)
					{
						if (argument_pass_list[n].type_id != t_INT)
						{
							vcerr("Expected argument %d of callback %s() to be an integer.", n, func->name);
						}
						PushInt(argument_pass_list[n].int_value);
					}
					else
					{
						PushInt(ResolveOperand());
					}
					PushString(empty_string);
					break;
				case t_STRING:
					PushInt(0);
					if (argument_pass)
					{
						if (argument_pass_list[n].type_id != t_STRING)
						{
							vcerr("Expected argument %d of callback %s() to be a string.", n, func->name);
						}
						PushString(argument_pass_list[n].string_value);
					}
					else
					{
						PushString(ResolveString());
					}
					break;
				case t_VARARG:
					PushInt(0);
					PushString(empty_string);
					break;
			}
		}
		for (;n<func->numlocals; n++)				// finish off allocating locals
		{
			PushInt(0);
			PushString(empty_string);
		}
	}

	std::vector<argument_t> vararg;

	if (argument_pass && argument_pass_list.size() > func->numargs - 1)
	{
		int n;
		for (n = func->numargs - 1; n < argument_pass_list.size(); n++)
		{
			vararg.push_back(argument_pass_list[n]);
		}
	}
	else if (CheckForVarargs())
	{
		ReadVararg(vararg);
	}
	vararg_stack.push_back(vararg);

	if(vararg.size())
	{
		int n;
		for (n = 0; n < vararg.size(); n++)
		{
			switch(vararg[n].type_id)
			{
				case t_INT:
					PushInt(vararg[n].int_value);
					PushString(empty_string);
					break;
				case t_STRING:
					PushInt(0);
					PushString(vararg[n].string_value);
					break;
			}
		}
	}

	int_stack_base = isp;
	str_stack_base = ssp;

	// save core and code offset
	int save_pos = currentvc->curpos();
	int save_cimage = current_cimage;

	// setup for funcall
	current_cimage = cimage;
	currentvc = &coreimages[cimage];
	currentvc->setpos(func->codeofs);
	function_t *last_func = in_func;
	in_func = func;

	ExecuteBlock();   // execute!

	if (argument_pass)
	{
		ArgumentPassClear();
	}

	if (func->numlocals)	// free stack space
	{
		for (int n=0; n<func->numlocals + vararg.size(); n++)
		{
			PopInt();
			PopString();
		}
	}

	// Free variable argument stack space
	vararg_stack.pop_back();

	// restore stack bases, core and code offset
	current_cimage = save_cimage;
	currentvc = &coreimages[save_cimage];
	currentvc->setpos(save_pos);

	int_stack_base = save_intbase;
	str_stack_base = save_strbase;
	in_func = last_func;
}

void VCCore::HandleAssign()
{
	int	op, value, base=0, offset=0;
	byte c = currentvc->GrabC();

	if(c == strPLUGINVAR) {
		int idx = currentvc->GrabD();
		
		//setup the plugin context
		VCPlugins::Variable *var = VCPlugins::variableByIndex(idx);
		VCPlugins::VariableContext context;
		context._tag = this;
		context._str = _vcplugins_functioncontext_pop_str;
	
		//resolve the indices
		for(int i=0;i<var->dimensions;i++)
			context.indices.push_back(ResolveOperand());

		//check for valid assignment compilation
		c = currentvc->GrabC();
		if (c != aSET)
			vcerr("VC execution error: Corrupt string assignment");
		this->PushString(ResolveString());

		//write the variable
		VCPlugins::executeVariable(idx,context,VCPlugins::VARIABLE_WRITE);
		return;
	}

	if (c == strHSTR0)  // WRITESTR
	{
		StringRef tempstr;
		int idx = currentvc->GrabD();
		c = currentvc->GrabC();
		if (c != aSET)
			vcerr("VC execution error: Corrupt string assignment");
		switch (idx)
		{
			case 85: tempstr = ResolveString();
					 if (current_map) strcpy(current_map->renderstring, to_upper_copy(tempstr.str()).c_str());
					 break;
			case 95: clipboard_setText(ResolveString().c_str()); break;
			case 104: tempstr = ResolveString();
					 if (current_map) strcpy(current_map->savevspname, tempstr.c_str());
					 break;

			case 121: //trigger.onStep
					_trigger_onStep = ResolveString();
					break;
			case 122: //trigger.afterStep
					_trigger_afterStep = ResolveString();
					break;
			case 123: //trigger.beforeEntityScript
					_trigger_beforeEntityScript = ResolveString();
					break;
			case 124: //trigger.afterEntityScript
					_trigger_afterEntityScript = ResolveString();
					break;
			case 125: //trigger.onEntityCollide
					_trigger_onEntityCollide = ResolveString();
					break;

			default: vcerr("VCCore::HandleAssign() - bad HSTR0 (%d)", idx);
		}
		return;
	}
	if (c == strHSTR1)
	{
		int idx = currentvc->GrabD();
		int arg = ResolveOperand();
		c = currentvc->GrabC();
		if (c != aSET)
			vcerr("VC execution error: Corrupt string assignment");
		switch (idx)
		{
			case 65:
				if(arg >= 0 && arg < entities) {
					entity[arg]->script = ResolveString(); break;
				} else {
					ResolveString(); // invalid arg, just ignore the string
				}
				break;
			case 74: 
				if(arg >= 0 && arg < 256) {
					sprites[arg].thinkproc = ResolveString(); break;
				} else {
					ResolveString(); // invalid arg, just ignore the string
				}
				break;
			case 100: //Entity.Chr
				ScriptEngine::Set_EntityChr(arg,ResolveString());
				break;
			case 115: // Overkill (2006-07-30): Entity description
				ScriptEngine::Set_EntityDescription(arg,ResolveString());
				break;
			case 120: // Overkill (2007-05-02): Variable argument lists.
				SetStringArgument(arg, ResolveString());
				break;
			default: vcerr("VCCore::HandleAssign() - bad HSTR1 (%d, %d)", idx, arg);
		}
		return;
	}

	// string assignment
	if (c == strGLOBAL)
	{
		offset = currentvc->GrabD();
		offset = global_strings[offset]->ofs;
		c = currentvc->GrabC();
		if (c != aSET)
			vcerr("VC execution error: Corrupt string assignment");
		if (offset>=0 && offset<maxstr)
			vcstring[offset] = ResolveString();
		else
			vcerr("VCCore::HandleAssign() - bad offset to vc_strings (var)");
		return;
	}
	// string array assignment
	if (c == strARRAY)
	{
		int idx = currentvc->GrabD();
		base = global_strings[idx]->ofs;
		offset = 0;
		for (int i=0; i<global_strings[idx]->dim; i++)
		{
			int dimofs = ResolveOperand();
			for (int j=i+1; j<global_strings[idx]->dim; j++)
				dimofs *= global_strings[idx]->dims[j];
			offset += dimofs;
		}
		base += offset;
		c = currentvc->GrabC();
		if (c != aSET)
			vcerr("VC execution error: Corrupt string assignment");
		if (base>=0 && base<maxstr)
			vcstring[base] = ResolveString();
		else
			vcerr("HandleAssign: bad offset to vc_strings (arr)");
		return;
	}
	// local string assignment
	if (c == strLOCAL)
	{
		offset = currentvc->GrabD();
		c = currentvc->GrabC();
		if (c != aSET)
			vcerr("VCCore::HandleAssign() - Corrupt string assignment");
		if (offset>=0 && offset<40)
			str_stack[str_stack_base+offset] = ResolveString();
		else
			vcerr("HandleAssign: bad offset to local strings");
		return;
	}

	// integer assignment
	switch (c)
	{
		case intGLOBAL:
			base = global_ints[currentvc->GrabD()]->ofs;
			break;
		case intARRAY:
		{
			int idx = currentvc->GrabD();
			base = global_ints[idx]->ofs;
			offset = 0;
			for (int i=0; i<global_ints[idx]->dim; i++)
			{
				int dimofs = ResolveOperand();
				for (int j=i+1; j<global_ints[idx]->dim; j++)
					dimofs *= global_ints[idx]->dims[j];
				offset += dimofs;
			}
			base += offset;
			break;
		}
		case intHVAR0:
			base = currentvc->GrabD();
			break;
		case intHVAR1:
			base = currentvc->GrabD();
			offset = ResolveOperand();
			break;
		case intLOCAL:
			base = currentvc->GrabD();
			break;
		case intPLUGINVAR:
			break; //handle this beneath here
		default:
			vcerr("VC Execution error: Unknown assignment category.");
	}
	
	int intVariableIdx;
	VCPlugins::VariableContext context;
	if(c == intPLUGINVAR) {
		intVariableIdx = currentvc->GrabD();
		VCPlugins::Variable *var = VCPlugins::variableByIndex(intVariableIdx);
		context._tag = this;
		context._int = _vcplugins_functioncontext_pop_int;
				
		//resolve the indices
		for(int i=0;i<var->dimensions;i++)
			context.indices.push_back(ResolveOperand());

		op = currentvc->GrabC();
		if(op != aSET)
			VCPlugins::executeVariable(intVariableIdx,context,VCPlugins::VARIABLE_READ);
	}
	//--all other cases can be handled in ReadInt
	else {
		op = currentvc->GrabC();
		if(op != aSET) value = ReadInt((char)c, base, offset);
	}

	switch(op)
	{
		case aSET:		value = ResolveOperand(); break;
		case aINC:		value++; break;
		case aDEC:		value--; break;
		case aINCSET:	value += ResolveOperand(); break;
		case aDECSET:	value -= ResolveOperand(); break;

		default:
			vcerr("VC Execution error: Invalid assignment operator %d.", op);
	}
	
	if( c == intPLUGINVAR ) {
		PushInt(value);
		VCPlugins::executeVariable(intVariableIdx,context,VCPlugins::VARIABLE_WRITE);
	}
	else WriteInt(c, base, offset, value);
}

void VCCore::HandleIf()
{
	if (ProcessIf())
	{
		currentvc->GrabD();
		return;
	}
	int ofs = currentvc->GrabD();
	currentvc->setpos(ofs);
}

bool VCCore::ProcessIf()
{
	bool exec = ProcessIfOperand();
	return exec;
}

bool VCCore::ProcessIfOperand()
{
	int eval = ResolveOperand();
	return eval ? true : false;
}

bool VCCore::HandleSwitch()
{
	int realvalue = ResolveOperand();

	bool executed = false;
	while (true)
	{
		byte c=currentvc->GrabC();
		switch (c)
		{
			case opRETURN:
				return true;
			case opCASE:
			{
				int compvalue = ResolveOperand();
				int next = currentvc->GrabD();
				if (compvalue != realvalue)
				{
					currentvc->setpos(next);
					continue;
				}
				executed = true;
				ExecuteBlock();
				if(currentvc->curpos() != next) {
					// early exit from block, assume return
					return false;
				}
				break;
			}
			case opDEFAULT:
			{
				int next = currentvc->GrabD();
				if (executed)
				{
					currentvc->setpos(next);
					continue;
				}
				executed = true;
				ExecuteBlock();
				if(currentvc->curpos() != next) {
					 // early exit from block, assume return
					return false;
				}
				break;
			}
			default:
				vcerr("VCCore::HandleSwitch() - internal error, bad switch opcode (%d)", c);
		}
	}
	return true;
}

void VCCore::LookupOffset(int ofs, std::string &s)
{
	#ifdef ALLOW_SCRIPT_COMPILATION
	for (int i=0; i<userfuncs[current_cimage].size(); i++)
		if (ofs < userfuncs[current_cimage][i]->codeend)
		{
			VCCompiler *vcc = new VCCompiler();
			debuginfo debug = vcc->GetDebugForOffset(ofs);
			delete vcc;
			s += va("\t%s - %s(%d)\n", userfuncs[current_cimage][i]->name, debug.sourcefile, debug.linenum);
			return;
		}
	#endif
	s += va("\tUNKNOWN: %d\n", ofs);
}

void VCCore::DisplayError(CStringRef msg)
{
	std::string s = msg.c_str();
	s += ": \n\n";
	LookupOffset(currentvc->curpos(), s);
	/*int stackentries = vcsp - vcstack;
	for (int i=stackentries-1; i>=0; i--)
		FindSomethingAbout(vcstack[i], s);*/
	err(s.c_str());
}

void VCCore::ListStructMembers(std::vector<StringRef> &result, const char *structname)
{
	for (int i=0; i<struct_instances.size(); i++)
	{
		if (!strcasecmp(struct_instances[i]->name, structname))
		{
			for (int j = 0; j < struct_instances[i]->is->elements.size(); j++)
			{
				result.push_back(struct_instances[j]->is->elements[j]->name);
			}
			return;
		}
	}

	err("Unexpected condition in VCCore::ListStructMembers! please contact tech support immediately!");
}

bool VCCore::CopyArray(const char *srcname, const char *destname)
{
	int src_type;
	int dest_type;
	int src = -1;
	int dest = -1;

	// Look for int arrays that match src or dest
	for (int i=0; i<global_ints.size(); i++)
	{
		if (!strcasecmp(global_ints[i]->name, srcname))
		{
			src = i;
			src_type = t_INT;
		}
		else if (!strcasecmp(global_ints[i]->name, destname))
		{
			dest = i;
			dest_type = t_INT;
		}
	}

	// Look for string arrays that match src or dest
	for (int i=0; i<global_strings.size(); i++)
	{
		if (!strcasecmp(global_strings[i]->name, srcname))
		{
			src = i;
			src_type = t_STRING;
		}
		else if (!strcasecmp(global_strings[i]->name, destname))
		{
			dest = i;
			dest_type = t_STRING;
		}
	}

	// If either source or dest has no match,
	// or source and dest are different types, return
	if (src == -1
		|| dest == -1
		|| src_type != dest_type)
	{
		return false;
	}

	if (src_type == t_INT)
	{
		// If arrays aren't same dimensions, return.
		for (int i = 0; i < global_ints[src]->dim; i++)
		{
			if (global_ints[src]->dims[i] != global_ints[dest]->dims[i])
			{
				return false;
			}
		}
		// Copy!
		for (int i = 0; i < global_ints[src]->len; i++)
		{
			vcint[global_ints[dest]->ofs + i]
				= vcint[global_ints[src]->ofs + i];
		}
	}
	else if (src_type == t_STRING)
	{
		// If arrays aren't same dimensions, return.
		for (int i = 0; i < global_strings[src]->dim; i++)
		{
			if (global_strings[src]->dims[i] != global_strings[dest]->dims[i])
			{
				return false;
			}
		}
		// Copy!
		for (int i = 0; i < global_strings[src]->len; i++)
		{
			vcstring[global_strings[dest]->ofs + i]
				= vcstring[global_strings[src]->ofs + i];
		}
	}

	// Success!
	return true;
}

void VCCore::SetInt(const char *intname, int value)
{
	for (int i=0; i<global_ints.size(); i++)
		if (!strcasecmp(global_ints[i]->name, intname))
		{
			vcint[global_ints[i]->ofs] = value;
			return;
		}
}

int VCCore::GetInt(const char *intname)
{
	for (int i=0; i<global_ints.size(); i++)
		if (!strcasecmp(global_ints[i]->name, intname))
			return vcint[global_ints[i]->ofs];

	return 0;
}

void VCCore::SetStr(CStringRef strname, CStringRef value)
{
	for (int i=0; i<global_strings.size(); i++)
		if (!strcasecmp(global_strings[i]->name, strname.c_str()))
		{
			vcstring[global_strings[i]->ofs] = value;
			return;
		}
}

CStringRef VCCore::GetStr(const char *strname)
{
	for (int i=0; i<global_ints.size(); i++)
		if (!strcasecmp(global_strings[i]->name, strname))
			return vcstring[global_strings[i]->ofs];

	return empty_string;
}

void VCCore::SetIntArray(const char *intname, int index, int value)
{
	for (int i=0; i<global_ints.size(); i++)
		if (!strcasecmp(global_ints[i]->name, intname) && index>=0 && index<global_ints[i]->len)
		{
			vcint[global_ints[i]->ofs+index] = value;
			return;
		}
}

int VCCore::GetIntArray(const char *intname, int index)
{
	for (int i=0; i<global_ints.size(); i++)
		if (!strcasecmp(global_ints[i]->name, intname) && index>=0 && index<global_ints[i]->len)
			return vcint[global_ints[i]->ofs+index];

	return 0;
}

// aen 12/3/05 12:55am : fixed index checking against global_ints[i]->len to be global_strings[i]->len
void VCCore::SetStrArray(CStringRef strname, int index, CStringRef value)
{
	for (int i = 0; i < global_strings.size(); i++)
	{
		if (!strcasecmp(global_strings[i]->name, strname.c_str()) && index >= 0 && index < global_strings[i]->len)
		{
			vcstring[global_strings[i]->ofs + index] = value;
			return;
		}
	}
}

// aen 12/3/05 12:52am : fixed looping through global_ints.size() and checking index against global_ints[i]->len
CStringRef VCCore::GetStrArray(CStringRef strname, int index)
{
	for (int i = 0; i < global_strings.size(); i++)
	{
		if (!strcasecmp(global_strings[i]->name, strname.c_str()) && index >= 0 && index < global_strings[i]->len)
		{
			return vcstring[global_strings[i]->ofs + index];
		}
	}
			
	return empty_string;
}

/*************************** vc decompiler ***************************/

void VCCore::Decompile()
{
	vcd = fopen("system.vcd", "w");
	if (!vcd) err("Can't open system.vcd for writing");
	WriteGlobalVars();
	DecompileFunctions();
	fclose(vcd);
}

void VCCore::WriteGlobalVars()
{
	for (int i=0; i<global_ints.size(); i++)
	{
		fprintf(vcd, "int %s", global_ints[i]->name);
		for (int j=0; j<global_ints[i]->dim; j++)
			fprintf(vcd,"[%d]",global_ints[i]->dims[j]);
		fprintf(vcd, ";\n");
	}
	fprintf(vcd, "\n");

	for (int i=0; i<global_strings.size(); i++)
	{
		fprintf(vcd, "string %s", global_strings[i]->name);
		for (int j=0; j<global_strings[i]->dim; j++)
			fprintf(vcd,"[%d]",global_strings[i]->dims[j]);
		fprintf(vcd, ";\n");
	}
	fprintf(vcd, "\n");
}

void VCCore::DecompileFunctions()
{
	for(int cimage = 0; cimage < NUM_CIMAGES; cimage++)
		for (int i=0; i<userfuncs[cimage].size(); i++)
		{
			switch (userfuncs[cimage][i]->signature)
			{
				case 1: fprintf(vcd, "int "); break;
				case 3: fprintf(vcd, "string "); break;
				case 5: fprintf(vcd, "void "); break;
			}

			fprintf(vcd, "%s(",userfuncs[cimage][i]->name);
			for (int j=0; j<userfuncs[cimage][i]->numargs; j++)
			{
				switch (userfuncs[cimage][i]->argtype[j])
				{
					case 1: fprintf(vcd, "int %s", userfuncs[cimage][i]->localnames[j]); break;
					case 3: fprintf(vcd, "string %s", userfuncs[cimage][i]->localnames[j]); break;
				}
				if (j<userfuncs[cimage][i]->numargs-1)
					fprintf(vcd,", ");
			}
			fprintf(vcd, ")\n");
			fprintf(vcd, "{\n");
			for (int j=userfuncs[cimage][i]->numargs; j<userfuncs[cimage][i]->numlocals; j++)
			{
				switch (userfuncs[cimage][i]->argtype[j])
				{
					case 1: fprintf(vcd, "  int %s;\n", userfuncs[cimage][i]->localnames[j]); break;
					case 3: fprintf(vcd, "  string %s;\n", userfuncs[cimage][i]->localnames[j]); break;
				}
			}
			if (userfuncs[cimage][i]->numlocals > userfuncs[cimage][i]->numargs)
				fprintf(vcd, "\n");

			DecompileFunction(cimage, i);

			fprintf(vcd, "}\n\n");
		}
}

void VCCore::PrintTab()
{
	for (int i=0; i<dtablvl; i++)
	  fprintf(vcd, "  ");
}

void VCCore::DecompileFunction(int cimage, int i)
{
	currentvc = &coreimages[cimage];
	currentvc->setpos(userfuncs[cimage][i]->codeofs);
	dtablvl = 1;

	while (currentvc->curpos() < userfuncs[cimage][i]->codeend)
	{
		byte opcode = currentvc->GrabC();
		switch (opcode)
		{
			case opRETURN:		PrintTab(); fprintf(vcd, "return;\n"); break;
//			case opASSIGN:		HandleAssign(); break;
//			case opIF:			HandleIf(); break;
			case opLIBFUNC:		DecompileLibFunc(); break;
//			case opUSERFUNC:	ExecuteUserFunc(currentvc->GrabD()); break;
//			case opGOTO:		currentvc->setpos(currentvc->GrabD()); break;
//			case opSWITCH:		HandleSwitch(); break;
//			case opRETVALUE:	vcreturn=ResolveOperand();  break;
//			case opRETSTRING:	vcretstr=ResolveString(); break;
			default:
				err("well shit. internal error in VCCore::DecompileFunction");
		}
	}
}

void VCCore::DecompileString()
{

}

void VCCore::DecompileLibFunc()
{
	PrintTab();
	byte c = currentvc->GrabC();
	fprintf(vcd, "%s(", libfuncs[c].name);

	for (int i=0; i<libfuncs[c].argumentTypes.size(); i++)
	{
		switch (libfuncs[c].argumentTypes[i])
		{
			case t_INT:
				// DecompileOperand();
				break;
			case t_STRING: DecompileString(); break;
		}
	}
	fprintf(vcd, ");\n");
}

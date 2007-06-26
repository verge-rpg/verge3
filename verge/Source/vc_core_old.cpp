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

#define G_VC
#include "xerxes.h"
#include "opcodes.h"

/****************************** data ******************************/

#define USERFUNC_MARKER 10000

struct funcdecl
{
	char name[40];
	char argtype[20];
	char argname[20][40];
	int numargs, numlocals;
	int returntype;
	int syscodeofs;
};

struct strdecl
{
	char name[40];
	int ofs;
	int arraylen;
};

struct vardecl
{
	char name[40];
	int ofs;
	int arraylen;
};

struct floatdecl
{
	char name[40];
	int  ofs;
	int  arraylen;
};

int vc_paranoid         =0;     // paranoid VC checking
int vc_arraycheck       =0;     // array range-checking

char*	sysvc			=0;
char*	mapvc			=0;
char*	basevc			=0;		// VC pool ptrs
char*	code			=0;		// VC current instruction pointer (IP)

int*	globalint		=0;		// system.vc global int variables
int		maxint			=0;		// maximum allocated # of ints

string*	vc_strings		=0;		// vc string workspace
int		stralloc		=0;

double* vc_floats		=0;
int		maxfloats		=0;

int		vcreturn		=0;		// return value of last function
double  vcretfloat		=0.0;	// return value of last float function
string	vcretstr		="";	// return value of last string function

quad*	vcstack			=0;		// VC stack (seperate from main stack)
quad*	vcsp			=0;		// VC stack pointer [esp]

int		mapevents		=0;		// number of map events in this VC
int*	event_offsets	=0;		// map VC offset table

int		hookretrace		=0;
int		hooktimer		=0;
int		invc			=0;

char	kill			=0;

// FUNC/STR/VAR ARRAYS

funcdecl*	funcs		=0;
int			numfuncs	=0;

strdecl*	str			=0;
int			numstr		=0;

vardecl*	vars		=0;
int			numvars		=0;

floatdecl*  floats		=0;
int			numfloats	=0;

static int int_stack[1024+20];
static int int_stack_base=0, int_stack_ptr=0;

static string str_stack[1024+20];
static int str_stack_base=0, str_stack_ptr=0;

static double float_stack[1024+20];
static int float_stack_base=0, float_stack_ptr=0;

static int int_base[1024+20];
static int str_base[1024+20];
static int float_base[1024+20];
static int base_ptr=0;

static int int_last_base=0;
static int str_last_base=0;
static int float_last_base=0;

int bindarray[256] = { 0 };

static int sys_codesize=0;
static char* absolute_sys=0;

static char xvc_sig[8] = "VERGE30";

/******************* vc engine state variables ********************/

int cur_stick = 0;
char renderfunc[255];
char timerfunc[255];

int event_tx;
int event_ty;
int event_zone;
int event_entity;
int event_param;
string event_str;

/*************************** prototypes ***************************/

string ResolveString();
void ExecuteEvent(int i);
void ExecuteUserFunc(int i);
int ProcessOperand();
double ResolveFloat();
double ProcessFloatOperand();
int ProcessIfOperand();
int ResolveOperand();
void HandleStdLib();
void ExecuteBlock();
void vcpush(quad info);
quad vcpop();
void InitVCLibrary();
void ExecuteFunctionString(char *str);
void vcerr(char *str, ...);

/****************************** code ******************************/

static void PushBase(int ip, int fp, int sp)
{
	if (base_ptr<0 || base_ptr>=1024)
		err("PushBase: Stack overflow!");

	int_base[base_ptr]=int_stack_base=ip;
	str_base[base_ptr]=str_stack_base=sp;
	float_base[base_ptr]=float_stack_base=fp;

	base_ptr++;
}

static void PopBase()
{
	if (base_ptr<=0 || base_ptr>1024)
		err("PushBase: Stack underflow");

	base_ptr--;

	int_stack_base=int_base[base_ptr];
	str_stack_base=str_base[base_ptr];
	float_stack_base=float_base[base_ptr];
}

static void PushInt(int n)
{
	if (int_stack_ptr<0 || int_stack_ptr>=1024)
		err("PushInt: Stack overflow");
	int_stack[int_stack_ptr]=n;
	int_stack_ptr++;
}
static int PopInt()
{
	if (int_stack_ptr<=0 || int_stack_ptr>1024)
		err("PopInt: Stack underflow");
	--int_stack_ptr;
	return int_stack[int_stack_ptr];
}

static void PushFloat(double n)
{
	if (float_stack_ptr<0 || float_stack_ptr>1024)
		err("PushFloat: Stack overflow!");
	float_stack[float_stack_ptr++]=n;

}

static double PopFloat()
{
	if (float_stack_ptr<=0 || float_stack_ptr>1024)
		err("PopFloat: Stack underflow!");
	--float_stack_ptr;
	return float_stack[float_stack_ptr];
}

static void PushStr(string s)
{
	if (str_stack_ptr<0 || str_stack_ptr>1024)
		err("PushStr: Stack overflow!");
	str_stack[str_stack_ptr++]=s;
}
static string PopStr()
{
	if (str_stack_ptr<=0 || str_stack_ptr>1024)
		err("PopStr: Stack underflow!");
	--str_stack_ptr;
	return str_stack[str_stack_ptr];
}

void LoadSystemVC()
{
	VFILE *f;
	char buf[8];
	int n;

	log("Initializing VC interpreter");
	InitVCLibrary();
	f=vopen("system.xvc");
	if (!f)
		err("Could not open system.xvc");

	vread(buf, 8, f);
	if (strncmp(buf, xvc_sig, 8))
		err("LoadSystemVC: system.xvc contains invalid signature");

	vread(&n, 4, f); // skip something.. code offset I guess!
	vread(&sys_codesize, 4, f);

	vread(&numvars, 4, f);
	vread(&maxint, 4, f);
	vars = new vardecl[numvars];
	vread(vars, numvars * sizeof vardecl, f);

	vread(&numfloats, 4, f);
	vread(&maxfloats, 4, f);
	floats = new floatdecl[numfloats];
	vread(floats, numfloats * sizeof floatdecl, f);

	vread(&numstr, 4, f);
	vread(&stralloc, 4, f);
	str = new strdecl[numstr];
	vread(str, numstr * sizeof strdecl, f);

	vread(&numfuncs, 4, f);
	funcs = new funcdecl[numfuncs];
	for (int i=0; i<numfuncs; i++)
	{
		vread(funcs[i].name, 40, f);
		for (int j=0; j<20; j++)
		{
			vread(&funcs[i].argtype[j], 1, f);
			vread(funcs[i].argname[j], 40, f);
		}
		vread(&funcs[i].numargs, 4, f);
		vread(&funcs[i].numlocals, 4, f);
		vread(&funcs[i].returntype, 4, f);
		vread(&funcs[i].syscodeofs, 4, f);
	}

	// grab system script code size and allocate a buffer for it
	sysvc = new char[sys_codesize];
	absolute_sys = sysvc;

	// allocate global integer and string arrays
	if (maxint)
		globalint = new int[maxint];
	if (stralloc)
		vc_strings = new string[stralloc];

	// important! initialize all ints to 0.
	memset(globalint, 0, maxint*4);

	// read in system script code
	vread(sysvc, sys_codesize, f);
	vclose(f);

	// initialize VC stack
	vcstack = new quad[1500];
	vcsp = vcstack;

	log("system vclib init: %d funcs, %d ints (%d bytes), %d strings",
		numfuncs, numvars, maxint*4, numstr, stralloc);
}

void RunVCAutoexec()
{
	int n;

	// First, execute the globals initialization block
	vcpush(-1);
	basevc=sysvc;
	code=sysvc;
	ExecuteBlock();

	// Now find Autoexec() and call it if present
	for (n=0; n<numfuncs; n++)
	{
		char *x = funcs[n].name;
		strlwr(x);
		if (!strcmp(x,"autoexec"))
			break;
	}
	if (n<numfuncs)
		ExecuteUserFunc(n);
}

static int map_codesize=0;
static char* absolute_map=0;

void LoadMapVC(VFILE *f)
{
	vread(&mapevents, 4, f);
	if (event_offsets)
		delete[] event_offsets;
	event_offsets=new int[mapevents];
	if (!event_offsets)
		err("LoadMapVC: memory exhausted on event_offsets");
	vread(event_offsets, 4*mapevents, f);

	vread(&map_codesize, 4, f);

	log("%s: mapevents %d,  codesize %d", mapname, mapevents, map_codesize);
	mapvc=new char[map_codesize];
	absolute_map=mapvc;
	vread(mapvc, map_codesize, f);
}

void FreeMapVC()
{
	memset(bindarray, 0, sizeof bindarray);

	/* release memory */
	delete[] event_offsets;
	delete[] mapvc;
	event_offsets = 0;
}

byte GrabC()
{
	return *code++;
}

word GrabW()
{
	code+=2;
	return *(word *)(code-2);
}

quad GrabD()
{
	code+=4;
	return *(quad *)(code-4);
}

double GrabF()
{
	code+=8;
	return *(double *)(code-8);
}

string GrabString()
{
	string	ret;
	int		c;
	char	temp[32+1];

	ret="";
	c=0;
	while (*code)
	{
		temp[c++]=GrabC();
		if (c>=32)
		{
			c=temp[c]='\0';
			ret+=temp;
		}
	}
	if (c)
	{
		temp[c]='\0';
		ret+=temp;
	}
	code++;

	return ret;
}

#include "vc_library.h"
#undef max
#define max(x,a) x<a?x:0

int ReadInt(char category, int loc, int ofs)
{
/*	switch (category)
	{
	case op_INTGLOBAL:
		if (loc<0 || loc>=maxint)
			vcerr("ReadInt: bad offset to globalint (var)");
		return globalint[loc];
	case op_INTARRAY:
		if (loc<0 || loc>=maxint)
			vcerr("ReadInt: bad offset to globalint (arr)");
		return globalint[loc];
	case op_BUILTIN_SINGLE:
		switch (loc)
		{
			case 0: return 1;
			case 1: return systemtime;
			case 2: return vctimer;
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
			case 25: return up;
			case 26: return down;
			case 27: return left;
			case 28: return right;
			case 29: return b1;
			case 30: return b2;
			case 31: return b3;
			case 32: return b4;
			case 33: return event_tx;
			case 34: return event_ty;
			case 35: return event_zone;
			case 36: return event_entity;
			case 37: return event_param;
//eventparamstr?
			case 39: return xwin;
			case 40: return ywin;
//			case 41: return cameratracking;
			case 42: return entities;

			default: vcerr("Unknown HVAR0 (%d)", loc);
		}
		break;
	case op_BUILTIN_ARRAY:
		switch (loc)
		{
			case 0: return keys[max(ofs,256)]; break;
			case 1: return sticks[cur_stick].button[max(ofs,32)];
			case 2: return entity[max(ofs, entities)]->getx();
			case 3: return entity[max(ofs, entities)]->gety();
			case 4: return entity[max(ofs, entities)]->specframe;
			case 5: return entity[max(ofs, entities)]->chr->GetFrameConst(entity[max(ofs, entities)]->face, entity[max(ofs, entities)]->framect);
			case 6: return entity[max(ofs, entities)]->chr->hx;
			case 7: return entity[max(ofs, entities)]->chr->hy;
			case 8: return entity[max(ofs, entities)]->chr->hw;
			case 9: return entity[max(ofs, entities)]->chr->hh;
			default: vcerr("Unknown HVAR1 (%d, %d)", loc, ofs);
		}
		break;
	case op_INTLOCAL:
		if (loc<0 || loc>19)
			vcerr("ReadInt: bad offset to local ints: %d", loc);
		return int_stack[int_stack_base+loc];

	default:
		vcerr("VC Execution error: Invalid ReadInt category %d", (int) category);
	}*/
	return 0;
}

void WriteInt(char category, int loc, int ofs, int value)
{
/*	switch (category)
	{
		case op_INTGLOBAL:
			if (loc<0 || loc>=maxint)
				vcerr("WriteInt: bad offset to variable globalint (%d)", loc);
			globalint[loc]=value; break;
		case op_INTARRAY:
			if (loc<0 || loc>=maxint)
				vcerr("WriteInt: bad offset to array globalint (%d)", loc);
			globalint[loc]=value; break;
		case op_BUILTIN_SINGLE:
			switch (loc)
			{
				case 2: vctimer = value; break;
				case 3: lastpressed = value; break;
				case 6: mouse_l = value; break;
				case 7: mouse_r = value; break;
				case 8: mouse_m = value; break;
				case 9: mwheel = (float) value; break;
				case 17: cur_stick = value<4 ? value : 0; break;
				default: vcerr("Unknown HVAR0 (%d) (set %d)", loc, value);
			}
			break;
		case op_BUILTIN_ARRAY:
			switch (loc)
			{
				case 0: keys[ofs] = value; break;
				default: vcerr("Unknown HVAR1 (%d, %d) (set %d)", loc, ofs, value);
			}
			break;
		case op_INTLOCAL:
			if (loc<0 || loc>19)
				vcerr("WriteInt: bad offset to local ints: %d", loc);
			int_stack[int_stack_base+loc] = value;
			return;

		default:
			vcerr("VC Execution error: Invalid WriteInt category %d", (int) category);
	}*/
}

int ResolveOperand()
{
/*	int cr=0;
	int d=0;
	byte c=0;

	cr=ProcessOperand();	// Get base number
	while (1)
	{
		c=GrabC();
		switch (c)
		{
		case op_ADD: cr += ProcessOperand(); continue;
		case op_SUB: cr -= ProcessOperand(); continue;
		case op_DIV:
			d=ProcessOperand();
			if (!d) cr=0; else cr /= d;
			continue;
		case op_MULT: cr = cr * ProcessOperand(); continue;
		case op_MOD:
			d=ProcessOperand();
			if (!d) cr=0; else cr %= d;
			continue;
		case op_SHL: cr = cr << ProcessOperand(); continue;
		case op_SHR: cr = cr >> ProcessOperand(); continue;
		case op_AND: cr = cr & ProcessOperand(); continue;
		case op_OR:  cr = cr | ProcessOperand(); continue;
		case op_XOR: cr = cr ^ ProcessOperand(); continue;
		case op_END: break;
		}
		break;
	}
	return cr;*/
return 0;
}

int ProcessOperand()
{
/*	byte op_desc=0;
	byte c=0;
	quad d=0;
	quad ofs=0;

	op_desc=GrabC();
	switch (op_desc)
	{
		case op_LITERAL: return GrabD();
		case op_BUILTIN_SINGLE: c=GrabD(); return ReadInt(op_desc, c, 0);
		case op_BUILTIN_ARRAY: c=GrabD(); ofs=ResolveOperand(); return ReadInt(op_desc, c, ofs);
		case op_INTGLOBAL:  d=GrabD(); return ReadInt(op_desc, d, 0);
		case op_INTARRAY:
		{
			d=GrabD();
			ofs=ResolveOperand();
			if (!vc_arraycheck)
				return ReadInt(op_desc, d+ofs, 0);
			for (int i=0; i<numvars; i++)
				if (vars[i].ofs == d)
				{
					if (ofs >= vars[i].arraylen)
						vcerr("ProcessOperand(): range access violation on array %s (ofs %d)", vars[i].name, ofs);
					else
						return ReadInt(op_desc, d+ofs, 0);
				}
			vcerr("ProcessOperand(): internal VC error: array accesses no known array");
			break;
		}
		case op_INTLOCAL:
			c=GrabD();
			if (c>19) vcerr("ProcessOperand: bad offset to local ints %d", c);
			return int_stack[int_stack_base+c];
		case op_BUILTIN_FUNCT:
			HandleStdLib();
			return vcreturn;
		case op_USER_FUNCT:
			ExecuteUserFunc(GrabD());
			return vcreturn;
		case op_GROUP: return ResolveOperand();
		default:
			vcerr("VC Execution error: Invalid operand %d.", op_desc);
			break;
	}*/
	return 0;
}

string HandleStringOperand()
{
	string	ret;
/*	int		c;

	c=GrabC();
	switch (c)
	{
		case s_LITERAL:
			ret=GrabString();
			break;

		case s_GLOBAL:
			c=GrabD();
			if (c>=0 && c<stralloc)
				ret=vc_strings[c];
			else
				vcerr("HandleStringOperand: bad offset to vc_strings");
			break;

		case s_ARRAY:
			c=GrabD();
			c+=ResolveOperand();
			if (c>=0 || c<stralloc)
				ret=vc_strings[c];
			else
				vcerr("HandleStringOperand: bad offset to vc_strings");
			break;

		case s_TOSTR:
			ret=va("%d", ResolveOperand());
			break;

		case s_LEFT:
			ret=ResolveString();
			ret=ret.left(ResolveOperand());
			break;

		case s_RIGHT:
			ret=ResolveString();
			ret=ret.right(ResolveOperand());
			break;

		case s_MID:
			ret=ResolveString();
			c=ResolveOperand();
			ret=ret.mid(c, ResolveOperand());
			break;

		case s_CHR:
			ret=(char)ResolveOperand();
			break;

		case s_LOCAL:
			c=GrabD();
			if (c>=0 && c<20)
				ret = str_stack[str_stack_base+c];
			else
				vcerr("HandleStringOperand: bad offset to local strings");
			break;

		case s_USER_FUNCT:
			ExecuteUserFunc(GrabD());
			ret=vcretstr;
			break;

		case s_BUILTIN_FUNCT:
			HandleStdLib();
			ret=vcretstr;
			break;

		default:
			vcerr("Invalid VC string operand %d", c);
	}*/
	return ret;

}

string ResolveString()
{
	string	ret;
/*	int		c;

	ret=HandleStringOperand();
	do
	{
		c=GrabC();
		if (s_ADD==c)
			ret+=HandleStringOperand();
		else if (s_END!=c)
			vcerr("VC execution error: Unknown string operator %d", c);
	} while (c!=s_END);
*/
	return ret;
}

void vcpush(quad info)
{
	if (vcsp >= vcstack+1500)
		err("VC stack overflow.");

	*vcsp++ = info;
}

quad vcpop()
{
	if (vcsp <= vcstack)
		err("VC stack underflow.");

	return *--vcsp;
}

int ProcessIf()
{
/*	byte exec, c;

	exec=(byte)ProcessIfOperand();	// Get base value;
	while (1)
	{
		c=GrabC();
		switch (c)
		{
			case i_AND: exec=(byte)(exec & ProcessIfOperand()); continue;
			case i_OR: exec=(byte)(exec | ProcessIfOperand()); continue;
			case i_UNGROUP: break;
		}
		break;
	}
	return exec;*/
return 0;
}

int ProcessIfOperand()
{
/*	byte op_desc;
	int eval;

	eval=ResolveOperand();
	op_desc=GrabC();
	switch (op_desc)
	{
		case i_ZERO: if (!eval) return 1; else return 0;
		case i_NONZERO: if (eval) return 1; else return 0;
		case i_EQUALTO: if (eval == ResolveOperand()) return 1; else return 0;
		case i_NOTEQUAL: if (eval != ResolveOperand()) return 1; else return 0;
		case i_GREATER: if (eval > ResolveOperand()) return 1; else return 0;
		case i_NOTLESS: if (eval >= ResolveOperand()) return 1; else return 0;
		case i_LESS: if (eval < ResolveOperand()) return 1; else return 0;
		case i_NOTGREATER: if (eval <= ResolveOperand()) return 1; else return 0;
		case i_GROUP: if (ProcessIf()) return 1; else return 0;
	}
*/
	return 0;
}

void HandleIf()
{
	char *d;

	if (ProcessIf())
	{
		GrabD();
		return;
	}
	d		=(char *)GrabD();
	code	=(char *)(int)basevc+(int)d;
}

void HandleAssign()
{
	int	op, base, offset, value;

	byte c = GrabC();

// string assignment
/*	if (c == op_STRGLOBAL)
	{
		offset=GrabD();
		c=GrabC();
		if (c != a_SET)
			vcerr("VC execution error: Corrupt string assignment");
		if (offset>=0 && offset<stralloc)
			vc_strings[offset]=ResolveString();
		else
			vcerr("HandleAssign: bad offset to vc_strings (var)");
		return;
	}
// string array assignment
	if (c == op_STRARRAY)
	{
		offset=GrabD();
		offset+=ResolveOperand();
		c=GrabC();
		if (c != a_SET)
			vcerr("VC execution error: Corrupt string assignment");
		if (offset>=0 && offset<stralloc)
			vc_strings[offset]=ResolveString();
		else
			vcerr("HandleAssign: bad offset to vc_strings (arr)");
		return;
	}
// local string assignment
	if (c == op_STRLOCAL)
	{
		offset=GrabD();
		c=GrabC();
		if (c != a_SET)
			vcerr("VC execution error: Corrupt string assignment");
		if (offset>=0 && offset<20)
			str_stack[str_stack_base+offset]=ResolveString();
		else
			vcerr("HandleAssign: bad offset to local strings");
		return;
	}
*/
	// integer assignment
	base = offset = 0;
	switch (c)
	{
		case aINTGLOBAL:
			base = systemvc.GrabD();
			break;
		case aINTARRAY:
			base = systemvc.GrabD();
			int ofs = ResolveOperand();
/*			if (!vc_arraycheck)
			{
				base += ofs;
				break;
			}
			for (int i=0; i<numvars; i++)
				if (vars[i].ofs == base)
					if (ofs >= vars[i].arraylen)
						vcerr("HandleAssign(): range access violation on write to array %s (ofs %d)", vars[i].name, ofs);*/
			base += ofs;
			break;
		}
		case aHVAR0:
			base = systemvc.GrabD();
			break;
		case aHVAR1:
			base = systemvc.GrabD();
			offset = ResolveOperand();
			break;
/*		case op_INTLOCAL:
			base = GrabD();
			break;*/
		default:
			vcerr("VC Execution error: Unknown assignment category.");
	}
	value = ReadInt((char)c, base, offset);
	op = GrabC();
	switch(op)
	{
		case a_SET:		value=ResolveOperand(); break;
		case a_INC:		value++; break;
		case a_DEC:		value--; break;
		case a_INCSET:	value+=ResolveOperand(); break;
		case a_DECSET:	value-=ResolveOperand(); break;

		default:
			vcerr("VC Execution error: Invalid assignment operator %d.", op);
	}
	WriteInt((char)c, base, offset, value);*/
}

void HandleSwitch()
{
	/*int realvalue=ResolveOperand();
	byte c=GrabC();
	while (c != opRETURN)
	{
		int compvalue = ResolveOperand();
		byte *next = (byte *)GrabD();
		if (compvalue != realvalue)
		{
			code=(char *)(int)basevc+(int)next;
			c=GrabC();
			continue;
		}
		ExecuteBlock();
		c=GrabC();
	}*/
}

void ExecuteVC()
{
/*	byte c=0;

	while (1)
	{
		if (kill) break;

		c=GrabC();
		switch (c)
		{
			case opEXEC_STDLIB: HandleStdLib(); break;
			case opEXEC_USERFUNC: ExecuteUserFunc(GrabD()); break;
			case opIF: HandleIf(); break;
			case opELSE: break;
			case opGOTO: code=basevc+GrabD(); break;
			case opSWITCH: HandleSwitch(); break;
			case opASSIGN: HandleAssign(); break;
			case opRETURN: code=(char *) vcpop(); break;
			case opSETRETVAL: vcreturn=ResolveOperand(); break;
			case opSETRETSTRING: vcretstr=ResolveString(); break;

			default:
				vcerr("Internal VC execution error. (%d: %d)", (int) code - (int) basevc - 1, c);
		}

		if ((int)code != -1)
			continue;
		else
			break;
	}*/
}

void ExecuteBlock()
{
/*	byte c=0;

	while (1)
	{
		if (kill) break;

 		c=GrabC();
		switch (c)
		{
			case opEXEC_STDLIB: HandleStdLib(); break;
			case opEXEC_USERFUNC: ExecuteUserFunc(GrabD()); break;
			case opIF: HandleIf(); break;
			case opELSE: break;
			case opGOTO: code=basevc+GrabD(); break;
			case opSWITCH: HandleSwitch(); break;
			case opASSIGN: HandleAssign(); break;
			case opRETURN: code=(char *) vcpop(); break;
			case opSETRETVAL: vcreturn=ResolveOperand(); break;
			case opSETRETSTRING: vcretstr=ResolveString(); break;

			default:
				vcerr("Internal VC execution error. (%d: %d)", (int) code - (int) basevc - 1, c);
		}

		if (c != opRETURN)
			continue;
		break;
	}*/
}

void ExecuteEvent(int ev)
{
	if (ev<0 || ev>=mapevents)
		vcerr("ExecuteEvent: VC event out of bounds (%d)", ev);

	++invc;

	vcpush((quad)code);
	vcpush((quad)basevc);

	basevc	=mapvc;
	code	=basevc+event_offsets[ev];

	vcpush (-1);
	ExecuteVC();

	basevc	=(char *)vcpop();
	code	=(char *)vcpop();

	--invc;
}

void ExecuteUserFunc(int ufunc)
{
	funcdecl*	pfunc;
	int		n;
	int ilb=0, flb=0, slb=0;

	if (ufunc<0 || ufunc>=numfuncs)
		vcerr("ExecuteUserFunc: VC sys script out of bounds (%d/%d)", ufunc, numfuncs);

	pfunc=funcs+ufunc;

	ilb=int_last_base;
	slb=str_last_base;
	flb=float_last_base;
	PushBase(int_last_base, float_last_base, str_last_base);

	int		isp, fsp, ssp;

	// we do not set the new base until we're done reading in the arguments--
	// this is because we might still need to read in local vars passed from the
	// previous function (lookup for locals works off current base values).
	// for now, just tag the to-be bases.
		isp	=int_stack_ptr;
		ssp	=str_stack_ptr;
		fsp =float_stack_ptr;
// allocate stack space
	if (pfunc->numlocals)
	{
	// read in arguments
/*		for (n=0; n<pfunc->numargs; n++)
		{
			switch (pfunc->argtype[n])
			{
				case v_INT:
					PushInt(ResolveOperand());
					break;
				case v_FLOAT:
					PushFloat(ResolveFloat());
					break;
				case v_STRING:
					PushStr(ResolveString());
					break;
			}
		}
	// finish off allocating locals
		while (n<pfunc->numlocals)
		{
			switch (pfunc->argtype[n])
			{
				case v_INT:
					PushInt(0);
					break;
				case v_FLOAT:
					PushFloat(0.0);
				case v_STRING:
					PushStr("");
					break;
			}
			n++;
		}*/
	}
	// now we're safe to set the bases
	int_stack_base=int_last_base=isp;
	str_stack_base=str_last_base=ssp;
	float_stack_base=float_last_base=fsp;

	vcpush((quad)basevc);
	vcpush((quad)code);

	basevc	=sysvc;
	code	=(char *)(basevc + pfunc->syscodeofs);

	ExecuteBlock();

	basevc	=(char *)vcpop();

	// restore previous base
		PopBase();
		int_last_base=ilb;
		str_last_base=slb;
		float_last_base=flb;
// free stack space
	if (pfunc->numlocals)
	{
	// clear out all locals (args + 'true' locals)
		for (n=0; n<pfunc->numlocals; n++)
		{
/*			switch (pfunc->argtype[n])
			{
				case v_INT:
					PopInt();
					break;
				case v_FLOAT:
					PopFloat();
					break;
				case v_STRING:
					PopStr();
					break;
			}*/
		}
	}
}

void ExecuteFunctionString(char *str)
{
	string s = str;
	s = s.lower();

	for (int n=0; n<numfuncs; n++)
	{
		char *x = funcs[n].name;
		strlwr(x);
		if (!strcmp(x, s.c_str()))
			break;
	}
	if (n<numfuncs)
		ExecuteUserFunc(n);
	else
		vcerr("ExecuteFunctionString() - no function found: %s", s.c_str());
}

void HookRetrace()
{
	if (!hookretrace) return;

	if (hookretrace <  USERFUNC_MARKER)
		ExecuteEvent(hookretrace);
	if (hookretrace >= USERFUNC_MARKER)
		ExecuteUserFunc(hookretrace-USERFUNC_MARKER);
}

void HookTimer()
{
	if (!hooktimer) return;

	if (hooktimer <  USERFUNC_MARKER)
		ExecuteEvent(hooktimer);
	if (hooktimer >= USERFUNC_MARKER)
		ExecuteUserFunc(hooktimer-USERFUNC_MARKER);
}

void HookKey(int script)
{
	if (!script) return;
	if (script <  USERFUNC_MARKER)
		ExecuteEvent(script);
	if (script >= USERFUNC_MARKER)
		ExecuteUserFunc(script-USERFUNC_MARKER);
}

void FindSomethingAbout(int ofs, string &s)
{
	if (ofs == -1 || ofs == (int) sysvc || ofs == (int) mapvc)
		return;

	// both the map and system function-finders rely on map and
	// system script offsets being IN ASCENDING ORDER.
	// If that is ever changed, these will also need to be changed.

/*	if (ofs >= (int) mapvc && ofs <= (int) mapvc + map_codesize)
	{
		for (int i=0; i<mapevents; i++)
			if (ofs < (int) mapvc + event_offsets[i+1])
			{
				s += va("\t%s event %d (+%d)\n", current_map->mapname, i, ofs-(int)(mapvc+event_offsets[i]));
				return;
			}
	}*/
	if (ofs >= (int) sysvc && ofs <= (int) sysvc + sys_codesize)
	{
		for (int i=0; i<numfuncs; i++)
			if (ofs < (int) sysvc + funcs[i+1].syscodeofs)
			{
				s += va("\t%s (+%d)\n", funcs[i].name, ofs-(int)(sysvc+funcs[i].syscodeofs));
				return;
			}
	}
	s += va("\tUNKNOWN: %d\n", ofs);
}

void vcerr(char *str, ...)
{
	va_list argptr;
	char msg[256];

	va_start(argptr, str);
	vsprintf(msg, str, argptr);
	va_end(argptr);

	string s;
	s.assign(msg);
	s += ": \n\n";

	FindSomethingAbout((int) code, s);
	int stackentries = vcsp - vcstack;
	for (int i=stackentries-1; i>=0; i--)
		FindSomethingAbout(vcstack[i], s);
	err(s.c_str());
}

#undef G_VC
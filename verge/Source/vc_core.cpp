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


#include "xerxes.h"
#include "opcodes.h"

#include <sstream>
#include <stdarg.h>
#include <algorithm>

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



extern void VcBuildLibraryDispatchTable ();

/****************************** code ******************************/

VCCore::VCCore()
{
	userfuncMap[0] = userfuncMap[1] = userfuncMap[2] = 0;

	int_stack_base = 0;
	int_stack_ptr = 0;
	str_stack_base = 0;
	str_stack_ptr = 0;
	cb_stack_base = 0;
	cb_stack_ptr = 0;
	currentvc = &coreimages[CIMAGE_SYSTEM];
	current_cimage = CIMAGE_SYSTEM;
	LoadSystemXVC();
}

VCCore::~VCCore()
{
	int i, j;

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
	delete[] vccallback;
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
	global_vars.clear();
	struct_instances.clear();

	int ver;
	fread_le(&ver, f);
	if (ver != 1)
		err("VCCore::LoadSystemXVC() - system.xvc has incorrect version marker");

	// Globals.
	global_var_t var;
	int i, size;
	fread_le(&size, f);
	maxint = maxstr = maxcb = 0;
	
	for (int i=0; i<size; i++)
	{
		// Load variable definition from file.
		var = global_var_t(f);
		// Current "ofs" saved was the one to used by the compiler to properly expand structs/arrays. 
		// But we now need a offset specific to each type category.
		// Figure out each variable's offset in the global value arrays,
		// and calculate the max number of variables to allocate.
		if(var.type == t_INT)
		{
			var.ofs = maxint;
			maxint += var.len;
		}
		else if(var.type == t_STRING)
		{
			var.ofs = maxstr;
			maxstr += var.len;
		}
		else if(var.type == t_CALLBACK)
		{
			var.ofs = maxcb;
			maxcb += var.len;
		}

		// Push back.
		global_vars.push_back(var);
	}
	// Must allocate at least size 1 so we we can index these global arrays later.
	if(maxint == 0) maxint = 1;
	if(maxstr == 0) maxstr = 1;
	if(maxcb == 0) maxcb = 1;

	// Struct instances.
	fread_le(&size, f);
	for (i=0; i<size; i++)
		struct_instances.push_back(new struct_instance(f));

	// User functions.
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
	vccallback = new VergeCallback[maxcb];
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
	else {
		new(&coreimages[cimage])Chunk(); //inplace construct a new chunk to make sure everything is zapped
		coreimages[cimage].LoadChunk(f->fp);
	}
}

void VCCore::LoadMapScript(VFILE *f, CStringRef filename)
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

void VCCore::PushCallback(VergeCallback cb)
{
	if (cb_stack_ptr < 0 || cb_stack_ptr > 1024)
		err("VCCore::PushCallback() Stack overflow!");
	cb_stack[cb_stack_ptr++] = cb;
}

VergeCallback VCCore::PopCallback()
{
	if (cb_stack_ptr<=0 || cb_stack_ptr>1024)
		err("VCCore::PopCallback() Stack underflow!");
	return cb_stack[--cb_stack_ptr];
}

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
		case intLOCAL:
			if (loc<0 || loc>99)
				vcerr("VCCore::ReadInt() - bad offset to local ints: %d", loc);
			return int_stack[int_stack_base+loc];
		case intHVAR0:
		case intHVAR1:
			return ReadHvar(category,loc,ofs);
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
		case intHVAR0:
		case intHVAR1:
			WriteHvar(category,loc,ofs,value);
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
			return ReadInt(op, global_vars[d].ofs, 0);
		case intARRAY:
		{
			int idx = currentvc->GrabD();
			d = global_vars[idx].ofs;
			ofs = 0;
			for (int i=0; i<global_vars[idx].dim; i++)
			{
				int dimofs = ResolveOperand();
				int old = dimofs;
				for (int j=i+1; j<global_vars[idx].dim; j++)
					dimofs *= global_vars[idx].dims[j];
				ofs += dimofs;
				if (dimofs >= global_vars[idx].len)
					vcerr("Bad offset on reading array %s (%d/%d)", global_vars[idx].name, ofs, global_vars[idx].len);
			}
			return ReadInt(op, d+ofs, 0);
		}
		case intLOCAL:
			d=currentvc->GrabD();
			if (d>99) vcerr("ProcessOperand: bad offset to local ints %d", d);
			return int_stack[int_stack_base+d];
		// Local callback invocation.
		case cbLOCAL:
			d = currentvc->GrabD();
			c = currentvc->GrabC();
			if (c == opCBINVOKE)
			{
				if (d>=0 && d<40)
				{
					ExecuteCallback(cb_stack[cb_stack_base+d]);
				}
				else
				{
					vcerr("ProcessOperand: (int invocation) bad offset to local callbacks");
				}
			}
			else
			{
				vcerr("VCCore::ProcessOperand() - Corrupt callback invocation");
			}
			return vcreturn;
		// Callback operation (global var)
		case cbGLOBAL:
			d = currentvc->GrabD();
			d = global_vars[d].ofs;
			if (d>=0 && d<maxcb)
			{
				c = currentvc->GrabC();
				if(c == opCBINVOKE)
				{
					ExecuteCallback(vccallback[d]);
					return vcreturn;
				}
				else
				{
					vcerr("VCCore::ProcessOperand() - Corrupt callback invocation");
				}
			}
			else
				vcerr("VCCore::ProcessOperand(): bad offset to vc_callbacks (var), %d", d);
			break;
		// Callback operation (global array)
		case cbARRAY:
		{
			int idx = currentvc->GrabD();
			d = global_vars[idx].ofs;
			for (int i=0; i<global_vars[idx].dim; i++)
			{
				int dimofs = ResolveOperand();
				for (int j=i+1; j<global_vars[idx].dim; j++)
					dimofs *= global_vars[idx].dims[j];
				d += dimofs;
			}
			if (d>=0 && d<maxcb)
			{
				c = currentvc->GrabC();
				if(c == opCBINVOKE)
				{
					ExecuteCallback(vccallback[d]);
					return vcreturn;
				}
				else
				{
					vcerr("VCCore::ProcessOperand() - Corrupt callback invocation");
				}
			}
			else
				vcerr("VCCore::ProcessOperand(): bad offset to vc_callbacks (var), %d", d);
			break;
		}
		case intLIBFUNC:
			currentvc->GrabC(); // skip opLIBFUNC
			HandleLibFunc(currentvc->GrabW());
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
			d = global_vars[idx].ofs;
			if (d >= 0 && d < maxstr)
				ret = vcstring[d];
			else
				vcerr("VCCore::ProcessString() - bad offset to vcstring[] (strGLOBAL) %d, valid range [0,%d) \n global string name: '%s' ", d, maxstr, global_vars[idx].name);
			break;
		}
		case strARRAY:
		{
			int idx = currentvc->GrabD();
			d = global_vars[idx].ofs;

			for (int i=0; i<global_vars[idx].dim; i++)
			{
				int dimofs = ResolveOperand();
				for (int j=i+1; j<global_vars[idx].dim; j++)
					dimofs *= global_vars[idx].dims[j];
				d += dimofs;
			}
			if (d>=0 && d<maxstr)
				ret = vcstring[d];
			else {
				vcerr("VCCore::ProcessString() - bad offset to vcstring[] (strARRAY) %d, valid range [0,%d).\n global string name: '%s'", d, maxstr, global_vars[idx].name);
			}
			break;
		}
		case strHSTR0:   // _READSTR
		{
			int idx = currentvc->GrabD();
			ret = ReadHvar_str(c,idx,0);
			break;
		}
		case strHSTR1:
		{
			int idx = currentvc->GrabD();
			int arg = ResolveOperand();
			ret = ReadHvar_str(c,idx,arg);
			break;
		}
		case strINT: {
			char buf[16];
			sprintf(buf,"%d",ResolveOperand());
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
		// Local callback invocation.
		case cbLOCAL:
			d = currentvc->GrabD();
			c = currentvc->GrabC();
			if (c == opCBINVOKE)
			{
				if (d>=0 && d<40)
				{
					ExecuteCallback(cb_stack[cb_stack_base+d]);
				}
				else
				{
					vcerr("ProcessString: (string invocation) bad offset to local callbacks");
				}
			}
			else
			{
				vcerr("VCCore::ProcessString() - Corrupt callback invocation");
			}
			return vcretstr;
		// Callback operation (global var)
		case cbGLOBAL:
			d = currentvc->GrabD();
			d = global_vars[d].ofs;
			if (d>=0 && d<maxcb)
			{
				temp = currentvc->GrabC();
				if(temp == opCBINVOKE)
				{
					ExecuteCallback(vccallback[d]);
					return vcretstr;
				}
				else
				{
					vcerr("VCCore::ProcessString() - Corrupt callback invocation");
				}
			}
			else
				vcerr("VCCore::ProcessString(): bad offset to vc_callbacks (var), %d", d);
			break;
		// Callback operation (global array)
		case cbARRAY:
		{
			int idx = currentvc->GrabD();
			d = global_vars[idx].ofs;
			for (int i=0; i<global_vars[idx].dim; i++)
			{
				int dimofs = ResolveOperand();
				for (int j=i+1; j<global_vars[idx].dim; j++)
					dimofs *= global_vars[idx].dims[j];
				d += dimofs;
			}
			if (d>=0 && d<maxcb)
			{
				temp = currentvc->GrabC();
				if(temp == opCBINVOKE)
				{
					ExecuteCallback(vccallback[d]);
					return vcretstr;
				}
				else
				{
					vcerr("VCCore::ProcessString() - Corrupt callback invocation");
				}
			}
			else
				vcerr("VCCore::ProcessString(): bad offset to vc_callbacks (var), %d", d);
			break;
		}
		case strLIBFUNC:
			currentvc->GrabC(); // skip opLIBFUNC
			HandleLibFunc(currentvc->GrabW());
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

VergeCallback VCCore::ResolveCallback()
{
	int d, idx;
	byte c, temp;
	c = currentvc->GrabC();

	VergeCallback cb = VergeCallback();
	cb.opType = c;
	switch(c)
	{
		// Reference to library function.
		case opLIBFUNC:
			cb.functionIndex = currentvc->GrabW();
			return cb;
		// Reference to user function.
		case opUSERFUNC:
			cb.cimage = currentvc->GrabC();
			cb.functionIndex = currentvc->GrabD();
			return cb;
		// Return value from user function.
		case cbUSERFUNC:
			currentvc->GrabC(); // skip opUSERFUNC
			temp = currentvc->GrabC(); // to ensure correct order
			ExecuteUserFunc(temp, currentvc->GrabD());			
			return vcretcb;
		case cbLOCAL:
			d = currentvc->GrabD();
			if (d>=0 && d<40)
			{
				temp = currentvc->GrabC();
				// Copying another callback variable.
				if(temp == opCBCOPY)
				{
					return cb_stack[cb_stack_base + d];
				}
				// Copying the return value from the invocation of some callback.
				else if(temp == opCBINVOKE)
				{
					ExecuteCallback(cb_stack[cb_stack_base + d]);
					return vcretcb;
				}
				// Huh.
				else
				{
					vcerr("VCCore::ResolveCallback() - unknown callback reference opcode!");
				}
			}
		// Callback operation (global var)
		case cbGLOBAL:
			d = currentvc->GrabD();
			d = global_vars[d].ofs;
			if (d>=0 && d<maxcb)
			{
				temp = currentvc->GrabC();
				if(temp == opCBCOPY)
				{
					return vccallback[d];
				}
				else if(temp == opCBINVOKE)
				{
					ExecuteCallback(vccallback[d]);
					return vcretcb;
				}
			}
			else
				vcerr("VCCore::ResolveCallback(): bad offset to vc_callbacks (var), %d", d);
			break;
		// Callback operation (global array)
		case cbARRAY:
			idx = currentvc->GrabD();
			d = global_vars[idx].ofs;
			for (int i=0; i<global_vars[idx].dim; i++)
			{
				int dimofs = ResolveOperand();
				for (int j=i+1; j<global_vars[idx].dim; j++)
					dimofs *= global_vars[idx].dims[j];
				d += dimofs;
			}
			if (d>=0 && d<maxcb)
			{
				temp = currentvc->GrabC();
				if(temp == opCBCOPY)
				{
					return vccallback[d];
				}
				else if(temp == opCBINVOKE)
				{
					ExecuteCallback(vccallback[d]);
					return vcretcb;
				}
			}
			else
				vcerr("VCCore::ResolveCallback(): bad offset to vc_callbacks (var), %d", d);
			break;			
		default:
			vcerr("VCCore:ResolveCallback() - Unsupported callback expression opcode.");
	}
	return cb;
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
			case opLIBFUNC:		HandleLibFunc(currentvc->GrabW()); break;
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
			case opRETCB:		vcretcb=ResolveCallback(); break;
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

void VCCore::ExecuteCallback(const VergeCallback& cb, bool argument_pass)
{
	if(cb.functionIndex == -1)
	{
		if(!argument_pass)
		{
			vcerr("Failed to invoke callback, because it wasn't properly initialized. Can't call a null or non-existant function!");
		}
		return;
	}
	else if(cb.opType == opUSERFUNC)
	{
		ExecuteUserFunc(cb.cimage, cb.functionIndex, argument_pass);
	}
	else if(cb.opType == opLIBFUNC)
	{
		HandleLibFunc(cb.functionIndex);
	}
	else
	{
		vcerr("ExecuteUserFunc: Invalid optype supplied for callback given! (%d)", currentvc->curpos() - 1);
	}
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
	int save_cbbase = cb_stack_base;
	int isp = int_stack_ptr;
	int ssp = str_stack_ptr;
	int csp = cb_stack_ptr; 

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
					PushCallback(VergeCallback());
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
					PushCallback(VergeCallback());
					break;
				case t_CALLBACK:
					PushInt(0);
					PushString(empty_string);
					if (argument_pass)
					{
						// We can't pass callback arguments to the argument_pass list used by CallFunction.
						vcerr("Expected argument %d of callback %s() to be a callback.", n, func->name);
					}
					else
					{
						PushCallback(ResolveCallback());
					}
					
					break;
				case t_VARARG:
					PushInt(0);
					PushString(empty_string);
					PushCallback(VergeCallback());
					break;
			}
		}
		for (;n<func->numlocals; n++)				// finish off allocating locals
		{
			PushInt(0);
			PushString(empty_string);
			PushCallback(VergeCallback());
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
					PushCallback(VergeCallback());
					break;
				case t_STRING:
					PushInt(0);
					PushString(vararg[n].string_value);
					PushCallback(VergeCallback());
					break;
				default:
					vcerr("Unknown type found in vararg list", n, func->name);
			}
		}
	}

	int_stack_base = isp;
	str_stack_base = ssp;
	cb_stack_base = csp;

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
			PopCallback();
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
	cb_stack_base = save_cbbase;
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
			case 127: //trigger.afterPlayerMove
					_trigger_afterPlayerMove = ResolveString();
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
				if(arg >= 0 && arg < sprites.size()) {
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
		offset = global_vars[offset].ofs;
		c = currentvc->GrabC();
		if (c != aSET)
			vcerr("VC execution error: Corrupt string assignment");
		if (offset>=0 && offset<maxstr)
			vcstring[offset] = ResolveString();
		else
			vcerr("HandleAssign: bad offset to vc_strings (var), %d", offset);
		return;
	}
	// string array assignment
	if (c == strARRAY)
	{
		int idx = currentvc->GrabD();
		base = global_vars[idx].ofs;
		offset = 0;
		for (int i=0; i<global_vars[idx].dim; i++)
		{
			int dimofs = ResolveOperand();
			for (int j=i+1; j<global_vars[idx].dim; j++)
				dimofs *= global_vars[idx].dims[j];
			offset += dimofs;
		}
		base += offset;
		c = currentvc->GrabC();
		if (c != aSET)
			vcerr("VC execution error: Corrupt string assignment");
		if (base>=0 && base<maxstr)
			vcstring[base] = ResolveString();
		else
			vcerr("HandleAssign: bad offset to vc_strings (arr), %d %d %d", base, offset, base - offset);
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
	// Callback operation (local)
	if(c == cbLOCAL)
	{
		offset = currentvc->GrabD();
		c = currentvc->GrabC();
		
		if (offset>=0 && offset<40)
		{
			if (c == aSET)
			{
				cb_stack[cb_stack_base+offset] = ResolveCallback();
			}
			else if (c == opCBINVOKE)
			{
				ExecuteCallback(cb_stack[cb_stack_base+offset]);
			}
			else
			{
				vcerr("VCCore::HandleAssign() - Corrupt local callback operation");
			}
		}
		else
		{
			vcerr("HandleAssign: bad offset to local callbacks");
		}
		return;
	}
	// Callback operation (global var)
	if(c == cbGLOBAL)
	{
		offset = currentvc->GrabD();
		offset = global_vars[offset].ofs;
		c = currentvc->GrabC();
		if (offset>=0 && offset<maxcb)
		{
			if(c == aSET)
			{
				vccallback[offset] = ResolveCallback();
			}
			else if(c == opCBINVOKE)
			{
				ExecuteCallback(vccallback[offset]);
			}
			else
			{
				vcerr("VCCore::HandleAssign() - Corrupt global callback operation (var)");
			}
		}
		else
			vcerr("HandleAssign: bad offset to vc_callbacks (var), %d", offset);
		return;
	}
	// Callback operation (global array)
	if(c == cbARRAY)
	{
		int idx = currentvc->GrabD();
		offset = global_vars[idx].ofs;
		for (int i=0; i<global_vars[idx].dim; i++)
		{
			int dimofs = ResolveOperand();
			for (int j=i+1; j<global_vars[idx].dim; j++)
				dimofs *= global_vars[idx].dims[j];
			offset += dimofs;
		}
		c = currentvc->GrabC();
		if (offset>=0 && offset<maxcb)
		{
			if(c == aSET)
			{
				vccallback[offset] = ResolveCallback();
			}
			else if(c == opCBINVOKE)
			{
				ExecuteCallback(vccallback[offset]);
			}
			else
			{
				vcerr("VCCore::HandleAssign() - Corrupt global callback operation (arr)");
			}
		}
		else
			vcerr("HandleAssign: bad offset to vc_callbacks (arr), %d", offset);
		return;
	}

	// integer assignment
	switch (c)
	{
		case intGLOBAL:
			base = global_vars[currentvc->GrabD()].ofs;
			break;
		case intARRAY:
		{
			int idx = currentvc->GrabD();
			base = global_vars[idx].ofs;
			offset = 0;
			for (int i=0; i<global_vars[idx].dim; i++)
			{
				int dimofs = ResolveOperand();
				for (int j=i+1; j<global_vars[idx].dim; j++)
					dimofs *= global_vars[idx].dims[j];
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
	for (int i=0; i<global_vars.size(); i++)
	{
		if (!strcasecmp(global_vars[i].name, srcname))
		{
			src = i;
			src_type = t_INT;
		}
		else if (!strcasecmp(global_vars[i].name, destname))
		{
			dest = i;
			dest_type = t_INT;
		}
	}

	// Look for string arrays that match src or dest
	for (int i=0; i<global_vars.size(); i++)
	{
		if (!strcasecmp(global_vars[i].name, srcname))
		{
			src = i;
			src_type = t_STRING;
		}
		else if (!strcasecmp(global_vars[i].name, destname))
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
		for (int i = 0; i < global_vars[src].dim; i++)
		{
			if (global_vars[src].dims[i] != global_vars[dest].dims[i])
			{
				return false;
			}
		}
		// Copy!
		for (int i = 0; i < global_vars[src].len; i++)
		{
			vcint[global_vars[dest].ofs + i]
				= vcint[global_vars[src].ofs + i];
		}
	}
	else if (src_type == t_STRING)
	{
		// If arrays aren't same dimensions, return.
		for (int i = 0; i < global_vars[src].dim; i++)
		{
			if (global_vars[src].dims[i] != global_vars[dest].dims[i])
			{
				return false;
			}
		}
		// Copy!
		for (int i = 0; i < global_vars[src].len; i++)
		{
			vcstring[global_vars[dest].ofs + i]
				= vcstring[global_vars[src].ofs + i];
		}
	}

	// Success!
	return true;
}

void VCCore::SetInt(const char *intname, int value)
{
	for (int i=0; i<global_vars.size(); i++)
		if (!strcasecmp(global_vars[i].name, intname))
		{
			vcint[global_vars[i].ofs] = value;
			return;
		}
}

int VCCore::GetInt(const char *intname)
{
	for (int i=0; i<global_vars.size(); i++)
		if (!strcasecmp(global_vars[i].name, intname))
			return vcint[global_vars[i].ofs];

	return 0;
}

void VCCore::SetStr(CStringRef strname, CStringRef value)
{
	for (int i=0; i<global_vars.size(); i++)
		if (!strcasecmp(global_vars[i].name, strname.c_str()))
		{
			vcstring[global_vars[i].ofs] = value;
			return;
		}
}

CStringRef VCCore::GetStr(const char *strname)
{
	for (int i=0; i<global_vars.size(); i++)
		if (!strcasecmp(global_vars[i].name, strname))
			return vcstring[global_vars[i].ofs];

	return empty_string;
}

bool VCCore::StrExists(const char *strname)
{
	for (int i=0; i<global_vars.size(); i++)
		if (!strcasecmp(global_vars[i].name, strname))
			return true;

	return false;
}

bool VCCore::IntExists(const char *intname)
{
	for (int i=0; i<global_vars.size(); i++)
		if (!strcasecmp(global_vars[i].name, intname))
			return true;

	return false;
}

void VCCore::SetIntArray(const char *intname, int index, int value)
{
	for (int i=0; i<global_vars.size(); i++)
		if (!strcasecmp(global_vars[i].name, intname) && index>=0 && index<global_vars[i].len)
		{
			vcint[global_vars[i].ofs+index] = value;
			return;
		}
}

int VCCore::GetIntArray(const char *intname, int index)
{
	for (int i=0; i<global_vars.size(); i++)
		if (!strcasecmp(global_vars[i].name, intname) && index>=0 && index<global_vars[i].len)
			return vcint[global_vars[i].ofs+index];

	return 0;
}

// aen 12/3/05 12:55am : fixed index checking against global_vars[i]->len to be global_vars[i]->len
void VCCore::SetStrArray(CStringRef strname, int index, CStringRef value)
{
	for (int i = 0; i < global_vars.size(); i++)
	{
		if (!strcasecmp(global_vars[i].name, strname.c_str()) && index >= 0 && index < global_vars[i].len)
		{
			vcstring[global_vars[i].ofs + index] = value;
			return;
		}
	}
}

// aen 12/3/05 12:52am : fixed looping through global_vars.size() and checking index against global_vars[i]->len
CStringRef VCCore::GetStrArray(CStringRef strname, int index)
{
	for (int i = 0; i < global_vars.size(); i++)
	{
		if (!strcasecmp(global_vars[i].name, strname.c_str()) && index >= 0 && index < global_vars[i].len)
		{
			return vcstring[global_vars[i].ofs + index];
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
	for (int i=0; i<global_vars.size(); i++)
	{
		char* name;
		switch(global_vars[i].type)
		{
			case t_INT: name = "int"; break;
			case t_STRING: name = "string"; break;
			default:
				err("Unknown variable type (can't decompile)");
		}

		fprintf(vcd, "%s %s", name, global_vars[i].name);
		for (int j=0; j<global_vars[i].dim; j++)
			fprintf(vcd,"[%d]",global_vars[i].dims[j]);
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
	fprintf(vcd, "%s(", libfuncs[c].name.c_str());

	for (quad i=0; i<libfuncs[c].argumentTypes.size(); i++)
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

void VCCore::WriteHvar_derived(int category, int loc, int ofs, int value)
{
	switch (category)
	{
		case intHVAR0:
			vcerr("Unknown HVAR0 (%d) (set %d)", loc, value);
			break;
		case intHVAR1:
			switch (loc)
			{
				// Overkill (2007-05-02): Variable argument lists.
				case 117: return SetIntArgument(ofs, value);

				default: vcerr("Unknown HVAR1 (%d, %d) (set %d)", loc, ofs, value);
			}
			break;
		default:
			vcerr("Fatal Error Code Steamboat");
	}
}

int VCCore::ReadHvar_derived(int category, int loc, int ofs)
{
	switch(category)
	{
		case intHVAR0:
			switch(loc) {
				// Overkill (2007-05-02): Variable argument lists.
				case 116: return vararg_stack[vararg_stack.size() - 1].size();
				default: vcerr("Unknown HVAR0 (%d)", loc); 
			}
			return -1;
		case intHVAR1:
			switch(loc) {
				// Overkill (2007-05-02): Variable argument lists.
				case 117: return GetIntArgument(ofs);
				case 118: if (ofs >= 0 && ofs < vararg_stack[vararg_stack.size() - 1].size()) { return vararg_stack[vararg_stack.size() - 1][ofs].type_id == t_INT; } return 0;
				case 119: if (ofs >= 0 && ofs < vararg_stack[vararg_stack.size() - 1].size()) { return vararg_stack[vararg_stack.size() - 1][ofs].type_id == t_STRING; } return 0;
			default:
				vcerr("Unknown HVAR1 (%d, %d)", loc, ofs); 
			}
			return -1;

		default:
			vcerr("Fatal Error Code Eskimo");
			return -1;
	}
}
				

StringRef VCCore::ReadHvar_str_derived(int category, int loc, int ofs)
{
	switch(category)
	{
		case strHSTR0:
			vcerr("Unknown HSTR0 (%d)", loc); 
			break;
		case strHSTR1:
			switch (loc)
			{
				case 120: // Overkill (2007-02-05): Variable argument lists.
					return GetStringArgument(ofs);
				default:
					vcerr("Unknown HSTR1 (%d, %d)", loc, ofs); 
					break;
			}
			break;
		default:
			vcerr("Fatal Error Code Snail"); 
	}
	return empty_string;
}

void VCCore::WriteHvar_str_derived(int category, int loc, int ofs, CStringRef value)
{
	switch(category)
	{
		case strHSTR0:
			vcerr("Unknown HSTR0 (%d)", loc); 
			break;
		case strHSTR1:
			switch (loc)
			{
				case 120: // Overkill (2007-05-02): Variable argument lists.
					SetStringArgument(ofs, value);
					break;
				default:
					vcerr("Unknown HSTR1 (%d, %d)", loc, ofs); 
					break;
			}
			break;
		default:
			vcerr("Fatal Error Code Truculent"); 
	}
}
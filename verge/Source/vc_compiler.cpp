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
 * verge3: vc_compiler.cpp                                        *
 * copyright (c) 2002 vecna                                       *
 ******************************************************************/

#include "xerxes.h"
#include "opcodes.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

extern char *va(char* format, ...);
extern void err(const char* str, ...);
extern void log(char* str, ...);
extern char *stripext(char *s);
extern VCCore *vc;

int verbose = 0;

escape_sequence escape_codes[10] =
{
	escape_sequence("\\t", '\t'),
	escape_sequence("\\v", '\v'),
	escape_sequence("\\n", '\n'),
	escape_sequence("\\r", '\r'),
	escape_sequence("\\f", '\f'),
	escape_sequence("\\b", '\b'),
	escape_sequence("\\\\", '\\'),
	escape_sequence("\\'", '\''),
	escape_sequence("\\\"", '\"'),
	escape_sequence("\\0", '\0'),
};

//WHY do we need this??
int zstrcmp(const char *s1, const char *s2)
{
	while (true)
	{
//		if (tolower(*s1) != tolower(*s2))
		if (*s1 != *s2)
			return -1;
		if (!*s1)
			return 0;
		s1++;
		s2++;
	}
	return -1;
}

/***************** subclass Define *****************/

Define::Define(const char *k, const char *v, Define::Type type)
: type(type)
, key(k)
, value(v)
{
}

Define::~Define()
{
}

/***************** subclass HVar *****************/

HVar::HVar(char *n, char *d)
{
	name = new char[strlen(n)+1];
	strcpy(name, n);
	dim = strlen(d);
}

HVar::~HVar()
{
	delete[] name;
}

/***************** helperclass chunk *****************/

Chunk::Chunk()
: ptr(0)
{
}

Chunk::~Chunk()
{
}

void Chunk::LoadChunk(FILE *f)
{
	int size;
	fread_le(&size, f);
	chunk.resize(size);
	fread(&chunk[0], 1, size, f);
}

void Chunk::Append(FILE *f, bool save_pos)
{
	int pos = curpos();
	int size;
	fread_le(&size, f);
	int oldsize = chunk.size();
	int cursize = chunk.size() + size;

	//int temp = cursize / CHUNK_UNIT;
	//if(temp*CHUNK_UNIT != cursize)
	//	temp++;
	//chunksize = temp * CHUNK_UNIT;
	//char *buf = new char[chunksize];
	//memcpy(buf, chunk, oldsize);
	//delete[] chunk;
	//chunk = buf;
	//ptr = chunk;
	fread((char*)&chunk[0]+oldsize, 1, size, f);

	if(save_pos)
		setpos(pos);
}

void Chunk::SaveChunk(FILE *f)
{
	int size = chunk.size();
	int tempsize = size;
	flip(&tempsize);
	fwrite(&tempsize, 1, 4, f);
	fwrite(&chunk[0], 1, size, f);
}

int Chunk::curpos()
{
	return ptr;
}

int Chunk::setpos(int p)
{
	//mbg 27-nov-08 this doesnt look helpful
	//if (p<0)
	//	p = 0;
	//if (p>cursize)
	//	p = cursize;

	ptr = p;
	return p;
}

byte Chunk::GrabC()
{
	//mbg 27-nov-08 this doesnt look helpful
	//if (ptr+1>chunk+cursize)
	//	return 0;
	return (byte)refc();
}

word Chunk::GrabW()
{
	//mbg 27-nov-08 this doesnt look helpful
	//if (ptr+2>chunk+cursize)
	//	return 0;

#ifdef CRIPPLED
	char buf[2];
	buf[0] = readc();
	buf[1] = readc();
	return *(word *)buf;
#else
	word x = *(word *)(&chunk[ptr]);
	flip(&x);
	ptr += 2;
	return x;
#endif
}

quad Chunk::GrabD()
{
	//mbg 27-nov-08 this doesnt look helpful
	//if (ptr+4>chunk+cursize)
	//	return 0;

#ifdef CRIPPLED
	char buf[4];
	buf[0] = refc();
	buf[1] = refc();
	buf[2] = refc();
	buf[3] = refc();
	return *(quad *)buf;
#else
	quad x = *(quad *)(&chunk[ptr]);
	flip(&x);
	ptr+=4;
	return x;
#endif
}

CStringRef Chunk::GrabString()
{
	static int ctr = 0;
	//try to find this address in the string table
	TStringTable::iterator find = stringTable.find(ptr);
	CStringRef ret = 
		(find == stringTable.end()) 
			//need to add it to the string table
		? (stringTable[ptr] = StringRef(&chunk[ptr]))
		: find->second ;

	//advance code pointer
	ptr += ret.length()+1;
	return ret;
}

char Chunk::operator[](quad n) const
{
	//mbg 27-nov-08 this didnt look like helpful error checking,
	//BUT this is used for EOF detection by the compiler.
	//so, i leave it
	if(n<chunk.size())
		return chunk[n];
	else return 0;
}

void Chunk::EmitC(byte c)
{
	Expand(1);
	refc() = c;
}

void Chunk::EmitW(word w)
{
	Expand(2);
#ifdef CRIPPLED
	refc() = w;
	refc() = (w>>8);
#else
	*(word *)(&chunk[ptr]) = w;
	ptr += 2;
#endif
}

void Chunk::EmitD(quad d)
{
	Expand(4);
#ifdef CRIPPLED
	refc() = d;
	refc() = (d>>8);
	refc() = (d>>16);
	refc() = (d>>24);
#else
	*(quad *)(&chunk[ptr]) = d;
	ptr += 4;
#endif
}

void Chunk::EmitString(char *s)
{
	//mbg 27-nov-08 i wanted to do something clever here, but it broke other things.

	int string_len = strlen(s);
	int todolen = string_len+1;

	Expand(todolen);

	while (*s)
		refc() = *s++;

	//null terminate
	refc()='\0';

	////allocate room for at least a pointer. also, make it 4B aligned
	//int string_len = strlen(s);
	//int todolen = string_len+1;
	//todolen = std::max(todolen,4);
	//quad sptr = (quad)ptr;
	//quad xptr = sptr + todolen;
	//xptr = (xptr+3)&~3;
	//todolen = xptr-sptr;
	//
	//Expand(todolen);

	//while (*s)
	//	refc() = *s++;

	////null terminate and pad
	//for(int i=string_len;i<todolen;i++)
	//	refc()='\0';
}

void Chunk::SetD(int ofs, quad d)
{
	*((int *)&(chunk[ofs])) = d;
}

void Chunk::Expand(int amount)
{
	chunk.resize(chunk.size()+amount);
}

void Chunk::become(Chunk *c)
{
	chunk = c->chunk;
	ptr = chunk.size();
}

void Chunk::clear()
{
	chunk.clear();
	ptr = 0;
}

/***************** struct utility classes *****************/

global_var_t::global_var_t()
{
	memset(name, 0, IDENTIFIER_LEN);
	type = 0;
	ofs = 0;
	len = 0;
	dim = 0;
}

global_var_t::global_var_t(FILE *f)
{
	memset(&ext, 0, sizeof (ext));

	fread(name, 1, IDENTIFIER_LEN, f);
	fread(&type, 1, 1, f);
	fread_le(&ofs, f);
	fread_le(&len, f);
	fread_le(&dim, f);
	dims.resize(dim);
	for (int i = 0; i < dim; i++)
	{
		int mydimsize;
		fread_le(&mydimsize, f);
		dims[i] = mydimsize;
	}
}

global_var_t::~global_var_t()
{
	if(type == t_CALLBACK)
	{
		delete ext.callback;
	}
}

void global_var_t::write(FILE *f)
{
	fwrite(name, 1, IDENTIFIER_LEN, f);
	fwrite(&type, 1, 1, f);
	fwrite(&ofs, 1, 4, f);
	fwrite(&len, 1, 4, f);
	fwrite(&dim, 1, 4, f);
	for (int i = 0; i < dim; i++)
	{
		int mydimsize = dims[i];
		fwrite(&mydimsize, 1, 4, f);
	}
}

struct_element::struct_element()
{
	memset(&type, 0, 1);
	memset(name, 0, IDENTIFIER_LEN);
	memset(type_name, 0, IDENTIFIER_LEN);
	len = 0;
	dim = 0;
}

struct_element::struct_element(FILE *f)
{
	memset(&ext, 0, sizeof (ext));

	fread(&type, 1, 1, f);
	fread(name, 1, IDENTIFIER_LEN, f);
	fread(type_name, 1, IDENTIFIER_LEN, f);
	fread_le(&len, f);
	fread_le(&dim, f);
	dims.resize(dim);
	for (int i = 0; i < dim; i++)
	{
		int mydimsize;
		fread_le(&mydimsize, f);
		dims[i] = mydimsize;
	}
	//log("Loaded %s of type '%d' type name '%s'.", name, type, type_name);
}

struct_element::~struct_element()
{
	if(type == t_CALLBACK)
	{
		delete ext.callback;
	}
}

void struct_element::write(FILE *f)
{
	fwrite(&type, 1, 1, f);
	fwrite(name, 1, IDENTIFIER_LEN, f);
	fwrite(type_name, 1, IDENTIFIER_LEN, f);
	fwrite(&len, 1, 4, f);
	fwrite(&dim, 1, 4, f);
	for (int i = 0; i < dim; i++)
	{
		int mydimsize = dims[i];
		fwrite(&mydimsize, 1, 4, f);
	}
}

bool struct_element::equals(struct_element *rhs)
{
	// Compare variable type.
	if (this->type != rhs->type)
	{
		return false;
	}
	// Compare variable name, and variable's type's name.
	if (strcmp(this->name, rhs->name)
		|| strcmp(this->type_name, rhs->type_name))
	{
		return false;
	}
	// Compare length and number of dimensions.
	if (this->len != rhs->len || this->dim != rhs->dim)
	{
		return false;
	}
	// Compare each dimension.
	for (int i = 0; i < this->dim; i++)
	{
		if (this->dims[i] != rhs->dims[i])
		{
			return false;
		}
	}

	// Equal elements!
	return true;
}

struct_definition::struct_definition()
{
	memset(name, 0, IDENTIFIER_LEN);
}

struct_definition::struct_definition(FILE *f)
{
	fread(name, 1, IDENTIFIER_LEN, f);
	int element_count;
	fread_le(&element_count, f);
	elements.resize(element_count);
	for (int i = 0; i < element_count; i++)
	{
		elements[i] = new struct_element(f);
	}
}

struct_definition::~struct_definition()
{
	for (int j = 0; j < elements.size(); j++)
	{
		delete elements[j];
	}
}

void struct_definition::write(FILE *f)
{
	fwrite(name, 1, IDENTIFIER_LEN, f);
	int element_count = elements.size();
	fwrite(&element_count, 1, 4, f);
	for (int i = 0; i < element_count; i++)
	{
		elements[i]->write(f);
	}
}

bool struct_definition::equals(struct_definition *rhs)
{
	// Names are different.
	if (strcmp(this->name, rhs->name))
	{
		return false;
	}

	// Two struct defintions aren't equal
	// if they have different element count.
	if (this->elements.size() != rhs->elements.size())
	{
		return false;
	}

	// Now compare every variable.
	for (int i = 0; i < this->elements.size(); i++)
	{
		if (!this->elements[i]->equals(rhs->elements[i]))
		{
			return false;
		}
	}
	return true;
}

struct_instance::struct_instance()
: is(0)
, dim(0)
{
	name[0] = 0;
}

struct_instance::struct_instance(FILE *f)
{
	fread(name, 1, IDENTIFIER_LEN, f);
	fread_le(&dim,f);
	dims.resize(dim);
	for (int i = 0; i < dim; i++)
	{
		int mydimsize;
		fread_le(&mydimsize, f);
		dims[i] = mydimsize;
	}
	is = new struct_definition(f);
}

void struct_instance::write(FILE *f)
{
	fwrite(name, 1, IDENTIFIER_LEN, f);
	fwrite(&dim, 1, 4, f);
	dims.resize(dim);
	for (int i = 0; i < dim; i++)
	{
		int mydimsize = dims[i];
		fwrite(&mydimsize, 1, 4, f);
	}
	is->write(f);
}

callback_definition::callback_definition()
{
	memset(argtype, 0, sizeof (argtype));
	numargs = signature = 0;
}

callback_definition::~callback_definition()
{
	if(signature == t_CALLBACK)
	{
		delete sigext.callback;
	}
	for(int i = 0; i < numargs; i++)
	{
		if(argtype[i] == t_CALLBACK)
		{
			delete argext[i].callback;
		}
	}
}

function_t::function_t()
{
	memset(name, 0, IDENTIFIER_LEN);
	memset(argtype, 0, sizeof (argtype));
	memset(localnames, 0, sizeof (localnames));
	numargs = numlocals = signature = codeofs = coreimage = 0;

	memset(&sigext, 0, sizeof (sigext));
	memset(&argext, 0, sizeof (argext));
}

function_t::function_t(FILE *f)
{
	memset(argtype, 0, sizeof (argtype));
	memset(localnames, 0, sizeof (localnames));

	memset(&sigext, 0, sizeof (sigext));
	memset(&argext, 0, sizeof (argext));

	fread_le(&numargs, f);
	fread_le(&numlocals, f);
	fread_le(&signature, f);
	fread_le(&codeofs, f);
	fread_le(&codeend, f);
	fread(&coreimage, 1, 1, f);
	fread(name, 1, IDENTIFIER_LEN, f);
	fread(argtype, 1, numlocals, f);
	fread(localnames, IDENTIFIER_LEN, numlocals, f);
}

function_t::~function_t()
{
	if(signature == t_CALLBACK)
	{
		delete sigext.callback;
	}
	for(int i = 0; i < numargs; i++)
	{
		if(argtype[i] == t_CALLBACK)
		{
			delete argext[i].callback;
		}
	}
}

void function_t::write(FILE *f)
{
	fwrite(&numargs, 1, 4, f);
	fwrite(&numlocals, 1, 4, f);
	fwrite(&signature, 1, 4, f);
	fwrite(&codeofs, 1, 4, f);
	fwrite(&codeend, 1, 4, f);
	fwrite(&coreimage, 1, 1, f);
	fwrite(name, 1, IDENTIFIER_LEN, f);
	fwrite(argtype, 1, numlocals, f);
	fwrite(localnames, IDENTIFIER_LEN, numlocals, f);
}

/***************** class VCCompiler *****************/
#ifdef ALLOW_SCRIPT_COMPILATION
VCCompiler::VCCompiler(FileServer* file_server)
: lexer_initialized(false)
{
    if (NULL == file_server) {
        file_server = new FileServer();
    }
    this->file_server = file_server;

	vcerror = false;
	remove(VCLOG);

	vprint("Constructing VCCompiler object... ");

	// initialize library variables
	for (int i=0; i<NUM_HVARS; i++)
		hvars.push_back(new HVar(libvars[i][1], libvars[i][2]));
	vprint("HVARS...\n");

	// initialize built-in defines
	for (int i=0; i<NUM_HDEFS; i++)
		defines.push_back(new Define(hdefs[i].key, hdefs[i].value, hdefs[i].type));
	vprint("HDEFS...\n");

    // don't add source files unless we're compiling
    // something that we know should be added
    add_source_files = false;

	vprint("Done.\n");
}

VCCompiler::~VCCompiler()
{
	vprint("Deconstructing VCCompiler object... ");
	int i,j;
	// free defines
	for (i=0; i<defines.size(); i++)
	{
		delete defines[i];
	}
	// free hvars
	for (i=0; i<hvars.size(); i++)
	{
		delete hvars[i];
	}
	// free global ints
	for (i=0; i<global_vars.size(); i++)
	{
		delete global_vars[i];
	}
	// free global functions
	for (i=0; i<NUM_CIMAGES;i++)
	{
		for (j=0; j<funcs[i].size(); j++)
		{
			delete funcs[i][j];
		}
	}
	// free struct instances
	for (i=0; i<struct_instances.size(); i++)
	{
		delete struct_instances[i];
	}
	// free struct types
	for (i=0; i<struct_defs.size(); i++)
	{
		delete struct_defs[i];
	}
	// free alias types
	for (i=0; i<alias_defs.size(); i++)
	{
		delete alias_defs[i];
	}

	vprint("Done.");
}

void VCCompiler::vprint(char *message, ...)
{
	va_list lst;
	char string[1024];

	if (!verbose)
		return;

	va_start (lst, message);
	vsprintf (string, message, lst);
	va_end   (lst);

	FILE *f = fopen(VCLOG, "a");
	fputs(string, f);
	fclose(f);
}


bool VCCompiler::CompileAll()
{
	bool result = true;

	// compile to the system image
	target_cimage = CIMAGE_SYSTEM;

	// we only want to permit duplicates when patchin
	permit_duplicates = false;

	// only search system image for funcs
	reference_cimages.clear();
	reference_cimages.push_back(CIMAGE_SYSTEM);

	// system always starts with no functions
	funcs[CIMAGE_SYSTEM].clear();
	precompile_numfuncs = 0;

	try
	{
		debugon = false;
        // we add all the system and included source files
        add_source_files = true;
		PreProcess("system.vc");
		ScanPass(SCAN_ALL);
		CompilePass();
	}
    catch (LexerNotInitializedException lni) {
		(void)lni;
        sprintf(errmsg, "Lexer not initialized");
        result = false;
    } catch (CircularIncludeException cie) {
        sprintf(errmsg, cie.message.c_str());
        result = false;
    }
	catch (const char *str)
	{
		sprintf(errmsg, str);
		result = false;
	}

    add_source_files = false; // turn off for next compile

	log("VC System Compilation stats: \n%d globals (%d expanded), %d functions, %d total lines\n",
		 global_vars.size(), global_var_offset, funcs[CIMAGE_SYSTEM].size(), pp_total_lines);

	return result;
}

debuginfo VCCompiler::GetDebugForOffset(int ofs)
{
	debuginfo debug;
	debug.funcname[0] = 0;
	debug.sourcefile[0] = 0;
	debug.linenum = 0;
	precompile_numfuncs = 0;
	target_cimage = 0;
	// avoid errors due to duplicates
	permit_duplicates = true;

	try
	{
		PreProcess("system.vc");
		ScanPass(SCAN_IGNORE_NON_FUNC);

		debugon = true;
		debugofs = ofs;
		CompilePass();
	}
	catch (const char*)
	{
		debug.linenum = linenum;
		strcpy(debug.sourcefile, sourcefile);
	}
	debugon = false;
	return debug;
}


void VCCompiler::ExportSystemXVC()
{
	char xvc_sig[8] = "VERGE30";
	int ver = 1, size, i;

	FILE *f = fopen("system.xvc", "wb");
	if (!f)
		throw "VCCompiler::ExportSystemXVC - Could not open system.xvc for writing!";
	fwrite(xvc_sig, 1, 8, f);
	fwrite(&ver, 1, 4, f);

	size = global_vars.size();
	fwrite(&size, 1, 4, f);
	for (i=0; i<size; i++)
		global_vars[i]->write(f);

	size = struct_instances.size();
	fwrite(&size, 1, 4, f);
	for (i=0; i<size; i++)
		struct_instances[i]->write(f);

	size = funcs[CIMAGE_SYSTEM].size();
	fwrite(&size, 1, 4, f);
	for (i=0; i<size; i++)
		funcs[CIMAGE_SYSTEM][i]->write(f);

	output.SaveChunk(f);
	fclose(f);
}


bool VCCompiler::CompileMap(const char *f)
{
	bool result = true;

	vprint("Compiling %s.map", f);

	// compile to the map image
	target_cimage = CIMAGE_MAP;

	// don't allow duplicates
	permit_duplicates = false;

	// search system and map images for funcs
	reference_cimages.clear();
	reference_cimages.push_back(CIMAGE_MAP);
	reference_cimages.push_back(CIMAGE_SYSTEM);

	// maps never compile against other maps; we get rid of
	// any functions already here (but save them to restore
	// at the end in case the compile fails)
	std::vector<function_t*> old_funcs;
	funcs[CIMAGE_MAP].swap(old_funcs);
	precompile_numfuncs = 0; // never have any funcs in the target image

	try
	{
		PreProcess(va("%s.vc",f));
		ScanPass(SCAN_ERR_NON_FUNC);
		CompileMapPass();
	}
	catch (const char* str)
	{
		sprintf(errmsg, str);
		result = false;
	}

	// get num funcs
	int s = funcs[CIMAGE_MAP].size();
	int i;

	// if we're successful, write the map to the file
	if(result) {
		VFILE *mi = vopen(va("%s.map", f));
		if (!mi) err("unable to open %s.map", f);
		vseek(mi, 10, 0);

		int mapcoresize;
		vread(&mapcoresize, 4, mi);
		//log("DESTINATION POS: %d", mapcoresize);
		vseek(mi, 0, 0);
		byte* buf = new byte[mapcoresize];
		vread(buf, mapcoresize, mi);
		vclose(mi);

		FILE *mo = fopen(va("%s.map", f), "wb");
		if (!mo)
			err("couldn't open map for writing");
		fwrite(buf, 1, mapcoresize, mo);
		delete[] buf;

		// write num funcs
		//log("CURRENT POS: %d", ftell(mo));
		fwrite(&s, 1, 4, mo);
		for (i=0; i < s; i++)
			funcs[CIMAGE_MAP][i]->write(mo);
		log("Compiled %s.map, (%d lines), %d functions", f, pp_total_lines, s);
		int z = output.chunk.size();
		fwrite(&z, 1, 4, mo);
		fwrite(&output.chunk[0], 1, z, mo);
		fclose(mo);
	}

	// unload map functions (these are re-added
	// when maps are loaded into the core so that
	// new code can compile against it)
	for (i=0; i<s; i++)
	{
		delete funcs[CIMAGE_MAP][i];
	}

	// now restore old function list so we can keep going
	// if this compile failed or we don't load the new map core
	funcs[CIMAGE_MAP].swap(old_funcs);

	return result;
}


bool VCCompiler::CompileOther(char *f, int cimage, bool append, bool dups, std::vector<int>ref_cimages)
{
	bool result = true;

	// compile to the image given
	target_cimage = cimage;

	permit_duplicates = dups; // just go with what the caller wants

	// search ourselves for funcs, plus ref_cimages
	reference_cimages.clear();
	reference_cimages.push_back(cimage);
	for(int i = 0; i < ref_cimages.size(); i++) {
		// don't add ourselves again
		if(cimage != ref_cimages[i])
			reference_cimages.push_back(ref_cimages[i]);
	}
	// if we're not appending, clear the functions so far
	if(!append) {
		funcs[cimage].clear();
	}

	// since others can compile against themselves
	// (ie they can be later appended)
	// we have to ensure to only write functions we
	// compile this time
	precompile_numfuncs = funcs[cimage].size();

	try
	{
		PreProcess(f);
		ScanPass(SCAN_IGNORE_NON_FUNC);
		CompileOtherPass(append);
	}
	catch (const char* str)
	{
		sprintf(errmsg, str);
		result = false;
	}

	// get numfuncs
	int s = funcs[cimage].size()-precompile_numfuncs;
	int i;

	// write if successful
	if(result) {
		FILE * out = fopen(va("%s.out", f), "wb");
		if(!out)
			err("Couldn't open output file for temp compilation");

		fwrite(&s, 1, 4, out);
		for (i=precompile_numfuncs; i<funcs[cimage].size(); i++)
			funcs[cimage][i]->write(out);
		log("Compiled %s, (%d lines)  %d functions", f, pp_total_lines, s);

		// we also only write compiled code made after
		// the core we're appending to unless we're not appending
		int previous_size = append  ? vc->GetCore(target_cimage)->size()
									: 0;
		int z = output.size() - previous_size;
		int tempz = z;
		flip(&tempz);
		fwrite(&tempz, 1, 4, out);
		fwrite(&output.chunk[previous_size], 1, z, out);
		fclose(out);
	}

	for (i=funcs[cimage].size()-1; i>=precompile_numfuncs; i--)
	{
		delete funcs[cimage][i];
		funcs[cimage].pop_back();
	}

	return result;
}

void VCCompiler::PushFunction(int cimage, function_t *f) {
	// called when core loads new image so we can
	// compile against it
	funcs[cimage].push_back(f);
}

void VCCompiler::ClearFunctionList(int cimage) {
	// called when core unloads an image, so we don't
	// compile new images against it.
	funcs[cimage].clear();
}


/**************** Preprocessor Component ****************/

enum
{
    PP_ERROR,
    PP_WHITE,
    PP_PUNC,
    PP_DIGIT,
    PP_LETTER,
    PP_QUOTE,
    PP_DIRECTIVE,
    PP_NEWLINE
};

bool VCCompiler::PreProcess(char *fn)
{
	Init_pptbl();
	pp_total_lines = 0;
	pp_included_files.clear();
	source.clear();
	bool result = Process(fn);

// FIXME: When preprocessing is complete, the #defines list should
//        revert to only having the built-in #defines
//        But this will break incremental recompilation, since we
//        would always need to recompile all the included files,
//        or else we'd probably never be able to compile anything.
	return result;
}

void VCCompiler::Init_pptbl()
{
	int x;
	memset(pptbl, PP_ERROR, 256);

	for (x='A'; x<='Z'; x++) pptbl[x] = PP_LETTER;
	for (x='a'; x<='z'; x++) pptbl[x] = PP_LETTER;
	pptbl['_'] = PP_LETTER;

	for (x='0'; x<='9'; x++) pptbl[x] = PP_DIGIT;
	pptbl['$'] = PP_DIGIT;
	pptbl['\''] = PP_DIGIT;

	pptbl['+'] = pptbl['-'] = PP_PUNC;
	pptbl['*'] = pptbl['/'] = PP_PUNC;
	pptbl['%'] = PP_PUNC;
	pptbl['|'] = pptbl['&'] = PP_PUNC;
	pptbl['='] = pptbl['^'] = PP_PUNC;
	pptbl[','] = PP_PUNC;
	pptbl['.'] = PP_PUNC;
	pptbl['<'] = pptbl['>'] = PP_PUNC;
	pptbl['('] = pptbl[')'] = PP_PUNC;
	pptbl['['] = pptbl[']'] = PP_PUNC;
	pptbl['{'] = pptbl['}'] = PP_PUNC;
	pptbl[':'] = pptbl[';'] = PP_PUNC;
	pptbl['\\'] = pptbl['!'] = PP_PUNC;
	pptbl['~'] = PP_PUNC;

	pptbl[' '] = pptbl['\t'] = PP_WHITE;
	pptbl['\r'] = PP_WHITE;
	pptbl['\n'] = PP_NEWLINE;
	pptbl['\"'] = PP_QUOTE;
	pptbl['#'] = PP_DIRECTIVE;

    lexer_initialized = true;
}

void VCCompiler::pp_filetag(char *fn)
{
	source.EmitC(PP_FILETAG);
	source.EmitString(fn);
}

void VCCompiler::pp_linetag(int line)
{
	source.EmitC(PP_LINETAG);
	source.EmitD(line);
}

void VCCompiler::stripdefinevalue(char *v)
{
	for (int i=0; i<strlen(v); i++)
	{
		if ((v[i]=='/' && v[i+1]=='/') ||
			(v[i]=='/' && v[i+1]=='*'))
			v[i] = 0;
	}
}

void VCCompiler::check_for_circular_includes(char* filename) {
    for (int index = 0; index < pp_included_files.size(); index++) {
        if (!zstrcmp(filename,  pp_included_files[index])) {
            throw CircularIncludeException(va("preprocessor step: circular dependencies on %s", filename));
        }
    }
}

char* FileServer::fetch(char* filename) {
    VFILE* in = vopen(filename);
    if (!in) {
        throw va("preprocess step: could not open %s", filename);
    }

    int length_in_bytes = filesize(in);
    char* buf = new char[length_in_bytes + 2];
    memset(buf, 0, length_in_bytes + 2);

    vread(buf, length_in_bytes, in);
    vclose(in);

    buf[length_in_bytes++] = 0;
    buf[length_in_bytes] = 0;

    return buf;
}

void VCCompiler::collapse_whitespace(char*& code) {
    source.EmitC(' ');
    while (pptbl[*code++] == PP_WHITE) {
        // empty
    }
    code--;
}

bool VCCompiler::Process(char *fn)
{
	int  i;
	char *buf;

    if (!lexer_initialized) {
        throw LexerNotInitializedException();
    }

    check_for_circular_includes(fn);

	pp_included_files.push_back(fn); // no problem, add to list
		
    if(add_source_files) {
        AddSourceFile(fn);
    }

    try {
        buf = file_server->fetch(fn);
    } catch (char* pc) {
        pp_included_files.pop_back();
        throw pc;
    }

	// output initial filename and linenumber tags
	pp_filetag(fn);
	pp_linetag(1);

	int curline = 1;

	// main processing loop
	char *s = buf;
	while (*s)
	{
		char token_type = pptbl[*s];
		if (token_type == PP_WHITE)
		{
            collapse_whitespace(s);
			continue;
		}
		if (s[0]=='/' && s[1]=='/')			// c-style comment, skip to end of line
		{
			while (*s != '\n' && *s != 0) s++;			
			continue;
		}
		if (s[0]=='/' && s[1]=='*')			// c++ style comment. skip until */
		{
			s += 2; // skip those characters so /*/ is not a complete comment
			while (*s && !(s[0]=='*' && s[1] == '/'))
			{
				if (*s == '\n')
				{
					curline++;
					pp_total_lines++;
				}
				s++;
			}
			pp_linetag(curline);
			s+=2;			
			continue;
		}
		if (*s == '\n')						// newline
		{
			pp_linetag(++curline);
			pp_total_lines++;
			s++;
			continue;
		}
		if (*s == '\'')
		{
			source.EmitC(*s++);
			while (*s && *s != '\n' && *s != '\r' && *s != '\'')
			{
				if (IsEscapeSequence(s))
				{
					source.EmitC(*s++);
				}
				source.EmitC(*s++);
			}
			if (*s == '\'')
			{
				source.EmitC(*s++);
			}
		}
		if (*s == '#')						// crap, its a directive
		{
			// aen 12/4/05 10:12 : parse out full directive names
			s++;
			while (*s && (*s == ' ' || *s == '\t'))
			{
				s++;
			}
			if (!*s)
			{
				throw va("%s(%d) : fatal error: premature end of file", fn, curline);
			}
			char command[1024];
			int ci = 0;
			while (*s && *s != ' ' && *s != '\t' && *s != '\n' && *s != '\r')
			{
				command[ci++] = *s++;
			}
			command[ci] = 0;

			while (*s && (*s == ' ' || *s == '\t'))
			{
				s++;
			}
			//while
			if (!strcmp(command, "include"))
			{
				if (!*s)
				{
					throw va("%s(%d) : error: '#include' : expecting a quoted filename, found end of file", fn, curline);
				}
				else if (*s == '\n' || *s == '\r')
				{
					throw va("%s(%d) : error: '#include' : expecting a quoted filename, found newline", fn, curline);
				}
				if (*s != '"')
				{
					throw va("%s(%d) : error: '#include' : missing opening quote", fn, curline);
				}
				s++;
				char temp[256];
				memset(temp, 0, 256);
				while (*s && *s != '\n' && *s != '\r' && *s != '"')			// read through the last " mark
					temp[strlen(temp)] = *s++;
				if (!*s || *s == '\r' || *s == '\n')
				{
					throw va("%s(%d) : error: '#include' : missing closing quote", fn, curline);
				}

				bool r = Process(temp);		// Preprocess new file
				if (!r)
				{
					delete[] buf;
					pp_included_files.pop_back();
					return false;
				}
				while (*s && *s++ != '\n') {}
				pp_filetag(fn);
				pp_linetag(++curline);
				pp_total_lines++;
			}
			else if (!strcmp(command, "define"))
			{
				if (!*s)
				{
					throw va("%s(%d) : error: '#define' : expected an identifier, found end of file", fn, curline);
				}
				else if (*s == '\n' || *s == '\r')
				{
					throw va("%s(%d) : error: '#define' : expected an identifier, found newline", fn, curline);
				}
				char newkey[257];
				//char newvalue[256];
				memset(newkey, 0, 257);
				//memset(newvalue, 0, 256);
				//while (*s != ' ' && *s != '\t' && *s) s++; // skip to space after '#define'
				//while (*s == ' ' || *s == '\t') s++; // skip whitespace
				i = 0;
				int name_length = 0;
				char* macro_start = s;
				if (pptbl[*s] != PP_LETTER && pptbl[*s] != '_')
				{
					throw va("%s(%d): error : '#define' : macro name must be a valid identifier", fn, curline);
				}
				s++;
				name_length++;
				while ((pptbl[*s] == PP_LETTER || pptbl[*s] == PP_DIGIT) && *s != '$' && *s != '\'' && *s != ' ' && *s != '\t' && *s)
				{
					name_length++;
					s++;
				}
				if (!*s)
				{
					throw va("%s(%d): error: '#define' : unexpected end of file", fn, curline);
				}
				if (*s != ' ' && *s != '\n' && *s != '\r' && *s != '\t')
				{
					throw va("%s(%d): error: '#define' : invalid macro name", fn, curline);
				}
				if (name_length > 256)
				{
					throw va("%s(%d) : error: '#define' : macro names must be 256 characters or less (found %d characters)", fn, curline, name_length);
				}
				memcpy(newkey, macro_start, name_length);
				while (*s == ' ' || *s == '\t') s++; // skip whitespace after key name
				int value_length = 0;
				macro_start = s;
				while (*s != '\n' && *s)			// read in #define value
				{
					value_length++;
					s++;
				}
				if (value_length > 2048)
				{
					throw va("%s(%d) : error: '#define' : macro values must be 2048 characters or less (found %d characters)", fn, curline, value_length);
				}
					//newvalue[strlen(newvalue)] = *s++;
				char* newvalue = (char*) malloc((value_length + 1) * sizeof(char));
				memcpy(newvalue, macro_start, value_length);
				newvalue[value_length] = 0;

				stripdefinevalue(newvalue);	// get rid of comments in #define value
				pp_linetag(++curline);		// FIXME: check for duplicate #define?
				pp_total_lines++;

				// aen 12/4/05 11:44am : dupes overwrite previous def and log a warning
				bool macro_exists = false;
				for (i = 0; i < defines.size(); i++)
				{
					if (!zstrcmp(newkey, defines[i]->key.c_str()))
					{
						log("%s(%d) : warning: '%s' : macro redefinition", fn, curline, newkey);
						defines[i]->value = newvalue;
						macro_exists = true;
						break;
					}
				}
				if (!macro_exists)
				{
					if (defines.size() >= 2048)
					{
						throw va("%s(%d) : fatal error: too many macros. only 2048 macros are allowed.", fn, curline);
					}
					defines.push_back(new Define(newkey, newvalue));
				}
				s++;

				free(newvalue);
			}
			else
			{
				throw va("%s(%d) : fatal error: invalid preprocessor command '%s'", fn, curline, command);
				pp_included_files.pop_back();
				delete[] buf;
				return false;
			}
			continue;
		}
		if (token_type == PP_PUNC)
		{
			source.EmitC(*s++);
			continue;
		}
		if (token_type == PP_DIGIT)
		{
			source.EmitC(*s++);
			continue;
		}
		if (token_type == PP_QUOTE)			// dont preprocess text in quotes
		{
			source.EmitC(*s++);
			while (*s &&  *s != '\n' && *s != '"')
				source.EmitC(*s++);
			source.EmitC('"');
			s++;
			continue;
		}
		if (token_type == PP_LETTER)		// last case. we have to collect these
		{									// into tokens and check them vs. the
			char temp[256];					// list of #defines
			memset(temp, 0, 256);
			while (pptbl[*s] == PP_LETTER || pptbl[*s] == PP_DIGIT)
				temp[strlen(temp)] = *s++;
			
			bool def_found = false;
			for (int k=0; k<defines.size(); k++)
			{
				Define *def = defines[k];
				if (!zstrcmp(def->key.c_str(), temp))
				{
					def_found = true;
					if(def->type == Define::Type_PleaseQuote)
						source.EmitC('\"');
					for (i=0; i<def->value.size(); i++)
						source.EmitC(def->value[i]);
					if(def->type == Define::Type_PleaseQuote)
						source.EmitC('\"');
					break;
				}
			}
			if (def_found) continue;

			for (i=0; i<strlen(temp); i++)
				source.EmitC(temp[i]);			
			continue;
		}
		if (token_type == PP_ERROR)
		{
			throw va("%s(%d) : error: illegal character '0x%x' (%c)", fn, curline, *s, *s);
		}
		s++;		
	}

	/*
	len = source.ptr - source.chunk;
	char* blah=(char*)malloc(len + 1);
	memcpy(blah, source.chunk, len);
	for (i = 0; i < len; i++) {
		if (blah[i] == 0) {
			blah[i] = '\n';
		}
	}
	blah[len] = 0;
	log("preprocessed=%s",blah);
	free(blah);
	*/

	delete[] buf;
	pp_included_files.pop_back();
	return true;
}

/**************** Lexical Processor Component ****************/

enum
{
	WHITE,
	SPECIAL,
	DIGIT,
	LETTER
};

void VCCompiler::Init_Lexical()
{
	int i;

	for (i=0; i<256; i++) chr_table[i] = SPECIAL;
	for (i='0'; i<='9'; i++) chr_table[i] = DIGIT;
	for (i='A'; i<='Z'; i++) chr_table[i] = LETTER;
	for (i='a'; i<='z'; i++) chr_table[i] = LETTER;

	chr_table[PP_FILETAG] = WHITE;
	chr_table[PP_LINETAG] = WHITE;
	chr_table[10]  = WHITE;
	chr_table[13]  = WHITE;
	chr_table[' '] = WHITE;
	chr_table['_'] = LETTER;
	chr_table['$'] = DIGIT;
	chr_table[39]  = DIGIT;

	srcofs = 0;
	linenum = 0;
	memset(sourcefile, 0, 256);
}

bool VCCompiler::streq(const std::string & lhs, const std::string & rhs) {
	return streq(lhs.c_str(), rhs.c_str());
}

bool VCCompiler::streq(const char *a, const char *b)
{
	while (*a)
		if (tolower(*a++) != tolower(*b++))
			return false;
	return !*b;
}

int VCCompiler::hextoi(char *num)
{
    int v, i, n, l;

    l = strlen(num);
    if (l>8)
		throw va("%s(%d): Hex number too long, should be eight digits or less", sourcefile, linenum);

    strlwr(num);
    for (n=i=0; i<l; i++)
    {
        v = num[i]-48;
        if (v>9)
        {
			v-=39; // v-=7 for uppercase
			if (v>15)
				throw va("%s(%d): Invalid hex number", sourcefile, linenum);
        }
        n <<= 4;
        n += v;
    }
    return n;
}

void VCCompiler::ParseWhitespace()
{
	bool skipped = true;

	while (skipped)
	{
		skipped = false;
		while (source[srcofs] && source[srcofs]<=' ' && source[srcofs]>2)
		{
			srcofs++;
			skipped = true;
		}
        if (source[srcofs] == PP_FILETAG)
        {
			srcofs++;
			strcpy(sourcefile, &source.chunk[srcofs]);
			while (source[srcofs++]) {}
			skipped = true;
        }
        if (source[srcofs] == PP_LINETAG)
        {
			srcofs++;
			source.setpos(srcofs);
			linenum = source.GrabD();
			srcofs += 4;
			skipped = true;
        }
	}
}

void VCCompiler::ParseIdentifier()
{
	int i=0;
	while (chr_table[source[srcofs]] == LETTER || chr_table[source[srcofs]] == DIGIT)
	{
		token[i++] = source[srcofs++];
	}
	token[i] = 0;

	if (token[0] == 0)
	{
		throw va("%s(%d) : error: only A-Z, 0-9, and underscore allowed in identifiers. found '%c'", sourcefile, linenum, source[srcofs]);
	}

	// aen 12/3/05 11:23pm
	for (int j = 0; j < i; j++)
	{
		if (token[j] == '$')
		{
			throw va("%s(%d) : syntax error: '$' not allowed here: %s", sourcefile, linenum, token);
		}
		else if (token[j] == '\'')
		{
			throw va("%s(%d) : syntax error: single quote not allowed here: %s", sourcefile, linenum, token);
		}
	}
}

void VCCompiler::ParseNumber()
{
	// character constant
	if (source[srcofs] == '\'')
	{
		token[0] = source[srcofs++];
		// allow people to do: '\''
		if (source[srcofs] == '\\')
		{
			// gather it all for better error reporting
			char* p = token;
			p++;
			//*p++ = source[srcofs++];
			while (source[srcofs] != '\''
				&& source[srcofs]
				&& source[srcofs] != '\n'
				&& source[srcofs] != '\r')
			{
				// this is a special case; consume both characters in \' so ' doesn't cause premature end of loop
				if (source[srcofs] == '\\' && source[srcofs+1] == '\'' && source[srcofs+2] == '\'')
				{
					*p++ = source[srcofs++];
				}
				*p++ = source[srcofs++];
			}
			if (source[srcofs] == '\'')
			{
				*p++ = source[srcofs++];
			}
			*p = 0;

			if (IsHexEscapeSequence(token + 1))
			{
				if (strlen(token) != 6 || token[5] != '\'')
				{
					throw va("%s(%d) : syntax error: invalid hex escape sequence: %s", sourcefile, linenum, token);
				}
				char temp[3];
				temp[0] = token[3];
				temp[1] = token[4];
				temp[2] = 0;
				token_value = hextoi(temp);
			}
			else if (!IsEscapeSequence(token + 1) || (IsEscapeSequence(token + 1) && strlen(token) > 4))
			{
				throw va("%s(%d) : syntax error: invalid character constant escape sequence: %s", sourcefile, linenum, token);
			}
			// not a hex escape, just a normal escape
			else
			{
				// this is a special case; we want to allow '\', '\'', and '\\' as valid escape sequences, but
				// normally GetEscapeSequence would return the code for '\'' when matching against '\' because
				// we match starting inside the outer single quotes.
				if (!strcmp(token, "'\\'"))
				{
					token_value = (int) '\\';
				}
				else
				{
					escape_sequence* esc_seq = GetEscapeSequence(token + 1);
					token_value = 0;
					if (esc_seq)
					{
						token_value = esc_seq->value;
					}
					else
					{
						log("%s(%d) : warning: unknown escape sequence: %s; setting token value to zero", sourcefile, linenum, token);
					}
				}
			}
		} 
		else
		{
			token[1] = source[srcofs++];
			token[2] = source[srcofs++];
			token[3] = 0;

			if (token[2] != '\'')
			{
				throw va("%s(%d) : syntax error: invalid character constant", sourcefile, linenum);
			}

			token_value = token[1];
		}
	}
	else
	{
		int i = 0;

		// hex
		if (source[srcofs] == '$')
		{
			token[i++] = source[srcofs++];
			char ch = source[srcofs];
			// $ is not allowed after the first
			while (IsHexNumberChar(ch))
			{
				token[i++] = ch;
				ch = source[++srcofs];
			}
			
			// aen 12/3/05 11:09pm : & and ' are marked DIGIT but not allowed here
			for (int j = 1; j < i; j++)
			{
				if (token[j] == '$')
				{
					throw va("%s(%d): A hex number cannot contain more than one dollar sign: %s", sourcefile, linenum, token);
				}
				else if (token[j] == '\'')
				{
					throw va("%s(%d): A hex number cannot contain quotes: %s", sourcefile, linenum, token);
				}
			}
			token[i] = 0;
			token_value = hextoi(token + 1);
		}
		// not hex
		else
		{
			// need to prevent whitespace from becoming a number,
			// otherwise PP_LINETAG could be absorbed into a numeric literal  
			do
			{
				token[i++] = source[srcofs++];
			}
			while (IsNumberChar(source[srcofs]));
			token[i] = 0;

			// aen 12/3/05 11:11pm : & and ' are marked DIGIT but not allowed here
			for (int j = 0; j < i; j++)
			{
				if (token[j] == '$')
				{
					throw va("%s(%d): A number cannot contain a dollar sign unless it is the first character: %s", sourcefile, linenum, token);
				}
				else if (token[j] == '\'')
				{
					throw va("%s(%d): A number cannot contain a quote unless it is the first characer: %s", sourcefile, linenum, token);
				}
			}

			token_value = atoi(token);
		}
	}
}

void VCCompiler::ParsePunc()
{
	switch (source[srcofs])
	{
		case '(': token[0]='('; token[1]=0; srcofs++; break;
		case ')': token[0]=')'; token[1]=0; srcofs++; break;
		case '{': token[0]='{'; token[1]=0; srcofs++; break;
		case '}': token[0]='}'; token[1]=0; srcofs++; break;
		case '[': token[0]='['; token[1]=0; srcofs++; break;
		case ']': token[0]=']'; token[1]=0; srcofs++; break;
		case ',': token[0]=','; token[1]=0; srcofs++; break;
		case ':': token[0]=':'; token[1]=0; srcofs++; break;
		case ';': token[0]=';'; token[1]=0; srcofs++; break;
		case '/': token[0]='/'; token[1]=0; srcofs++; break;
		case '*': token[0]='*'; token[1]=0; srcofs++; break;
		case '^': token[0]='^'; token[1]=0; srcofs++; break;
		case '%': token[0]='%'; token[1]=0; srcofs++; break;
		case '\"': token[0]='\"'; token[1]=0; srcofs++; break;
		case '~': token[0]='~'; token[1]=0; srcofs++; break;
		case '.' :
			token[0]='.'; srcofs++;
			token[1] = token[2] = token[3] = 0;
			if (source[srcofs]=='.')
			{
				token[1]='.';
				srcofs++;
			}
			if (source[srcofs]=='.')
			{
				token[2]='.';
				srcofs++;
			}
			break;
		case '+' :
			token[0]='+';
			srcofs++;
			if (source[srcofs]=='+')
			{
				token[1]='+';
				srcofs++;
			}
			else if (source[srcofs]=='=')
			{
				token[1]='=';
				srcofs++;
			} else
				token[1]=0;
			token[2]=0;
			break;
		case '-' :
			token[0]='-';
			srcofs++;
			if (source[srcofs]=='-')
			{
				token[1]='-';
				srcofs++;
			}
			else if (source[srcofs]=='=')
			{
				token[1]='=';
				srcofs++;
			}
			else
				token[1]=0;
			token[2]=0;
			break;
		case '>' :
			token[0]='>';
			srcofs++;
			if (source[srcofs]=='=')
			{
				token[1]='=';
				token[2]=0;
				srcofs++;
				break;
			}
			if (source[srcofs]=='>')
			{
				token[1]='>';
				token[2]=0;
				srcofs++;
				break;
			}
			token[1]=0;
			break;
		case '<':
			token[0]='<';
			srcofs++;
			if (source[srcofs]=='=')
			{
				token[1]='=';
				token[2] = 0;
				srcofs++;
				break;
			}
			if (source[srcofs]=='<')
			{
				token[1]='<';
				token[2] = 0;
				srcofs++;
				break;
			}
			token[1]=0;
			break;
		case '!':
			token[0]='!';
			srcofs++;
			if (source[srcofs]=='=')
			{
				token[1]='=';
				token[2]=0;
				srcofs++;
				break;
			}
			token[1]=0;
			break;
		case '=':
			token[0]='=';
			srcofs++;
			if (source[srcofs]=='=')
			{
				token[1]=0;
				srcofs++;
			}
			else
				token[1]=0;
			break;
		case '&':
			token[0]='&';
			srcofs++;
			if (source[srcofs]=='&')
			{
				token[1]='&';
				token[2]=0;
				srcofs++;
				break;
			}
			token[1]=0;
			break;
		case '|':
			token[0]='|';
			srcofs++;
			if (source[srcofs]=='|')
			{
				token[1]='|';
				token[2]=0;
				srcofs++;
				break;
			}
			token[1]=0;
			break;
		default:
			srcofs++;
			break;
	}
}

void VCCompiler::GetToken()
{
	// skip whitespace, read in next token, figure out what it is
	ParseWhitespace();
	token_value = 0;
	if (source[srcofs] == '.')
		token_value = 0;
	switch (chr_table[source[srcofs]])
	{
		case LETTER:
			ParseIdentifier();
			break;
		case DIGIT:
			ParseNumber();
			break;
		case SPECIAL:
			ParsePunc();
			break;
	}
}

void VCCompiler::GetToken(int n)
{
	for (int i=0; i<n; i++)
		GetToken();
}

bool VCCompiler::TokenIs(char *s)
{
	return streq(token, s);
}

bool VCCompiler::NextIs(char *s)
{// FIXME: this does not save/restore file/linenumber information.
	int lineprev = linenum;
	int save_ofs = srcofs;
	int save_tokenvalue = token_value;
	char save_token[1024];
	strcpy(save_token, token);

	GetToken();
	bool result = TokenIs(s);

	srcofs = save_ofs;
	token_value = save_tokenvalue;
	strcpy(token, save_token);
	return result;
}

bool VCCompiler::NextIs(int n, char *s)
{// FIXME: this does not save/restore file/linenumber information.
	int lineprev = linenum;
	int save_ofs = srcofs;
	char save_token[1024];
	strcpy(save_token, token);

	GetToken(n);
	bool result = TokenIs(s);

	srcofs = save_ofs;
	strcpy(token, save_token);

	return result;
}

void VCCompiler::Expect(char *a)
{
	int lineprev = linenum;
	GetToken();
	if (!TokenIs(a))
	{
		throw va("%s(%d): Expecting \"%s\", but got \"%s\" instead", sourcefile, linenum, a, token);
	}
}

bool VCCompiler::IsEscapeSequence(char* s)
{
	return GetEscapeSequence(s) != 0;
}

escape_sequence* VCCompiler::GetEscapeSequence(char* s)
{
	for (int i = 0; i < 10; i++)
	{
		if (!memcmp(escape_codes[i].sequence, s, 2))
		{
			return &escape_codes[i];
		}
	}
	return 0;
}

bool VCCompiler::IsKeyword(char* s)
{
	return !strcmp(s,"int")
		|| !strcmp(s,"string")
		|| !strcmp(s,"void")
		|| !strcmp(s,"struct")
		|| !strcmp(s,"for")
		|| !strcmp(s,"while")
		|| !strcmp(s,"if")
		|| !strcmp(s,"else")
		|| !strcmp(s,"switch")
		|| !strcmp(s,"unless")
		|| !strcmp(s,"until")
		|| !strcmp(s,"case")
		|| !strcmp(s,"default")
		|| !strcmp(s,"and")
		|| !strcmp(s,"or")
		|| !strcmp(s,"not")
		|| !strcmp(s,"callback")
		|| !strcmp(s,"alias");
}

bool VCCompiler::IsHexEscapeSequence(char* s)
{
	return !memcmp("\\x", s, 2);
}

bool VCCompiler::IsNumberChar(char ch)
{
	return chr_table[ch] == DIGIT;
}

bool VCCompiler::IsHexNumberChar(char ch)
{
	return (chr_table[ch] == DIGIT
		|| ch == 'A' || ch == 'a'
		|| ch == 'B' || ch == 'b'
		|| ch == 'C' || ch == 'c'
		|| ch == 'D' || ch == 'd'
		|| ch == 'E' || ch == 'e'
		|| ch == 'F' || ch == 'f');
}

void VCCompiler::GetIdentifierToken()
{
	ParseWhitespace();
	ParseIdentifier();
}

int VCCompiler::TypenameToTypeID(char* s)
{
	if(streq(s, "int"))
	{
		return t_INT;
	}
	if(streq(s, "string") || streq(s, "str"))
	{
		return t_STRING;
	}
	if(streq(s, "void"))
	{
		return t_VOID;
	}
	if(streq(s, "callback"))
	{
		return t_CALLBACK;
	}
	if(streq(s, "..."))
	{
		return t_VARARG;
	}
	// Couldn't figure it out.
	return t_NOTFOUND;
}

/**************** Scanning Pass Component ****************/

void VCCompiler::ScanPass(scan_t type)
{	
	Init_Lexical();

	if(type == SCAN_ALL) 
	{
		// if we are reading new ints and strings,
		// we need to clear them first
		global_vars.clear();
		global_var_offset = 0;
	}

	bool isPrimitive;
	bool isFunc;
/*FILE *f = fopen("prepos.txt", "wb");  // FIXME remove
source.SaveChunk(f);
fclose(f);*/
	while (true)
	{
		ParseWhitespace();
		if (!source[srcofs]) return;

		int save_srcofs = srcofs;
		GetToken();

		isPrimitive = false;
		isFunc = false;
		if (TokenIs("int") || TokenIs("string") || TokenIs("str"))
		{
			isPrimitive = true;
			GetIdentifierToken();
			ParseWhitespace();
			if (source[srcofs] == '(')
			{
				isFunc = true;
			}
			// restore
			srcofs = save_srcofs;
			GetToken();
		}
		else if (TokenIs("callback"))
		{			
			isPrimitive = true;
			SkipCallbackDefinition();
			
			// Name of variable/function.
			GetIdentifierToken();
			ParseWhitespace();

			// Brackets, means it's a function.
			if (source[srcofs] == '(')
			{
				isFunc = true;
			}
			// restore
			srcofs = save_srcofs;
			GetToken();
		}
		else if (TokenIs("void"))
		{
			isFunc = true;
		}
		else
		{
			// An aliased type?
			if(FindAliasByName(token) != NULL)
			{
				// Currently only the int/string can be aliased.
				// So the aliased type is a primitive.
				isPrimitive = true;

				GetIdentifierToken();
				ParseWhitespace();
				if (source[srcofs] == '(')
				{
					isFunc = true;
				}
				// restore
				srcofs = save_srcofs;
				GetToken();
			}
		}

		// Global variable declaration.
		if (isPrimitive && !isFunc)
		{
			ParseGlobalDecl(type);
			continue;
		}
		// Function declaration.
		if ((TokenIs("void") || isPrimitive) && isFunc)
		{
			ParseFuncDecl(type);
			continue;
		}
		// A strong type definition.
		if(TokenIs("alias"))
		{
			ParseAliasDecl(type);
			continue;
		}

		// Struct definition.
		if (TokenIs("struct")) 
		{
			ParseStructDecl(type);
			continue;
		}
		// at this point, its not any of the above type of declarations. Check to see if this
		// token is a struct definition, in which case we're declaring a struct instance
		bool found = false;		
		for (int i=0; i<struct_defs.size(); i++)
		{
			if (streq(token, struct_defs[i]->name))
			{
				ParseStructInstance(type);				
				found = true;
				break;
			}
		}
		if (found) continue;

		// token is not what we're expecting. generate an error.
		throw va("%s(%d): Expecting a variable or function declaration - do you have a missing brace? (%s)", sourcefile, linenum, token);
	}
}

void VCCompiler::SkipDeclare()
{
	if(TokenIs("struct")) {
		GetIdentifierToken(); // its name
		GetToken(); // opening brace
		SkipBrackets();
		// get final ; if there is one
		if(NextIs(";"))
			GetToken();
		return;
	}

	while(!TokenIs(";")) {
		GetToken();
		if (TokenIs("\""))
		{
			while (source[srcofs] != '"')
				srcofs++;
			srcofs++;
			continue;
		}

		if(!source[srcofs]) {
			throw va("%s(%d): Variable declaration doesn't end!", sourcefile, linenum);
		}
	}
}

void VCCompiler::SkipBrackets()
{
	int orig_line = linenum;
	char orig_file[256];
	strcpy(orig_file, sourcefile);

	while (!NextIs("}"))
	{
		if (!source[srcofs])
			throw va("%s(%d): No matching bracket.", orig_file, orig_line);

		GetToken();
		if (TokenIs("\""))
		{
/*			GetToken();
*			while (!TokenIs("\""))
			{
				GetToken();
				if (!source[srcofs])
				{
					sprintf(errmsg, "%s(%d): No matching end-quote.", orig_file, orig_line);
					vcerror = true;
					return;
				}
			}*/
			while (source[srcofs] != '"')
				srcofs++;
			srcofs++;
			continue;
		}

		if (TokenIs("{"))
			SkipBrackets();
	}
	GetToken();
}

void VCCompiler::SkipArguments()
{
	int orig_line = linenum;
	char orig_file[256];
	strcpy(orig_file, sourcefile);

	// a ( *must* be the first part to an argument list.
	Expect("(");
	int bracketCount = 1;

	// Skip argument type definition.
	// Account for nesting (which can happen due to callback definitions) by counting depth.
	// Read until we're at 0 bracket depth again.
	while(bracketCount > 0)
	{
		if (!source[srcofs])
			throw va("%s(%d): Missing closing bracket for callback declaration.", orig_file, orig_line);
		GetToken();
		if(TokenIs("("))
		{
			bracketCount++;
		}
		else if(TokenIs(")"))
		{
			bracketCount--;
		}
	}
}

void VCCompiler::SkipCallbackDefinition()
{
	int orig_line = linenum;
	char orig_file[256];
	strcpy(orig_file, sourcefile);

	int bracketsExpected = 1;
	int bracketsOpen = 0;

	// Skip argument type definition.
	// Account for nesting (which can happen due to callback definitions) by counting depth.
	// Read until we're at 0 bracket depth again.
	while(bracketsExpected > 0 || bracketsOpen > 0)
	{
		if (!source[srcofs])
			throw va("%s(%d): Malformed callback declaration"
				"(reached EOF;"
				"currently %d brackets are still open,"
				"and still expecting %d sets of brackets to appear).",
				orig_file, orig_line, bracketsOpen, bracketsExpected);
		GetToken();
		if(TokenIs("callback"))
		{
			bracketsExpected++;
		}
		if(TokenIs("("))
		{
			if(bracketsExpected > 0)
			{
				bracketsExpected--;
			}
			bracketsOpen++;
		}
		else if(TokenIs(")"))
		{
			bracketsOpen--;
		}
	}
}

void VCCompiler::CheckNameDup(char *s)
{
	int i;

	if (IsKeyword(s))
	{
		throw va("%s(%d) : syntax error: expecting identifier; keyword not allowed here: %s", sourcefile, linenum, token);
	}

	// this is as good a place as any to make sure the identifier isn't too long
	if (strlen(s) >= IDENTIFIER_LEN)
		throw va("%s(%d): identifier name %s is too long. (max: %d characters)", sourcefile, linenum, s, IDENTIFIER_LEN-1);

	// check vs. system library functions
	for (i=0; i<NUM_LIBFUNCS; i++)
		if (streq(libfuncs[i].name, token))
			throw va("%s(%d): %s is already claimed by a built-in library function. Please choose a unique name.", sourcefile, linenum, s);

	// check vs. all other functions compiled so far
	for(i=precompile_numfuncs; i<funcs[target_cimage].size(); i++)
		if(streq(funcs[target_cimage][i]->name, token))
			throw va("%s(%d): %s is already claimed by a user function. Please choose a unique name.", sourcefile, linenum, s);

	// check reference core images if we're preventing duplicates
	if(!permit_duplicates) {
		for(int index = 0; index < reference_cimages.size(); index++) {
			int cimage = reference_cimages[index];
			if(cimage == target_cimage)
				continue;
			for(i=0; i<funcs[cimage].size(); i++)
				if(streq(funcs[cimage][i]->name, token))
					throw va("%s(%d): %s is already claimed by a user function. Please choose a unique name.", sourcefile, linenum, s);
		}
	}

	// check vs. HVars
	for (i=0; i<hvars.size(); i++)
		if (streq(hvars[i]->name, token))
        	throw va("%s(%d): %s is already claimed by a built-in library variable. Please choose a unique name.", sourcefile, linenum, s);

	// check vs. global ints
	for (i=0; i<global_vars.size(); i++)
		if (streq(global_vars[i]->name, token))
			throw va("%s(%d): %s is already claimed by a global user variable. Please choose a unique name.", sourcefile, linenum, s);

	// check vs. struct types
	for (i=0; i<struct_defs.size(); i++)
		if (streq(struct_defs[i]->name, token))
			throw va("%s(%d): %s is already claimed by a user struct definition. Please choose a unique name.", sourcefile, linenum, s);

	// check vs. alias types
	for (i=0; i<alias_defs.size(); i++)
		if (streq(alias_defs[i]->name, token))
			throw va("%s(%d): %s is already claimed by a type alias definition. Please choose a unique name.", sourcefile, linenum, s);

	// check vs. struct instances
	for (i=0; i<struct_instances.size(); i++)
		if (streq(struct_instances[i]->name, token))
			throw va("%s(%d): %s is already claimed by a user global variable. Please choose a unique name.", sourcefile, linenum, s);
}

// Overkill (2006-05-06)
// Handles name checking for structure elements.
void VCCompiler::CheckStructElementNameDup(char *s, struct_definition *def)
{
	int i;
	if (IsKeyword(s))
	{
		throw va("%s(%d) : syntax error: expecting identifier; keyword not allowed here: %s", sourcefile, linenum, token);
	}
	// this is as good a place as any to make sure the identifier isn't too long
	if (strlen(s) >= IDENTIFIER_LEN)
		throw va("%s(%d): identifier name %s is too long. (max: %d characters)", sourcefile, linenum, s, IDENTIFIER_LEN-1);

	// check vs. HVars
	for (i=0; i<hvars.size(); i++)
		if (streq(hvars[i]->name, token))
        	throw va("%s(%d): %s is already claimed by a built-in library variable. Please choose a unique name.", sourcefile, linenum, s);
	
	// check vs. struct types
	for (i=0; i<struct_defs.size(); i++)
		if (streq(struct_defs[i]->name, token))
			throw va("%s(%d): %s is already claimed by a user struct definition. Please choose a unique name.", sourcefile, linenum, s);

	// check vs. alias types
	for (i=0; i<alias_defs.size(); i++)
		if (streq(alias_defs[i]->name, token))
			throw va("%s(%d): %s is already claimed by a type alias definition. Please choose a unique name.", sourcefile, linenum, s);

	// check vs. existing elements in the struct.
	for (i=0; i<def->elements.size(); i++)
		if (streq(def->elements[i]->name, token))
			throw va("%s(%d): %s is already claimed by another variable in this struct. Please choose a unique name.", sourcefile, linenum, s);
}


void VCCompiler::ParseGlobalDecl(scan_t type)
{
	switch(type) {
		case SCAN_IGNORE_NON_FUNC:
			SkipDeclare();
			return;
		case SCAN_ERR_NON_FUNC:
			throw va("%s(%d): Cannot declare global variables in map VC!", sourcefile, linenum);
		case SCAN_ALL: // continue normally
			break;
	}

	char var_type;
	ext_definition ext;
	ext.type = EXT_NONE;

	var_type = TypenameToTypeID(token);
	if(var_type == t_CALLBACK)
	{
		ext.type = EXT_CALLBACK;
		ParseCallbackDefinition(&(ext.callback));
	}
	// Aliased type?
	{
		alias_definition* alias = FindAliasByName(token);
		if(alias != NULL)
		{
			// The real data type to declare.
			var_type = alias->type;

			// Keep alias reference in the extended definition info
			ext.type = EXT_ALIAS;
			ext.alias = alias;
		}
	}
	// Couldn't resolve.
	if(var_type == t_VOID || var_type == t_VARARG || var_type == t_NOTFOUND)
	{
		throw va("%s(%d): ParseGlobalDecl: unknown variable type %s found in global declaration", sourcefile, linenum, token);
	}

	while (true)
	{
		char var_name[256];
		int  var_len = 1;
		int	 var_dim = 0;
		std::vector<int> var_dims;
		var_dims.push_back(0);
		std::string initstr = "";

		GetIdentifierToken();		// grab name of int
		strcpy(var_name, token);
		CheckNameDup(var_name);

		while (NextIs("["))			// it's an array
		{
			GetToken(2);
			var_len *= token_value;
			if (var_dim)
			{
				var_dims.push_back(0);
			}
			var_dims[var_dim++] = token_value;
			Expect("]");
		}

		if (NextIs("="))			// got an initializerzzz
		{
			GetToken();
			int group = 0;
			while (true)
			{
				if (TokenIs("("))
					group++;
				if (TokenIs(")"))
					group--;
				if (TokenIs("\""))          // Only special case is a quoted string.
				{
					initstr += '\"';
					while (source[srcofs] != '\"')
					{
						if (!source[srcofs])
							throw va("%s(%d): Reached unexpected end of file, probable missing end-quote (\")", sourcefile, linenum);
						initstr += source[srcofs++];
					}
					initstr += source[srcofs++];
					source.setpos(srcofs);
					if (!group && (NextIs(";") || NextIs(","))) break;
					GetToken();
					continue;
				}
				initstr += token;
				if (!group && (NextIs(";") || NextIs(","))) break;
				GetToken();
			}
		}

		global_var_t *my_var = new global_var_t;
		strcpy(my_var->name, var_name);
		my_var->type = var_type;
		my_var->ext = ext;
		my_var->len = var_len;
		my_var->ofs = global_var_offset;
		my_var->dim = var_dim;
		//memcpy(my_int->dims, var_dims, sizeof (var_dims));
		my_var->dims = var_dims;
		my_var->initializer = initstr.c_str();
		global_vars.push_back(my_var);
		vprint("global %s: type %d, offset %8d, size %6d, initializer: %s\n", var_name, var_type, global_var_offset, var_len, initstr.c_str());
		global_var_offset += var_len;

		if (NextIs(";")) break;
		Expect(",");
	}
	Expect(";");
}


void VCCompiler::ParseCallbackDefinition(callback_definition** def)
{
	(*def) = new callback_definition;
	int numargs = 0;

	GetToken();
	
	(*def)->sigext.type = EXT_NONE;
	int sig = TypenameToTypeID(token);
	// The token 'callback' is followed by a definition.
	if (sig == t_CALLBACK)
	{
		(*def)->sigext.type = EXT_CALLBACK;
		ParseCallbackDefinition(&((*def)->sigext.callback));
	}
	else
	{
		// Aliased type?
		{
			alias_definition* alias = FindAliasByName(token);
			if(alias != NULL)
			{
				// The real data type to declare.
				sig = alias->type;

				// Keep alias reference in the extended definition info
				(*def)->sigext.type = EXT_ALIAS;
				(*def)->sigext.alias = alias;
			}
		}
		// Unhandled type.
		// Give more relevant error messages when possible.
		if(sig == t_NOTFOUND || sig == t_VARARG)
		{
			// check vs. struct types
			for (int i=0; i<struct_defs.size(); i++)
				if (streq(struct_defs[i]->name, token))
				{
					throw va("%s(%d): '%s' is a struct. Structs cannot be used as an argument type in a callback definition.", sourcefile, linenum, token);
				}
			// Otherwise: generic error.
			throw va("%s(%d): invalid argument type '%s' in callback definition", sourcefile, linenum, token);
		}
	}
	(*def)->signature = sig;

	Expect("(");
	while (!NextIs(")"))
	{
		GetToken();

		(*def)->argext[numargs].type = EXT_NONE;
		int argtype = TypenameToTypeID(token);
		// The token 'callback' is followed by a definition.
		if (argtype == t_CALLBACK)
		{
			(*def)->argext[numargs].type = EXT_CALLBACK;
			ParseCallbackDefinition(&((*def)->argext[numargs].callback));
		}
		else
		{
			// Aliased type?
			{
				alias_definition* alias = FindAliasByName(token);
				if(alias != NULL)
				{
					// The real data type to declare.
					argtype = alias->type;

					// Keep alias reference in the extended definition info
					(*def)->argext[numargs].type = EXT_ALIAS;
					(*def)->argext[numargs].alias = alias;
				}
			}
			// Unhandled type.
			// Give more relevant error messages when possible.
			if (argtype == t_NOTFOUND || argtype == t_VOID)
			{
				// check vs. struct types
				for (int i=0; i<struct_defs.size(); i++)
					if (streq(struct_defs[i]->name, token))
					{
						throw va("%s(%d): '%s' is a struct. Structs cannot be used as an argument type in a callback definition.", sourcefile, linenum, token);
					}
				// Otherwise: generic error.
				throw va("%s(%d): invalid argument type '%s' in callback definition", sourcefile, linenum, token);
			}
		}
		(*def)->argtype[numargs] = argtype;
		
		if(!NextIs(",") && !NextIs(")"))
		{
			GetIdentifierToken();
			CheckNameDup(token);
		}
		if (argtype != t_VARARG && NextIs(","))
		{
			GetToken();	
			if (NextIs(")"))
			{
				throw va("%s(%d) : syntax error: trailing comma not allowed in argument list", sourcefile, linenum);
			}
		}
		else if (!NextIs(")"))
		{
			GetToken();
			if (argtype == t_VARARG)
			{
				throw va("%s(%d): variable argument lists must be the last argument of a variadic function.", sourcefile, linenum);
			}
			else
			{
				throw va("%s(%d) : syntax error: arguments must be separated by commas. found: %s", sourcefile, linenum, token); 
			}
		}
		numargs++;
	}
	(*def)->numargs = numargs;
	Expect(")");
}

void VCCompiler::ParseFuncDecl(scan_t type) 
{
	// type ignored -- always parse functions
	function_t *myfunc = new function_t;

	myfunc->sigext.type = EXT_NONE;
	int sig = TypenameToTypeID(token);
	// The token 'callback' is followed by a definition.
	if (sig == t_CALLBACK)
	{
		myfunc->sigext.type = EXT_CALLBACK;
		ParseCallbackDefinition(&(myfunc->sigext.callback));
	}
	else
	{
		// Aliased type?
		{
			alias_definition* alias = FindAliasByName(token);
			if(alias != NULL)
			{
				// The real data type to declare.
				sig = alias->type;

				// Keep alias reference in the extended definition info
				myfunc->sigext.type = EXT_ALIAS;
				myfunc->sigext.alias = alias;
			}
		}
		// Unhandled type.
		// Give more relevant error messages when possible.
		if(sig == t_NOTFOUND || sig == t_VARARG)
		{
			// check vs. struct types
			for (int i=0; i<struct_defs.size(); i++)
				if (streq(struct_defs[i]->name, token))
				{
					throw va("%s(%d): '%s' is a struct. Structs cannot be used as a return type in a function.", sourcefile, linenum, token);
				}
			// Otherwise: generic error.
			throw va("%s(%d) : error: invalid return type: %s", sourcefile, linenum, token);
		}
	}
	myfunc->signature = sig;

	myfunc->coreimage = target_cimage;

	GetIdentifierToken();
	CheckNameDup(token);

	int numargs = 0;
	strcpy(myfunc->name, token);
	Expect("(");
	while (!NextIs(")"))
	{
		GetToken();

		myfunc->argext[numargs].type = EXT_NONE;
		int argtype = TypenameToTypeID(token);
		// The token 'callback' is followed by a definition.
		if (argtype == t_CALLBACK)
		{
			myfunc->argext[numargs].type = EXT_CALLBACK;
			ParseCallbackDefinition(&(myfunc->argext[numargs].callback));
		}
		else
		{
			// Aliased type?
			{
				alias_definition* alias = FindAliasByName(token);
				if(alias != NULL)
				{
					// The real data type to declare.
					argtype = alias->type;

					// Keep alias reference in the extended definition info
					myfunc->argext[numargs].type = EXT_ALIAS;
					myfunc->argext[numargs].alias = alias;
				}
			}
			// Unhandled type.
			// Give more relevant error messages when possible.
			if(argtype == t_NOTFOUND || argtype == t_VOID)
			{
				// check vs. struct types
				for (int i=0; i<struct_defs.size(); i++)
					if (streq(struct_defs[i]->name, token))
					{
						throw va("%s(%d): '%s' is a struct. Structs cannot be used as an argument type in a function.", sourcefile, linenum, token);
					}
				// Otherwise: generic error.
				throw va("%s(%d): invalid argument type '%s'", sourcefile, linenum, token);
			}
		}
		myfunc->argtype[numargs] = argtype;
		
		GetIdentifierToken();
		CheckNameDup(token);

		if (streq(myfunc->name, token))
		{
			throw va("%s(%d) : error: '%s' : variable has same name as function", sourcefile, linenum, token);
		}
		for (int i = 0; i < numargs; i++)
		{
			if (streq(myfunc->localnames[i], token))
			{
				throw va("%s(%d) : error: '%s' : redefinition", sourcefile, linenum, token);
			}
		}
		strcpy(myfunc->localnames[numargs], token);
		if (argtype != t_VARARG && NextIs(","))
		{
			GetToken();	
			if (NextIs(")"))
			{
				throw va("%s(%d) : syntax error: trailing comma not allowed in argument list", sourcefile, linenum);
			}
		}
		else if (!NextIs(")"))
		{
			GetToken();
			if (argtype == t_VARARG)
			{
				throw va("%s(%d): variable argument lists must be the last argument of a variadic function.", sourcefile, linenum);
			}
			else
			{
				throw va("%s(%d) : syntax error: arguments must be separated by commas. found: %s", sourcefile, linenum, token); 
			}
		}
		numargs++;
	}
	myfunc->numargs = numargs;
	Expect(")");
	Expect("{");
	SkipBrackets();

	funcs[target_cimage].push_back(myfunc);
	vprint("func %s found with %d args and shit.\n",myfunc->name, myfunc->numargs);
}

alias_definition* VCCompiler::FindAliasByName(char* name)
{
	for (int i=0; i < alias_defs.size(); i++)
	{
		if (streq(name, alias_defs[i]->name))
		{
			return alias_defs[i];
		}
	}
	return NULL;
}

void VCCompiler::ParseAliasDecl(scan_t type)
{
	switch(type)
	{
		case SCAN_IGNORE_NON_FUNC:
			SkipDeclare();
			return;
		case SCAN_ERR_NON_FUNC:
			throw va("%s(%d): Cannot declare new structs in map VC!", sourcefile, linenum);
		case SCAN_ALL: // continue normally
			break;
	}

	alias_definition *alias = new alias_definition;

	// Grab name of the type to be aliased.
	GetToken();
	alias->type = TypenameToTypeID(token);
	if(alias->type != t_INT && alias->type != t_STRING)
	{
		throw va("%s(%d): Type alias found for invalid type %s. Only aliases for int or string are allowed.", sourcefile, linenum, token);
	}

	// Grab name of new type.
	GetToken();
	CheckNameDup(token);
	strcpy(alias->name, token);

	vprint("alias type %s (for type id %d). \n", alias->name, alias->type);
	alias_defs.push_back(alias);

	Expect(";");
}

void VCCompiler::ParseStructMemberDecl(struct_definition* mystruct)
{
	GetToken(); // Get the typename.
	int variable_type = TypenameToTypeID(token);

    char type_name[80];
    strcpy(type_name, token);

	ext_definition ext;
	ext.type = EXT_NONE;

	if(variable_type == t_CALLBACK)
	{
		ext.type = EXT_CALLBACK;
		ParseCallbackDefinition(&(ext.callback));
	}
	else if(variable_type == t_NOTFOUND)
	{
		alias_definition* alias = FindAliasByName(token);
		// Alias?
		if(alias != NULL)
		{
			// The real data type to declare.
			variable_type = alias->type;

			// Keep alias reference in the extended definition info
			ext.type = EXT_ALIAS;
			ext.alias = alias;
		}
		else
		{
			// Couldn't find the type. Assume it's a structure, and resolve during code generation phase.
			variable_type = t_STRUCT;
		}
	}
	else if(variable_type == t_VOID || variable_type == t_VARARG)
	{
		throw va("%s(%d) : Invalid variable type %s found in struct definition.", sourcefile, linenum, token);
	}

    while (true)
    {
        struct_element *myelem = new struct_element;
        strcpy(myelem->type_name, type_name);
        myelem->dim = 0;
        myelem->len = 1;
        myelem->dims.push_back(0);

        GetToken();             // grab name
        strcpy(myelem->name, token);

        CheckStructElementNameDup(token, mystruct);

        while (NextIs("["))     // it's an array
        {
            GetToken(2);
            myelem->len *= token_value;
            if (myelem->dim)
            {
                myelem->dims.push_back(0);
            }
            myelem->dims[myelem->dim++] = token_value;
            Expect("]");
        }
        myelem->type = variable_type;
		myelem->ext = ext;
        mystruct->elements.push_back(myelem);
        if (NextIs(";")) break;
        Expect(",");
    }
    Expect(";");
}

void VCCompiler::ParseStructDecl(scan_t type)
{
	switch(type) {
		case SCAN_IGNORE_NON_FUNC:
			SkipDeclare();
			return;
		case SCAN_ERR_NON_FUNC:
			throw va("%s(%d): Cannot declare new structs in map VC!", sourcefile, linenum);
		case SCAN_ALL: // continue normally
			break;
	}

	struct_definition *mystruct = new struct_definition;
	GetToken();
	CheckNameDup(token);
	strcpy(mystruct->name, token);

	Expect("{");
	while (!NextIs("}"))
	{
		ParseStructMemberDecl(mystruct);
	}
	vprint("struct type declare %s: %d elements \n", mystruct->name, mystruct->elements.size());
	struct_defs.push_back(mystruct);
	Expect("}");
	if (NextIs(";"))
		GetToken();
}

void VCCompiler::ParseStructInstance(scan_t type)
{
	switch(type) {
		case SCAN_IGNORE_NON_FUNC:
			SkipDeclare();
			return;
		case SCAN_ERR_NON_FUNC:
			throw va("%s(%d): Cannot declare global variables in map VC!", sourcefile, linenum);
		case SCAN_ALL: // continue normally
			break;
	}

	struct_definition *def;
	int i;
	for (i=0; i<struct_defs.size(); i++)
		if (streq(token, struct_defs[i]->name))
			break;
	def = struct_defs[i];

	GetToken();							// read in name of struct instance
	CheckNameDup(token);
	struct_instance *inst = new struct_instance;
	strcpy(inst->name, token);
	inst->is = def;
	inst->dim = 0;
	inst->dims.push_back(0);

	while (NextIs("["))					// great, a struct array
	{
		Expect("[");
		GetToken();
		if (inst->dim)
		{
			inst->dims.push_back(0);
		}
		inst->dims[inst->dim++] = token_value;
		Expect("]");
	}

	struct_instances.push_back(inst);	// finalize this instance

	Expect(";");
}

/**************** Compile Pass Component ****************/

enum
{
	ID_NOMATCH,
	ID_LIBFUNC,
	ID_USERFUNC,
	ID_VARIABLE,
	ID_HVAR,
	ID_GLOBALINT,
	ID_GLOBALSTR,
	ID_LOCALINT,
	ID_LOCALSTR,
	ID_STRUCT,
	ID_PLUGINFUNC,
	ID_PLUGINVAR,
	ID_VARARG_LIST,
	ID_LOCALCB,
	ID_GLOBALCB,
};

// Overkill: Struct instances are created just before compilation.
// This way it can ensure all struct types are defined,
// and types that have no match can be found.
void VCCompiler::CreateStructInstance(struct_instance *inst)
{
	vprint("struct instance declared %s of type %s (%d dimensions) \n", inst->name, inst->is->name, inst->dim);

	for (int i=0; i<inst->is->elements.size(); i++)
	{
		// build struct-variables instances ..
		struct_element *elem = inst->is->elements[i];
		switch (elem->type)
		{
			case t_INT:
			case t_STRING:
			case t_CALLBACK:
			{
				global_var_t *myvar = new global_var_t();
				myvar->type = elem->type;
				myvar->ext = elem->ext;
				sprintf(myvar->name, "%c%s_%s", 1, inst->name, elem->name);
				if (!inst->dim)
				{
					myvar->len = elem->len;
					myvar->dim = elem->dim;

					myvar->dims = elem->dims;
				}
				else
				{
					myvar->len = elem->len;
					myvar->dim = elem->dim;
					myvar->dims = elem->dims;
					for (std::vector<int>::reverse_iterator it = inst->dims.rbegin();
							it != inst->dims.rend(); it++)
					{
						myvar->len *= *it;
						// If this already is an array, append to the beginning.
						if (myvar->dim)
						{
							myvar->dims.insert(myvar->dims.begin(), *it);
						}
						// If this wasn't an array, set the start element.
						else
						{
							myvar->dims[0] = *it;
						}
						
						myvar->dim++;
					}
				}
				myvar->ofs = global_var_offset;
				global_var_offset += myvar->len;
				global_vars.push_back(myvar);
				break;
			}
			case t_STRUCT:
			{
				struct_instance *myinst = new struct_instance;
				sprintf(myinst->name, "%c%s_%s", 1, inst->name, elem->name);
				struct_definition *def = 0;
				int i;
				for (i=0; i<struct_defs.size(); i++)
				{
					if (streq(elem->type_name, struct_defs[i]->name))
					{
						def = struct_defs[i];
					}
				}
				if (!def)
				{
					throw va("%s(%d): The struct definition of type '%s' declares the variable '%s' of unknown type '%s'", sourcefile, linenum, inst->is->name, elem->name, elem->type_name);
				}
				myinst->is = def;
				if (streq(myinst->is->name, inst->is->name))
				{
					throw va("%s(%d): The struct definition of type '%s' declares the variable '%s' of type '%s'. Recursive nesting of structures is not allowed.", sourcefile, linenum, inst->is->name, elem->name, elem->type_name);
				}
				if (!inst->dim)
				{
					myinst->dim = elem->dim;
					myinst->dims = elem->dims;
				}
				else
				{
					myinst->dim = elem->dim;
					myinst->dims = elem->dims;
					for (std::vector<int>::reverse_iterator it = inst->dims.rbegin();
							it != inst->dims.rend(); it++)
					{
						// If this already is an array, append to the beginning.
						if (myinst->dim)
						{
							myinst->dims.insert(myinst->dims.begin(), *it);
						}
						// If this wasn't an array, set the start element.
						else
						{
							myinst->dims[0] = *it;
						}
						myinst->dim++;
					}
				}

				struct_instances.push_back(myinst);
				break;
			}
			default:
				throw va("%s(%d): Internal error in VCCompiler::CreateStructInstance()", sourcefile, linenum);
		}
	}
}

void VCCompiler::CompilePass()
{
	output.clear();
	source.setpos(0);
	srcofs = 0;
	in_func = 0;

	for (int i = 0; i < struct_instances.size(); i++)
	{
		CreateStructInstance(struct_instances[i]);
	}

	CompileGlobalInitializers();

	ParseWhitespace(); // need to do this first for empty files
	while (source[srcofs])
	{
		// Holy crap, this is hacky as hell.
		// Anyways, if we find the word "callback", we skip past the type definition,
		// and try and see if what's after it is a global variable or a function.
		if (NextIs("callback"))
		{
			GetToken(); // Got the keyword.
			SkipCallbackDefinition(); // Get the callback definition

			// Check if the token after the word token is a (.
			if(NextIs(2, "("))
			{
				// It's a function. Oh boy!
				CompileFunction(true);
			}
			else
			{
				// It's a variable, but since we skipped the type definition
				// we can't use skip variables. Instead, we'll use skip declare, because that simply reads until a ;
				SkipDeclare();
			}
		}
		else if (NextIs(3,"("))
			CompileFunction(false);
		else
			SkipVariables();
		ParseWhitespace();
	}
}

void VCCompiler::CompileMapPass()
{
	output.clear();
	source.setpos(0);
	srcofs = 0;
	in_func = 0;

	ParseWhitespace(); // need to do this first for empty files
	while (source[srcofs])
	{
		if (NextIs(3,"("))
			CompileFunction(false);
		ParseWhitespace();
	}
}

void VCCompiler::CompileOtherPass(bool append)
{
	// we want begin compilation right after
	// the core alread loaded if we're appending,
	// otherwise we start from a clear output
	if(append)
		output.become(vc->GetCore(target_cimage));
	else
		output.clear();

	output.setpos(output.size());

	source.setpos(0);
	srcofs = 0;
	in_func = 0;

	ParseWhitespace(); // need to do this first for empty files
	while (source[srcofs])
	{
		if (NextIs(3,"("))
			CompileFunction(false);
		else
			SkipVariables();
		ParseWhitespace();
	}
}

void VCCompiler::CompileGlobalInitializers()
{
	vprint("Compiling sysvc_global_initializers ->>>>\n");
	function_t *myfunc = new function_t();
	strcpy(myfunc->name, "sysvc_global_initializers");
	myfunc->numargs = 0;
	myfunc->numlocals = 0;
	myfunc->signature = t_VOID;
	myfunc->codeofs = output.curpos();
	myfunc->coreimage = CIMAGE_SYSTEM;
	in_func = myfunc;

	Chunk temp;
	temp.become(&source);
	int tempofs = srcofs;

	char buffer[1024], *p;
	int i, j;
	for (i=0, j=0; i<global_vars.size(); i++)
	{
		if (!global_vars[i]->initializer.length())
			continue;

		source.clear();
		srcofs=0;
		sprintf(buffer, "%s%s;", global_vars[i]->name, global_vars[i]->initializer.c_str());
		for (j=0, p=buffer; *p; p++, j++) {
			source.Expand(1);
			source.chunk[j] = *p;
		}
		CompileStatement();
	}

	output.EmitC(opRETURN);

	source.become(&temp);
	srcofs = tempofs;
	source.setpos(srcofs);
	funcs[CIMAGE_SYSTEM].push_back(myfunc);
	in_func = 0;
	vprint("finished compiling globalinitializers\n");
}

function_t* VCCompiler::FetchFunction(char *s)
{
	// this function is used to get the function object for
	// a named function that is about to be compiled. As such,
	// we search the target_cimage only.

	// get reference to the funcs we're searching
	std::vector<function_t*> &target_funcs = funcs[target_cimage];

	// search backwards so patch images can include a function
	// more than once, and we use the last version.
	// This is important because the last version is the one
	// we want to compile code for in CompileFunction()
	for(int i = target_funcs.size()-1; i>=0; i--) {
		if (streq(s, target_funcs[i]->name))
			return target_funcs[i];
	}

	return 0;
}

void VCCompiler::SkipVariables()
{
	GetToken();						// variable type
	while (true)
	{
		if (TokenIs("struct"))
		{
			GetToken(2); // name, open brace
			SkipBrackets();
			if (NextIs(";"))
				Expect(";");
			return;
		}
		if(TokenIs("alias"))
		{
			GetToken(2); // type, new name
			Expect(";");
			return;
		}

		GetToken();					// variable name
		while (NextIs("["))			// array subscripts
			GetToken(3);

		if (NextIs("="))			// initializer
		{
			GetToken();
			int group=0;
			while (true)
			{
				if (TokenIs("("))
					group++;
				if (TokenIs(")"))
					group--;
				if (TokenIs("\""))  // quoted string
				{
					while (source[srcofs++] != '\"') {}
					source.setpos(srcofs);
					if (!group && (NextIs(";") || NextIs(","))) break;
					GetToken();
					continue;
				}
				if (!group && (NextIs(";") || NextIs(","))) break;
				GetToken();
			}
		}
		if (NextIs(";")) break;
		Expect(",");
	}
	Expect(";");
}


void VCCompiler::CompileFunction(bool returns_callback)
{
	if(!returns_callback)			// function return type (signature)
		GetToken();					//		(we only grab this when the function isn't known to be returning a callback (definition skipped earlier))
	GetToken();						// function name
	in_func = FetchFunction(token);	// grab the record for this function

	vprint("compiling function %s ->>>>\n", token);

	SkipArguments();				// get to the end of the arguements already
	Expect("{");
	returntype = in_func->signature;
	returnext = in_func->sigext;
	in_func->codeofs = output.curpos();
	in_func->numlocals = in_func->numargs;  // not really, but it'll be incremented as locals are declared

	bool is_variable_declaration;
	while (!NextIs("}"))
	{
		// Variable declaration? Try and see...
		{
			is_variable_declaration = true;
			// Save old token, in case we're wrong.
			int save_srcofs = srcofs;
			GetToken();

			char argtype = TypenameToTypeID(token);
			ext_definition argext;
			argext.type = EXT_NONE;

			if(argtype == t_CALLBACK)
			{
				argext.type = EXT_CALLBACK;
				ParseCallbackDefinition(&(argext.callback));
			}
			// Try other types.
			if(argtype == t_NOTFOUND)
			{
				// Alias?
				alias_definition* alias = FindAliasByName(token);
				if(alias != NULL)
				{
					// The real data type to declare.
					argtype = alias->type;

					// Keep alias reference in the extended definition info
					argext.type = EXT_ALIAS;
					argext.alias = alias;
				}
			}
			// Failed. Restore to old position.
			if(argtype != t_INT && argtype != t_STRING && argtype != t_CALLBACK)
			{
				is_variable_declaration = false;
				srcofs = save_srcofs;
			}
			// Local declaration list.
			else
			{
				in_func->argtype[in_func->numlocals] = argtype;
				in_func->argext[in_func->numlocals] = argext;

				// Name.
				GetIdentifierToken();
				CheckNameDup(token);
				for (int i = 0; i < in_func->numlocals; i++)
				{
					if (streq(token, in_func->localnames[i]))
					{
						throw va("%s(%d) : error: '%s' : redefinition", sourcefile, linenum, token);
					}
				}
				memcpy(in_func->localnames[in_func->numlocals++], token, 80);
				if (NextIs("=")) // initializer
				{
					CheckIdentifier(token);
					HandleAssign();					
				}
				while (!NextIs(";"))
				{
					Expect(",");
					in_func->argtype[in_func->numlocals] = argtype;
					in_func->argext[in_func->numlocals] = argext;
					GetIdentifierToken();
					CheckNameDup(token);
					for (int i = 0; i < in_func->numlocals; i++)
					{
						if (streq(token, in_func->localnames[i]))
						{
							throw va("%s(%d) : error: '%s' : redefinition", sourcefile, linenum, token);
						}
					}
					memcpy(in_func->localnames[in_func->numlocals++], token, 80);
					if (NextIs("=")) // initializer
					{					
						CheckIdentifier(token);
						HandleAssign();
					}
				}
				Expect(";");
			}
		}
		// Wasn't a variable declaration. Must be a statement!
		if (!is_variable_declaration)
			CompileStatement();
	}
	Expect("}");
	output.EmitC(opRETURN);
	in_func->codeend = output.curpos();
	vprint("Finished compiling function %s\n", in_func->name);
	in_func = 0;	
}

void VCCompiler::CheckIdentifier(char *s)
{
	int i,j;

	// check library functions first
	for (i=0; i<NUM_LIBFUNCS; i++)
		if (streq(s, libfuncs[i].name.c_str()))
		{
			id_type = ID_LIBFUNC;
			id_index = i;
			id_subtype = libfuncs[i].returnType;
			return;
		}

	// check user functions next,
	// each core image in order of reference_cimages
	for (i=0; i<reference_cimages.size(); i++)
	{
		int current_cimage = reference_cimages[i];
		// we search in reverse order so that we always
		// bind to the most recent one in that core image
		for(j=funcs[current_cimage].size()-1; j>=0; j--)
		{
			if(streq(s, funcs[current_cimage][j]->name))
			{
				id_type = ID_USERFUNC;
				id_subtype = funcs[current_cimage][j]->signature;
				id_cimage = current_cimage;
				id_index = j;
				return;
			}
		}
	}

	// check HVars next
	for (i=0; i<hvars.size(); i++)
		if (streq(s, hvars[i]->name))
		{
			id_type = ID_VARIABLE;
			id_subtype = ID_HVAR;
			id_array = (hvars[i]->dim) ? 1 : 0;
			id_index = i;
			return;
		}

	// check globals next
	for (i=0; i<global_vars.size(); i++)
		if (streq(s, global_vars[i]->name))
		{
			id_type = ID_VARIABLE;
			switch(global_vars[i]->type)
			{
				case t_INT: id_subtype = ID_GLOBALINT; break;
				case t_STRING: id_subtype = ID_GLOBALSTR; break;
				case t_CALLBACK: id_subtype = ID_GLOBALCB; id_ext = global_vars[i]->ext; break;
			}
			id_array = (global_vars[i]->len > 0) ? 1 : 0; // len == 0 is a non-array
			id_index = i;
			return;
		}

	// check function arguements and local variables
	if (in_func)
	{
		for (i=0; i<in_func->numlocals; i++)
		{
			if (streq(s, in_func->localnames[i]))
			{
				switch (in_func->argtype[i])
				{
					case t_INT:
						id_type = ID_VARIABLE;
						id_subtype = ID_LOCALINT;
						id_array = 0;
						id_index = i;
						return;
					case t_STRING:
						id_type = ID_VARIABLE;
						id_subtype = ID_LOCALSTR;
						id_array = 0;
						id_index = i;
						return;
					case t_CALLBACK:
						id_type = ID_VARIABLE;
						id_subtype = ID_LOCALCB;
						id_array = 0;
						id_index = i;
						id_ext = in_func->argext[i];
						return;
					case t_VARARG:
						id_type = ID_VARARG_LIST;
						if (NextIs("."))
						{
							char buf[100];

							GetToken();
							GetToken();
							sprintf(buf, "@__argument.%s", token);
							for (i=0; i<hvars.size(); i++)
								if (streq(buf, hvars[i]->name))
								{
									id_type = ID_VARIABLE;
									id_subtype = ID_HVAR;
									id_array = (hvars[i]->dim) ? 1 : 0;
									id_index = i;
									return;
								}
						}
						return;
					default:
						break;	// bad.
				}
			}
		}
	}

	// check struct instances
	for (i=0; i<struct_instances.size(); i++)
		if (streq(s, struct_instances[i]->name))
		{
			id_type = ID_STRUCT;
			id_subtype = ID_STRUCT;
			//id_array = struct_instances[i]->dim;
			id_array = (struct_instances[i]->dim > 0) ? 1 : 0; // dim == 0 is a non-array
			id_index = i;
			return;
		}

	//check plugins
	VCPlugins::IdentifierLookup lookup;
	VCPlugins::checkIdentifier(s,lookup);
	if(lookup.kind == VCPlugins::IdentifierLookup::FUNCTION) {
		id_type = ID_PLUGINFUNC;
		id_index = lookup.plugin->index;
		id_subtype = lookup.plugin->type;
		return;
	} else if(lookup.kind == VCPlugins::IdentifierLookup::VARIABLE) {
		id_type = ID_PLUGINVAR;
		id_index = lookup.plugin->index;
		id_array = lookup.variable->dimensions;
		id_subtype = lookup.plugin->type;
		id_access = 0;
		if(lookup.variable->readfunc) id_access |= ID_ACCESS_READ;
		if(lookup.variable->writefunc) id_access |= ID_ACCESS_WRITE;
		return;
	}



	char buf[100], buf2[100];

	/* ok special handling here. At this point we have found no other matches.
	   However one additional valid possibility remains: if the user is referencing a
	   builtin var like mouse.x or entity.y[], then the token which will be read in is
	   only "entity" or "mouse", which won't match anything. So now we'll check to see
	   if a . is after the current token, and if it is, we'll read it in, read in
	   the following thing, and then scan _hvar's only_ to find a new match. */

	if (NextIs("."))
	{
		sprintf(buf2, s);
		GetToken(2);
		sprintf(buf, "%s.%s", buf2, token);
		for (i=0; i<hvars.size(); i++)
			if (streq(buf, hvars[i]->name))
			{
				id_type = ID_VARIABLE;
				id_subtype = ID_HVAR;
				id_array = (hvars[i]->dim) ? 1 : 0;
				id_index = i;
				return;
			}
	}

	id_type = ID_NOMATCH;
}

// Overkill (2006-06-30):
// Checks if the token is definitely a string for error messages.
bool VCCompiler::TokenIsStringExpression()
{
	if (TokenIs("\""))
	{
		return true;
	}
	if (TokenIs("str") || TokenIs("string"))
    {
		if (NextIs("("))
		{
			return true;
		}
    }
    if (TokenIs("left"))
    {
		if (NextIs("("))
		{
			return true;
		}
    }
    if (TokenIs("right"))
    {
		if (NextIs("("))
		{
			return true;
		}
    }
    if (TokenIs("mid"))
    {
		if (NextIs("("))
		{
			return true;
		}
    }
	return false;
}

// Overkill (2006-06-30):
// Checks if the token is definitely an int for error messages.
bool VCCompiler::TokenIsIntExpression()
{
    if (chr_table[token[0]] == DIGIT)
    {
		return true;
	}
	if (TokenIs("("))
    {
		return true;
	}
	if (TokenIs("!") || TokenIs("not"))
	{
		// logical negation
		return true;
	}
	if (TokenIs("~"))
	{
		// binary NOT
		return true;
	}
	if (TokenIs("-"))
	{
		// negation
		return true;
	}
	return false;
}

void VCCompiler::CompileAtom()
{
	// check for things that bind really tightly,
	// and so basically are part of the atom
	if (NextIs("("))
    {
		output.EmitC(intGROUP);
		GetToken();
		CompileOperand();
		Expect(")");
		return;
	} else if (NextIs("!") || NextIs("not")) {
		// logical negation
		output.EmitC(ifZERO);
		GetToken(); // skip !
		CompileAtom(); // try again
		return;
	} else if (NextIs("~")) {
		// binary NOT
		output.EmitC(iopNOT);
		GetToken(); // skip ~
		CompileAtom();
		return;
	} else if (NextIs("-")) {
		// negation 
		output.EmitC(iopNEGATE);
		GetToken(); // skip -
		CompileAtom();
		return;
	} else if (NextIs("+")) {
		// Unary + (ignore, but allow)
		GetToken(); // skip +
		CompileAtom();
		return;
	} else if (NextIs("FunctionExists")) {
		// Function existance detection
		GetToken(); // skip
		Expect("(");
		output.EmitC(opCBFUNCEXISTS);
		CompileCallback(NULL);
		Expect(")");
		return;
	} else if(NextIs("int")) {
		// String-to-integer casting, alias for val().
		GetToken(); // skip
		output.EmitC(intLIBFUNC);
		CheckIdentifier("val");
        HandleLibraryFunc();
        return;
	}

    GetToken();

	// Check for integer literal
    if (chr_table[token[0]] == DIGIT)
    {
        output.EmitC(intLITERAL);
        output.EmitD(token_value);
        return;
    }

	// Check for variable or function.
	CheckIdentifier(token);

	if(id_type == ID_PLUGINFUNC)
	{
		if(id_subtype != VCPlugins::RETURN_INT)
		    throw va("%s(%d): plugin function %s does not return an int!", sourcefile, linenum, token);
        output.EmitC(intPLUGINFUNC);
        HandlePluginFunc();
        return;
	}
    if (id_type == ID_LIBFUNC)
    {
		if (libfuncs[id_index].returnType != t_INT && libfuncs[id_index].returnType != t_BOOL)
			throw va("%s(%d): %s does not return an int", sourcefile, linenum, token);
        output.EmitC(intLIBFUNC);
        HandleLibraryFunc();
        return;
    }
    if (id_type == ID_USERFUNC)
    {
		if (funcs[id_cimage][id_index]->signature != t_INT)
		    throw va("%s(%d): %s does not return an int", sourcefile, linenum, token);
        output.EmitC(intUSERFUNC);
        HandleUserFunc();
        return;
    }


	int varofs = srcofs;
	std::string varname = std::string(token);
	ext_definition ext;
	int type = HandleVariable(&ext);
	// Found an int, good.
	if (type == t_INT)
	{
		return;
	}
	if(type == t_CALLBACK)
	{
		if(NextIs("("))
		{
			GetToken();
			HandleCallbackInvocation(ext.callback);
		}
		else
		{
			throw va("%s(%d): Expected an int, but the callback expression, '%s', was found instead.", sourcefile, linenum, token);
		}
		return;
	}
	// Found something, but it wasn't an int.
	if (type != -1)
	{
		int thisofs = srcofs;
		srcofs = varofs;
		while(srcofs < thisofs)
		{
			ParseWhitespace();
			if (source[srcofs] == '[')
			{
				varname += "[";
				while (source[srcofs] != ']' && srcofs < thisofs)
				{
					srcofs++;
				}
				varname += "]";
			}
			else
			{
				varname += source[srcofs];
			}
			srcofs++;
		}
		throw va("%s(%d): %s is not an int", sourcefile, linenum, varname.c_str());
	}

	// Overkill (2006-06-30): At this point, there are no matches.
	// Try and see if they passed a string instead.
	if (TokenIsStringExpression())
	{
		throw va("%s(%d): Expected an int, but the string expression, '%s', was found instead.", sourcefile, linenum, token);
	}

	throw va("%s(%d): Could not resolve identifier '%s'!", sourcefile, linenum, token);
}

// call to compile an expression that results in an int
void VCCompiler::CompileOperand()
{
	// Stategy:
	// We compile the left hand side normally
	// for each level of precedence. Anything after
	// that is grouped to maintain precedence in the
	// runtime core. Each of CompileOperand, CompileTerm
	// CompileSubTerm and CompileAtom deal with a different
	// precedence level.

	CompileTerm();

    while (true)
    {
		if		(NextIs("&&") || NextIs("and"))	{output.EmitC(ifAND);			GetToken();}
		else if (NextIs("||") || NextIs("or"))  {output.EmitC(ifOR);			GetToken();}
		else break;

		output.EmitC(intGROUP);
		CompileTerm();
		output.EmitC(iopEND);
	}
	output.EmitC(iopEND); // always end an operand with a iopEND
}

void VCCompiler::CompileTerm()
{
	CompileSubTerm();

	while(true) {
		if		(NextIs("="))   { output.EmitC(ifEQUAL);          GetToken();}
		else if (NextIs("!="))  { output.EmitC(ifNOTEQUAL);       GetToken();}
		else if (NextIs(">"))   { output.EmitC(ifGREATER);        GetToken();}
		else if (NextIs(">="))  { output.EmitC(ifGREATEROREQUAL); GetToken();}
		else if (NextIs("<"))   { output.EmitC(ifLESS);           GetToken();}
		else if (NextIs("<="))  { output.EmitC(ifLESSOREQUAL);    GetToken();}
		else break;

		output.EmitC(intGROUP);
		CompileSubTerm();
		output.EmitC(iopEND);
	}

}

void VCCompiler::CompileSubTerm()
{
	CompileAtom();

	while(true) {
		if (NextIs("+"))		{ output.EmitC(iopADD);  GetToken(); }
		else if (NextIs("-"))	{ output.EmitC(iopSUB);  GetToken(); }
		else if (NextIs("/"))	{ output.EmitC(iopDIV);  GetToken(); }
		else if (NextIs("*"))	{ output.EmitC(iopMULT); GetToken(); }
		else if (NextIs("%"))	{ output.EmitC(iopMOD);  GetToken(); }
		else if (NextIs(">>"))  { output.EmitC(iopSHR);  GetToken(); }
		else if (NextIs("<<"))  { output.EmitC(iopSHL);  GetToken(); }
		else if (NextIs("&"))	{ output.EmitC(iopAND);  GetToken(); }
		else if (NextIs("|"))   { output.EmitC(iopOR);   GetToken(); }
		else if (NextIs("^"))	{ output.EmitC(iopXOR);  GetToken(); }
		else break;

		CompileAtom();
	}
}

void VCCompiler::EmitStringLiteral()
{
	Expect("\"");
	source.setpos(srcofs);    //FIXME: watch for this. Why do I have to set this? this may sneak up later.
    while (source[srcofs] != '\"')
    {
        if (source[srcofs] == '\n')
			throw va("%s(%d): Unterminated string literal.", sourcefile, linenum);
        output.EmitC(source.GrabC());
		srcofs++;
    }
	output.EmitC(0);
    Expect("\"");
}

void VCCompiler::EmitSingleStringLiteral()
{
	Expect("\'");
	source.setpos(srcofs);    //FIXME: watch for this. Why do I have to set this? this may sneak up later.
    while (source[srcofs] != '\'')
    {
        if (source[srcofs] == '\n')
			throw va("%s(%d): Unterminated string literal.", sourcefile, linenum);
        output.EmitC(source.GrabC());
		srcofs++;
    }
	output.EmitC(0);
    Expect("\'");
	//log("%d", srcofs);
}

void VCCompiler::ProcessString()
{
	if (NextIs("\""))
    {
        output.EmitC(strLITERAL);
        EmitStringLiteral();
        return;
    }

    GetToken();
	CheckIdentifier(token);

	if (TokenIs("str") || TokenIs("string"))
    {
        output.EmitC(strINT);
        Expect("(");
        CompileOperand();
        Expect(")");
        return;
    }
    if (TokenIs("left"))
    {
        output.EmitC(strLEFT);
        Expect("(");
        CompileString();
        Expect(",");
        CompileOperand();
        Expect(")");
        return;
    }
    if (TokenIs("right"))
    {
        output.EmitC(strRIGHT);
        Expect("(");
        CompileString();
        Expect(",");
        CompileOperand();
        Expect(")");
        return;
    }
    if (TokenIs("mid"))
    {
        output.EmitC(strMID);
        Expect("(");
        CompileString();
        Expect(",");
        CompileOperand();
        Expect(",");
        CompileOperand();
        Expect(")");
        return;
    }

	if(id_type == ID_PLUGINFUNC)
	{
		if(id_subtype != VCPlugins::RETURN_STRING)
		    throw va("%s(%d): plugin variable %s does not return a string!", sourcefile, linenum, token);
        output.EmitC(strPLUGINFUNC);
        HandlePluginFunc();
        return;
	}
	if (id_type == ID_LIBFUNC)
    {
        if (libfuncs[id_index].returnType != t_STRING)
			throw va("%s(%d): %s does not return a string", sourcefile, linenum, token);
        output.EmitC(strLIBFUNC);
        HandleLibraryFunc();
        return;
    }
    if (id_type == ID_USERFUNC)
    {
		if (funcs[id_cimage][id_index]->signature != t_STRING)
		    throw va("%s(%d): %s does not return a string", sourcefile, linenum, token);
        output.EmitC(strUSERFUNC);
        HandleUserFunc();
        return;
    }

	int varofs = srcofs;
	std::string varname = std::string(token);
	ext_definition ext;
	int type = HandleVariable(&ext);
	// Found a string, good.
	if (type == t_STRING)
	{
		return;
	}
	if(type == t_CALLBACK)
	{
		if(NextIs("("))
		{
			GetToken();
			HandleCallbackInvocation(ext.callback);
		}
		else
		{
			throw va("%s(%d): Expected an string, but the callback expression, '%s', was found instead.", sourcefile, linenum, token);
		}
		return;
	}

	// Found something, but it wasn't a string.
	if (type != -1)
	{
		int thisofs = srcofs;
		srcofs = varofs;
		while(srcofs < thisofs)
		{
			ParseWhitespace();
			if (source[srcofs] == '[')
			{
				varname += "[";
				while (source[srcofs] != ']' && srcofs < thisofs)
				{
					srcofs++;
				}
				varname += "]";
			}
			else
			{
				varname += source[srcofs];
			}
			srcofs++;
		}
		throw va("%s(%d): %s is not a string", sourcefile, linenum, varname.c_str());
	}

	if (TokenIsIntExpression())
	{
		throw va("%s(%d): Expected a string, but the int expression, '%s', was found instead.", sourcefile, linenum, token);
	}

	throw va("%s(%d): %s is not a known string identifier", sourcefile, linenum, token);
}

void VCCompiler::CompileString()
{
    while (true)
    {
        ProcessString();
        if (NextIs("+"))
        {
            output.EmitC(sADD);
            GetToken();
            continue;
        }
        else
        {
            output.EmitC(sEND);
            break;
        }
	}
}

// Compare signatures of two functions/callbacks, recursing as necessary.
// Returns true if they are identical, false if there are differences.
// IMPORTANT: IF YOU UPDATE THIS, UPDATE THE FUNCTION_T VERSION TOO.
bool VCCompiler::VerifySignatureMatch(callback_definition* expected, callback_definition* value)
{
	// Return values don't match.
	if(expected->signature != value->signature)
	{
		return false;
	}
	// Okay, hooray, the return values are a match!
	else 
	{
		// But if it's a callback, we have to recurse to prove they have identical callback definitions.
		if(expected->signature == t_CALLBACK)
		{
			if(!VerifySignatureMatch(expected->sigext.callback, value->sigext.callback))
			{
				return false;
			}
		}
	}

	// Number of arguments don't match, so the arguments won't match.
	if(expected->numargs != value->numargs)
	{
		return false;
	}

	for(int i = 0; i < expected->numargs; i++)
	{
		// Argument types don't match.
		if(expected->argtype[i] != value->argtype[i])
		{
			return false;
		}
		// Okay, hooray, the argument types being compared are a match!
		else 
		{
			// But if it's a callback, we have to recurse to prove they have identical callback definitions.
			if(expected->signature == t_CALLBACK)
			{
				if(!VerifySignatureMatch(expected->argext[i].callback, value->argext[i].callback))
				{
					return false;
				}
			}
		}
	}

	// They were identical. Woo.
	return true;
}

// Compare signatures of two functions/callbacks, recursing as necessary.
// Returns true if they are identical, false if there are differences.
// IMPORTANT: IF YOU UPDATE THIS, UPDATE THE CALLBACK_DEFINITION VERSION TOO.
bool VCCompiler::VerifySignatureMatch(callback_definition* expected, function_t* value)
{
	// Return values don't match.
	if(expected->signature != value->signature)
	{
		return false;
	}
	// Okay, hooray, the return values are a match!
	else 
	{
		// But if it's a callback, we have to recurse to prove they have identical callback definitions.
		if(expected->signature == t_CALLBACK)
		{
			if(!VerifySignatureMatch(expected->sigext.callback, value->sigext.callback))
			{
				return false;
			}
		}
	}

	// Number of arguments don't match, so the arguments won't match.
	if(expected->numargs != value->numargs)
	{
		return false;
	}

	for(int i = 0; i < expected->numargs; i++)
	{
		// Argument types don't match.
		if(expected->argtype[i] != value->argtype[i])
		{
			return false;
		}
		// Okay, hooray, the argument types being compared are a match!
		else 
		{
			// But if it's a callback, we have to recurse to prove they have identical callback definitions.
			if(expected->signature == t_CALLBACK)
			{
				if(!VerifySignatureMatch(expected->argext[i].callback, value->argext[i].callback))
				{
					return false;
				}
			}
		}
	}

	// They were identical. Woo.
	return true;
}

// Compile an expression for assignment to a callback variable.
// The expression must match the definition of the variable.
// If def is NULL, then any callback applies.
void VCCompiler::CompileCallback(callback_definition* def)
{
	// A run-time resolved callback reference.
	if (CheckExpressionType() == t_STRING)
	{
		output.EmitC(t_STRING);
		CompileString();
		return;
	}

	// Okay, err. grab the token, and try the other cases.
    GetToken();

	// The null function (the default value for callbacks).
	if(TokenIs("0"))
	{
		output.EmitC(t_INT);
		return;
	}

	// Okay, onto identifier named things.
	CheckIdentifier(token);

	// Library function reference.
	if (id_type == ID_LIBFUNC)
    {
		bool fail = false;
		if(def != NULL && libfuncs[id_index].returnType != def->signature)
		{
			fail = true;
		}

		int i = 0;
		while (def != NULL && i < libfuncs[id_index].argumentTypes.size())
		{
			if(fail || libfuncs[id_index].argumentTypes[i] != def->argtype[i])
			{
				fail = true;
				break;
			}
			i++;
		}
		// No library function returns a callback.
		if(NextIs("("))
		{
			throw va("%s(%d): Function %s's return value does not match the callback signature expected", sourcefile, linenum, libfuncs[id_index].name);
		}

		output.EmitC(opLIBFUNC);
		// Overkill (2006-06-07): Now functions past 255 work.
		// Yay! We'll probably never reach the 65535 mark,
		// so we're safe again.
		output.EmitW(id_index);
        return;
    }
	// User function
    if (id_type == ID_USERFUNC)
    {
		int index = id_index;
		function_t *func = funcs[id_cimage][id_index];

		// If it's a function call, then handle that.
		// That means the the value given to the callback is the return value of the function call.
		// The returned value from the call must be a callback that recursively matches the definition of the callback expected.
		if(NextIs("("))
		{
			// First, we'd better whine if the function does not return a callback, or the callback returned is not the one we expected.
			if(def != NULL && (func->signature != t_CALLBACK || !VerifySignatureMatch(def, func->sigext.callback)))
			{
				// TODO: comprehensive error report with full signatures printed.
				throw va("%s(%d): Function %s's return value does not match the callback signature expected", sourcefile, linenum, func->name);
			}
			// This is confusingly close to the bytecode for function references, save for one byte.
			output.EmitC(cbUSERFUNC);
			HandleUserFunc();
		}
		// A function reference, not a function call.
		// That means the the value given to the callback is a reference to the function.
		// so the function itself must recursively match the signature of the callback expected.
		else
		{
			if(def != NULL && !VerifySignatureMatch(def, func))
			{
				// TODO: comprehensive error report with full signatures printed.
				throw va("%s(%d): Function %s does not match the callback signature expected", sourcefile, linenum, func->name);
			}
			output.EmitC(opUSERFUNC);
			output.EmitC(func->coreimage);
			output.EmitD(index);
		}
		return;
    }

	int varofs = srcofs;
	std::string varname = std::string(token);
	ext_definition ext;
	int type = HandleVariable(&ext);
	// Found a callback variable.
	if (type == t_CALLBACK)
	{
		// Value given to the callback is the return value of another callback.
		if(NextIs("("))
		{
			if(def != NULL && !VerifySignatureMatch(def, ext.callback->sigext.callback))
			{
				throw va("%s(%d): Callback's return type does not match the callback signature expected", sourcefile, linenum);
			}
			GetToken();
			HandleCallbackInvocation(ext.callback);
		}
		// Value given to the callback is a copy of the other callback.
		else
		{
			// Make sure it matches.
			if(def != NULL && !VerifySignatureMatch(def, ext.callback))
			{
				throw va("%s(%d): Callback does not match the callback signature expected", sourcefile, linenum);
			}
			// We emit a "copy" opcode so to not confuse with local callbacks that are being called.
			output.EmitC(opCBCOPY);
		}
		return;
	}

	// Found something, but it wasn't a callback.
	if (type != -1)
	{
		int thisofs = srcofs;
		srcofs = varofs;
		while(srcofs < thisofs)
		{
			ParseWhitespace();
			if (source[srcofs] == '[')
			{
				varname += "[";
				while (source[srcofs] != ']' && srcofs < thisofs)
				{
					srcofs++;
				}
				varname += "]";
			}
			else
			{
				varname += source[srcofs];
			}
			srcofs++;
		}
		throw va("%s(%d): %s is not a callback", sourcefile, linenum, varname.c_str());
	}
	if (TokenIsIntExpression())
	{
		throw va("%s(%d): Expected a callback, but the int expression, '%s', was found instead.", sourcefile, linenum, token);
	}
	throw va("%s(%d): %s is not a known callback identifier", sourcefile, linenum, token);
}

void VCCompiler::CompileStatement()
{
	int current_offset = srcofs;
	if (debugon && output.curpos() >= debugofs)
		throw "debug offset hit";

	// Syntactic sugar for CallFunction, via parentheses after a string.
	// Allows for "MyFunction"(); instead of CallFunction("MyFunction");
	if (CheckExpressionType() == t_STRING)
	{
		int temp = output.curpos();
		CompileString();
		output.setpos(temp);

		if (NextIs("("))
		{
			srcofs = current_offset;
			
			CheckIdentifier("CallFunction");
			output.EmitC(opLIBFUNC);
			output.EmitW(id_index);
			CompileString();
			Expect("(");
			output.EmitC(opVARARG_START);
			while (true)
			{
				if (NextIs(")"))
				{
					break;
				}
				int type = CheckExpressionType();
				output.EmitC(type);
				if (type == -1)
				{
					GetToken();
					throw va("%s(%d) Could not resolve identifier '%s'!", sourcefile, linenum, token);
					break;
				}
				if (type == t_VARARG)
				{
					GetToken();
					CheckIdentifier(token);

					// Don't need to do anything more, we just skip over the token now,
					// since we already know its type.
					// break, there can't be any more arguments after this one.
					break;
				}
				if (type == t_INT)
				{
					CompileOperand();
					if (NextIs(",")) GetToken();
				}
				if (type == t_STRING)
				{
					CompileString();
					if (NextIs(",")) GetToken();
				}
			}
			Expect(")");
			Expect(";");
			output.EmitC(opVARARG_END);

			return;
		}
		srcofs = current_offset;
	}

	GetToken();

	// check language keywords.
	if (TokenIs("return"))	{ HandleReturn(); return; }
	if (TokenIs("if"))		{ HandleIf(); return; }
	if (TokenIs("unless"))		{ HandleIf(true); return; } // Overkill (2006-07-06): Unless is opposite of if.
	if (TokenIs("while"))	{ HandleWhile(); return; }
	if (TokenIs("until"))		{ HandleWhile(true); return; } // Overkill (2006-07-06): Until is opposite of while.
	if (TokenIs("for"))	{ HandleFor(); return ; }
	if (TokenIs("switch"))	{ HandleSwitch(); return; }

	//check function/variable names
	CheckIdentifier(token);
	switch (id_type)
	{
		case ID_NOMATCH:
			throw va("%s(%d): \"%s\" is not a recognized keyword or identifier!", sourcefile, linenum, token);
			return;
		case ID_LIBFUNC:
			HandleLibraryFunc();
			Expect(";");
			return;
		case ID_PLUGINFUNC:
			HandlePluginFunc();
			Expect(";");
			return;
		case ID_USERFUNC:
			HandleUserFunc();
			Expect(";");
			return;
		
		case ID_PLUGINVAR:
		case ID_VARIABLE:
		case ID_STRUCT:
			HandleAssign();
			if (NextIs(")"))
			{
				Expect(")");
				return;
			}
			Expect(";");
			return;
		default:
			throw va("%s(%d): unimplemented id_type code... ", sourcefile, linenum);
			return;
	}
}

void VCCompiler::HandleReturn()
{
	switch (returntype)
	{
		case t_VOID:
			Expect(";");
            output.EmitC(opRETURN);
            return;
		case t_INT:
			output.EmitC(opRETVALUE);
			CompileOperand();
			Expect(";");
			output.EmitC(opRETURN);
			return;
		case t_STRING:
			output.EmitC(opRETSTRING);
			CompileString();
			Expect(";");
			output.EmitC(opRETURN);
			return;
		case t_CALLBACK:
			output.EmitC(opRETCB);
			CompileCallback(returnext.callback);
			Expect(";");
			output.EmitC(opRETURN);
			return;
		default:
			throw va("%s(%d): Internal compiler error in VCCompiler::HandleReturn()!", sourcefile, linenum);
	}
}

void VCCompiler::HandleLibraryFunc()
{
	static bool initialized = false;
	static callback_definition voidNoArgsCallback;
	if(!initialized)
	{
		voidNoArgsCallback.signature = t_VOID;
		voidNoArgsCallback.numargs = 0;
		initialized = true;
	}


	Expect("(");
	output.EmitC(opLIBFUNC);
	// Overkill (2006-06-07): Now functions past 255 work.
	// Yay! We'll probably never reach the 65535 mark,
	// so we're safe again.
	output.EmitW(id_index);

	int myindex = id_index;		// id_index may be changed during the arguement reading process


	int i = 0;
	bool varargs = false;
	while (true)
	{
		if (NextIs(")"))
		{
			if (i < libfuncs[myindex].argumentTypes.size() && libfuncs[myindex].argumentTypes[i] == t_VARARG)
			{
				output.EmitC(opVARARG_START);
				varargs = true;
				i++;
			}
			break;
		}
		if (i < libfuncs[myindex].argumentTypes.size())
		{
			switch (libfuncs[myindex].argumentTypes[i])
			{
				case t_INT:
				case t_BOOL:
					CompileOperand();
					if (NextIs(",")) GetToken();
					break;
				case t_STRING:
					CompileString();
					if (NextIs(",")) GetToken();
					break;
				case t_CALLBACK:
					// Void, no args callback.
					// If for some reason there's a need for other callback types,
					// add some new type enumerations to opcodes.h.
					// And setup in the if(!initialized) { ... } segment above.
					CompileCallback(&voidNoArgsCallback);
					if (NextIs(",")) GetToken();
					break;
				case t_VARARG:
					output.EmitC(opVARARG_START);
					varargs = true;
					break;
				default:
					throw va("VCCompiler::HandleLibraryFunc() - Internal error, no such identifier type for libfunc %d", myindex);
					return;
			}
		}
		else
		{
			int type = CheckExpressionType();
			output.EmitC(type);
			if (type == -1)
			{
				GetToken();
				throw va("%s(%d) Could not resolve identifier '%s'!", sourcefile, linenum, token);
				break;
			}
			if (type == t_VARARG)
			{
				GetToken();
				CheckIdentifier(token);

				// Don't need to do anything more, we just skip over the token now,
				// since we already know its type.
				// break, there can't be any more arguments after this one.
				break;
			}
			if (type == t_INT)
			{
				CompileOperand();
				if (NextIs(",")) GetToken();
			}
			if (type == t_STRING)
			{
				CompileString();
				if (NextIs(",")) GetToken();
			}
		}
		i++;
	}
	Expect(")");
	if (varargs)
	{
		output.EmitC(opVARARG_END);
	}
	if (i < libfuncs[myindex].argumentTypes.size() || (i > libfuncs[myindex].argumentTypes.size() && !varargs))
	{
		int size = libfuncs[myindex].argumentTypes.size();

		// Varargs need not be specified so don't include in the count here.
		if(libfuncs[myindex].argumentTypes[size - 1] == t_VARARG)
		{
			size--;
		}
		throw va("%s(%d) Built in function %s() expects %d arguments. (Got %d)", sourcefile, linenum, libfuncs[myindex].name.c_str(), size, i);
	}

	id_index = myindex;
}

void VCCompiler::HandlePluginFunc() {
	Expect("(");
	output.EmitC(opPLUGINFUNC);
	output.EmitD(id_index);
	VCPlugins::Function *function = VCPlugins::functionByIndex(id_index);
	for(char *cp = function->arguments;*cp;cp++) {
		switch(*cp) {
			case '1': CompileOperand(); if(NextIs(",")) GetToken(); break;
			case '3': CompileString(); if(NextIs(",")) GetToken(); break;
		}
	}
	Expect(")");
}

void VCCompiler::HandleCallbackInvocation(callback_definition* func)
{
	output.EmitC(opCBINVOKE);

	int i = 0;
	bool varargs = false;
	while (true)
	{
		if (NextIs(")"))
		{
			if (i < func->numargs && func->argtype[i] == t_VARARG)
			{
				output.EmitC(opVARARG_START);
				varargs = true;
				i++;
			}
			break;
		}
		if (i < func->numargs)
		{
			switch (func->argtype[i])
			{
				case t_INT:
					CompileOperand();
					if (NextIs(",")) GetToken();
					break;
				case t_STRING:
					CompileString();
					if (NextIs(",")) GetToken();
					break;
				case t_CALLBACK:
					CompileCallback(func->argext[i].callback);
					if (NextIs(",")) GetToken();
					break;
				case t_VARARG:
					output.EmitC(opVARARG_START);
					varargs = true;
					break;
			}
		}
		else
		{
			int type = CheckExpressionType();
			output.EmitC(type);
			if (type == -1)
			{
				GetToken();
				throw va("%s(%d) Could not resolve identifier '%s'!", sourcefile, linenum, token);
				break;
			}
			else if (type == t_VARARG)
			{
				GetToken();
				CheckIdentifier(token);

				// Don't need to do anything more, we just skip over the token now,
				// since we already know its type.
				// break, there can't be any more arguments after this one.
				break;
			}
			else if (type == t_INT)
			{
				CompileOperand();
				if (NextIs(",")) GetToken();
			}
			else if (type == t_STRING)
			{
				CompileString();
				if (NextIs(",")) GetToken();
			}
			else
			{
				throw va("%s(%d) Identifier '%s' has a value that is not permitted in variadic function lists!", sourcefile, linenum, token);
			}
		}
		i++;
	}
	Expect(")");

	if (varargs)
	{
		output.EmitC(opVARARG_END);
	}
	if (i < func->numargs || (i > func->numargs && !varargs))
	{
		int size = func->numargs;

		// Varargs need not be specified so don't include in the count here.
		if(func->argtype[size - 1] == t_VARARG)
		{
			size--;
		}

		throw va("%s(%d) Callback invocation expects %d arguments. (Got %d)", sourcefile, linenum, size, i);
	}
	// Need to pad after the function call if the invocation fails due to it not existing.
	output.EmitC(opCBPADDING);
}

void VCCompiler::HandleUserFunc()
{
	function_t *func = funcs[id_cimage][id_index];
	output.EmitC(opUSERFUNC);
	output.EmitC(id_cimage);
	output.EmitD(id_index);
	int myindex = id_index;		// id_* may be changed during the arguement reading process
	int mycimage = id_cimage;
    Expect("(");

	int i = 0;
	bool varargs = false;
	while (true)
	{
		if (NextIs(")"))
		{
			if (i < func->numargs && func->argtype[i] == t_VARARG)
			{
				output.EmitC(opVARARG_START);
				varargs = true;
				i++;
			}
			break;
		}
		if (i < func->numargs)
		{
			switch (func->argtype[i])
			{
				case t_INT:
					CompileOperand();
					if (NextIs(",")) GetToken();
					break;
				case t_STRING:
					CompileString();
					if (NextIs(",")) GetToken();
					break;
				case t_CALLBACK:
					CompileCallback(func->argext[i].callback);
					if (NextIs(",")) GetToken();
					break;
				case t_VARARG:
					output.EmitC(opVARARG_START);
					varargs = true;
					break;
			}
		}
		else
		{
			int type = CheckExpressionType();
			output.EmitC(type);
			if (type == -1)
			{
				GetToken();
				throw va("%s(%d) Could not resolve identifier '%s'!", sourcefile, linenum, token);
				break;
			}
			else if (type == t_VARARG)
			{
				GetToken();
				CheckIdentifier(token);

				// Don't need to do anything more, we just skip over the token now,
				// since we already know its type.
				// break, there can't be any more arguments after this one.
				break;
			}
			else if (type == t_INT)
			{
				CompileOperand();
				if (NextIs(",")) GetToken();
			}
			else if (type == t_STRING)
			{
				CompileString();
				if (NextIs(",")) GetToken();
			}
			else
			{
				throw va("%s(%d) Identifier '%s' has a value that is not permitted in variadic function lists!", sourcefile, linenum, token);
			}
		}
		i++;
	}
	Expect(")");

	if (varargs)
	{
		output.EmitC(opVARARG_END);
	}
	if (i < func->numargs || (i > func->numargs && !varargs))
	{
		int size = func->numargs;

		// Varargs need not be specified so don't include in the count here.
		if(func->argtype[size - 1] == t_VARARG)
		{
			size--;
		}

		throw va("%s(%d) User function %s() expects %d arguments. (Got %d)", sourcefile, linenum, func->name, size, i);
	}

	id_index = myindex;
	id_cimage = mycimage;
}

int VCCompiler::CheckExpressionType()
{
	int type;
	int thisofs = srcofs;

	GetToken();

	if (TokenIsStringExpression())
	{
		srcofs = thisofs;
		return t_STRING;
	}
	if (TokenIsIntExpression())
	{
		srcofs = thisofs;
		return t_INT;
	}

	int temp = output.curpos();
	CheckIdentifier(token);
	if (id_type == ID_LIBFUNC || id_type == ID_USERFUNC || id_type == ID_PLUGINFUNC)
	{
		srcofs = thisofs;
		return id_subtype;
	}
	type = HandleVariable(NULL);
	output.setpos(temp);
	srcofs = thisofs;
	return type;
}

// Parse a variable reference.
// The ext argument gives extended info on a variable (such as a callback) if not null.
int VCCompiler::HandleVariable(ext_definition* ext)
{
	int vartype = -1; // Type of variable to be set.
	int type = -1; // Overkill (2006-05-05): t_STRING or t_INT or t_CALLBACK. Type of value expected.
	if (id_type == ID_NOMATCH)
	{
		return -1;
	}
	else if(id_type == ID_VARARG_LIST)
	{
		return t_VARARG;
	}
	else if(id_type == ID_PLUGINVAR)
	{
		if(id_subtype == VCPlugins::RETURN_INT) vartype = intPLUGINVAR;
		if(id_subtype == VCPlugins::RETURN_STRING) vartype = strPLUGINVAR;
	}
	else
	{
		switch (id_subtype)
		{
			case ID_GLOBALINT:
				vartype = (id_array ? intARRAY : intGLOBAL);
				type = t_INT;
				break;
			case ID_GLOBALSTR:
				vartype = (id_array ? strARRAY : strGLOBAL);
				type = t_STRING;
				break;
			case ID_LOCALINT:
				vartype = intLOCAL;
				type = t_INT;
				break;
			case ID_LOCALSTR:
				vartype = strLOCAL;
				type = t_STRING;
				break;
			case ID_LOCALCB:
				vartype = cbLOCAL;
				type = t_CALLBACK;
				if(ext) *ext = id_ext;
				break;
			case ID_GLOBALCB:
				vartype = (id_array ? cbARRAY : cbGLOBAL);
				type = t_CALLBACK;
				if(ext) *ext = id_ext;
				break;
			case ID_HVAR:
			{
				int pt = libvars[id_index][0][0];
				if (pt == '0' + t_INT
					|| pt == '0' + t_BOOL)
				{
					vartype = (id_array ? intHVAR1 : intHVAR0);
					type = t_INT;
				}
				else
				{
					vartype = (id_array ? strHSTR1 : strHSTR0);
					type = t_STRING;
				}
				break;
			}
			case ID_STRUCT:
				vartype = ID_STRUCT;
				type = t_STRUCT;
				break;
			default:
				throw va("%s(%d) VCCompiler::HandleVariable() - could not resolve type of token '%s'", sourcefile, linenum, token);
				return -1;
		}
	}

	//log("Variable %s", token);

	int struct_index;
	switch (vartype)
	{
		case intPLUGINVAR:
		case strPLUGINVAR: {
				if( !(id_access & ID_ACCESS_WRITE) )
					throw va("%s(%d): plugin variable %s is not writeable!", sourcefile, linenum, token);
				output.EmitC(vartype);
				output.EmitD(id_index);
				int count = id_array;
				for(int i=0;i<count;i++) {
					Expect("[");
					CompileOperand();
					Expect("]");
				}
				break;
			}
		case intLOCAL:
		case intGLOBAL:
		case strLOCAL:
		case strGLOBAL:
		case cbLOCAL:
		case cbGLOBAL:
			output.EmitC(vartype);
			output.EmitD(id_index);
			break;
		case intARRAY:
		case strARRAY:
		case cbARRAY:
			output.EmitC(vartype);
			output.EmitD(id_index);
			for (int i=0; i<global_vars[id_index]->dim; i++)
			{
				Expect("[");
				int myid = id_index;
				CompileOperand();
				id_index = myid;
				Expect("]");
			}
			break;
		case intHVAR0:
		case strHSTR0:
			output.EmitC(vartype);
			output.EmitD(id_index);
			break;
		case intHVAR1:
		case strHSTR1:
		{
			output.EmitC(vartype);
			output.EmitD(id_index);
			Expect("[");
			int myid = id_index;
            CompileOperand();
			id_index = myid;
            Expect("]");
			break;
		}
		case ID_STRUCT:
		{
			// You might wanna grab a coke before you try to figure this out.
			// Anyways, fun stuff lies below.

			// This variable stores the spot of all of the structs's array dimensions.
			// Needed since things basically turn into multidim int or string arrays at runtime.
			std::vector<int> struct_dim_ptr;
			// The parent structure.
			int parent_struct;
			// Make this the current struct.
			struct_index = id_index;
			id_array = 0;
			// Overkill (2006-07-04): Multidim arrays of structs.
			for (int j = 0; j < struct_instances[struct_index]->dim; j++)
			{
				// Expect a [
				Expect("[");
				// Rememember where this spot is, because it'll
				// Give us a dimension index for our resolved int/string member.
				struct_dim_ptr.push_back(srcofs);
				// Keep the current spot in the compiled bytecode.
				int temp = output.curpos();
				// id_* will change after CompileOperand()
				int myarray = id_array;
				// Get our index.
				CompileOperand();
				// Restore the only id_* we care about.
				id_array = myarray;
				id_array++;
				// Backup before the index is given in the bytecode.
				output.setpos(temp);
				// Expect a closing ]
				Expect("]");
			}
			// Overkill (2006-05-06):
			// We're breaking because we must be trying to assign a struct to a struct.
			if (!NextIs("."))
			{
				break;
			}
			Expect(".");

			// Get a token after the dot.
			GetToken();

			struct_element *elem = 0;

			int i;
			// Find a member in the struct that has the same name as the token.
			for (i=0; i < struct_instances[struct_index]->is->elements.size(); i++)
			{
				if (streq(struct_instances[struct_index]->is->elements[i]->name, token))
				{
					elem = struct_instances[struct_index]->is->elements[i];
					break;
				}
			}
			// No match.
			if (!elem)
			{
				throw va("%s(%d): %s is not a member of %s", sourcefile, linenum, token, struct_instances[struct_index]->is->name);
			}

			// Overkill (2006-05-05):
			// Use this element's type to determine what operations can be done.
			type = elem->type;

			// Overkill: Nested structs.
			if (elem->type == t_STRUCT)
			{
				char itsname[80];
				// Keep looping until the member of a struct isn't a struct,
				// or until they don't have anymore dots.
				while(true)
				{
					int elem_is_array;
					sprintf(itsname, "%c%s_%s", 1, struct_instances[struct_index]->name, elem->name);

					// Get index of struct_instance of this element
					for (i=0; i<struct_instances.size(); i++)
					{
						if (streq(struct_instances[i]->name, itsname))
						{
							parent_struct = struct_index;
							struct_index = i;
							elem_is_array = (elem->dims[0] > 0) ? 1 : 0;
							break;
						}
					}

					// If the struct is an array,
					if (elem_is_array)
					{
						for (int j=struct_instances[parent_struct]->dim; j<struct_instances[struct_index]->dim; j++)
						{
							// Expect a [
							Expect("[");
							// Rememember where this spot is, because it'll
							// Give us a dimension index for our resolved int/string member.
							struct_dim_ptr.push_back(srcofs);
							// Keep the current spot in the compiled bytecode.
							int temp = output.curpos();
							// id_* will change after CompileOperand()
							int myarray = id_array;
							// Get our index.
							CompileOperand();
							// Restore the only id_* we care about.
							id_array = myarray;
							id_array++;
							// Backup before the index is given in the bytecode.
							output.setpos(temp);
							// Expect a closing ]
							Expect("]");
						}
					}
					
					// Remains a struct.
					if (!NextIs("."))
					{
						break;
					}
					Expect(".");

					elem = 0;

					// Get the token after the dot.
					GetToken();
					// Find a member in the struct that has the same name as the token.
					for (i = 0; i < struct_instances[struct_index]->is->elements.size(); i++)
					{
						if (streq(struct_instances[struct_index]->is->elements[i]->name, token))
						{
							elem = struct_instances[struct_index]->is->elements[i];
							break;
						}
					}
					// No match.
					if (!elem)
					{
						throw va("%s(%d): %s is not a member of %s", sourcefile, linenum, token, struct_instances[struct_index]->is->name);
					}

					// Overkill:
					// Finally, not a struct!
					if (elem->type != t_STRUCT)
					{
						break;
					}
				}
				// Overkill (2006-05-05):
				// Use this element's type to determine what operations can be done.
				type = elem->type;
			}
			switch (elem->type)
			{
				case t_STRUCT: // Still a struct. okay.
					break;
				case t_INT:
				case t_STRING:
				case t_CALLBACK:
					switch(elem->type)
					{
						case t_INT:
							vartype = (elem->len == 1 && !id_array) ? intGLOBAL : intARRAY;
							break;
						case t_STRING:
							vartype = (elem->len == 1 && !id_array) ? strGLOBAL : strARRAY;
							break;
						case t_CALLBACK:
							vartype = (elem->len == 1 && !id_array) ? cbGLOBAL : cbARRAY;
							if(ext) *ext = elem->ext;
							break;
					}

					char itsname[80];
					sprintf(itsname, "%c%s_%s", 1, struct_instances[struct_index]->name, elem->name);
					// get index for this element
					for (i=0; i<global_vars.size(); i++)
					{
						if (streq(global_vars[i]->name, itsname))
						{
							output.EmitC(vartype);
							output.EmitD(i);
							break;
						}
					}
					// output array indexes.
					if (id_array)
					{
						//log("Struct int arr: %d", struct_dim_ptr.size());
						for (std::vector<int>::iterator it = struct_dim_ptr.begin();
							it != struct_dim_ptr.end(); it++)
						{
							int thisofs = srcofs;
							srcofs = *it;
							int myarray = id_array;
							CompileOperand();
							id_array = myarray;
							srcofs = thisofs;
						}
						for (int j=id_array; j<global_vars[i]->dim; j++)
						{
							Expect("[");
							CompileOperand();
							Expect("]");
						}
					}
					break;
			}
			break;
		}
		default:
			throw va("%s(%d) Internal error in VCCompiler::HandleVariable()! Variable type id %d is unknown.", sourcefile, linenum, vartype);
			return -1;
	}

	return type;
}

void VCCompiler::HandleAssign()
{
	char varname[2048];
	int temp = output.curpos();
	int vartype = -1;
	int type = -1;

	if(id_type == ID_PLUGINVAR)
	{
		if(id_subtype == VCPlugins::RETURN_INT) vartype = intPLUGINVAR;
		if(id_subtype == VCPlugins::RETURN_STRING) vartype = strPLUGINVAR;
	}
	strcpy(varname, token);

	output.EmitC(opASSIGN);

	int varofs = srcofs;
	ext_definition ext;
	type = HandleVariable(&ext);

	GetToken();
	if (!TokenIs("=") && (vartype==intPLUGINVAR || vartype==strPLUGINVAR)){
		if( !(id_access & ID_ACCESS_READ) )
			throw va("%s(%d): plugin variable %s is not readable!", sourcefile, linenum, varname);
	}

	if (TokenIs("++") && type == t_INT) { output.EmitC(aINC); return; } else
	if (TokenIs("--") && type == t_INT) { output.EmitC(aDEC); return; } else
	if (TokenIs("+=") && type == t_INT) { output.EmitC(aINCSET); } else
	// Overkill (2006-06-29): += Concatination operator implemented at last!
	if (TokenIs("+=") && type == t_STRING)
    {
		output.EmitC(aSET);

		int thisofs = srcofs;
		srcofs = varofs;
		HandleVariable(NULL);
		srcofs = thisofs;
		output.EmitC(sADD);
	} else
	if (TokenIs("-=") && type == t_INT) { output.EmitC(aDECSET); } else
	// Overkill (2006-05-06): = assignment allowed if type is not a struct.
	if (TokenIs("=") && (type == t_INT || type == t_STRING || type == t_CALLBACK))  { output.EmitC(aSET); } else
	// Callbacks can be invoked.
	// Since they're variables, and any statement that leads with a variable is handled by this function,
	// a void callback invocation () is handled under this "assignment" section.
	if (TokenIs("(") && (type == t_CALLBACK))
	{
		HandleCallbackInvocation(ext.callback);
		return;
	}

	// Overkill (2006-05-05):
	// If the above is a bad assignment operator, error.
	// However, I have made these errors relevant to the type of the variable being set.
	else
	{
		if (type == t_INT)
		{
			throw va("%s(%d) Invalid integer assignment operator: \"%s\"", sourcefile, linenum, token);
		}
		else if (type == t_STRING)
		{
			throw va("%s(%d) Invalid string assignment operator: \"%s\"", sourcefile, linenum, token);
		}
		else if (type == t_CALLBACK)
		{
			throw va("%s(%d) Invalid callback assignment operator: \"%s\"", sourcefile, linenum, token);
		}
		else if (type == t_VARARG)
		{
			throw va("%s(%d) Variable argument lists cannot be assigned. The operator \"%s\" is invalid.", sourcefile, linenum, token);
		}
		// Overkill (2006-05-06):
		// Error on attempt to do struct assignments.
		else if (type == t_STRUCT)
		{
			// If the token is a [, we're probably adding in one
			// to many dimensions to our struct array, or putting one in a bad spot.
			if (TokenIs("["))
			{
				throw va("%s(%d) Unexpected [] operator.", sourcefile, linenum);
			}
			else
			{
				throw va("%s(%d) Invalid struct assignment operator: \"%s\". Instead, try setting individual members inside of the struct.", sourcefile, linenum, token);
			}
		}
		// What kind of variable is this? Oh well. Something went wrong anyway.
		else
		{
			throw va("%s(%d) Invalid variable assignment operator \"%s\"", sourcefile, linenum, token); 
		}
	}

	if (type == t_STRING)
		CompileString();
	else if (type == t_CALLBACK)
		CompileCallback(ext.callback);
	else
		CompileOperand();
}

void VCCompiler::HandleIfGroup(bool falsify)
{
	// Falsify, for unless/until.
	if (falsify)
	{
		// NOT !
		output.EmitC(ifZERO);
		// BEGIN (
		output.EmitC(intGROUP);
	}
	CompileOperand();
	if (falsify)
	{
		// END )
		output.EmitC(iopEND);
	}
}

void VCCompiler::HandleIf(bool falsify)
{
	int falseofs, elseofs, endofs;

	output.EmitC(opIF);
	Expect("(");
	HandleIfGroup(falsify);
	Expect(")");

	falseofs = output.curpos();
	output.EmitD(0);

	if (!NextIs("{"))
	{
		CompileStatement();
		if (NextIs("else"))
		{
			output.EmitC(opGOTO);
			elseofs = output.curpos();
			output.EmitD(0);
		}
	}
	else
	{
		Expect("{");
		while (!NextIs("}"))
			CompileStatement();

		Expect("}");
		if (NextIs("else"))
		{
			output.EmitC(opGOTO);
			elseofs = output.curpos();
			output.EmitD(0);
		}
	}

	endofs = output.curpos();
	output.setpos(falseofs);
	output.EmitD(endofs);
	output.setpos(endofs);

	if (NextIs("else"))
	{
		GetToken();
		if (!NextIs("{"))
			CompileStatement();
		else
		{
			Expect("{");
			while (!NextIs("}"))
				CompileStatement();
			Expect("}");
		}
		endofs = output.curpos();
		output.setpos(elseofs);
		output.EmitD(endofs);
		output.setpos(endofs);
	}
}

void VCCompiler::HandleWhile(bool falsify)
{
	int falseofs, top, end;

	top = output.curpos();
	output.EmitC(opIF);
	Expect("(");
	HandleIfGroup(falsify);
	Expect(")");

	falseofs = output.curpos();
	output.EmitD(0);

	if (!NextIs("{"))
	{
		CompileStatement();
		output.EmitC(opGOTO);
		output.EmitD(top);
	}
	else
	{
		Expect("{");
		while (!NextIs("}"))
			CompileStatement();
		
		Expect("}");
		output.EmitC(opGOTO);
		output.EmitD(top);
	}

	end = output.curpos();
	output.setpos(falseofs);
	output.EmitD(end);
	output.setpos(end);
}

/////////////////////////////////
//old one
/////////////////////////////////
//void VCCompiler::HandleFor()
//{
//	Expect("(");
//	while (!TokenIs(";"))			// initializer
//		CompileStatement();
//
//	int conditional = srcofs;		// conditional
//	while (!NextIs(";"))
//		GetToken();
//	GetToken();
//
//	int post = srcofs;				// poststep
//	while (!NextIs(")"))
//		GetToken();
//	GetToken();
//
//	int beginloop;
//	int afterloop;
//
//	beginloop = output.curpos();
//
//	int temp = srcofs;
//
//	// compile conditional
//	source.setpos(conditional);
//	srcofs = conditional;
//	output.EmitC(opIF);
//	HandleIfGroup();
//	afterloop = output.curpos();
//	output.EmitD(0); //placeholder for after-loop area
//	//output.EmitC(opGOTO);
//	//output.EmitD(output.curpos()+4); //placeholder for loop-b
//	source.setpos(temp);
//	srcofs = temp;
//
//	// compile loop body
//	int loopstart = output.curpos();
//	if (!NextIs("{"))
//		CompileStatement();
//	else
//	{
//		Expect("{");
//		while (!NextIs("}"))
//			CompileStatement();
//		Expect("}");
//	}
//
//	// compile post modifier (i++, etc)
//	temp = srcofs;
//	source.setpos(post);
//	srcofs = post;
//	while (!TokenIs(")"))
//		CompileStatement();
//
//	source.setpos(temp);
//	srcofs = temp;
//
//
//	output.EmitC(opGOTO);
//	output.EmitD(beginloop);
//
//	output.SetD(afterloop,output.curpos());
//
//}

/////////////////////////////////
//vecna's new one
/////////////////////////////////
//void VCCompiler::HandleFor()
//{
//	Expect("(");
//	while (!TokenIs(";"))			// initializer
//		CompileStatement();
//
//	int loopstart = output.curpos(); // Conditional
//	output.EmitC(opIF);
//	HandleIfGroup();
//	int falseofs = output.curpos();
//	output.EmitD(0);
//
//	int post = srcofs;				// poststep
//	while (!NextIs(")"))
//		GetToken();
//	GetToken();
//
//	// compile loop body
//
//	if (!NextIs("{"))
//	{
//if (NextIs("for"))			//fixme
//falseofs = falseofs;
//		CompileStatement();
//	}
//	else
//	{
//		Expect("{");
//		while (!NextIs("}"))
//			CompileStatement();
//		Expect("}");
//	}
//
//	// compile post modifier (i++, etc)
//	int temp = srcofs;
//	source.setpos(post);
//	srcofs = post;
//	while (!TokenIs(")"))
//		CompileStatement();
//
//	output.EmitC(opGOTO);
//	output.EmitD(loopstart);
//
//	// Output falseofs and clean up
//	int outpos = output.curpos();
//	output.setpos(falseofs);
//	output.EmitD(outpos);
//	output.setpos(outpos);
//	source.setpos(temp);
//	srcofs = temp;
//}

/////////////////////////////////
//zero's new one
/////////////////////////////////
void VCCompiler::HandleFor()
{
	Expect("(");
	while (!TokenIs(";"))			// initializer
		CompileStatement();

	int conditional = srcofs;		// conditional
	while (!NextIs(";"))
		GetToken();
	GetToken();

	int post = srcofs;				// poststep
	while (!NextIs(")"))
		GetToken();
	GetToken();

	int placeLoopEnd;
	int temp;
	int markerLoopStart;

	// compile conditional
	markerLoopStart = output.curpos();
	temp = srcofs;
	source.setpos(conditional);
	srcofs = conditional;
	output.EmitC(opIF);
	HandleIfGroup();
	placeLoopEnd = output.curpos();
	output.EmitD(-1); //fill in later with end of loop
	source.setpos(temp);
	srcofs = temp;


	// compile loop body
	if (!NextIs("{"))
		CompileStatement();
	else
	{
		Expect("{");
		while (!NextIs("}"))
			CompileStatement();
		Expect("}");
	}

	// compile post modifier (i++, etc)
	temp = srcofs;
	source.setpos(post);
	srcofs = post;
	if(!NextIs(")"))
	do
	{
		CompileStatement();
	} while (!TokenIs(")"));
	source.setpos(temp);
	srcofs = temp;

	output.EmitC(opGOTO);
	output.EmitD(markerLoopStart);

	output.SetD(placeLoopEnd,output.curpos());
}

void VCCompiler::HandleSwitch()
{
	int temp, returnofs;

	output.EmitC(opSWITCH);
	Expect("(");
	CompileOperand();
	Expect(")");
	Expect("{");

	// case .. option loop

	while (!NextIs("}"))
	{
		if (NextIs("default"))
		{
			Expect("default");
			output.EmitC(opDEFAULT);
		}
		else
		{
			Expect("case");
			output.EmitC(opCASE);
			CompileOperand();
		}
		Expect(":");
		returnofs = output.curpos();
		output.EmitD(0);
		while (!NextIs("case") && !NextIs("}") && !NextIs("default"))
			CompileStatement();
		output.EmitC(opRETURN);
		temp = output.curpos();
		output.setpos(returnofs);
		output.EmitD(temp);
		output.setpos(temp);
	}
	Expect("}");
	output.EmitC(opRETURN);
}

#endif

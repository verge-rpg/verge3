/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


int HandleForImage(image *img);
image *ImageForHandle(int handle);
void SetHandleImage(int handle, image *img);
void FreeImageHandle(int handle);

extern int cf_r1, cf_g1, cf_b1;
extern int cf_r2, cf_g2, cf_b2;
extern int cf_rr, cf_gr, cf_br;

struct imagehandle
{
	image *ref;
	bool active;
};

extern imagehandle imagebay[];

//QWIK-DOCS:
//'1' -> RETURN_INT
//'3' -> RETURN_STRING
//'5' -> RETURN_VOID
//CASE INSENSITIVE
#include <algorithm>
class VCPlugins {
public:

	// return,    identifier,    signature
	//{"5", "Exit", "3" },

	class Context {
	public:
		//set return values
		void setReturnInt(int value) { _intret(_tag,value); }
		void setReturnString(std::string value) { _strret(_tag,value); }

		//read operands
		std::string getOperandString() { return _str(_tag); }
		int getOperandInt() { return _int(_tag); }

		//operands
		std::vector<int> ints;
		std::vector<std::string> strings;

		//internal
		void *_tag;
		void (*_intret)(void *tag, int val);
		void (*_strret)(void *tag, std::string &val);
		int (*_int)(void *tag);
		std::string (*_str)(void *tag);
	};

	class VariableContext : public Context {
	public:
		//which indices of the array were accessed?
		std::vector<int> indices;
	};

	class FunctionContext : public Context {
	public:
	};

	struct Plugin {
	public:
		char *name;
		int index;
		int type;
	};

	struct Function : public Plugin {
	public:
		char *arguments;
		void(*funcptr)(FunctionContext &context);
	};

	struct Variable : public Plugin {
	public:
		int dimensions;
		void(*readfunc)(VariableContext &context);
		void(*writefunc)(VariableContext &context);
	};

	static std::map<std::string,Function*> functions;
	static std::map<std::string,Variable*> variables;
	static std::vector<Plugin*> plugins;

	static Function *functionByIndex(int index) { return (Function *)plugins[index]; }
	static Variable *variableByIndex(int index) { return (Variable *)plugins[index]; }

	struct IdentifierLookup {
	public:
		//types of identifiers we can find
		static const int NOTFOUND = 0;
		static const int FUNCTION = 1;
		static const int VARIABLE = 2;

		int kind;

		//the variable or function if the found identifier is either
		Function *function;
		Variable *variable;

		//if the identifier is found, this will be the function or variable casted up
		Plugin *plugin;
	};

	static void checkIdentifier(char *ident, IdentifierLookup &result) {
		if(functions.size() == 0 && variables.size() == 0) {
			result.kind = IdentifierLookup::NOTFOUND;
			return;
		};
		
		
		std::string strident = std::string(ident);
		std::transform(strident.begin(),strident.end(),strident.begin(),tolower);

		if(functions.size() != 0) {
			std::map<std::string,Function*>::iterator it = functions.find(strident);
			if(it != functions.end()) {
				result.kind = IdentifierLookup::FUNCTION;
				result.function = it->second;
				result.plugin = it->second;
				return;
			}
		}

		if(variables.size() != 0) {
			std::map<std::string,Variable*>::iterator it = variables.find(strident);
			if(it != variables.end()) {
				result.kind = IdentifierLookup::VARIABLE;
				result.variable = it->second;
				result.plugin = it->second;
				return;
			}
		}

		//not found
		result.kind = IdentifierLookup::NOTFOUND;
	}

	static void validateIdentifier(char *ident) {
		(void)ident;
		//TODO : make builtins always overriden by user functions and builtins
	}

	static const int RETURN_VOID = 0;
	static const int RETURN_INT = 1;
	static const int RETURN_STRING = 2;

	static const int VARIABLE_READ = 0;
	static const int VARIABLE_WRITE = 1;

	static void execFunction(int index, FunctionContext &context) {
		Function *func = functionByIndex(index);
		//prefetch all arguments
		char *cp = func->arguments;
		while(*cp) {
			switch(*cp) {
				case '1': context.ints.push_back(context.getOperandInt()); break;
				case '3': context.strings.push_back(context.getOperandString()); break;
			}
			cp++;
		}
		func->funcptr(context);
	}

	static void executeVariable(int index, VariableContext &context, int operation) {
		if(operation == VARIABLE_READ)
			variableByIndex(index)->readfunc(context);
		else if(operation == VARIABLE_WRITE)
			variableByIndex(index)->writefunc(context);
		else throw va("Unexpected operation %d in VCPlugins::readVariable",operation);
	}

	static int _currId;
	static void registerFunction(char *return_type, char *name, char *arguments, void (*funcptr)(FunctionContext &context)) {
		Function *f = new Function();
		if(return_type[0] == '1') f->type = RETURN_INT;
		else if(return_type[0] == '3') f->type = RETURN_STRING;
		else if(return_type[0] == '5') f->type = RETURN_VOID;
		else throw va("Unexpected return type `%s` in registerFunction()",return_type);

		f->index = _currId++;
		f->name = name;
		f->arguments = arguments;
		f->funcptr = funcptr;

		std::string strname = std::string(name);
		std::transform(strname.begin(),strname.end(),strname.begin(),tolower);
		functions[strname] = f;
		plugins.push_back(f);
	}

	static void registerVariable(char *return_type, char *name, int dimensions, void (*readfunc)(VariableContext &context), void (*writefunc)(VariableContext &context)) {
		Variable *v = new Variable();
		if(return_type[0] == '1') v->type = RETURN_INT;
		else if(return_type[0] == '3') v->type = RETURN_STRING;
		else throw va("Unexpected return type `%s` in registerFunction()",return_type);
		v->dimensions = dimensions;
		v->name = name;
		v->readfunc = readfunc;
		v->writefunc = writefunc;
		std::string strname = std::string(name);
		std::transform(strname.begin(),strname.end(),strname.begin(),tolower);
		variables[strname] = v;
		plugins.push_back(v);
	}

	static void init() {
	}

};

struct argument_t
{
	byte type_id;
	int int_value;
	StringRef string_value;
};

class VCCore : public ScriptEngine {
public:
	VCCore();
	~VCCore();

	void LoadCore(VFILE *f, int cimage, bool append = false, bool patch_others = false);
	void UnloadCore(int cimage);
	Chunk *GetCore(int cimage);

	void LoadMapScript(VFILE *f, CStringRef filename);
	void ExecAutoexec();
	void Decompile();
	bool ExecuteFunctionString(const StringRef& script);
	bool FunctionExists(const StringRef &func);
	int ResolveOperand();
	StringRef ResolveString();
	void DisplayError(const StringRef &msg);
	void vcerr(char *str, ...) {
	  	va_list argptr;
		char msg[256];

		va_start(argptr, str);
		vsprintf(msg, str, argptr);
		va_end(argptr);
		Error("%s",msg);
	}

	int vcreturn;
	StringRef vcretstr;

	void ListStructMembers(std::vector<StringRef> &result, const char *structname);

	bool CopyArray(const char *srcname, const char *destname);

	bool CheckForVarargs();
	void ReadVararg(std::vector<argument_t>& vararg);
	void IgnoreVararg();

	std::vector<argument_t> argument_pass_list;

	void ArgumentPassAddInt(int value);
	void ArgumentPassAddString(StringRef value);
	void ArgumentPassClear();

	void SetInt(const char *intname, int value);
	int  GetInt(const char *intname);
	void SetStr(CStringRef strname, CStringRef value);
	CStringRef GetStr(const char *strname);
	void SetIntArray(const char *intname, int index, int value);
	int  GetIntArray(const char *intname, int index);
	bool IntExists(const char *intname);
	bool StrExists(const char *strname);
	void SetStrArray(CStringRef strname, int index, CStringRef value);
	CStringRef GetStrArray(CStringRef strname, int index);

	std::vector<function_t*>	userfuncs[NUM_CIMAGES];
	
	struct TUserFuncMap {
		quad hash;
		int index;
		bool operator < (TUserFuncMap const & rhs) const { return hash < rhs.hash; }		
	};
	TUserFuncMap* userfuncMap[NUM_CIMAGES];

private:
	std::vector<int_t>					global_ints;
	std::vector<string_t>				global_strings;
	std::vector<struct_instance*>		struct_instances;

	// patches pairs of <cimage,findex>
	typedef std::pair<int,int> funcpatch_t;
	typedef std::map <funcpatch_t, funcpatch_t> patch_table_t;

	patch_table_t	patch_table;

	int int_stack[1024+20];
	int int_stack_base, int_stack_ptr;
	StringRef str_stack[1024+20];
	int str_stack_base, str_stack_ptr;
	int int_last_base, str_last_base;
	std::vector < std::vector<argument_t> > vararg_stack;
	function_t *in_func;

	int *vcint;
	StringRef *vcstring;
	int maxint, maxstr;

	Chunk* currentvc;
	int current_cimage;
	Chunk coreimages[NUM_CIMAGES];

	void LoadSystemXVC();
	void LookupOffset(int ofs, std::string &s);

	void PushInt(int n);
	int  PopInt();
	void PushString(CStringRef s);
	StringRef PopString();

	StringRef ProcessString();
	int  ProcessOperand();
	int  ReadInt(int c, int i, int ofs);
	void WriteInt(int c, int i, int ofs, int value);
	void HandleAssign();
	void HandleIf();
	bool ProcessIf();
	bool ProcessIfOperand();
	bool HandleSwitch(); // returns if it exits normally (else was a return)
	void HandleLibFunc();
	void HandlePluginFunc(int id);
	void HandlePluginVarRead(int id);
	void ExecuteBlock();
	void ExecuteUserFunc(int cimage, int i, bool argument_pass = false);

	int GetIntArgument(int index);
	StringRef GetStringArgument(int index);
	void SetIntArgument(int index, int value);
	void SetStringArgument(int index, CStringRef value);

	FILE *vcd;
	int dtablvl;

	void WriteGlobalVars();
	void DecompileFunctions();
	void DecompileFunction(int cimage, int i);
	void PrintTab();

	void DecompileString();
	void DecompileLibFunc();

	//plugin accessor callbacks
	static int _vcplugins_functioncontext_get_int(void *tag) { return ((VCCore *)tag)->ResolveOperand(); }
	static std::string _vcplugins_functioncontext_get_str(void *tag) { return std::string(((VCCore *)tag)->ResolveString().c_str()); }
	static void _vcplugins_functioncontext_get_intret(void *tag, int val) { ((VCCore *)tag)->vcreturn = val; }
	static void _vcplugins_functioncontext_get_strret(void *tag, std::string &val) { ((VCCore *)tag)->vcretstr = std::string(val.c_str()); }
	static int _vcplugins_functioncontext_pop_int(void *tag) { return ((VCCore *)tag)->PopInt(); }
	static std::string _vcplugins_functioncontext_pop_str(void *tag) { return std::string(((VCCore *)tag)->PopString().c_str()); }

};

extern bool die;
extern int vc_paranoid, vc_arraycheck;
extern StringRef renderfunc, timerfunc;

extern int event_tx;
extern int event_ty;
extern int event_zone;
extern int event_entity;
extern int event_param;
extern int event_sprite;
extern int event_entity_hit;

extern int __grue_actor_index;

extern int invc;

extern StringRef _trigger_onStep, _trigger_afterStep;
extern StringRef _trigger_beforeEntityScript, _trigger_afterEntityScript;
extern StringRef _trigger_onEntityCollide;
extern StringRef _trigger_afterPlayerMove;
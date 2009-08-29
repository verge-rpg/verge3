/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


#include <map>
#include <vector>
#include <string>
#ifndef MYTYPES
  typedef unsigned int   quad;
  typedef unsigned short word;
  typedef unsigned char  byte;
#endif

#include "opcodes.h"

#define PP_FILETAG			1
#define PP_LINETAG			2
#define IDENTIFIER_LEN		80
#define VCLOG				"vcc_verbose.txt"

#define CIMAGE_SYSTEM 0		//system vc
#define CIMAGE_MAP    1		//map vc
#define CIMAGE_TEMP   2		//vars internal to vc functions?
#define NUM_CIMAGES   3

extern int verbose;

struct escape_sequence
{
	char* sequence;
	int value;
	escape_sequence(char* sequence, int value)
	{
		this->sequence = sequence;
		this->value = value;
	}
};

extern escape_sequence escape_codes[10];



class HVar
{
public:
	char *name;
	int dim;
	HVar(char *n, char *d);
	~HVar();
};

struct callback_definition;
class struct_definition;
union ext_definition
{
    callback_definition* callback;
    struct_definition* structure;
};

struct callback_definition
{
    int signature;
    ext_definition sigext;
    char argtype[60];
    ext_definition argext[60];
    int numargs; 

	callback_definition();
	~callback_definition();
};

class global_var_t
{
public:
	char type;
	ext_definition ext;

	char name[IDENTIFIER_LEN];
	int ofs, len;
	int dim;
	std::vector<int> dims;
	StringRef initializer;
	global_var_t();
	global_var_t(FILE *f);
	~global_var_t();
	void write(FILE *f);
};

class struct_element
{
public:
	char type;
	ext_definition ext;

	char name[IDENTIFIER_LEN];

	// For nested structs.
	char type_name[IDENTIFIER_LEN];

	int len;
	int dim;
	std::vector<int> dims;
	struct_element();
	struct_element(FILE *f);
	~struct_element();

	bool equals(struct_element *rhs);
	void write(FILE *f);
};

class struct_definition
{
public:
	char name[IDENTIFIER_LEN];
	std::vector<struct_element*> elements;
	struct_definition();
	struct_definition(FILE *f);
	~struct_definition();


	bool equals(struct_definition *rhs);
	void write(FILE *f);
};

class struct_instance
{
public:
	char name[IDENTIFIER_LEN];
	struct_definition *is;
	int dim;
	std::vector<int> dims;
	struct_instance();
	struct_instance(FILE *f);
	void write(FILE *f);
};

class function_t
{
public:
	char name[IDENTIFIER_LEN];
	char argtype[60];
	ext_definition argext[60];
	char localnames[100][IDENTIFIER_LEN];
	int numargs, numlocals;
	int signature;
	ext_definition sigext;
	int codeofs;
	int codeend;
	char coreimage;
	function_t();
	function_t(FILE *f);
	~function_t();
	void write(FILE *f);
};

class Chunk
{
public:
	std::vector<char> chunk;
	int ptr;
	int size() { return chunk.size(); }

	//struct Smasher {
	//	size_t operator()(const char*& x) const { return (size_t)x; }

	//   bool operator()( const char* &that1, const char*& that2 ) const 
	//	{ 
	//	  return that1 == that2;
	//	} 

	//};


	typedef std::map<int,StringRef> TStringTable;
	TStringTable stringTable;

	Chunk();
	~Chunk();
	void LoadChunk(FILE *f);
	void SaveChunk(FILE *f);
	void Append(FILE *f, bool save_pos = false);
	int curpos();
	int setpos(int p);
	void EmitC(byte c);
	void EmitW(word w);
	void EmitD(quad d);
	void EmitString(char *s);
	void SetD(int ofs, quad d);
	byte GrabC();
	word GrabW();
	quad GrabD();
	CStringRef GrabString();
	char operator[](quad n) const;
	void Expand(int amount);
	void become(Chunk *c);
	void clear();
	char& refc() { return chunk[ptr++]; }
};

struct debuginfo
{
	char sourcefile[80];
	char funcname[80];
	int  linenum;
};

typedef enum  {
    SCAN_ALL,
    SCAN_IGNORE_NON_FUNC,
    SCAN_ERR_NON_FUNC
} scan_t;

struct CircularIncludeException {
public:
    std::string message;
    CircularIncludeException(std::string message): message(message) {}
};

struct LexerNotInitializedException {};

class FileServer {
public:
    virtual char* fetch(char* filename);
};

#ifdef ALLOW_SCRIPT_COMPILATION
class VCCompiler : public MapScriptCompiler
{
    FileServer* file_server;
    bool lexer_initialized;

public:
	char errmsg[2048];
	bool vcerror;
	// source and output images for
	// the current code image being compiled
	Chunk source, output;

	VCCompiler(FileServer* file_server = NULL);
	~VCCompiler();

	// call to compile system.vc - starts fresh. Only use
	// at startup. To recompile something at runtime to
	// the system core, use CompileOther. Returns false
	// on error.
	bool CompileAll();
	void ExportSystemXVC();

	// call to compile a single map.
	// Any map in core right now will be ignored
	// when compiling this one. Returns false on
	// failure. Can keep going if this happens, but
	// the (partially) compiled map shouldn't be loaded.
	bool CompileMap(const char *f);

	// call to compile some other code at runtime
	// Must be loaded into the VCCore before anything
	// else is compiled/loaded against its core image,
	// or not at all.
	// If append is true, it must be loaded into the core
	// with append = true.
	// permit_duplicates being true means that functions
	// may be compiled that have the same name as those
	// in the reference core images
	// ref_cimages is a list of images to compile against,
	// in addition to the current image. If nothing passed,
	// defaults to system image.
	// Returns false on failure. Can keep going if this happens,
	// but the (partially) compiled core shouldn't be loaded.
	bool CompileOther(char *f, int cimage, bool append = false, bool permit_duplicates = false, std::vector<int> ref_cimages = std::vector<int>(1, CIMAGE_SYSTEM));
	debuginfo GetDebugForOffset(int ofs);
	void vprint(char *s, ...);

	bool debugon;
	int debugofs;

	// for allowing temp functions to
	// access map-local and other temp functions
	void PushFunction(int cimage, function_t *f);
	void ClearFunctionList(int cimage);

    void ScanPass(scan_t type);
    std::vector<struct_definition*> struct_defs;
    std::vector<char*> pp_included_files;
    void check_for_circular_includes(char* filename);
    bool PreProcess(char *fn);
    bool Process(char *fn);
    void Init_pptbl();
    void collapse_whitespace(char*& code);

private:
	std::vector<Define*> defines;
	std::vector<HVar*> hvars;

	// preprocessor component
	char pptbl[256];
	int  pp_total_lines;
    bool add_source_files;

	void pp_filetag(char *fn);
	void pp_linetag(int line);
	void stripdefinevalue(char *v);

	// lexical processing component
	char chr_table[256];
	char sourcefile[256];
	char token[2048];
	int token_value;
	quad srcofs, linenum;

	void Init_Lexical();
	bool streq(const std::string & lhs, const std::string & rhs);
	bool streq(const char *a, const char *b);
	int  hextoi(char *h);
	void ParseWhitespace();
	void ParseIdentifier();
	void ParseNumber();
	void ParsePunc();
	void GetToken();
	void GetToken(int n);
	bool TokenIs(char *s);
	bool NextIs(char *s);
	bool NextIs(int n, char *s);
	void Expect(char *s);
	bool IsEscapeSequence(char* s);
	escape_sequence *GetEscapeSequence(char *s);
	bool IsKeyword(char *s);
	bool IsHexEscapeSequence(char* s);
	bool IsNumberChar(char ch);
	bool IsHexNumberChar(char ch);
	void GetIdentifierToken();

	// scanning pass component
	std::vector<global_var_t*>					global_vars;
	std::vector<struct_instance*>		struct_instances;

	int global_var_offset;

	// storage for functions
	// funcs stores all functions in each code image
	// these images are populated when code is being compiled
	// and by currently loaded code
	std::vector<function_t*>	funcs[NUM_CIMAGES];

	// the code images we should search for function calls
	// in the order they should be searched. This should
	// include target_cimage if we can call functions being
	// defined as we're compiling
	std::vector<int> reference_cimages;

	// the code image we are compiling into (new functions
	// should be part of this image)
	int target_cimage;

	// the number of functions in the target image
	// before compilation; allows writing out only
	// the new functions when we're compiling with
	// the intent to patch
	int precompile_numfuncs;

	// set to true to indicate that duplicate functions
	// from pervious compilations are OK. If it's false,
	// they are considered errors. Duplicate functions
	// in a single compilation pass are always considered
	// errors.
	bool permit_duplicates;

	void SkipBrackets();
	void SkipFunction();
	void SkipDeclare();
	void SkipArguments();
	void SkipCallbackDefinition();
	void CheckNameDup(char *s);

	// Overkill (2006-05-06): 
	// Elements inside structures have different naming rules.
	void CheckStructElementNameDup (char *s, struct_definition *def);

	void ParseGlobalDecl(scan_t type);
	void ParseCallbackDefinition(callback_definition** def);
	void ParseFuncDecl(scan_t type);
	void ParseStructDecl(scan_t type);
    void ParseStructDeclVar(struct_definition* mystruct, int variable_type);
	void ParseStructInstance(scan_t type);

	// code generation component
	int returntype;
	ext_definition returnext;

	function_t *in_func;
	int id_type, id_subtype, id_array, id_index, id_access;
	ext_definition id_ext;
	static const int ID_ACCESS_READ = 1; 
	static const int ID_ACCESS_WRITE = 2;
	char id_cimage;

	void CreateStructInstance(struct_instance *inst);
	void CompilePass();
	void CompileMapPass();
	void CompileOtherPass(bool append);
	void CompileFunction(bool returns_callback);
	void CompileGlobalInitializers();
	void SkipVariables();

	function_t* FetchFunction(char *s);
	void CheckIdentifier(char *s);
	int CheckExpressionType(); // Overkill: Checks what type the next token is.
	bool TokenIsStringExpression();
	bool TokenIsIntExpression();
	struct_element ResolveStruct();
	void EmitStringLiteral();
	void EmitSingleStringLiteral();
	void ProcessString();
	void HandleStructAssignment(int dest, int src, bool first);
	int ResolveStructAssignment(int compare_struct);

	void CompileOperand();
	void CompileTerm();
	void CompileSubTerm();
	void CompileAtom();

	bool VerifySignatureMatch(callback_definition* expected, function_t* value);
	bool VerifySignatureMatch(callback_definition* expected, callback_definition* value);
	void CompileCallback(callback_definition* def);

	void CompileString();
	void CompileStatement();

	void HandleReturn();
	void HandleLibraryFunc();
	void HandleUserFunc();
	void HandleCallbackInvocation(callback_definition* def);
	void HandlePluginFunc();
	int HandleVariable(ext_definition* ext);
	void HandleAssign();
	void HandleIf(bool falsify = false);
	void HandleIfGroup(bool falsify = false);
	void HandleWhile(bool falsify = false);
	void HandleFor();
	void HandleSwitch();
};
#endif

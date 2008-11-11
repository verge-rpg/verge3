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
 * verge3: vc_library.cpp                                         *
 * copyright (c) 2002 vecna                                       *
 ******************************************************************/

#include "xerxes.h"
#include "opcodes.h"
#include <sstream>

/****************************** data ******************************/

extern VCCore *vc;

int cf_r1, cf_g1, cf_b1;
int cf_r2, cf_g2, cf_b2;
int cf_rr, cf_gr, cf_br;

/***************************** devilry ****************************/

#define VC_LIBFUNC(name) \
	void name (); \
	VcFunctionBinding _bind_ ## name (#name, name); \
	void name

VcFunctionBindingDecl _bind_decl[1000];
int _bind_decl_ctr = 0;

void vc_initLibrary() {
	for(int i=0;i<_bind_decl_ctr;i++) {
		std::string name = _bind_decl[i].name;
		std::transform(name.begin(), name.end(), name.begin(), tolower);
		VcGetLibfuncBindings()[name] = _bind_decl[i].fn;
	}
}



VcFunctionImplTable & VcGetLibfuncBindings () {
	static VcFunctionImplTable table;
	return table;
}

VcFunctionDispatchTable & VcGetLibfuncDispatch () {
	static VcFunctionDispatchTable table;
	return table;
}

void VcBuildLibraryDispatchTable () {
	VcFunctionImplTable & bindings = VcGetLibfuncBindings();
	VcFunctionDispatchTable & dispatch = VcGetLibfuncDispatch();
	for (int i = 0; i < NUM_LIBFUNCS; i++) {
		VcFunction & theFunc = libfuncs[i];
		std::string key;
		{
			std::stringstream buf;
			buf << "vc_" << theFunc.name;
			key = buf.str();
		}
		std::transform(key.begin(), key.end(), key.begin(), tolower);
		VcFunctionImpl ptr = bindings[key];
		dispatch[i] = ptr;
	}
}

/****************************** code ******************************/

dict *DictForHandle(int handle)
{
	if (handle == 0)
		vc->vcerr("DictForHandle() - Null dict reference, probably an uninitialized dict handle");

	if (handle<0 || handle >= Handle::getHandleCount(HANDLE_TYPE_DICT) )
		vc->vcerr("DictForHandle() - Dict reference is bogus! (%d)", handle);

	dict* ptr = (dict*) Handle::getPointer(HANDLE_TYPE_DICT,handle);

	if (ptr == NULL)
		vc->vcerr("DictForHandle() - Dict reference is valid but no dict is allocated for this handle. You may have mistakenly freed it and continued to use it.");
	return ptr;
}

void FreeDictHandle(int handle)
{
	Handle::free(HANDLE_TYPE_DICT,handle);
}

void SetHandleDict(int handle, dict *d)
{
	Handle::setPointer(HANDLE_TYPE_DICT, handle, (void*)d);
}

int HandleForDict(dict *d)
{
	return Handle::alloc(HANDLE_TYPE_DICT, d);
}

VC_LIBFUNC(vc_Exit) () {
	StringRef message = vc->ResolveString();
	err("%s", message.c_str());
}

VC_LIBFUNC(vc_Log) () { 
	vc->Log(vc->ResolveString()); 
}

VC_LIBFUNC(vc_NewImage) () {
	int xsize = vc->ResolveOperand();
	int ysize = vc->ResolveOperand();
	vc->vcreturn = vc->NewImage(xsize,ysize);
}

VC_LIBFUNC(vc_MakeColor) ()
{
	int r = vc->ResolveOperand();
	int g = vc->ResolveOperand();
	int b = vc->ResolveOperand();
	vc->vcreturn = vc->MakeColor(r,g,b);
}

VC_LIBFUNC(vc_SetLucent) () { vc->SetLucent(vc->ResolveOperand()); }

VC_LIBFUNC(vc_SetClip) ()
{
	int x1 = vc->ResolveOperand();
	int y1 = vc->ResolveOperand();
	int x2 = vc->ResolveOperand();
	int y2 = vc->ResolveOperand();
	int i = vc->ResolveOperand();
	vc->SetClip(x1, y1, x2, y2, i);
}

VC_LIBFUNC(vc_LoadImage) () { 
	vc->vcreturn = vc->LoadImage(vc->ResolveString()); 
}

VC_LIBFUNC(vc_LoadImage0) ()  { 
	vc->vcreturn = vc->LoadImage0(vc->ResolveString()); 
}

VC_LIBFUNC(vc_LoadImage8) ()  { vc->vcreturn = vc->LoadImage8(vc->ResolveString()); }

VC_LIBFUNC(vc_ShowPage) () { vc->ShowPage(); }
VC_LIBFUNC(vc_UpdateControls) () { vc->UpdateControls(); }

VC_LIBFUNC(vc_Blit) () {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int xxx = vc->ResolveOperand();
	int yyy = vc->ResolveOperand();
	vc->Blit(x, y, xxx, yyy);
}

VC_LIBFUNC(vc_TBlit) ()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	image *s = ImageForHandle(vc->ResolveOperand());
	image *d = ImageForHandle(vc->ResolveOperand());
	TBlit(x, y, s, d);
}

VC_LIBFUNC(vc_AdditiveBlit) () {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->AdditiveBlit(x, y, s, d);
}

VC_LIBFUNC(vc_TAdditiveBlit) ()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->TAdditiveBlit(x, y, s, d);
}

VC_LIBFUNC(vc_SubtractiveBlit) () {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->SubtractiveBlit(x, y, s, d);
}

VC_LIBFUNC(vc_TSubtractiveBlit) ()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->TSubtractiveBlit(x, y, s, d);
}

VC_LIBFUNC(vc_WrapBlit) () {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->WrapBlit(x, y, s, d);
}

VC_LIBFUNC(vc_TWrapBlit) () {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->TWrapBlit(x, y, s, d);
}

VC_LIBFUNC(vc_ScaleBlit) () {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int dw = vc->ResolveOperand();
	int dh = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->ScaleBlit(x, y, dw, dh, s, d);
}

VC_LIBFUNC(vc_TScaleBlit) ()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int dw = vc->ResolveOperand();
	int dh = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->TScaleBlit(x, y, dw, dh, s, d);
}

VC_LIBFUNC(vc_RGB) () {
	int r = vc->ResolveOperand();
	int g = vc->ResolveOperand();
	int b = vc->ResolveOperand();
	vc->vcreturn = vc->rgb(r,g,b);
}

VC_LIBFUNC(vc_SetPixel) () {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int c = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->SetPixel(x, y, c, d);
}

VC_LIBFUNC(vc_GetPixel) () {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	vc->vcreturn = vc->GetPixel(x,y,s);
}

VC_LIBFUNC(vc_Line) () {
	int x1 = vc->ResolveOperand();
	int y1 = vc->ResolveOperand();
	int x2 = vc->ResolveOperand();
	int y2 = vc->ResolveOperand();
	int c = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->Line(x1, y1, x2, y2, c, d);
}

VC_LIBFUNC(vc_Rect) ()
{
	int x1 = vc->ResolveOperand();
	int y1 = vc->ResolveOperand();
	int x2 = vc->ResolveOperand();
	int y2 = vc->ResolveOperand();
	int c = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->Rect(x1, y1, x2, y2, c, d);
}

VC_LIBFUNC(vc_RectFill) ()
{
	int x1 = vc->ResolveOperand();
	int y1 = vc->ResolveOperand();
	int x2 = vc->ResolveOperand();
	int y2 = vc->ResolveOperand();
	int c = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->RectFill(x1, y1, x2, y2, c, d);
}

VC_LIBFUNC(vc_Circle) () {
	int x1 = vc->ResolveOperand();
	int y1 = vc->ResolveOperand();
	int xr = vc->ResolveOperand();
	int yr = vc->ResolveOperand();
	int c = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->Circle(x1, y1, xr, yr, c, d);
}

VC_LIBFUNC(vc_CircleFill) () {
	int x1 = vc->ResolveOperand();
	int y1 = vc->ResolveOperand();
	int xr = vc->ResolveOperand();
	int yr = vc->ResolveOperand();
	int c = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->CircleFill(x1, y1, xr, yr, c, d);
}

VC_LIBFUNC(vc_GetR) () { vc->vcreturn = vc->GetR(vc->ResolveOperand()); }
VC_LIBFUNC(vc_GetG) () { vc->vcreturn = vc->GetG(vc->ResolveOperand()); }
VC_LIBFUNC(vc_GetB) () { vc->vcreturn = vc->GetB(vc->ResolveOperand()); }

VC_LIBFUNC(vc_RotScale) () {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int angle = vc->ResolveOperand();
	int scale = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->RotScale(x,y,angle,scale,s,d);
}

VC_LIBFUNC(vc_FreeImage) () { vc->FreeImage(vc->ResolveOperand()); }

VC_LIBFUNC(vc_Silhouette) () {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int c = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->Silhouette(x, y, c, s, d);
}

VC_LIBFUNC(vc_GrabRegion) () {
	int sx1 = vc->ResolveOperand();
	int sy1 = vc->ResolveOperand();
	int sx2 = vc->ResolveOperand();
	int sy2 = vc->ResolveOperand();
	int dx = vc->ResolveOperand();
	int dy = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->GrabRegion(sx1,sy1,sx2,sy2,dx,dy,s,d);
}

VC_LIBFUNC(vc_TGrabRegion) ()
{
	int sx1 = vc->ResolveOperand();
	int sy1 = vc->ResolveOperand();
	int sx2 = vc->ResolveOperand();
	int sy2 = vc->ResolveOperand();
	int dx = vc->ResolveOperand();
	int dy = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->TGrabRegion(sx1,sy1,sx2,sy2,dx,dy,s,d);
}

VC_LIBFUNC(vc_Mosaic) () {
	int xgran = vc->ResolveOperand();
	int ygran = vc->ResolveOperand();
	int dest = vc->ResolveOperand();
	vc->Mosaic(xgran, ygran, dest);
}

VC_LIBFUNC(vc_DuplicateImage) () { vc->vcreturn = vc->DuplicateImage(vc->ResolveOperand()); }

VC_LIBFUNC(vc_Triangle) ()
{
	int x1 = vc->ResolveOperand();
	int y1 = vc->ResolveOperand();
	int x2 = vc->ResolveOperand();
	int y2 = vc->ResolveOperand();
	int x3 = vc->ResolveOperand();
	int y3 = vc->ResolveOperand();
	int c = vc->ResolveOperand();
	int dest = vc->ResolveOperand();
	vc->Triangle(x1, y1, x2, y2, x3, y3, c, dest);
}

VC_LIBFUNC(vc_ImageWidth) () { vc->vcreturn = vc->ImageWidth(vc->ResolveOperand()); }
VC_LIBFUNC(vc_ImageHeight) () { vc->vcreturn = vc->ImageHeight(vc->ResolveOperand()); }

VC_LIBFUNC(vc_LoadFontEx) () {
	StringRef filename = vc->ResolveString();
	int width = vc->ResolveOperand();
	int height = vc->ResolveOperand();
	vc->vcreturn = vc->LoadFont(filename,width,height);
}

VC_LIBFUNC(vc_SetCharacterWidth) ()
{
	Font *font = (Font*)vc->ResolveOperand();
	int character = vc->ResolveOperand();
	int width = vc->ResolveOperand();
	font->SetCharacterWidth(character,width);
}

VC_LIBFUNC(vc_LoadFont) () {
	StringRef filename = vc->ResolveString();
	vc->vcreturn = vc->LoadFontEx(filename);
}

VC_LIBFUNC(vc_EnableVariableWidth) () { vc->EnableVariableWidth(vc->ResolveOperand()); }

VC_LIBFUNC(vc_PrintString) () {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int dest = vc->ResolveOperand();
	int fh =vc->ResolveOperand();
	StringRef text = vc->ResolveString();
	vc->PrintString(x,y,dest,fh,text);
}

VC_LIBFUNC(vc_PrintRight) () {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int dest = vc->ResolveOperand();
	int fh =vc->ResolveOperand();
	StringRef text = vc->ResolveString();
	vc->PrintRight(x,y,dest,fh,text);
}

VC_LIBFUNC(vc_PrintCenter) () {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int dest = vc->ResolveOperand();
	int fh =vc->ResolveOperand();
	StringRef text = vc->ResolveString();
	vc->PrintCenter(x,y,dest,fh,text);
}


VC_LIBFUNC(vc_TextWidth) () {
	int fh = vc->ResolveOperand();
	StringRef text = vc->ResolveString();
	vc->vcreturn = vc->TextWidth(fh,text);
}

VC_LIBFUNC(vc_FreeFont) () { vc->FreeFont(vc->ResolveOperand()); }

VC_LIBFUNC(vc_Random) () { 
	int min = vc->ResolveOperand();
	int max = vc->ResolveOperand();
	vc->vcreturn = vc->Random(min, max);
}

VC_LIBFUNC(vc_len) () { vc->vcreturn = vc->Len(vc->ResolveString()); }
VC_LIBFUNC(vc_val) () { vc->vcreturn = vc->Val(vc->ResolveString()); }

VC_LIBFUNC(vc_Unpress) () { vc->Unpress(vc->ResolveOperand()); }

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

VC_LIBFUNC(vc_FileOpen) () {
	StringRef fname = vc->ResolveString();
	int mode = vc->ResolveOperand();
	vc->vcreturn = vc->FileOpen(fname,mode);
}

VC_LIBFUNC(vc_FileClose) () { vc->FileClose(vc->ResolveOperand()); }

VC_LIBFUNC(vc_FileWrite) () {
	int handle = vc->ResolveOperand();
	StringRef s = vc->ResolveString();
	vc->FileWrite(handle,s);
}

VC_LIBFUNC(vc_FileWriteln) () {
	int handle = vc->ResolveOperand();
	StringRef s = vc->ResolveString();
	vc->FileWriteln(handle,s);
}

VC_LIBFUNC(vc_FileReadln) () { vc->vcretstr = vc->FileReadln(vc->ResolveOperand()); }
VC_LIBFUNC(vc_FileReadToken) () { vc->vcretstr = vc->FileReadToken(vc->ResolveOperand()); }

VC_LIBFUNC(vc_FileSeekLine) () {
	int handle = vc->ResolveOperand();
	int line = vc->ResolveOperand();
	vc->FileSeekLine(handle,line);
}


VC_LIBFUNC(vc_FileEOF) () { vc->vcreturn = vc->FileEOF(vc->ResolveOperand())?1:0; }

VC_LIBFUNC(vc_LoadSound) () { vc->vcreturn = vc->LoadSound(vc->ResolveString()); }
VC_LIBFUNC(vc_FreeSound) () { vc->FreeSound(vc->ResolveOperand()); }

VC_LIBFUNC(vc_PlaySound) () {
	int slot = vc->ResolveOperand();
	int volume = vc->ResolveOperand();
	vc->vcreturn = vc->PlaySound(slot,volume);
}

VC_LIBFUNC(vc_CallFunction) ()
{
	StringRef func = vc->ResolveString();
	std::vector<argument_t> arguments;
	if (vc->CheckForVarargs())
	{
		int i;
		vc->ReadVararg(arguments);
		for (i = 0; i < arguments.size(); i++)
		{
			switch (arguments[i].type_id)
			{
				case t_INT:
					vc->ArgumentPassAddInt(arguments[i].int_value);
					break;
				case t_STRING:
					vc->ArgumentPassAddString(arguments[i].string_value);
					break;
			}
		}
	}
	vc->ExecuteFunctionString(func.c_str());
}

VC_LIBFUNC(vc_AssignArray) ()
{
	// FIXME: restore this. (cannot read globalint from here)
	/*int n = vc->ResolveOperand();
	string s = vc->ResolveString();

	if (s[0] != '{')
		vc->vcerr("vc_AssignArray() - string not properly formatted!");

	for (int ofs=1;;ofs++)
	{
		string numtoken;
		while (s[ofs] != ',' && s[ofs] != '}' )
			numtoken += s[ofs++];
		globalint[n++] = atoi(numtoken.c_str());
		if (s[ofs] == '}') break;
	}*/
}

VC_LIBFUNC(vc_FileSeekPos) () {
	int handle = vc->ResolveOperand();
	int offset = vc->ResolveOperand();
	int mode = vc->ResolveOperand();
	vc->FileSeekPos(handle,offset,mode);
}

VC_LIBFUNC(vc_FileCurrentPos) () { vc->vcreturn = vc->FileCurrentPos(vc->ResolveOperand()); }
VC_LIBFUNC(vc_FileWriteByte) () {
	int handle = vc->ResolveOperand();
	int var = vc->ResolveOperand();
	vc->FileWriteByte(handle,var);
}

VC_LIBFUNC(vc_FileWriteWord) () {
	int handle = vc->ResolveOperand();
	int var = vc->ResolveOperand();
	vc->FileWriteWord(handle,var);
}

VC_LIBFUNC(vc_FileWriteQuad) () {
	int handle = vc->ResolveOperand();
	int var = vc->ResolveOperand();
	vc->FileWriteQuad(handle,var);
}

VC_LIBFUNC(vc_FileWriteString) () {
	int handle = vc->ResolveOperand();
	StringRef s = vc->ResolveString();
	vc->FileWriteString(handle,s);
}

VC_LIBFUNC(vc_FileReadByte) () { vc->vcreturn = vc->FileReadByte(vc->ResolveOperand()); }
VC_LIBFUNC(vc_FileReadWord) () { vc->vcreturn = vc->FileReadWord(vc->ResolveOperand()); }
VC_LIBFUNC(vc_FileReadQuad) () { vc->vcreturn = vc->FileReadQuad(vc->ResolveOperand()); }
VC_LIBFUNC(vc_FileReadString) () { vc->vcretstr = vc->FileReadString(vc->ResolveOperand()); }

VC_LIBFUNC(vc_sqrt) () { vc->vcreturn = vc->sqrt(vc->ResolveOperand()); }
VC_LIBFUNC(vc_pow) () {
	int a = vc->ResolveOperand();
	int b = vc->ResolveOperand();
	vc->vcreturn = vc->pow(a,b);
}

VC_LIBFUNC(vc_SetAppName) () { vc->SetAppName(vc->ResolveString()); }
VC_LIBFUNC(vc_SetResolution) () { 
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	vc->SetResolution(x,y);
}

VC_LIBFUNC(vc_BlitLucent) ()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int lucent = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->BlitLucent(x,y,lucent,s,d);
}

VC_LIBFUNC(vc_TBlitLucent) ()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int lucent = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->TBlitLucent(x,y,lucent,s,d);
}

VC_LIBFUNC(vc_Map) ()
{
	StringRef map = vc->ResolveString();
	vc->Map(map);
}

VC_LIBFUNC(vc_strcmp) ()
{
	StringRef s1 = vc->ResolveString();
	StringRef s2 = vc->ResolveString();
	vc->vcreturn = vc->Strcmp(s1,s2);
}

VC_LIBFUNC(vc_strdup) ()
{
	StringRef s = vc->ResolveString();
	int times = vc->ResolveOperand();
	vc->vcretstr = vc->Strdup(s,times);
}

VC_LIBFUNC(vc_HookTimer) () { vc->HookTimer(vc->ResolveString()); }
VC_LIBFUNC(vc_HookRetrace) () { vc->HookRetrace(vc->ResolveString()); }
VC_LIBFUNC(vc_HookKey) () { 
	int k = vc->ResolveOperand();
	StringRef s = vc->ResolveString();
	vc->HookKey(k,s);
}
VC_LIBFUNC(vc_HookButton) () { 
	int b = vc->ResolveOperand();
	StringRef s = vc->ResolveString();
	vc->HookButton(b,s);
}

VC_LIBFUNC(vc_HookEntityRender) ()
{
	int i = vc->ResolveOperand();
	StringRef s = vc->ResolveString();
	vc->HookEntityRender(i,s);
}

VC_LIBFUNC(vc_BlitTile) ()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int t = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->BlitTile(x,y,t,d);
}

VC_LIBFUNC(vc_TBlitTile) ()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int t = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->TBlitTile(x,y,t,d);
}

VC_LIBFUNC(vc_BlitEntityFrame) ()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int e = vc->ResolveOperand();
	int f = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->BlitEntityFrame(x,y,e,f,d);
}

VC_LIBFUNC(vc_GetTile) () {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int i = vc->ResolveOperand();
	vc->vcreturn = vc->GetTile(x,y,i);
}

VC_LIBFUNC(vc_SetTile) ()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int i = vc->ResolveOperand();
	int z = vc->ResolveOperand();
	vc->SetTile(x,y,i,z);
}

VC_LIBFUNC(vc_GetZone) ()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	vc->vcreturn = vc->GetZone(x,y);
}

VC_LIBFUNC(vc_SetZone) ()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int z = vc->ResolveOperand();
	vc->SetZone(x,y,z);
}

VC_LIBFUNC(vc_SuperSecretThingy) () {
	int xskew = vc->ResolveOperand();
	int yofs = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->SuperSecretThingy(xskew, yofs, y, s, d);
}

VC_LIBFUNC(vc_BlitWrap) () {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->BlitWrap(x, y, s, d);
}

VC_LIBFUNC(vc_ColorFilter) () {
	int filter = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->ColorFilter(filter, d);
}

VC_LIBFUNC(vc_ImageShell) () {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int w = vc->ResolveOperand();
	int h = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	vc->vcreturn = vc->ImageShell(x,y,w,h,s);
}

VC_LIBFUNC(vc_Malloc) ()
{
	int s = vc->ResolveOperand();
	vc->vcreturn = (int) malloc(s);
}

VC_LIBFUNC(vc_MemFree) ()
{
	free((void *) vc->ResolveOperand());
}

VC_LIBFUNC(vc_MemCopy) ()
{
	int src = vc->ResolveOperand();
	int dst = vc->ResolveOperand();
	int len = vc->ResolveOperand();

	memcpy((void*) dst, (void *) src, len);
}


VC_LIBFUNC(vc_sin) () { vc->vcreturn = vc->sin(vc->ResolveOperand()); }
VC_LIBFUNC(vc_cos) () { vc->vcreturn = vc->cos(vc->ResolveOperand()); }
VC_LIBFUNC(vc_tan) () { vc->vcreturn = vc->tan(vc->ResolveOperand()); }
VC_LIBFUNC(vc_fsin) () { vc->vcreturn = vc->fsin(vc->ResolveOperand()); }
VC_LIBFUNC(vc_fcos) () { vc->vcreturn = vc->fcos(vc->ResolveOperand()); }
VC_LIBFUNC(vc_ftan) () { vc->vcreturn = vc->ftan(vc->ResolveOperand()); }

VC_LIBFUNC(vc_asin) () { vc->vcreturn = vc->asin(vc->ResolveOperand()); }
VC_LIBFUNC(vc_fasin) () { vc->vcreturn = vc->fasin(vc->ResolveOperand()); }
VC_LIBFUNC(vc_acos) () { vc->vcreturn = vc->acos(vc->ResolveOperand()); }
VC_LIBFUNC(vc_facos) () { vc->vcreturn = vc->facos(vc->ResolveOperand()); }
VC_LIBFUNC(vc_atan) () { vc->vcreturn = vc->atan(vc->ResolveOperand()); }
VC_LIBFUNC(vc_fatan) () { vc->vcreturn = vc->fatan(vc->ResolveOperand()); }

VC_LIBFUNC(vc_AlphaBlit) () {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int a = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->AlphaBlit(x, y, s, a, d);
}


VC_LIBFUNC(vc_WindowCreate) () {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int w = vc->ResolveOperand();
	int h = vc->ResolveOperand();
	StringRef s = vc->ResolveString();
	vc->vcreturn = vc->WindowCreate(x,y,w,h,s);
}

VC_LIBFUNC(vc_WindowGetImage) () { vc->vcreturn = vc->WindowGetImage(vc->ResolveOperand()); }
VC_LIBFUNC(vc_WindowClose) () { vc->WindowClose(vc->ResolveOperand()); }

VC_LIBFUNC(vc_WindowSetSize) () {
	int win = vc->ResolveOperand();
	int w = vc->ResolveOperand();
	int h = vc->ResolveOperand();
	vc->WindowSetSize(win,w,h);
}

VC_LIBFUNC(vc_WindowSetResolution) () {
	int win = vc->ResolveOperand();
	int w = vc->ResolveOperand();
	int h = vc->ResolveOperand();
	vc->WindowSetResolution(win,w,h);
}

VC_LIBFUNC(vc_WindowSetPosition) () {
	int win = vc->ResolveOperand();
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	vc->WindowSetPosition(win,x,y);
}

VC_LIBFUNC(vc_WindowSetTitle) () {
	int win = vc->ResolveOperand();
	StringRef s = vc->ResolveString();
	vc->WindowSetTitle(win,s);
}

VC_LIBFUNC(vc_WindowHide) () { vc->WindowHide(vc->ResolveOperand()); }
VC_LIBFUNC(vc_WindowShow) () { vc->WindowHide(vc->ResolveOperand()); }

VC_LIBFUNC(vc_WindowGetXRes) () { vc->vcreturn = vc->WindowGetXRes(vc->ResolveOperand()); }
VC_LIBFUNC(vc_WindowGetYRes) () { vc->vcreturn = vc->WindowGetYRes(vc->ResolveOperand()); }
VC_LIBFUNC(vc_WindowGetWidth) () { vc->vcreturn = vc->WindowGetWidth(vc->ResolveOperand()); }
VC_LIBFUNC(vc_WindowGetHeight) () { vc->vcreturn = vc->WindowGetHeight(vc->ResolveOperand()); }

VC_LIBFUNC(vc_WindowPositionCommand) () {
	int win = vc->ResolveOperand();
	int command = vc->ResolveOperand();
	int arg1 = vc->ResolveOperand();
	int arg2 = vc->ResolveOperand();
	vc->WindowPositionCommand(win,command,arg1,arg2);
}

VC_LIBFUNC(vc_SetSongPaused) () {
	int h = vc->ResolveOperand();
	int p = vc->ResolveOperand();
	vc->SetSongPaused(h, p);
}

VC_LIBFUNC(vc_SetSongVolume) () {
	int h = vc->ResolveOperand();
	int v = vc->ResolveOperand();
	vc->SetSongVolume(h, v);
}

VC_LIBFUNC(vc_SetSongPos) () {
	int h = vc->ResolveOperand();
	int v = vc->ResolveOperand();
	vc->SetSongPos(h, v);
}

VC_LIBFUNC(vc_SetMusicVolume) () { vc->SetMusicVolume(vc->ResolveOperand()); }


VC_LIBFUNC(vc_TokenCount) () {
	StringRef s = vc->ResolveString();
	StringRef d = vc->ResolveString();
	vc->vcreturn = vc->TokenCount(s,d);
}

VC_LIBFUNC(vc_GetToken) ()
{
	StringRef s = vc->ResolveString();
	StringRef d = vc->ResolveString();
	int i = vc->ResolveOperand();
	vc->vcretstr = vc->GetToken(s,d,i);
}

VC_LIBFUNC(vc_ToLower) () { vc->vcretstr = vc->ToLower(vc->ResolveString()); }
VC_LIBFUNC(vc_ToUpper) () { vc->vcretstr = vc->ToUpper(vc->ResolveString()); }

// Overkill: 2005-12-28
// Thank you, Zip.
VC_LIBFUNC(vc_strpos) ()
{
	StringRef sub = vc->ResolveString();
	StringRef source = vc->ResolveString();
	int start = vc->ResolveOperand();
	vc->vcreturn = source.str().find(sub, start);
}

// Overkill: 2005-12-28
// Helper function.
int GetTokenPos(StringRef teststr, StringRef tokens, int pos, int tok)
{
	int i = 0;
	bool last = false;
	int count = 0;
	int length = teststr.length();
	if (pos == 0)
	{
		return 0;
	}
	for (i = 0; i < teststr.length(); i++)
	{
		if (isdelim(teststr[i], tokens))
		{
			if (!last)
			{
				count++;
				if (count == pos && tok == 0)
				{
					return i;
				}
				last = true;
			}
		}
		else if (last)
		{
			if (count == pos)
			{
				return i;
			}
			last = false;
		}
	}
	return 0;
}

// Overkill: 2005-12-28
// Thank you, Zip.
VC_LIBFUNC(vc_GetTokenPos) ()
{

	StringRef teststr = vc->ResolveString();
	StringRef tokens = vc->ResolveString();
	int pos = vc->ResolveOperand();
	int tok = vc->ResolveOperand();
	vc->vcreturn = GetTokenPos(teststr, tokens, pos, tok);
}


// Overkill: 2005-12-28
// Thank you, Zip.
VC_LIBFUNC(vc_TokenLeft) ()	// Excludes token.
{
	StringRef full = vc->ResolveString();
	StringRef tokens = vc->ResolveString();
	int pos = vc->ResolveOperand();
	if (pos < 1)
	{
		vc->vcretstr = "";
		return;
	}
	pos = GetTokenPos(full, tokens, pos, 0);
	if (pos == full.length())
	{
		vc->vcretstr = full;
		return;
	}
	else
	{
		vc->vcretstr = vc_strleft(full,pos);
	}
}

// Overkill: 2005-12-28
// Thank you, Zip.
// Overkill (2006-07-28):
//	Fixed a bug where it included the delimiter character in TokenRight()
VC_LIBFUNC(vc_TokenRight) ()
{
	StringRef full = vc->ResolveString();
	StringRef tokens = vc->ResolveString();
	int pos = vc->ResolveOperand();
	if (pos < 1)
	{
		vc->vcretstr = full;
		return;
	}
	pos = GetTokenPos(full, tokens, pos, 1);
	if (pos == full.length())
	{
		vc->vcretstr = "";
		return;
	}
	else
	{
		vc->vcretstr = vc_strright(full, full.length() - pos);
	}
}


// Overkill: 2005-12-28
// Thank you, Zip.
VC_LIBFUNC(vc_strovr) ()
{
	StringRef rep = vc->ResolveString();
	StringRef source = vc->ResolveString();
	int offset = vc->ResolveOperand();
	vc->vcretstr = strovr(source, rep, offset);
}

// Overkill: 2005-12-19
// Thank you, Zip.
VC_LIBFUNC(vc_WrapText) ()
// Pass: The font to use, the string to wrap, the length in pixels to fit into
// Return: The passed string with \n characters inserted as breaks
// Assmes: The font is valid, and will overrun if a word is longer than linelen
// Note: Existing breaks will be respected, but adjacent \n characters will be
//     replaced with a single \n so add a space for multiple line breaks
{

	int wt_font = vc->ResolveOperand();
	StringRef wt_s = vc->ResolveString();
	int wt_linelen = vc->ResolveOperand();
	vc->vcretstr = ScriptEngine::WrapText(wt_font,wt_s,wt_linelen);
}


VC_LIBFUNC(vc_FontHeight) () { vc->vcreturn = vc->FontHeight(vc->ResolveOperand()); }

VC_LIBFUNC(vc_MixColor) () {
	int c1 = vc->ResolveOperand();
	int c2 = vc->ResolveOperand();
	int p = vc->ResolveOperand();
	vc->vcreturn = vc->MixColor(c1,c2,p);
}

VC_LIBFUNC(vc_CHR) () { vc->vcretstr = vc->Chr(vc->ResolveOperand()); }

VC_LIBFUNC(vc_PlayMovie) () { vc->vcreturn = vc->PlayMovie(vc->ResolveString()); }
VC_LIBFUNC(vc_AbortMovie) () { vc->AbortMovie(); }

VC_LIBFUNC(vc_MovieLoad) () {
	StringRef s = vc->ResolveString();
	int mute = vc->ResolveOperand();
	vc->vcreturn = vc->MovieLoad(s,mute!=0);
}

VC_LIBFUNC(vc_MoviePlay) () {
	int m = vc->ResolveOperand();
	int loop = vc->ResolveOperand();
	vc->MoviePlay(m,loop!=0);
}

VC_LIBFUNC(vc_MovieGetImage) () {  vc->vcreturn = vc->MovieGetImage(vc->ResolveOperand()); }
VC_LIBFUNC(vc_MovieRender) () { vc->MovieRender(vc->ResolveOperand()); }
VC_LIBFUNC(vc_MovieClose) () { vc->MovieClose(vc->ResolveOperand()); }
VC_LIBFUNC(vc_MovieGetCurrFrame) () { vc->vcreturn = vc->MovieGetCurrFrame(vc->ResolveOperand()); }
VC_LIBFUNC(vc_MovieGetFramerate) () { vc->vcreturn = vc->MovieGetFramerate(vc->ResolveOperand()); }
VC_LIBFUNC(vc_MovieNextFrame) () { vc->MovieNextFrame(vc->ResolveOperand()); }
VC_LIBFUNC(vc_MovieSetFrame) () {
	int m = vc->ResolveOperand();
	int f = vc->ResolveOperand();
	vc->MovieSetFrame(m,f);
}

VC_LIBFUNC(vc_GetObsPixel) () {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	vc->vcreturn = vc->GetObsPixel(x,y);
}

VC_LIBFUNC(vc_GetObs) ()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	vc->vcreturn = vc->GetObs(x,y);
}

VC_LIBFUNC(vc_SetObs) ()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int c = vc->ResolveOperand();
	vc->SetObs(x,y,c);
}

VC_LIBFUNC(vc_EntitySpawn) () {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	StringRef s = vc->ResolveString();
	vc->vcreturn = vc->EntitySpawn(x,y,s);
}

VC_LIBFUNC(vc_SetPlayer) () { vc->SetPlayer(vc->ResolveOperand()); }
VC_LIBFUNC(vc_GetPlayer) () { vc->vcreturn = vc->GetPlayer(); }

VC_LIBFUNC(vc_EntityStalk) ()
{
	int stalker = vc->ResolveOperand();
	int stalkee = vc->ResolveOperand();
	vc->EntityStalk(stalker,stalkee);
}

VC_LIBFUNC(vc_EntityMove) () {
	int e = vc->ResolveOperand();
	StringRef s = vc->ResolveString();
	vc->EntityMove(e,s);
}

VC_LIBFUNC(vc_PlayerMove) ()
{
	StringRef s = vc->ResolveString();
	vc->PlayerMove(s);
}

VC_LIBFUNC(vc_ChangeCHR) () {
	int e = vc->ResolveOperand();
	StringRef c = vc->ResolveString();
	vc->ChangeCHR(e,c);
}
VC_LIBFUNC(vc_EntitySetWanderZone) () { vc->EntitySetWanderZone(vc->ResolveOperand()); }
VC_LIBFUNC(vc_EntitySetWanderRect) ()
{
	int e = vc->ResolveOperand();
	int x1 = vc->ResolveOperand();
	int y1 = vc->ResolveOperand();
	int x2 = vc->ResolveOperand();
	int y2 = vc->ResolveOperand();
	vc->EntitySetWanderRect(e,x1,y1,x2,y2);
}
VC_LIBFUNC(vc_EntityStop) () { vc->EntityStop(vc->ResolveOperand()); }
VC_LIBFUNC(vc_EntitySetWanderDelay) ()
{
	int e = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->EntitySetWanderDelay(e,d);
}
VC_LIBFUNC(vc_SetEntitiesPaused) () { vc->SetEntitiesPaused(vc->ResolveOperand()); }

VC_LIBFUNC(vc_Render) () { vc->Render(); }
VC_LIBFUNC(vc_RenderMap) () {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int dest = vc->ResolveOperand();
	vc->RenderMap(x,y,dest);
}
VC_LIBFUNC(vc_GetSprite) () { vc->vcreturn = vc->GetSprite(); }
VC_LIBFUNC(vc_ResetSprites) () { vc->ResetSprites(); }

VC_LIBFUNC(vc_SetButtonKey) () { 
	int b = vc->ResolveOperand();
	int k = vc->ResolveOperand();
	vc->SetButtonKey(b,k);
}
VC_LIBFUNC(vc_SetButtonJB) () { 
	int b = vc->ResolveOperand();
	int jb = vc->ResolveOperand();
	vc->SetButtonJB(b,jb);
}

VC_LIBFUNC(vc_FunctionExists) ()
{
	StringRef f = vc->ResolveString();
	vc->vcreturn = vc->FunctionExists(f.c_str());
}

VC_LIBFUNC(vc_atan2) () {
	int y = vc->ResolveOperand();
	int x = vc->ResolveOperand();
	vc->vcreturn = vc->atan2(y,x);
}

VC_LIBFUNC(vc_fatan2) ()
{
	int y = vc->ResolveOperand();
	int x = vc->ResolveOperand();
	vc->vcreturn = vc->fatan2(y,x);
}

VC_LIBFUNC(vc_CopyImageToClipboard) () { vc->CopyImageToClipboard(vc->ResolveOperand()); }
VC_LIBFUNC(vc_GetImageFromClipboard) () { vc->vcreturn = vc->GetImageFromClipboard(); }

VC_LIBFUNC(vc_SetInt) ()
{
	StringRef intname = vc->ResolveString();
	int value = vc->ResolveOperand();
	vc->SetInt(intname.c_str(), value);
}

VC_LIBFUNC(vc_GetInt) ()
{
	StringRef intname = vc->ResolveString();
	vc->vcreturn = vc->GetInt(intname.c_str());
}

VC_LIBFUNC(vc_SetString) ()
{
	StringRef strname = vc->ResolveString();
	StringRef value = vc->ResolveString();
	vc->SetStr(strname.c_str(), value);
}

VC_LIBFUNC(vc_GetString) ()
{
	StringRef strname = vc->ResolveString();
	vc->vcretstr = vc->GetStr(strname.c_str());
}

VC_LIBFUNC(vc_SetIntArray) ()
{
	StringRef intname = vc->ResolveString();
	int index = vc->ResolveOperand();
	int value = vc->ResolveOperand();
	vc->SetIntArray(intname.c_str(), index, value);
}

VC_LIBFUNC(vc_GetIntArray) ()
{
	StringRef intname = vc->ResolveString();
	int index = vc->ResolveOperand();
	vc->vcreturn = vc->GetIntArray(intname.c_str(), index);
}

VC_LIBFUNC(vc_SetStringArray) ()
{
	StringRef strname = vc->ResolveString();
	int index = vc->ResolveOperand();
	StringRef value = vc->ResolveString();
	vc->SetStrArray(strname.c_str(), index, value);
}

VC_LIBFUNC(vc_GetStringArray) ()
{
	StringRef strname = vc->ResolveString();
	int index = vc->ResolveOperand();
	vc->vcretstr = vc->GetStrArray(strname.c_str(), index);
}

VC_LIBFUNC(vc_FlipBlit) () {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int fx = vc->ResolveOperand();
	int fy = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->FlipBlit(x, y, fx!=0, fy!=0, s, d);
}

VC_LIBFUNC(vc_Connect) () { vc->vcreturn = vc->Connect(vc->ResolveString()); }
VC_LIBFUNC(vc_GetConnection) () { vc->vcreturn = vc->GetConnection(); }
VC_LIBFUNC(vc_SocketConnected) () { vc->vcreturn = vc->SocketConnected(vc->ResolveOperand())?1:0; }
VC_LIBFUNC(vc_SocketHasData) ()  { vc->vcreturn = vc->SocketHasData(vc->ResolveOperand())?1:0; }
VC_LIBFUNC(vc_SocketGetString) () { vc->vcretstr = vc->SocketGetString(vc->ResolveOperand()); }

VC_LIBFUNC(vc_SocketSendString) () {
	int sh = vc->ResolveOperand();
	StringRef str = vc->ResolveString();
	vc->SocketSendString(sh,str);
}

VC_LIBFUNC(vc_SocketClose) () { vc->SocketClose(vc->ResolveOperand()); }

VC_LIBFUNC(vc_SetCustomColorFilter) ()
{
	int c1 = vc->ResolveOperand();
	int c2 = vc->ResolveOperand();
	vc->SetCustomColorFilter(c1, c2);
}

VC_LIBFUNC(vc_SocketSendInt) () { 
	int sh = vc->ResolveOperand();
	int i = vc->ResolveOperand();
	vc->SocketSendInt(sh,i);
}

VC_LIBFUNC(vc_SocketGetInt) () { vc->vcreturn = vc->SocketGetInt(vc->ResolveOperand()); }
VC_LIBFUNC(vc_GetUrlText) () { vc->vcretstr = vc->GetUrlText(vc->ResolveString()); }
VC_LIBFUNC(vc_GetUrlImage) () { vc->vcreturn = vc->GetUrlImage(vc->ResolveString()); }

VC_LIBFUNC(vc_SocketSendFile) () {
	int sh = vc->ResolveOperand();
	StringRef fn = vc->ResolveString();
	vc->SocketSendFile(sh,fn);
}

VC_LIBFUNC(vc_SocketGetFile) () {
	int sh = vc->ResolveOperand();
	StringRef override = vc->ResolveString();
	vc->vcretstr = vc->SocketGetFile(sh,override); 
}

VC_LIBFUNC(vc_ListFilePattern) () { vc->vcretstr = vc->ListFilePattern(vc->ResolveString());}

VC_LIBFUNC(vc_ImageValid) () { vc->vcreturn = vc->ImageValid(vc->ResolveOperand()); }
VC_LIBFUNC(vc_Asc) () { vc->vcreturn = vc->Asc(vc->ResolveString()); }

VC_LIBFUNC(vc_DictNew) () {
	dict *d = new dict();
	vc->vcreturn = HandleForDict(d);
}

VC_LIBFUNC(vc_DictFree) () {
	int handle = vc->ResolveOperand();
	dict *d = DictForHandle(handle);
	FreeDictHandle(handle);
	delete d;
}

VC_LIBFUNC(vc_DictGetString) () {
	dict *d = DictForHandle(vc->ResolveOperand());
	vc->vcretstr = d->GetString(vc->ResolveString());
}

VC_LIBFUNC(vc_DictSetString) () {
	dict *d = DictForHandle(vc->ResolveOperand());
	StringRef key = vc->ResolveString();
	StringRef value = vc->ResolveString();
	d->SetString(key, value);
}

VC_LIBFUNC(vc_DictContains) () {
	dict *d = DictForHandle(vc->ResolveOperand());
	vc->vcreturn = d->ContainsString(vc->ResolveString());
}

VC_LIBFUNC(vc_DictSize) () {
	dict *d = DictForHandle(vc->ResolveOperand());
	vc->vcreturn = d->Size();
}

VC_LIBFUNC(vc_DictGetInt) () {
	dict *d = DictForHandle(vc->ResolveOperand());
	vc->vcreturn = atoi(d->GetString(vc->ResolveString()).c_str());
}

VC_LIBFUNC(vc_DictSetInt) () {
	dict *d = DictForHandle(vc->ResolveOperand());
	StringRef key = vc->ResolveString();
	int value = vc->ResolveOperand();
	d->SetString(key, va("%d", value));
}

VC_LIBFUNC(vc_DictRemove) () {
	dict *d = DictForHandle(vc->ResolveOperand());
	StringRef key = vc->ResolveString();
	d->RemoveString(key);
}

// Overkill: 2007-06-20
VC_LIBFUNC(vc_DictListKeys) () {
	dict *d = DictForHandle(vc->ResolveOperand());
	StringRef separator = vc->ResolveString();
	vc->vcretstr = d->ListKeys(separator);
}

// Overkill: 12/18/05
VC_LIBFUNC(vc_max) ()
{
	int a = vc->ResolveOperand();
	int b = vc->ResolveOperand();
	vc->vcreturn = (a < b ? b : a);
}

// Overkill: 12/18/05
VC_LIBFUNC(vc_min) ()
{
	int a = vc->ResolveOperand();
	int b = vc->ResolveOperand();
	vc->vcreturn = (a > b ? b : a);
}

// Overkill: 12/18/05
VC_LIBFUNC(vc_abs) ()
{
	int a = vc->ResolveOperand();
	vc->vcreturn = (a < 0 ? -a : a);
}

// Overkill: 12/19/05
VC_LIBFUNC(vc_sgn) ()
{
	int a = vc->ResolveOperand();
	vc->vcreturn = (((a) < 0) ? -1 : ((a) > 0) ? 1 : 0);
}

// Overkill 2006-02-04
VC_LIBFUNC(vc_RectVGrad) ()
{
	int x1 = vc->ResolveOperand();
	int y1 = vc->ResolveOperand();
	int x2 = vc->ResolveOperand();
	int y2 = vc->ResolveOperand();
	int c = vc->ResolveOperand();
	int c2 = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	ScriptEngine::RectVGrad(x1, y1, x2, y2, c, c2, d);
}

// Overkill 2006-02-04
VC_LIBFUNC(vc_RectHGrad) ()
{
	int x1 = vc->ResolveOperand();
	int y1 = vc->ResolveOperand();
	int x2 = vc->ResolveOperand();
	int y2 = vc->ResolveOperand();
	int c = vc->ResolveOperand();
	int c2 = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	ScriptEngine::RectHGrad(x1, y1, x2, y2, c, c2, d);
}

// janus 2006-07-22
VC_LIBFUNC(vc_RectRGrad) ()
{
	int x1 = vc->ResolveOperand();
	int y1 = vc->ResolveOperand();
	int x2 = vc->ResolveOperand();
	int y2 = vc->ResolveOperand();
	int c = vc->ResolveOperand();
	int c2 = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	ScriptEngine::RectRGrad(x1, y1, x2, y2, c, c2, d);
}

// janus 2006-07-22
VC_LIBFUNC(vc_Rect4Grad) ()
{
	int x1 = vc->ResolveOperand();
	int y1 = vc->ResolveOperand();
	int x2 = vc->ResolveOperand();
	int y2 = vc->ResolveOperand();
	int c1 = vc->ResolveOperand();
	int c2 = vc->ResolveOperand();
	int c3 = vc->ResolveOperand();
	int c4 = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	ScriptEngine::Rect4Grad(x1, y1, x2, y2, c1, c2, c3, c4, d);
}

// Overkill (2006-06-25): Returns the cube root of a number.
VC_LIBFUNC(vc_cbrt) ()
{
	vc->vcreturn = (int) (float) pow((float) vc->ResolveOperand(), (float) 1 / 3);
}

// Overkill (2006-06-30): Gets the contents of the key buffer.
// TODO: Implement for other platforms.
VC_LIBFUNC(vc_GetKeyBuffer) ()
{
	vc->vcretstr = vc->GetKeyBuffer();
}

// Overkill (2006-06-30): Clears the contents of the key buffer.
// TODO: Implement for other platforms.
VC_LIBFUNC(vc_FlushKeyBuffer) ()
{
	vc->FlushKeyBuffer();
}

// Overkill (2006-06-30): Sets the delay in centiseconds before key repeat.
// TODO: Implement for other platforms.
VC_LIBFUNC(vc_SetKeyDelay) ()
{
	int d = vc->ResolveOperand();
	vc->SetKeyDelay(d);

}
// Overkill (2006-07-20):
// Saves a CHR file, using an open file handle, saving the specified entity.
VC_LIBFUNC(vc_FileWriteCHR) () {
	int handle = vc->ResolveOperand();
	int ent = vc->ResolveOperand();
	vc->FileWriteCHR(handle,ent);
}

// Overkill (2006-07-20):
// Saves a MAP file, using an open file handle, saving the current map.
VC_LIBFUNC(vc_FileWriteMAP) () {
	int handle = vc->ResolveOperand();
	vc->FileWriteMAP(handle);
}
// Overkill (2006-07-20):
// Saves a VSP file, using an open file handle, saving the current map's VSP.
VC_LIBFUNC(vc_FileWriteVSP) () {
	int handle = vc->ResolveOperand();
	vc->FileWriteVSP(handle);
}

// Overkill (2006-07-20):
// Compiles the specified MAP filename.
VC_LIBFUNC(vc_CompileMap) ()
{
	if(releasemode)
	{
		vc->vcerr("vc_CompileMap() - Can't compile map in release mode!");
	}
	#ifdef ALLOW_SCRIPT_COMPILATION

	StringRef filename = vc->ResolveString();
	// Get the filename sans .map extension.
	char *s = stripext(filename.c_str());
	// If the vc file exists, compile it in.
	if (Exist(va("%s.vc", s)))
	{
		if(!vcc->CompileMap(s)) {
			showMessageBox(vcc->errmsg);
			return;
		}
	}
	else
	{
		vc->vcerr("vc_CompileMap() - could not compile %s.vc!", s);
	}
	#endif
}

VC_LIBFUNC(vc_ListStructMembers) ()
{
	StringRef structname = vc->ResolveString();
	std::vector<StringRef> result;
	vc->ListStructMembers(result, structname.c_str());
	std::string temp;

	for(std::vector<StringRef>::iterator i = result.begin();
		i != result.end();
		i++)
	{
		temp += (*i).str() + "|";
	}

	vc->vcretstr = temp;
}

VC_LIBFUNC(vc_CopyArray) ()
{
	StringRef src = vc->ResolveString();
	StringRef dest = vc->ResolveString();
	bool result = vc->CopyArray(src.c_str(), dest.c_str());
	vc->vcreturn = result;
}

// Overkill (2006-11-20)
VC_LIBFUNC(vc_SoundIsPlaying) () { vc->vcreturn = ScriptEngine::SoundIsPlaying(vc->ResolveOperand()); }

// Overkill (2007-05-04)
VC_LIBFUNC(vc_GetH) ()
{
	vc->vcreturn = ScriptEngine::GetH(vc->ResolveOperand());
}

// Overkill (2007-05-04)
VC_LIBFUNC(vc_GetS) ()
{
	vc->vcreturn = ScriptEngine::GetS(vc->ResolveOperand());
}

// Overkill (2007-05-04)
VC_LIBFUNC(vc_GetV) ()
{
	vc->vcreturn = ScriptEngine::GetV(vc->ResolveOperand());
}

// Overkill (2007-05-04)
VC_LIBFUNC(vc_HSV) ()
{
	int h = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int v = vc->ResolveOperand();
	vc->vcreturn = HSVtoColor(h, s, v);
}

// Overkill (2007-05-04)
VC_LIBFUNC(vc_HueReplace) ()
{
	int hue_find = vc->ResolveOperand();
	int hue_tolerance = vc->ResolveOperand();
	int hue_replace = vc->ResolveOperand();
	int dest = vc->ResolveOperand();
	ScriptEngine::HueReplace(hue_find, hue_tolerance, hue_replace, dest);
}

// Overkill (2007-05-04)
VC_LIBFUNC(vc_ColorReplace) ()
{
	int find = vc->ResolveOperand();
	int replace = vc->ResolveOperand();
	int dest = vc->ResolveOperand();
	ScriptEngine::ColorReplace(find, replace, dest);
}

VC_LIBFUNC(vc_ListBuiltinFunctions) ()
{
	std::string temp;
	for (int i = 0; i < NUM_LIBFUNCS; i++)
	{
		temp += libfuncs[i].name + "|";
	}
	vc->vcretstr = temp;
}

VC_LIBFUNC(vc_ListBuiltinVariables) ()
{
	std::string temp;
	for (int i = 0; i < NUM_HVARS; i++)
	{
		temp += std::string(libvars[i][1]) + "|";
	}
	vc->vcretstr = temp;
}

VC_LIBFUNC(vc_ListBuiltinDefines) ()
{
	std::string temp;
	for (int i = 0; i < NUM_HDEFS; i++)
	{
		temp += std::string(hdefs[i][0]) + "|";
	}
	vc->vcretstr = temp;
}

VC_LIBFUNC(vc_GetUserSystemVcFunctionCount) ()
{
	vc->vcreturn = 0;
	vc->vcreturn = vc->userfuncs[CIMAGE_SYSTEM].size();
}

VC_LIBFUNC(vc_GetUserSystemVcFunctionByIndex) ()
{
	vc->vcretstr = "";
	int index = vc->ResolveOperand();
	int maxSize = vc->userfuncs[CIMAGE_SYSTEM].size();

	if( index < 0 || index > maxSize )
	{
		vc->vcerr("VC Execution error: Invalid offset: (%d).  Valid range: (0-%d)", index, maxSize );
	}

	StringRef myString = (vc->userfuncs[CIMAGE_SYSTEM].at(index))->name;
	vc->vcretstr = myString;
}

// Overkill (2008-04-17): Socket port can be switched to something besides 45150.
VC_LIBFUNC(vc_SetConnectionPort) ()
{
	int port = vc->ResolveOperand();
	vc->SetConnectionPort(port);
}

// Overkill (2008-04-17): Sockets can send and receive raw length-delimited strings
VC_LIBFUNC(vc_SocketGetRaw) ()
{
	int sh = vc->ResolveOperand();
	int len = vc->ResolveOperand();
	vc->vcretstr = vc->SocketGetRaw(sh, len);
}

// Overkill (2008-04-17): Sckets can send and receive raw length-delimited strings
VC_LIBFUNC(vc_SocketSendRaw) ()
{
	int sh = vc->ResolveOperand();
	StringRef str = vc->ResolveString();
	vc->SocketSendRaw(sh, str);
}

// Overkill (2008-04-20): Peek at how many bytes are in buffer. Requested by ustor.
VC_LIBFUNC(vc_SocketByteCount) ()
{
	int sh = vc->ResolveOperand();
	vc->vcreturn = vc->SocketByteCount(sh);
}


VC_LIBFUNC(vc_GetSystemSaveDir)()
{
  StringRef appname = vc->ResolveString();
  vc->vcretstr = GetSystemSaveDirectory(appname);
}

// ===================== End VC Standard Function Library =====================

void VCCore::HandleLibFunc()
{
	// Overkill (2006-06-07): Now functions past 255 work.
	// Yay! We'll probably never reach the 65535 mark,
	// so we're safe again.
	word c = currentvc->GrabW();

	VcFunctionDispatchTable & dispatch = VcGetLibfuncDispatch();
	VcFunctionImpl ptr = dispatch[c];
	if (ptr) {
		ptr();
		return;
	}

	switch (c)
	{
		case 33: vc->vcreturn = vc->LoadSong(vc->ResolveString()); break;
		case 34: vc->PlaySong(vc->ResolveOperand()); break;
		case 35: vc->StopSong(vc->ResolveOperand()); break;
		case 36: vc->PlayMusic(vc->ResolveString()); break;
		case 37: vc->StopMusic(); break;
		case 38: vc->StopSound(vc->ResolveOperand()); break;
		case 39: vc->FreeSong(vc->ResolveOperand()); break;
		case 64: DebugBreakpoint("User breakpoint"); break;
		case 108: showMessageBox(vc->ResolveString()); break;
		case 139: vc->vcreturn = vc->GetSongVolume(vc->ResolveOperand()); break;
		case 140: vc->vcreturn = vc->GetSongPos(vc->ResolveOperand()); break;
		case 174: vc->SetRandSeed(vc->ResolveOperand()); break;
		case 200: Sleep(vc->ResolveOperand()); break;
		default:
			vc->vcerr("VC Execution error: Invalid vc STDLIB index. (%d)", (int) c);
	}
}

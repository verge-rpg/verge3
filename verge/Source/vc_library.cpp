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

VcFunctionImplTable implTable;
VcFunctionDispatchTable dispatchTable;

void VcBuildLibraryDispatchTable () {
	for (int i = 0; i < NUM_LIBFUNCS; i++) {
		VcFunction & theFunc = libfuncs[i];
		std::string key;
		{
			std::stringstream buf;
			buf << "vc_" << theFunc.name;
			key = buf.str();
		}
		std::transform(key.begin(), key.end(), key.begin(), tolower);
		VcFunctionImpl ptr = implTable[key];
		dispatchTable[i] = ptr;
	}
}

void vc_initLibrary() {
	for(int i=0;i<_bind_decl_ctr;i++) {
		std::string name = _bind_decl[i].name;
		std::transform(name.begin(), name.end(), name.begin(), tolower);
		implTable[name] = _bind_decl[i].fn;
	}
	VcBuildLibraryDispatchTable();
}

/****************************** code ******************************/

dict *DictForHandle(int handle)
{
	if (handle == 0)
		se->vcerr("DictForHandle() - Null dict reference, probably an uninitialized dict handle");

	if (handle<0 || handle >= Handle::getHandleCount(HANDLE_TYPE_DICT) )
		se->vcerr("DictForHandle() - Dict reference is bogus! (%d)", handle);

	dict* ptr = (dict*) Handle::getPointer(HANDLE_TYPE_DICT,handle);

	if (ptr == NULL)
		se->vcerr("DictForHandle() - Dict reference is valid but no dict is allocated for this handle. You may have mistakenly freed it and continued to use it.");
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
	StringRef message = se->ResolveString();
	err("%s", message.c_str());
}

VC_LIBFUNC(vc_Log) () { 
	se->Log(se->ResolveString()); 
}

VC_LIBFUNC(vc_NewImage) () {
	int xsize = se->ResolveOperand();
	int ysize = se->ResolveOperand();
	se->vcreturn = se->NewImage(xsize,ysize);
}

VC_LIBFUNC(vc_MakeColor) ()
{
	int r = se->ResolveOperand();
	int g = se->ResolveOperand();
	int b = se->ResolveOperand();
	se->vcreturn = se->MakeColor(r,g,b);
}

VC_LIBFUNC(vc_SetLucent) () { se->SetLucent(se->ResolveOperand()); }

VC_LIBFUNC(vc_SetClip) ()
{
	int x1 = se->ResolveOperand();
	int y1 = se->ResolveOperand();
	int x2 = se->ResolveOperand();
	int y2 = se->ResolveOperand();
	int i = se->ResolveOperand();
	se->SetClip(x1, y1, x2, y2, i);
}

VC_LIBFUNC(vc_LoadImage) () { 
	se->vcreturn = se->LoadImage(se->ResolveString()); 
}

VC_LIBFUNC(vc_LoadImage0) ()  { 
	se->vcreturn = se->LoadImage0(se->ResolveString()); 
}

VC_LIBFUNC(vc_LoadImage8) ()  { se->vcreturn = se->LoadImage8(se->ResolveString()); }

VC_LIBFUNC(vc_ShowPage) () { se->ShowPage(); }
VC_LIBFUNC(vc_UpdateControls) () { se->UpdateControls(); }

VC_LIBFUNC(vc_Blit) () {
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	int xxx = se->ResolveOperand();
	int yyy = se->ResolveOperand();
	se->Blit(x, y, xxx, yyy);
}

VC_LIBFUNC(vc_TBlit) ()
{
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	image *s = ImageForHandle(se->ResolveOperand());
	image *d = ImageForHandle(se->ResolveOperand());
	TBlit(x, y, s, d);
}

VC_LIBFUNC(vc_AdditiveBlit) () {
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	int s = se->ResolveOperand();
	int d = se->ResolveOperand();
	se->AdditiveBlit(x, y, s, d);
}

VC_LIBFUNC(vc_TAdditiveBlit) ()
{
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	int s = se->ResolveOperand();
	int d = se->ResolveOperand();
	se->TAdditiveBlit(x, y, s, d);
}

VC_LIBFUNC(vc_SubtractiveBlit) () {
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	int s = se->ResolveOperand();
	int d = se->ResolveOperand();
	se->SubtractiveBlit(x, y, s, d);
}

VC_LIBFUNC(vc_TSubtractiveBlit) ()
{
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	int s = se->ResolveOperand();
	int d = se->ResolveOperand();
	se->TSubtractiveBlit(x, y, s, d);
}

VC_LIBFUNC(vc_WrapBlit) () {
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	int s = se->ResolveOperand();
	int d = se->ResolveOperand();
	se->WrapBlit(x, y, s, d);
}

VC_LIBFUNC(vc_TWrapBlit) () {
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	int s = se->ResolveOperand();
	int d = se->ResolveOperand();
	se->TWrapBlit(x, y, s, d);
}

VC_LIBFUNC(vc_ScaleBlit) () {
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	int dw = se->ResolveOperand();
	int dh = se->ResolveOperand();
	int s = se->ResolveOperand();
	int d = se->ResolveOperand();
	se->ScaleBlit(x, y, dw, dh, s, d);
}

VC_LIBFUNC(vc_TScaleBlit) ()
{
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	int dw = se->ResolveOperand();
	int dh = se->ResolveOperand();
	int s = se->ResolveOperand();
	int d = se->ResolveOperand();
	se->TScaleBlit(x, y, dw, dh, s, d);
}

VC_LIBFUNC(vc_RGB) () {
	int r = se->ResolveOperand();
	int g = se->ResolveOperand();
	int b = se->ResolveOperand();
	se->vcreturn = se->rgb(r,g,b);
}

VC_LIBFUNC(vc_SetPixel) () {
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	int c = se->ResolveOperand();
	int d = se->ResolveOperand();
	se->SetPixel(x, y, c, d);
}

VC_LIBFUNC(vc_GetPixel) () {
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	int s = se->ResolveOperand();
	se->vcreturn = se->GetPixel(x,y,s);
}

VC_LIBFUNC(vc_Line) () {
	int x1 = se->ResolveOperand();
	int y1 = se->ResolveOperand();
	int x2 = se->ResolveOperand();
	int y2 = se->ResolveOperand();
	int c = se->ResolveOperand();
	int d = se->ResolveOperand();
	se->Line(x1, y1, x2, y2, c, d);
}

VC_LIBFUNC(vc_Rect) ()
{
	int x1 = se->ResolveOperand();
	int y1 = se->ResolveOperand();
	int x2 = se->ResolveOperand();
	int y2 = se->ResolveOperand();
	int c = se->ResolveOperand();
	int d = se->ResolveOperand();
	se->Rect(x1, y1, x2, y2, c, d);
}

VC_LIBFUNC(vc_RectFill) ()
{
	int x1 = se->ResolveOperand();
	int y1 = se->ResolveOperand();
	int x2 = se->ResolveOperand();
	int y2 = se->ResolveOperand();
	int c = se->ResolveOperand();
	int d = se->ResolveOperand();
	se->RectFill(x1, y1, x2, y2, c, d);
}

VC_LIBFUNC(vc_Circle) () {
	int x1 = se->ResolveOperand();
	int y1 = se->ResolveOperand();
	int xr = se->ResolveOperand();
	int yr = se->ResolveOperand();
	int c = se->ResolveOperand();
	int d = se->ResolveOperand();
	se->Circle(x1, y1, xr, yr, c, d);
}

VC_LIBFUNC(vc_CircleFill) () {
	int x1 = se->ResolveOperand();
	int y1 = se->ResolveOperand();
	int xr = se->ResolveOperand();
	int yr = se->ResolveOperand();
	int c = se->ResolveOperand();
	int d = se->ResolveOperand();
	se->CircleFill(x1, y1, xr, yr, c, d);
}

VC_LIBFUNC(vc_GetR) () { se->vcreturn = se->GetR(se->ResolveOperand()); }
VC_LIBFUNC(vc_GetG) () { se->vcreturn = se->GetG(se->ResolveOperand()); }
VC_LIBFUNC(vc_GetB) () { se->vcreturn = se->GetB(se->ResolveOperand()); }

VC_LIBFUNC(vc_RotScale) () {
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	int angle = se->ResolveOperand();
	int scale = se->ResolveOperand();
	int s = se->ResolveOperand();
	int d = se->ResolveOperand();
	se->RotScale(x,y,angle,scale,s,d);
}

VC_LIBFUNC(vc_FreeImage) () { se->FreeImage(se->ResolveOperand()); }

VC_LIBFUNC(vc_Silhouette) () {
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	int c = se->ResolveOperand();
	int s = se->ResolveOperand();
	int d = se->ResolveOperand();
	se->Silhouette(x, y, c, s, d);
}

VC_LIBFUNC(vc_GrabRegion) () {
	int sx1 = se->ResolveOperand();
	int sy1 = se->ResolveOperand();
	int sx2 = se->ResolveOperand();
	int sy2 = se->ResolveOperand();
	int dx = se->ResolveOperand();
	int dy = se->ResolveOperand();
	int s = se->ResolveOperand();
	int d = se->ResolveOperand();
	se->GrabRegion(sx1,sy1,sx2,sy2,dx,dy,s,d);
}

VC_LIBFUNC(vc_TGrabRegion) ()
{
	int sx1 = se->ResolveOperand();
	int sy1 = se->ResolveOperand();
	int sx2 = se->ResolveOperand();
	int sy2 = se->ResolveOperand();
	int dx = se->ResolveOperand();
	int dy = se->ResolveOperand();
	int s = se->ResolveOperand();
	int d = se->ResolveOperand();
	se->TGrabRegion(sx1,sy1,sx2,sy2,dx,dy,s,d);
}

VC_LIBFUNC(vc_Mosaic) () {
	int xgran = se->ResolveOperand();
	int ygran = se->ResolveOperand();
	int dest = se->ResolveOperand();
	se->Mosaic(xgran, ygran, dest);
}

VC_LIBFUNC(vc_DuplicateImage) () { se->vcreturn = se->DuplicateImage(se->ResolveOperand()); }

VC_LIBFUNC(vc_Triangle) ()
{
	int x1 = se->ResolveOperand();
	int y1 = se->ResolveOperand();
	int x2 = se->ResolveOperand();
	int y2 = se->ResolveOperand();
	int x3 = se->ResolveOperand();
	int y3 = se->ResolveOperand();
	int c = se->ResolveOperand();
	int dest = se->ResolveOperand();
	se->Triangle(x1, y1, x2, y2, x3, y3, c, dest);
}

VC_LIBFUNC(vc_ImageWidth) () { se->vcreturn = se->ImageWidth(se->ResolveOperand()); }
VC_LIBFUNC(vc_ImageHeight) () { se->vcreturn = se->ImageHeight(se->ResolveOperand()); }

VC_LIBFUNC(vc_LoadFontEx) () {
	StringRef filename = se->ResolveString();
	int width = se->ResolveOperand();
	int height = se->ResolveOperand();
	se->vcreturn = se->LoadFont(filename,width,height);
}

VC_LIBFUNC(vc_SetCharacterWidth) ()
{
	Font *font = (Font*)se->ResolveOperand();
	int character = se->ResolveOperand();
	int width = se->ResolveOperand();
	font->SetCharacterWidth(character,width);
}

VC_LIBFUNC(vc_LoadFont) () {
	StringRef filename = se->ResolveString();
	se->vcreturn = se->LoadFontEx(filename);
}

VC_LIBFUNC(vc_EnableVariableWidth) () { se->EnableVariableWidth(se->ResolveOperand()); }

VC_LIBFUNC(vc_PrintString) () {
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	int dest = se->ResolveOperand();
	int fh =se->ResolveOperand();
	StringRef text = se->ResolveString();
	se->PrintString(x,y,dest,fh,text);
}

VC_LIBFUNC(vc_PrintRight) () {
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	int dest = se->ResolveOperand();
	int fh =se->ResolveOperand();
	StringRef text = se->ResolveString();
	se->PrintRight(x,y,dest,fh,text);
}

VC_LIBFUNC(vc_PrintCenter) () {
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	int dest = se->ResolveOperand();
	int fh =se->ResolveOperand();
	StringRef text = se->ResolveString();
	se->PrintCenter(x,y,dest,fh,text);
}


VC_LIBFUNC(vc_TextWidth) () {
	int fh = se->ResolveOperand();
	StringRef text = se->ResolveString();
	se->vcreturn = se->TextWidth(fh,text);
}

VC_LIBFUNC(vc_FreeFont) () { se->FreeFont(se->ResolveOperand()); }

VC_LIBFUNC(vc_Random) () { 
	int min = se->ResolveOperand();
	int max = se->ResolveOperand();
	se->vcreturn = se->Random(min, max);
}

VC_LIBFUNC(vc_len) () { se->vcreturn = se->Len(se->ResolveString()); }
VC_LIBFUNC(vc_val) () { se->vcreturn = se->Val(se->ResolveString()); }

VC_LIBFUNC(vc_Unpress) () { se->Unpress(se->ResolveOperand()); }

VC_LIBFUNC(vc_FileOpen) () {
	StringRef fname = se->ResolveString();
	int mode = se->ResolveOperand();
	se->vcreturn = se->FileOpen(fname,mode);
}

VC_LIBFUNC(vc_FileClose) () { se->FileClose(se->ResolveOperand()); }

VC_LIBFUNC(vc_FileWrite) () {
	int handle = se->ResolveOperand();
	StringRef s = se->ResolveString();
	se->FileWrite(handle,s);
}

VC_LIBFUNC(vc_FileWriteln) () {
	int handle = se->ResolveOperand();
	StringRef s = se->ResolveString();
	se->FileWriteln(handle,s);
}

VC_LIBFUNC(vc_FileReadln) () { se->vcretstr = se->FileReadln(se->ResolveOperand()); }
VC_LIBFUNC(vc_FileReadToken) () { se->vcretstr = se->FileReadToken(se->ResolveOperand()); }

VC_LIBFUNC(vc_FileSeekLine) () {
	int handle = se->ResolveOperand();
	int line = se->ResolveOperand();
	se->FileSeekLine(handle,line);
}


VC_LIBFUNC(vc_FileEOF) () { se->vcreturn = se->FileEOF(se->ResolveOperand())?1:0; }

VC_LIBFUNC(vc_LoadSound) () { se->vcreturn = se->LoadSound(se->ResolveString()); }
VC_LIBFUNC(vc_FreeSound) () { se->FreeSound(se->ResolveOperand()); }

VC_LIBFUNC(vc_PlaySound) () {
	int slot = se->ResolveOperand();
	int volume = se->ResolveOperand();
	se->vcreturn = se->PlaySound(slot,volume);
}

VC_LIBFUNC(vc_CallFunction) ()
{
	StringRef func = se->ResolveString();
	std::vector<argument_t> arguments;
	if (se->CheckForVarargs())
	{
		int i;
		se->ReadVararg(arguments);
		for (i = 0; i < arguments.size(); i++)
		{
			switch (arguments[i].type_id)
			{
				case t_INT:
					se->ArgumentPassAddInt(arguments[i].int_value);
					break;
				case t_STRING:
					se->ArgumentPassAddString(arguments[i].string_value);
					break;
			}
		}
	}
	se->ExecuteFunctionString(func.c_str());
}

VC_LIBFUNC(vc_AssignArray) ()
{
	// FIXME: restore this. (cannot read globalint from here)
	/*int n = se->ResolveOperand();
	string s = se->ResolveString();

	if (s[0] != '{')
		se->vcerr("vc_AssignArray() - string not properly formatted!");

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
	int handle = se->ResolveOperand();
	int offset = se->ResolveOperand();
	int mode = se->ResolveOperand();
	se->FileSeekPos(handle,offset,mode);
}

VC_LIBFUNC(vc_FileCurrentPos) () { se->vcreturn = se->FileCurrentPos(se->ResolveOperand()); }
VC_LIBFUNC(vc_FileWriteByte) () {
	int handle = se->ResolveOperand();
	int var = se->ResolveOperand();
	se->FileWriteByte(handle,var);
}

VC_LIBFUNC(vc_FileWriteWord) () {
	int handle = se->ResolveOperand();
	int var = se->ResolveOperand();
	se->FileWriteWord(handle,var);
}

VC_LIBFUNC(vc_FileWriteQuad) () {
	int handle = se->ResolveOperand();
	int var = se->ResolveOperand();
	se->FileWriteQuad(handle,var);
}

VC_LIBFUNC(vc_FileWriteString) () {
	int handle = se->ResolveOperand();
	StringRef s = se->ResolveString();
	se->FileWriteString(handle,s);
}

VC_LIBFUNC(vc_FileReadByte) () { se->vcreturn = se->FileReadByte(se->ResolveOperand()); }
VC_LIBFUNC(vc_FileReadWord) () { se->vcreturn = se->FileReadWord(se->ResolveOperand()); }
VC_LIBFUNC(vc_FileReadQuad) () { se->vcreturn = se->FileReadQuad(se->ResolveOperand()); }
VC_LIBFUNC(vc_FileReadString) () { se->vcretstr = se->FileReadString(se->ResolveOperand()); }

VC_LIBFUNC(vc_sqrt) () { se->vcreturn = se->sqrt(se->ResolveOperand()); }
VC_LIBFUNC(vc_pow) () {
	int a = se->ResolveOperand();
	int b = se->ResolveOperand();
	se->vcreturn = se->pow(a,b);
}

VC_LIBFUNC(vc_SetAppName) () { se->SetAppName(se->ResolveString()); }
VC_LIBFUNC(vc_SetResolution) () { 
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	se->SetResolution(x,y);
}

VC_LIBFUNC(vc_BlitLucent) ()
{
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	int lucent = se->ResolveOperand();
	int s = se->ResolveOperand();
	int d = se->ResolveOperand();
	se->BlitLucent(x,y,lucent,s,d);
}

VC_LIBFUNC(vc_TBlitLucent) ()
{
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	int lucent = se->ResolveOperand();
	int s = se->ResolveOperand();
	int d = se->ResolveOperand();
	se->TBlitLucent(x,y,lucent,s,d);
}

VC_LIBFUNC(vc_Map) ()
{
	StringRef map = se->ResolveString();
	se->Map(map);
}

VC_LIBFUNC(vc_strcmp) ()
{
	StringRef s1 = se->ResolveString();
	StringRef s2 = se->ResolveString();
	se->vcreturn = se->Strcmp(s1,s2);
}

VC_LIBFUNC(vc_strdup) ()
{
	StringRef s = se->ResolveString();
	int times = se->ResolveOperand();
	se->vcretstr = se->Strdup(s,times);
}

VC_LIBFUNC(vc_HookTimer) () { se->HookTimer(se->ResolveString()); }
VC_LIBFUNC(vc_HookRetrace) () { se->HookRetrace(se->ResolveString()); }
VC_LIBFUNC(vc_HookKey) () { 
	int k = se->ResolveOperand();
	StringRef s = se->ResolveString();
	se->HookKey(k,s);
}
VC_LIBFUNC(vc_HookButton) () { 
	int b = se->ResolveOperand();
	StringRef s = se->ResolveString();
	se->HookButton(b,s);
}

VC_LIBFUNC(vc_HookEntityRender) ()
{
	int i = se->ResolveOperand();
	StringRef s = se->ResolveString();
	se->HookEntityRender(i,s);
}

VC_LIBFUNC(vc_BlitTile) ()
{
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	int t = se->ResolveOperand();
	int d = se->ResolveOperand();
	se->BlitTile(x,y,t,d);
}

VC_LIBFUNC(vc_TBlitTile) ()
{
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	int t = se->ResolveOperand();
	int d = se->ResolveOperand();
	se->TBlitTile(x,y,t,d);
}

VC_LIBFUNC(vc_BlitEntityFrame) ()
{
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	int e = se->ResolveOperand();
	int f = se->ResolveOperand();
	int d = se->ResolveOperand();
	se->BlitEntityFrame(x,y,e,f,d);
}

VC_LIBFUNC(vc_GetTile) () {
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	int i = se->ResolveOperand();
	se->vcreturn = se->GetTile(x,y,i);
}

VC_LIBFUNC(vc_SetTile) ()
{
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	int i = se->ResolveOperand();
	int z = se->ResolveOperand();
	se->SetTile(x,y,i,z);
}

VC_LIBFUNC(vc_GetZone) ()
{
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	se->vcreturn = se->GetZone(x,y);
}

VC_LIBFUNC(vc_SetZone) ()
{
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	int z = se->ResolveOperand();
	se->SetZone(x,y,z);
}

VC_LIBFUNC(vc_SuperSecretThingy) () {
	int xskew = se->ResolveOperand();
	int yofs = se->ResolveOperand();
	int y = se->ResolveOperand();
	int s = se->ResolveOperand();
	int d = se->ResolveOperand();
	se->SuperSecretThingy(xskew, yofs, y, s, d);
}

VC_LIBFUNC(vc_BlitWrap) () {
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	int s = se->ResolveOperand();
	int d = se->ResolveOperand();
	se->BlitWrap(x, y, s, d);
}

VC_LIBFUNC(vc_ColorFilter) () {
	int filter = se->ResolveOperand();
	int d = se->ResolveOperand();
	se->ColorFilter(filter, d);
}

VC_LIBFUNC(vc_ImageShell) () {
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	int w = se->ResolveOperand();
	int h = se->ResolveOperand();
	int s = se->ResolveOperand();
	se->vcreturn = se->ImageShell(x,y,w,h,s);
}

VC_LIBFUNC(vc_Malloc) ()
{
	int s = se->ResolveOperand();
	se->vcreturn = (int) malloc(s);
}

VC_LIBFUNC(vc_MemFree) ()
{
	free((void *) se->ResolveOperand());
}

VC_LIBFUNC(vc_MemCopy) ()
{
	int src = se->ResolveOperand();
	int dst = se->ResolveOperand();
	int len = se->ResolveOperand();

	memcpy((void*) dst, (void *) src, len);
}


VC_LIBFUNC(vc_sin) () { se->vcreturn = se->sin(se->ResolveOperand()); }
VC_LIBFUNC(vc_cos) () { se->vcreturn = se->cos(se->ResolveOperand()); }
VC_LIBFUNC(vc_tan) () { se->vcreturn = se->tan(se->ResolveOperand()); }
VC_LIBFUNC(vc_fsin) () { se->vcreturn = se->fsin(se->ResolveOperand()); }
VC_LIBFUNC(vc_fcos) () { se->vcreturn = se->fcos(se->ResolveOperand()); }
VC_LIBFUNC(vc_ftan) () { se->vcreturn = se->ftan(se->ResolveOperand()); }

VC_LIBFUNC(vc_asin) () { se->vcreturn = se->asin(se->ResolveOperand()); }
VC_LIBFUNC(vc_fasin) () { se->vcreturn = se->fasin(se->ResolveOperand()); }
VC_LIBFUNC(vc_acos) () { se->vcreturn = se->acos(se->ResolveOperand()); }
VC_LIBFUNC(vc_facos) () { se->vcreturn = se->facos(se->ResolveOperand()); }
VC_LIBFUNC(vc_atan) () { se->vcreturn = se->atan(se->ResolveOperand()); }
VC_LIBFUNC(vc_fatan) () { se->vcreturn = se->fatan(se->ResolveOperand()); }

VC_LIBFUNC(vc_AlphaBlit) () {
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	int s = se->ResolveOperand();
	int a = se->ResolveOperand();
	int d = se->ResolveOperand();
	se->AlphaBlit(x, y, s, a, d);
}


VC_LIBFUNC(vc_WindowCreate) () {
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	int w = se->ResolveOperand();
	int h = se->ResolveOperand();
	StringRef s = se->ResolveString();
	se->vcreturn = se->WindowCreate(x,y,w,h,s);
}

VC_LIBFUNC(vc_WindowGetImage) () { se->vcreturn = se->WindowGetImage(se->ResolveOperand()); }
VC_LIBFUNC(vc_WindowClose) () { se->WindowClose(se->ResolveOperand()); }

VC_LIBFUNC(vc_WindowSetSize) () {
	int win = se->ResolveOperand();
	int w = se->ResolveOperand();
	int h = se->ResolveOperand();
	se->WindowSetSize(win,w,h);
}

VC_LIBFUNC(vc_WindowSetResolution) () {
	int win = se->ResolveOperand();
	int w = se->ResolveOperand();
	int h = se->ResolveOperand();
	se->WindowSetResolution(win,w,h);
}

VC_LIBFUNC(vc_WindowSetPosition) () {
	int win = se->ResolveOperand();
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	se->WindowSetPosition(win,x,y);
}

VC_LIBFUNC(vc_WindowSetTitle) () {
	int win = se->ResolveOperand();
	StringRef s = se->ResolveString();
	se->WindowSetTitle(win,s);
}

VC_LIBFUNC(vc_WindowHide) () { se->WindowHide(se->ResolveOperand()); }
VC_LIBFUNC(vc_WindowShow) () { se->WindowHide(se->ResolveOperand()); }

VC_LIBFUNC(vc_WindowGetXRes) () { se->vcreturn = se->WindowGetXRes(se->ResolveOperand()); }
VC_LIBFUNC(vc_WindowGetYRes) () { se->vcreturn = se->WindowGetYRes(se->ResolveOperand()); }
VC_LIBFUNC(vc_WindowGetWidth) () { se->vcreturn = se->WindowGetWidth(se->ResolveOperand()); }
VC_LIBFUNC(vc_WindowGetHeight) () { se->vcreturn = se->WindowGetHeight(se->ResolveOperand()); }

VC_LIBFUNC(vc_WindowPositionCommand) () {
	int win = se->ResolveOperand();
	int command = se->ResolveOperand();
	int arg1 = se->ResolveOperand();
	int arg2 = se->ResolveOperand();
	se->WindowPositionCommand(win,command,arg1,arg2);
}

VC_LIBFUNC(vc_SetSongPaused) () {
	int h = se->ResolveOperand();
	int p = se->ResolveOperand();
	se->SetSongPaused(h, p);
}

VC_LIBFUNC(vc_SetSongVolume) () {
	int h = se->ResolveOperand();
	int v = se->ResolveOperand();
	se->SetSongVolume(h, v);
}

VC_LIBFUNC(vc_SetSongPos) () {
	int h = se->ResolveOperand();
	int v = se->ResolveOperand();
	se->SetSongPos(h, v);
}

VC_LIBFUNC(vc_SetMusicVolume) () { se->SetMusicVolume(se->ResolveOperand()); }


VC_LIBFUNC(vc_TokenCount) () {
	StringRef s = se->ResolveString();
	StringRef d = se->ResolveString();
	se->vcreturn = se->TokenCount(s,d);
}

VC_LIBFUNC(vc_GetToken) ()
{
	StringRef s = se->ResolveString();
	StringRef d = se->ResolveString();
	int i = se->ResolveOperand();
	se->vcretstr = se->GetToken(s,d,i);
}

VC_LIBFUNC(vc_ToLower) () { se->vcretstr = se->ToLower(se->ResolveString()); }
VC_LIBFUNC(vc_ToUpper) () { se->vcretstr = se->ToUpper(se->ResolveString()); }

// Overkill: 2005-12-28
// Thank you, Zip.
VC_LIBFUNC(vc_strpos) ()
{
	StringRef sub = se->ResolveString();
	StringRef source = se->ResolveString();
	int start = se->ResolveOperand();
	se->vcreturn = source.str().find(sub.str(), start);
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

	StringRef teststr = se->ResolveString();
	StringRef tokens = se->ResolveString();
	int pos = se->ResolveOperand();
	int tok = se->ResolveOperand();
	se->vcreturn = GetTokenPos(teststr, tokens, pos, tok);
}


// Overkill: 2005-12-28
// Thank you, Zip.
VC_LIBFUNC(vc_TokenLeft) ()	// Excludes token.
{
	StringRef full = se->ResolveString();
	StringRef tokens = se->ResolveString();
	int pos = se->ResolveOperand();
	if (pos < 1)
	{
		se->vcretstr = empty_string;
		return;
	}
	pos = GetTokenPos(full, tokens, pos, 0);
	if (pos == full.length())
	{
		se->vcretstr = full;
		return;
	}
	else
	{
		se->vcretstr = vc_strleft(full,pos);
	}
}

// Overkill: 2005-12-28
// Thank you, Zip.
// Overkill (2006-07-28):
//	Fixed a bug where it included the delimiter character in TokenRight()
VC_LIBFUNC(vc_TokenRight) ()
{
	StringRef full = se->ResolveString();
	StringRef tokens = se->ResolveString();
	int pos = se->ResolveOperand();
	if (pos < 1)
	{
		se->vcretstr = full;
		return;
	}
	pos = GetTokenPos(full, tokens, pos, 1);
	if (pos == full.length())
	{
		se->vcretstr = empty_string;
		return;
	}
	else
	{
		se->vcretstr = vc_strright(full, full.length() - pos);
	}
}


// Overkill: 2005-12-28
// Thank you, Zip.
VC_LIBFUNC(vc_strovr) ()
{
	StringRef rep = se->ResolveString();
	StringRef source = se->ResolveString();
	int offset = se->ResolveOperand();
	se->vcretstr = strovr(source, rep, offset);
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

	int wt_font = se->ResolveOperand();
	StringRef wt_s = se->ResolveString();
	int wt_linelen = se->ResolveOperand();
	se->vcretstr = ScriptEngine::WrapText(wt_font,wt_s,wt_linelen);
}


VC_LIBFUNC(vc_FontHeight) () { se->vcreturn = se->FontHeight(se->ResolveOperand()); }

VC_LIBFUNC(vc_MixColor) () {
	int c1 = se->ResolveOperand();
	int c2 = se->ResolveOperand();
	int p = se->ResolveOperand();
	se->vcreturn = se->MixColor(c1,c2,p);
}

VC_LIBFUNC(vc_CHR) () { se->vcretstr = se->Chr(se->ResolveOperand()); }

VC_LIBFUNC(vc_PlayMovie) () { se->vcreturn = se->PlayMovie(se->ResolveString()); }
VC_LIBFUNC(vc_AbortMovie) () { se->AbortMovie(); }

VC_LIBFUNC(vc_MovieLoad) () {
	StringRef s = se->ResolveString();
	int mute = se->ResolveOperand();
	se->vcreturn = se->MovieLoad(s,mute!=0);
}

VC_LIBFUNC(vc_MoviePlay) () {
	int m = se->ResolveOperand();
	int loop = se->ResolveOperand();
	se->MoviePlay(m,loop!=0);
}

VC_LIBFUNC(vc_MovieGetImage) () {  se->vcreturn = se->MovieGetImage(se->ResolveOperand()); }
VC_LIBFUNC(vc_MovieRender) () { se->MovieRender(se->ResolveOperand()); }
VC_LIBFUNC(vc_MovieClose) () { se->MovieClose(se->ResolveOperand()); }
VC_LIBFUNC(vc_MovieGetCurrFrame) () { se->vcreturn = se->MovieGetCurrFrame(se->ResolveOperand()); }
VC_LIBFUNC(vc_MovieGetFramerate) () { se->vcreturn = se->MovieGetFramerate(se->ResolveOperand()); }
VC_LIBFUNC(vc_MovieNextFrame) () { se->MovieNextFrame(se->ResolveOperand()); }
VC_LIBFUNC(vc_MovieSetFrame) () {
	int m = se->ResolveOperand();
	int f = se->ResolveOperand();
	se->MovieSetFrame(m,f);
}

VC_LIBFUNC(vc_GetObsPixel) () {
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	se->vcreturn = se->GetObsPixel(x,y);
}

VC_LIBFUNC(vc_GetObs) ()
{
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	se->vcreturn = se->GetObs(x,y);
}

VC_LIBFUNC(vc_SetObs) ()
{
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	int c = se->ResolveOperand();
	se->SetObs(x,y,c);
}

VC_LIBFUNC(vc_EntitySpawn) () {
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	StringRef s = se->ResolveString();
	se->vcreturn = se->EntitySpawn(x,y,s);
}

VC_LIBFUNC(vc_SetPlayer) () { se->SetPlayer(se->ResolveOperand()); }
VC_LIBFUNC(vc_GetPlayer) () { se->vcreturn = se->GetPlayer(); }

VC_LIBFUNC(vc_EntityStalk) ()
{
	int stalker = se->ResolveOperand();
	int stalkee = se->ResolveOperand();
	se->EntityStalk(stalker,stalkee);
}

VC_LIBFUNC(vc_EntityMove) () {
	int e = se->ResolveOperand();
	StringRef s = se->ResolveString();
	se->EntityMove(e,s);
}

VC_LIBFUNC(vc_PlayerMove) ()
{
	StringRef s = se->ResolveString();
	se->PlayerMove(s);
}

VC_LIBFUNC(vc_ChangeCHR) () {
	int e = se->ResolveOperand();
	StringRef c = se->ResolveString();
	se->ChangeCHR(e,c);
}
VC_LIBFUNC(vc_EntitySetWanderZone) () { se->EntitySetWanderZone(se->ResolveOperand()); }
VC_LIBFUNC(vc_EntitySetWanderRect) ()
{
	int e = se->ResolveOperand();
	int x1 = se->ResolveOperand();
	int y1 = se->ResolveOperand();
	int x2 = se->ResolveOperand();
	int y2 = se->ResolveOperand();
	se->EntitySetWanderRect(e,x1,y1,x2,y2);
}
VC_LIBFUNC(vc_EntityStop) () { se->EntityStop(se->ResolveOperand()); }
VC_LIBFUNC(vc_EntitySetWanderDelay) ()
{
	int e = se->ResolveOperand();
	int d = se->ResolveOperand();
	se->EntitySetWanderDelay(e,d);
}
VC_LIBFUNC(vc_SetEntitiesPaused) () { se->SetEntitiesPaused(se->ResolveOperand()); }

VC_LIBFUNC(vc_Render) () { se->Render(); }
VC_LIBFUNC(vc_RenderMap) () {
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	int dest = se->ResolveOperand();
	se->RenderMap(x,y,dest);
}
VC_LIBFUNC(vc_GetSprite) () { se->vcreturn = se->GetSprite(); }
VC_LIBFUNC(vc_ResetSprites) () { se->ResetSprites(); }

VC_LIBFUNC(vc_SetButtonKey) () { 
	int b = se->ResolveOperand();
	int k = se->ResolveOperand();
	se->SetButtonKey(b,k);
}
VC_LIBFUNC(vc_SetButtonJB) () { 
	int b = se->ResolveOperand();
	int jb = se->ResolveOperand();
	se->SetButtonJB(b,jb);
}

VC_LIBFUNC(vc_FunctionExists) ()
{
	StringRef f = se->ResolveString();
	se->vcreturn = se->FunctionExists(f.c_str());
}

VC_LIBFUNC(vc_atan2) () {
	int y = se->ResolveOperand();
	int x = se->ResolveOperand();
	se->vcreturn = se->atan2(y,x);
}

VC_LIBFUNC(vc_fatan2) ()
{
	int y = se->ResolveOperand();
	int x = se->ResolveOperand();
	se->vcreturn = se->fatan2(y,x);
}

VC_LIBFUNC(vc_CopyImageToClipboard) () { se->CopyImageToClipboard(se->ResolveOperand()); }
VC_LIBFUNC(vc_GetImageFromClipboard) () { se->vcreturn = se->GetImageFromClipboard(); }

VC_LIBFUNC(vc_SetInt) ()
{
	StringRef intname = se->ResolveString();
	int value = se->ResolveOperand();
	vc->SetInt(intname.c_str(), value);
}

VC_LIBFUNC(vc_GetInt) ()
{
	StringRef intname = se->ResolveString();
	se->vcreturn = vc->GetInt(intname.c_str());
}

VC_LIBFUNC(vc_IntExists) ()
{
	StringRef intname = se->ResolveString();

	if( vc->IntExists(intname.c_str()) ) {
		se->vcreturn = 1;
	} else {
		se->vcreturn = 0;
	}
}

VC_LIBFUNC(vc_StrExists) ()
{
	StringRef strname = se->ResolveString();

	if( vc->StrExists(strname.c_str()) ) {
		se->vcreturn = 1;
	} else {
		se->vcreturn = 0;
	}
}

VC_LIBFUNC(vc_SetString) ()
{
	StringRef strname = se->ResolveString();
	StringRef value = se->ResolveString();
	vc->SetStr(strname, value);
}

VC_LIBFUNC(vc_GetString) ()
{
	StringRef strname = se->ResolveString();
	se->vcretstr = vc->GetStr(strname.c_str());
}

VC_LIBFUNC(vc_SetIntArray) ()
{
	StringRef intname = se->ResolveString();
	int index = se->ResolveOperand();
	int value = se->ResolveOperand();
	vc->SetIntArray(intname.c_str(), index, value);
}

VC_LIBFUNC(vc_GetIntArray) ()
{
	StringRef intname = se->ResolveString();
	int index = se->ResolveOperand();
	se->vcreturn = vc->GetIntArray(intname.c_str(), index);
}

VC_LIBFUNC(vc_SetStringArray) ()
{
	StringRef strname = se->ResolveString();
	int index = se->ResolveOperand();
	StringRef value = se->ResolveString();
	vc->SetStrArray(strname, index, value);
}

VC_LIBFUNC(vc_GetStringArray) ()
{
	StringRef strname = se->ResolveString();
	int index = se->ResolveOperand();
	se->vcretstr = vc->GetStrArray(strname, index);
}

VC_LIBFUNC(vc_FlipBlit) () {
	int x = se->ResolveOperand();
	int y = se->ResolveOperand();
	int fx = se->ResolveOperand();
	int fy = se->ResolveOperand();
	int s = se->ResolveOperand();
	int d = se->ResolveOperand();
	se->FlipBlit(x, y, fx!=0, fy!=0, s, d);
}

VC_LIBFUNC(vc_Connect) () { se->vcreturn = se->Connect(se->ResolveString()); }
VC_LIBFUNC(vc_GetConnection) () { se->vcreturn = se->GetConnection(); }
VC_LIBFUNC(vc_SocketConnected) () { se->vcreturn = se->SocketConnected(se->ResolveOperand())?1:0; }
VC_LIBFUNC(vc_SocketHasData) ()  { se->vcreturn = se->SocketHasData(se->ResolveOperand())?1:0; }
VC_LIBFUNC(vc_SocketGetString) () { se->vcretstr = se->SocketGetString(se->ResolveOperand()); }

VC_LIBFUNC(vc_SocketSendString) () {
	int sh = se->ResolveOperand();
	StringRef str = se->ResolveString();
	se->SocketSendString(sh,str);
}

VC_LIBFUNC(vc_SocketClose) () { se->SocketClose(se->ResolveOperand()); }

VC_LIBFUNC(vc_SetCustomColorFilter) ()
{
	int c1 = se->ResolveOperand();
	int c2 = se->ResolveOperand();
	se->SetCustomColorFilter(c1, c2);
}

VC_LIBFUNC(vc_SocketSendInt) () { 
	int sh = se->ResolveOperand();
	int i = se->ResolveOperand();
	se->SocketSendInt(sh,i);
}

VC_LIBFUNC(vc_SocketGetInt) () { se->vcreturn = se->SocketGetInt(se->ResolveOperand()); }
VC_LIBFUNC(vc_GetUrlText) () { se->vcretstr = se->GetUrlText(se->ResolveString()); }
VC_LIBFUNC(vc_GetUrlImage) () { se->vcreturn = se->GetUrlImage(se->ResolveString()); }

VC_LIBFUNC(vc_SocketSendFile) () {
	int sh = se->ResolveOperand();
	StringRef fn = se->ResolveString();
	se->SocketSendFile(sh,fn);
}

VC_LIBFUNC(vc_SocketGetFile) () {
	int sh = se->ResolveOperand();
	StringRef override = se->ResolveString();
	se->vcretstr = se->SocketGetFile(sh,override); 
}

VC_LIBFUNC(vc_ListFilePattern) () { se->vcretstr = se->ListFilePattern(se->ResolveString());}

VC_LIBFUNC(vc_ImageValid) () { se->vcreturn = se->ImageValid(se->ResolveOperand()); }
VC_LIBFUNC(vc_Asc) () { se->vcreturn = se->Asc(se->ResolveString()); }

VC_LIBFUNC(vc_DictNew) () {
	dict *d = new dict();
	se->vcreturn = HandleForDict(d);
}

VC_LIBFUNC(vc_DictFree) () {
	int handle = se->ResolveOperand();
	dict *d = DictForHandle(handle);
	FreeDictHandle(handle);
	delete d;
}

VC_LIBFUNC(vc_DictGetString) () {
	dict *d = DictForHandle(se->ResolveOperand());
	se->vcretstr = d->GetString(se->ResolveString());
}

VC_LIBFUNC(vc_DictSetString) () {
	dict *d = DictForHandle(se->ResolveOperand());
	StringRef key = se->ResolveString();
	StringRef value = se->ResolveString();
	d->SetString(key, value);
}

VC_LIBFUNC(vc_DictContains) () {
	dict *d = DictForHandle(se->ResolveOperand());
	se->vcreturn = d->ContainsString(se->ResolveString());
}

VC_LIBFUNC(vc_DictSize) () {
	dict *d = DictForHandle(se->ResolveOperand());
	se->vcreturn = d->Size();
}

VC_LIBFUNC(vc_DictGetInt) () {
	dict *d = DictForHandle(se->ResolveOperand());
	se->vcreturn = atoi(d->GetString(se->ResolveString()).c_str());
}

VC_LIBFUNC(vc_DictSetInt) () {
	dict *d = DictForHandle(se->ResolveOperand());
	StringRef key = se->ResolveString();
	int value = se->ResolveOperand();
	d->SetString(key, va("%d", value));
}

VC_LIBFUNC(vc_DictRemove) () {
	dict *d = DictForHandle(se->ResolveOperand());
	StringRef key = se->ResolveString();
	d->RemoveString(key);
}

// Overkill: 2007-06-20
VC_LIBFUNC(vc_DictListKeys) () {
	dict *d = DictForHandle(se->ResolveOperand());
	StringRef separator = se->ResolveString();
	se->vcretstr = d->ListKeys(separator);
}

// Overkill: 12/18/05
VC_LIBFUNC(vc_max) ()
{
	int a = se->ResolveOperand();
	int b = se->ResolveOperand();
	se->vcreturn = (a < b ? b : a);
}

// Overkill: 12/18/05
VC_LIBFUNC(vc_min) ()
{
	int a = se->ResolveOperand();
	int b = se->ResolveOperand();
	se->vcreturn = (a > b ? b : a);
}

// Overkill: 12/18/05
VC_LIBFUNC(vc_abs) ()
{
	int a = se->ResolveOperand();
	se->vcreturn = (a < 0 ? -a : a);
}

// Overkill: 12/19/05
VC_LIBFUNC(vc_sgn) ()
{
	int a = se->ResolveOperand();
	se->vcreturn = (((a) < 0) ? -1 : ((a) > 0) ? 1 : 0);
}

// Overkill 2006-02-04
VC_LIBFUNC(vc_RectVGrad) ()
{
	int x1 = se->ResolveOperand();
	int y1 = se->ResolveOperand();
	int x2 = se->ResolveOperand();
	int y2 = se->ResolveOperand();
	int c = se->ResolveOperand();
	int c2 = se->ResolveOperand();
	int d = se->ResolveOperand();
	ScriptEngine::RectVGrad(x1, y1, x2, y2, c, c2, d);
}

// Overkill 2006-02-04
VC_LIBFUNC(vc_RectHGrad) ()
{
	int x1 = se->ResolveOperand();
	int y1 = se->ResolveOperand();
	int x2 = se->ResolveOperand();
	int y2 = se->ResolveOperand();
	int c = se->ResolveOperand();
	int c2 = se->ResolveOperand();
	int d = se->ResolveOperand();
	ScriptEngine::RectHGrad(x1, y1, x2, y2, c, c2, d);
}

// janus 2006-07-22
VC_LIBFUNC(vc_RectRGrad) ()
{
	int x1 = se->ResolveOperand();
	int y1 = se->ResolveOperand();
	int x2 = se->ResolveOperand();
	int y2 = se->ResolveOperand();
	int c = se->ResolveOperand();
	int c2 = se->ResolveOperand();
	int d = se->ResolveOperand();
	ScriptEngine::RectRGrad(x1, y1, x2, y2, c, c2, d);
}

// janus 2006-07-22
VC_LIBFUNC(vc_Rect4Grad) ()
{
	int x1 = se->ResolveOperand();
	int y1 = se->ResolveOperand();
	int x2 = se->ResolveOperand();
	int y2 = se->ResolveOperand();
	int c1 = se->ResolveOperand();
	int c2 = se->ResolveOperand();
	int c3 = se->ResolveOperand();
	int c4 = se->ResolveOperand();
	int d = se->ResolveOperand();
	ScriptEngine::Rect4Grad(x1, y1, x2, y2, c1, c2, c3, c4, d);
}

// Overkill (2006-06-25): Returns the cube root of a number.
VC_LIBFUNC(vc_cbrt) ()
{
	se->vcreturn = (int) (float) pow((float) se->ResolveOperand(), (float) 1 / 3);
}

// Overkill (2006-06-30): Gets the contents of the key buffer.
// TODO: Implement for other platforms.
VC_LIBFUNC(vc_GetKeyBuffer) ()
{
	se->vcretstr = se->GetKeyBuffer();
}

// Overkill (2006-06-30): Clears the contents of the key buffer.
// TODO: Implement for other platforms.
VC_LIBFUNC(vc_FlushKeyBuffer) ()
{
	se->FlushKeyBuffer();
}

// Overkill (2006-06-30): Sets the delay in centiseconds before key repeat.
// TODO: Implement for other platforms.
VC_LIBFUNC(vc_SetKeyDelay) ()
{
	int d = se->ResolveOperand();
	se->SetKeyDelay(d);

}
// Overkill (2006-07-20):
// Saves a CHR file, using an open file handle, saving the specified entity.
VC_LIBFUNC(vc_FileWriteCHR) () {
	int handle = se->ResolveOperand();
	int ent = se->ResolveOperand();
	se->FileWriteCHR(handle,ent);
}

// Overkill (2006-07-20):
// Saves a MAP file, using an open file handle, saving the current map.
VC_LIBFUNC(vc_FileWriteMAP) () {
	int handle = se->ResolveOperand();
	se->FileWriteMAP(handle);
}
// Overkill (2006-07-20):
// Saves a VSP file, using an open file handle, saving the current map's VSP.
VC_LIBFUNC(vc_FileWriteVSP) () {
	int handle = se->ResolveOperand();
	se->FileWriteVSP(handle);
}

// Overkill (2006-07-20):
// Compiles the specified MAP filename.
VC_LIBFUNC(vc_CompileMap) ()
{
	if(releasemode)
	{
		se->vcerr("vc_CompileMap() - Can't compile map in release mode!");
	}
	#ifdef ALLOW_SCRIPT_COMPILATION

	StringRef filename = se->ResolveString();
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
		se->vcerr("vc_CompileMap() - could not compile %s.vc!", s);
	}
	#endif
}

VC_LIBFUNC(vc_ListStructMembers) ()
{
	StringRef structname = se->ResolveString();
	std::vector<StringRef> result;
	vc->ListStructMembers(result, structname.c_str());
	std::string temp;

	for(std::vector<StringRef>::iterator i = result.begin();
		i != result.end();
		i++)
	{
		temp += (*i).str() + "|";
	}

	se->vcretstr = temp;
}

VC_LIBFUNC(vc_CopyArray) ()
{
	StringRef src = se->ResolveString();
	StringRef dest = se->ResolveString();
	bool result = vc->CopyArray(src.c_str(), dest.c_str());
	se->vcreturn = result;
}

// Overkill (2006-11-20)
VC_LIBFUNC(vc_SoundIsPlaying) () { se->vcreturn = ScriptEngine::SoundIsPlaying(se->ResolveOperand()); }

// Overkill (2007-05-04)
VC_LIBFUNC(vc_GetH) ()
{
	se->vcreturn = ScriptEngine::GetH(se->ResolveOperand());
}

// Overkill (2007-05-04)
VC_LIBFUNC(vc_GetS) ()
{
	se->vcreturn = ScriptEngine::GetS(se->ResolveOperand());
}

// Overkill (2007-05-04)
VC_LIBFUNC(vc_GetV) ()
{
	se->vcreturn = ScriptEngine::GetV(se->ResolveOperand());
}

// Overkill (2007-05-04)
VC_LIBFUNC(vc_HSV) ()
{
	int h = se->ResolveOperand();
	int s = se->ResolveOperand();
	int v = se->ResolveOperand();
	se->vcreturn = HSVtoColor(h, s, v);
}

// Overkill (2007-05-04)
VC_LIBFUNC(vc_HueReplace) ()
{
	int hue_find = se->ResolveOperand();
	int hue_tolerance = se->ResolveOperand();
	int hue_replace = se->ResolveOperand();
	int dest = se->ResolveOperand();
	ScriptEngine::HueReplace(hue_find, hue_tolerance, hue_replace, dest);
}

// Overkill (2007-05-04)
VC_LIBFUNC(vc_ColorReplace) ()
{
	int find = se->ResolveOperand();
	int replace = se->ResolveOperand();
	int dest = se->ResolveOperand();
	ScriptEngine::ColorReplace(find, replace, dest);
}

VC_LIBFUNC(vc_ListBuiltinFunctions) ()
{
	std::string temp;
	for (int i = 0; i < NUM_LIBFUNCS; i++)
	{
		temp += libfuncs[i].name + "|";
	}
	se->vcretstr = temp;
}

VC_LIBFUNC(vc_ListBuiltinVariables) ()
{
	std::string temp;
	for (int i = 0; i < NUM_HVARS; i++)
	{
		temp += std::string(libvars[i][1]) + "|";
	}
	se->vcretstr = temp;
}

VC_LIBFUNC(vc_ListBuiltinDefines) ()
{
	std::string temp;
	for (int i = 0; i < NUM_HDEFS; i++)
	{
		temp += std::string(hdefs[i].key) + "|";
	}
	se->vcretstr = temp;
}

VC_LIBFUNC(vc_GetUserSystemVcFunctionCount) ()
{
	se->vcreturn = 0;
	se->vcreturn = vc->userfuncs[CIMAGE_SYSTEM].size();
}

VC_LIBFUNC(vc_GetUserSystemVcFunctionByIndex) ()
{
	int index = se->ResolveOperand();
	int maxSize = vc->userfuncs[CIMAGE_SYSTEM].size();

	if( index < 0 || index > maxSize )
	{
		se->vcerr("VC Execution error: Invalid offset: (%d).  Valid range: (0-%d)", index, maxSize );
	}

	StringRef myString = (vc->userfuncs[CIMAGE_SYSTEM].at(index))->name;
	se->vcretstr = myString;
}

// Overkill (2008-04-17): Socket port can be switched to something besides 45150.
VC_LIBFUNC(vc_SetConnectionPort) ()
{
	int port = se->ResolveOperand();
	se->SetConnectionPort(port);
}

// Overkill (2008-04-17): Sockets can send and receive raw length-delimited strings
VC_LIBFUNC(vc_SocketGetRaw) ()
{
	int sh = se->ResolveOperand();
	int len = se->ResolveOperand();
	se->vcretstr = se->SocketGetRaw(sh, len);
}

// Overkill (2008-04-17): Sckets can send and receive raw length-delimited strings
VC_LIBFUNC(vc_SocketSendRaw) ()
{
	int sh = se->ResolveOperand();
	StringRef str = se->ResolveString();
	se->SocketSendRaw(sh, str);
}

// Overkill (2008-04-20): Peek at how many bytes are in buffer. Requested by ustor.
VC_LIBFUNC(vc_SocketByteCount) ()
{
	int sh = se->ResolveOperand();
	se->vcreturn = se->SocketByteCount(sh);
}


VC_LIBFUNC(vc_GetSystemSaveDir)()
{
  StringRef appname = se->ResolveString();
  se->vcretstr = GetSystemSaveDirectory(appname);
}

extern int _input_killswitch;
VC_LIBFUNC(vc_GetInputKillSwitch) ()
{
	se->vcreturn = _input_killswitch;
}

VC_LIBFUNC(vc_SetInputKillSwitch) ()
{
	int kill = se->ResolveOperand();
	_input_killswitch = kill;
}

VC_LIBFUNC(vc_LoadSong) ()
{
	se->vcreturn = se->LoadSong(se->ResolveString());
}

VC_LIBFUNC(vc_PlaySong) ()
{
	se->PlaySong(se->ResolveOperand());
}

VC_LIBFUNC(vc_StopSong) ()
{
	se->StopSong(se->ResolveOperand());
}

VC_LIBFUNC(vc_PlayMusic) ()
{
	se->PlayMusic(se->ResolveString());
}

VC_LIBFUNC(vc_StopMusic) ()
{
	se->StopMusic();
}

VC_LIBFUNC(vc_StopSound) ()
{
	se->StopSound(se->ResolveOperand());
}

VC_LIBFUNC(vc_FreeSong) ()
{
	se->FreeSong(se->ResolveOperand());
}

VC_LIBFUNC(vc_GetSongVolume) ()
{
	se->vcreturn = se->GetSongVolume(se->ResolveOperand());
}

VC_LIBFUNC(vc_GetSongPos) ()
{
	se->vcreturn = se->GetSongPos(se->ResolveOperand());
}

VC_LIBFUNC(vc_SetRandSeed) ()
{
	se->SetRandSeed(se->ResolveOperand());
}

VC_LIBFUNC(vc_Sleep) ()
{
	Sleep(se->ResolveOperand());
}

VC_LIBFUNC(vc_MessageBox) ()
{
	showMessageBox(se->ResolveString());
}


// ===================== End VC Standard Function Library =====================

void VCCore::HandleLibFunc(word c)
{
	VcFunctionImpl ptr = dispatchTable[c];
	if (ptr) {
		ptr();
		return;
	}

	// HEY YOU! Only add to here if:
	// 1) you only need it in VC, ie. a language feature or something not useful in Lua,
	// 2) it's short.
	// Otherwise make a function, mmkay.
	switch (c)
	{		
		case 64: DebugBreakpoint("User breakpoint"); break;
		default:
			se->vcerr("VC Execution error: Invalid vc STDLIB index. (%d)", (int) c);
	}
}

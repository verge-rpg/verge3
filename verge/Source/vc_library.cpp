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

/****************************** data ******************************/

extern VCCore *vc;

int cf_r1, cf_g1, cf_b1;
int cf_r2, cf_g2, cf_b2;
int cf_rr, cf_gr, cf_br;

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

void vc_Exit()
{
	std::string message = vc->ResolveString();
	err("%s", message.c_str());
}

void vc_Log() { vc->Log(vc->ResolveString()); }

void vc_NewImage() {
	int xsize = vc->ResolveOperand();
	int ysize = vc->ResolveOperand();
	vc->vcreturn = vc->NewImage(xsize,ysize);
}

void vc_MakeColor()
{
	int r = vc->ResolveOperand();
	int g = vc->ResolveOperand();
	int b = vc->ResolveOperand();
	vc->vcreturn = vc->MakeColor(r,g,b);
}

void vc_SetLucent() { vc->SetLucent(vc->ResolveOperand()); }

void vc_SetClip()
{
	int x1 = vc->ResolveOperand();
	int y1 = vc->ResolveOperand();
	int x2 = vc->ResolveOperand();
	int y2 = vc->ResolveOperand();
	int i = vc->ResolveOperand();
	vc->SetClip(x1, y1, x2, y2, i);
}

void vc_LoadImage() { vc->vcreturn = vc->LoadImage(vc->ResolveString()); }
void vc_LoadImage0()  { vc->vcreturn = vc->LoadImage0(vc->ResolveString()); }
void vc_LoadImage8()  { vc->vcreturn = vc->LoadImage8(vc->ResolveString()); }

void vc_ShowPage() { vc->ShowPage(); }
void vc_UpdateControls() { vc->UpdateControls(); }

void vc_Blit() {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int xxx = vc->ResolveOperand();
	int yyy = vc->ResolveOperand();
	vc->Blit(x, y, xxx, yyy);
}

void vc_TBlit()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	image *s = ImageForHandle(vc->ResolveOperand());
	image *d = ImageForHandle(vc->ResolveOperand());
	TBlit(x, y, s, d);
}

void vc_AdditiveBlit() {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->AdditiveBlit(x, y, s, d);
}

void vc_TAdditiveBlit()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->TAdditiveBlit(x, y, s, d);
}

void vc_SubtractiveBlit() {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->SubtractiveBlit(x, y, s, d);
}

void vc_TSubtractiveBlit()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->TSubtractiveBlit(x, y, s, d);
}

void vc_WrapBlit() {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->WrapBlit(x, y, s, d);
}

void vc_TWrapBlit() {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->TWrapBlit(x, y, s, d);
}

void vc_ScaleBlit() {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int dw = vc->ResolveOperand();
	int dh = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->ScaleBlit(x, y, dw, dh, s, d);
}

void vc_TScaleBlit()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int dw = vc->ResolveOperand();
	int dh = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->TScaleBlit(x, y, dw, dh, s, d);
}

void vc_RGB() {
	int r = vc->ResolveOperand();
	int g = vc->ResolveOperand();
	int b = vc->ResolveOperand();
	vc->vcreturn = vc->rgb(r,g,b);
}

void vc_SetPixel() {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int c = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->SetPixel(x, y, c, d);
}

void vc_GetPixel() {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	vc->vcreturn = vc->GetPixel(x,y,s);
}

void vc_Line() {
	int x1 = vc->ResolveOperand();
	int y1 = vc->ResolveOperand();
	int x2 = vc->ResolveOperand();
	int y2 = vc->ResolveOperand();
	int c = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->Line(x1, y1, x2, y2, c, d);
}

void vc_Rect()
{
	int x1 = vc->ResolveOperand();
	int y1 = vc->ResolveOperand();
	int x2 = vc->ResolveOperand();
	int y2 = vc->ResolveOperand();
	int c = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->Rect(x1, y1, x2, y2, c, d);
}

void vc_RectFill()
{
	int x1 = vc->ResolveOperand();
	int y1 = vc->ResolveOperand();
	int x2 = vc->ResolveOperand();
	int y2 = vc->ResolveOperand();
	int c = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->RectFill(x1, y1, x2, y2, c, d);
}

void vc_Circle() {
	int x1 = vc->ResolveOperand();
	int y1 = vc->ResolveOperand();
	int xr = vc->ResolveOperand();
	int yr = vc->ResolveOperand();
	int c = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->Circle(x1, y1, xr, yr, c, d);
}

void vc_CircleFill() {
	int x1 = vc->ResolveOperand();
	int y1 = vc->ResolveOperand();
	int xr = vc->ResolveOperand();
	int yr = vc->ResolveOperand();
	int c = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->CircleFill(x1, y1, xr, yr, c, d);
}

void vc_GetR() { vc->vcreturn = vc->GetR(vc->ResolveOperand()); }
void vc_GetG() { vc->vcreturn = vc->GetG(vc->ResolveOperand()); }
void vc_GetB() { vc->vcreturn = vc->GetB(vc->ResolveOperand()); }

void vc_RotScale() {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int angle = vc->ResolveOperand();
	int scale = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->RotScale(x,y,angle,scale,s,d);
}

void vc_FreeImage() { vc->FreeImage(vc->ResolveOperand()); }

void vc_Silhouette() {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int c = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->Silhouette(x, y, c, s, d);
}

void vc_GrabRegion() {
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

void vc_TGrabRegion()
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

void vc_Mosaic() {
	int xgran = vc->ResolveOperand();
	int ygran = vc->ResolveOperand();
	int dest = vc->ResolveOperand();
	vc->Mosaic(xgran, ygran, dest);
}

void vc_DuplicateImage() { vc->vcreturn = vc->DuplicateImage(vc->ResolveOperand()); }

void vc_Triangle()
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

void vc_ImageWidth() { vc->vcreturn = vc->ImageWidth(vc->ResolveOperand()); }
void vc_ImageHeight() { vc->vcreturn = vc->ImageHeight(vc->ResolveOperand()); }

void vc_LoadFont() {
	std::string filename = vc->ResolveString();
	int width = vc->ResolveOperand();
	int height = vc->ResolveOperand();
	vc->vcreturn = vc->LoadFont(filename,width,height);
}

void vc_SetCharacterWidth()
{
	Font *font = (Font*)vc->ResolveOperand();
	int character = vc->ResolveOperand();
	int width = vc->ResolveOperand();
	font->SetCharacterWidth(character,width);
}

void vc_LoadFontEx() {
	std::string filename = vc->ResolveString();
	vc->vcreturn = vc->LoadFontEx(filename);
}

void vc_EnableVariableWidth() { vc->EnableVariableWidth(vc->ResolveOperand()); }

void vc_PrintString() {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int dest = vc->ResolveOperand();
	int fh =vc->ResolveOperand();
	std::string text = vc->ResolveString();
	vc->PrintString(x,y,dest,fh,text);
}

void vc_PrintRight() {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int dest = vc->ResolveOperand();
	int fh =vc->ResolveOperand();
	std::string text = vc->ResolveString();
	vc->PrintRight(x,y,dest,fh,text);
}

void vc_PrintCenter() {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int dest = vc->ResolveOperand();
	int fh =vc->ResolveOperand();
	std::string text = vc->ResolveString();
	vc->PrintCenter(x,y,dest,fh,text);
}


void vc_TextWidth() {
	int fh = vc->ResolveOperand();
	std::string text = vc->ResolveString();
	vc->vcreturn = vc->TextWidth(fh,text);
}

void vc_FreeFont() { vc->FreeFont(vc->ResolveOperand()); }

void vc_Random() { 
	int min = vc->ResolveOperand();
	int max = vc->ResolveOperand();
	vc->vcreturn = vc->Random(min, max);
}

void vc_len() { vc->vcreturn = vc->Len(vc->ResolveString()); }
void vc_val() { vc->vcreturn = vc->Val(vc->ResolveString()); }

void vc_Unpress() { vc->Unpress(vc->ResolveOperand()); }

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

void vc_FileOpen() {
	std::string fname = vc->ResolveString();
	int mode = vc->ResolveOperand();
	vc->vcreturn = vc->FileOpen(fname,mode);
}

void vc_FileClose() { vc->FileClose(vc->ResolveOperand()); }

void vc_FileWrite() {
	int handle = vc->ResolveOperand();
	std::string s = vc->ResolveString();
	vc->FileWrite(handle,s);
}

void vc_FileWriteln() {
	int handle = vc->ResolveOperand();
	std::string s = vc->ResolveString();
	vc->FileWriteln(handle,s);
}

void vc_FileReadln() { vc->vcretstr = vc->FileReadln(vc->ResolveOperand()); }
void vc_FileReadToken() { vc->vcretstr = vc->FileReadToken(vc->ResolveOperand()); }

void vc_FileSeekLine() {
	int handle = vc->ResolveOperand();
	int line = vc->ResolveOperand();
	vc->FileSeekLine(handle,line);
}


void vc_FileEOF() { vc->vcreturn = vc->FileEOF(vc->ResolveOperand())?1:0; }

void vc_LoadSound() { vc->vcreturn = vc->LoadSound(vc->ResolveString()); }
void vc_FreeSound() { vc->FreeSound(vc->ResolveOperand()); }

void vc_PlaySound() {
	int slot = vc->ResolveOperand();
	int volume = vc->ResolveOperand();
	vc->vcreturn = vc->PlaySound(slot,volume);
}

void vc_CallFunction()
{
	std::string func = vc->ResolveString();
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

void vc_AssignArray()
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

void vc_FileSeekPos() {
	int handle = vc->ResolveOperand();
	int offset = vc->ResolveOperand();
	int mode = vc->ResolveOperand();
	vc->FileSeekPos(handle,offset,mode);
}

void vc_FileCurrentPos() { vc->vcreturn = vc->FileCurrentPos(vc->ResolveOperand()); }
void vc_FileWriteByte() {
	int handle = vc->ResolveOperand();
	int var = vc->ResolveOperand();
	vc->FileWriteByte(handle,var);
}

void vc_FileWriteWord() {
	int handle = vc->ResolveOperand();
	int var = vc->ResolveOperand();
	vc->FileWriteWord(handle,var);
}

void vc_FileWriteQuad() {
	int handle = vc->ResolveOperand();
	int var = vc->ResolveOperand();
	vc->FileWriteQuad(handle,var);
}

void vc_FileWriteString() {
	int handle = vc->ResolveOperand();
	std::string s = vc->ResolveString();
	vc->FileWriteString(handle,s);
}

void vc_FileReadByte() { vc->vcreturn = vc->FileReadByte(vc->ResolveOperand()); }
void vc_FileReadWord() { vc->vcreturn = vc->FileReadWord(vc->ResolveOperand()); }
void vc_FileReadQuad() { vc->vcreturn = vc->FileReadQuad(vc->ResolveOperand()); }
void vc_FileReadString() { vc->vcretstr = vc->FileReadString(vc->ResolveOperand()); }

void vc_sqrt() { vc->vcreturn = vc->sqrt(vc->ResolveOperand()); }
void vc_pow() {
	int a = vc->ResolveOperand();
	int b = vc->ResolveOperand();
	vc->vcreturn = vc->pow(a,b);
}

void vc_SetAppName() { vc->SetAppName(vc->ResolveString()); }
void vc_SetResolution() { 
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	vc->SetResolution(x,y);
}

void vc_BlitLucent()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int lucent = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->BlitLucent(x,y,lucent,s,d);
}

void vc_TBlitLucent()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int lucent = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->TBlitLucent(x,y,lucent,s,d);
}

void vc_Map()
{
	std::string map = vc->ResolveString();
	vc->Map(map);
}

void vc_strcmp()
{
	std::string s1 = vc->ResolveString();
	std::string s2 = vc->ResolveString();
	vc->vcreturn = vc->Strcmp(s1,s2);
}

void vc_strdup()
{
	std::string s = vc->ResolveString();
	int times = vc->ResolveOperand();
	vc->vcretstr = vc->Strdup(s,times);
}

void vc_HookTimer() { vc->HookTimer(vc->ResolveString()); }
void vc_HookRetrace() { vc->HookRetrace(vc->ResolveString()); }
void vc_HookKey() { 
	int k = vc->ResolveOperand();
	std::string s = vc->ResolveString();
	vc->HookKey(k,s);
}
void vc_HookButton() { 
	int b = vc->ResolveOperand();
	std::string s = vc->ResolveString();
	vc->HookButton(b,s);
}

void vc_HookEntityRender()
{
	int i = vc->ResolveOperand();
	std::string s = vc->ResolveString();
	vc->HookEntityRender(i,s);
}

void vc_BlitTile()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int t = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->BlitTile(x,y,t,d);
}

void vc_TBlitTile()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int t = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->TBlitTile(x,y,t,d);
}

void vc_BlitEntityFrame()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int e = vc->ResolveOperand();
	int f = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->BlitEntityFrame(x,y,e,f,d);
}

void vc_GetTile() {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int i = vc->ResolveOperand();
	vc->vcreturn = vc->GetTile(x,y,i);
}

void vc_SetTile()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int i = vc->ResolveOperand();
	int z = vc->ResolveOperand();
	vc->SetTile(x,y,i,z);
}

void vc_GetZone()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	vc->vcreturn = vc->GetZone(x,y);
}

void vc_SetZone()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int z = vc->ResolveOperand();
	vc->SetZone(x,y,z);
}

void vc_SuperSecretThingy() {
	int xskew = vc->ResolveOperand();
	int yofs = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->SuperSecretThingy(xskew, yofs, y, s, d);
}

void vc_BlitWrap() {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->BlitWrap(x, y, s, d);
}

void vc_ColorFilter() {
	int filter = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->ColorFilter(filter, d);
}

void vc_ImageShell() {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int w = vc->ResolveOperand();
	int h = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	vc->vcreturn = vc->ImageShell(x,y,w,h,s);
}

void vc_Malloc()
{
	int s = vc->ResolveOperand();
	vc->vcreturn = (int) malloc(s);
}

void vc_MemFree()
{
	free((void *) vc->ResolveOperand());
}

void vc_MemCopy()
{
	int src = vc->ResolveOperand();
	int dst = vc->ResolveOperand();
	int len = vc->ResolveOperand();

	memcpy((void*) dst, (void *) src, len);
}


void vc_sin() { vc->vcreturn = vc->sin(vc->ResolveOperand()); }
void vc_cos() { vc->vcreturn = vc->cos(vc->ResolveOperand()); }
void vc_tan() { vc->vcreturn = vc->tan(vc->ResolveOperand()); }
void vc_fsin() { vc->vcreturn = vc->fsin(vc->ResolveOperand()); }
void vc_fcos() { vc->vcreturn = vc->fcos(vc->ResolveOperand()); }
void vc_ftan() { vc->vcreturn = vc->ftan(vc->ResolveOperand()); }

void vc_asin() { vc->vcreturn = vc->asin(vc->ResolveOperand()); }
void vc_fasin() { vc->vcreturn = vc->fasin(vc->ResolveOperand()); }
void vc_acos() { vc->vcreturn = vc->acos(vc->ResolveOperand()); }
void vc_facos() { vc->vcreturn = vc->facos(vc->ResolveOperand()); }
void vc_atan() { vc->vcreturn = vc->atan(vc->ResolveOperand()); }
void vc_fatan() { vc->vcreturn = vc->fatan(vc->ResolveOperand()); }

void vc_AlphaBlit() {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int a = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->AlphaBlit(x, y, s, a, d);
}


void vc_WindowCreate() {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int w = vc->ResolveOperand();
	int h = vc->ResolveOperand();
	std::string s = vc->ResolveString();
	vc->vcreturn = vc->WindowCreate(x,y,w,h,s);
}

void vc_WindowGetImage() { vc->vcreturn = vc->WindowGetImage(vc->ResolveOperand()); }
void vc_WindowClose() { vc->WindowClose(vc->ResolveOperand()); }

void vc_WindowSetSize() {
	int win = vc->ResolveOperand();
	int w = vc->ResolveOperand();
	int h = vc->ResolveOperand();
	vc->WindowSetSize(win,w,h);
}

void vc_WindowSetResolution() {
	int win = vc->ResolveOperand();
	int w = vc->ResolveOperand();
	int h = vc->ResolveOperand();
	vc->WindowSetResolution(win,w,h);
}

void vc_WindowSetPosition() {
	int win = vc->ResolveOperand();
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	vc->WindowSetPosition(win,x,y);
}

void vc_WindowSetTitle() {
	int win = vc->ResolveOperand();
	std::string s = vc->ResolveString();
	vc->WindowSetTitle(win,s);
}

void vc_WindowHide() { vc->WindowHide(vc->ResolveOperand()); }
void vc_WindowShow() { vc->WindowHide(vc->ResolveOperand()); }

void vc_WindowGetXRes() { vc->vcreturn = vc->WindowGetXRes(vc->ResolveOperand()); }
void vc_WindowGetYRes() { vc->vcreturn = vc->WindowGetYRes(vc->ResolveOperand()); }
void vc_WindowGetWidth() { vc->vcreturn = vc->WindowGetWidth(vc->ResolveOperand()); }
void vc_WindowGetHeight() { vc->vcreturn = vc->WindowGetHeight(vc->ResolveOperand()); }

void vc_WindowPositionCommand() {
	int win = vc->ResolveOperand();
	int command = vc->ResolveOperand();
	int arg1 = vc->ResolveOperand();
	int arg2 = vc->ResolveOperand();
	vc->WindowPositionCommand(win,command,arg1,arg2);
}

void vc_SetSongPaused() {
	int h = vc->ResolveOperand();
	int p = vc->ResolveOperand();
	vc->SetSongPaused(h, p);
}

void vc_SetSongVolume() {
	int h = vc->ResolveOperand();
	int v = vc->ResolveOperand();
	vc->SetSongVolume(h, v);
}

void vc_SetSongPos() {
	int h = vc->ResolveOperand();
	int v = vc->ResolveOperand();
	vc->SetSongPos(h, v);
}

void vc_SetMusicVolume() { vc->SetMusicVolume(vc->ResolveOperand()); }


bool isdelim(char c, std::string s);
void vc_TokenCount() {
	std::string s = vc->ResolveString();
	std::string d = vc->ResolveString();
	vc->vcreturn = vc->TokenCount(s,d);
}

void vc_GetToken()
{
	std::string s = vc->ResolveString();
	std::string d = vc->ResolveString();
	int i = vc->ResolveOperand();
	vc->vcretstr = vc->GetToken(s,d,i);
}

void vc_ToLower() { vc->vcretstr = vc->ToLower(vc->ResolveString()); }
void vc_ToUpper() { vc->vcretstr = vc->ToUpper(vc->ResolveString()); }

// Overkill: 2005-12-28
// Thank you, Zip.
void vc_strpos()
{
	std::string sub = vc->ResolveString();
	std::string source = vc->ResolveString();
	int start = vc->ResolveOperand();
	vc->vcreturn = source.find(sub, start);
}

// Overkill: 2005-12-28
// Helper function.
int GetTokenPos(std::string teststr, std::string tokens, int pos, int tok)
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
void vc_GetTokenPos()
{

	std::string teststr = vc->ResolveString();
	std::string tokens = vc->ResolveString();
	int pos = vc->ResolveOperand();
	int tok = vc->ResolveOperand();
	vc->vcreturn = GetTokenPos(teststr, tokens, pos, tok);
}


// Overkill: 2005-12-28
// Thank you, Zip.
void vc_TokenLeft()	// Excludes token.
{
	std::string full = vc->ResolveString();
	std::string tokens = vc->ResolveString();
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
void vc_TokenRight()
{
	std::string full = vc->ResolveString();
	std::string tokens = vc->ResolveString();
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
void vc_strovr()
{
	std::string rep = vc->ResolveString();
	std::string source = vc->ResolveString();
	int offset = vc->ResolveOperand();
	vc->vcretstr = strovr(source, rep, offset);
}

// Overkill: 2005-12-19
// Thank you, Zip.
void vc_WrapText()
// Pass: The font to use, the string to wrap, the length in pixels to fit into
// Return: The passed string with \n characters inserted as breaks
// Assmes: The font is valid, and will overrun if a word is longer than linelen
// Note: Existing breaks will be respected, but adjacent \n characters will be
//     replaced with a single \n so add a space for multiple line breaks
{

	int wt_font = vc->ResolveOperand();
	std::string wt_s = vc->ResolveString();
	int wt_linelen = vc->ResolveOperand();
	vc->vcretstr = ScriptEngine::WrapText(wt_font,wt_s,wt_linelen);
}


void vc_FontHeight() { vc->vcreturn = vc->FontHeight(vc->ResolveOperand()); }

void vc_MixColor() {
	int c1 = vc->ResolveOperand();
	int c2 = vc->ResolveOperand();
	int p = vc->ResolveOperand();
	vc->vcreturn = vc->MixColor(c1,c2,p);
}

void vc_CHR() { vc->vcretstr = vc->Chr(vc->ResolveOperand()); }

void vc_PlayMovie() { vc->vcreturn = vc->PlayMovie(vc->ResolveString()); }
void vc_AbortMovie() { vc->AbortMovie(); }

void vc_MovieLoad() {
	std::string s = vc->ResolveString();
	int mute = vc->ResolveOperand();
	vc->vcreturn = vc->MovieLoad(s,mute!=0);
}

void vc_MoviePlay() {
	int m = vc->ResolveOperand();
	int loop = vc->ResolveOperand();
	vc->MoviePlay(m,loop!=0);
}

void vc_MovieGetImage() {  vc->vcreturn = vc->MovieGetImage(vc->ResolveOperand()); }
void vc_MovieRender() { vc->MovieRender(vc->ResolveOperand()); }
void vc_MovieClose() { vc->MovieClose(vc->ResolveOperand()); }
void vc_MovieGetCurrFrame() { vc->vcreturn = vc->MovieGetCurrFrame(vc->ResolveOperand()); }
void vc_MovieGetFramerate() { vc->vcreturn = vc->MovieGetFramerate(vc->ResolveOperand()); }
void vc_MovieNextFrame() { vc->MovieNextFrame(vc->ResolveOperand()); }
void vc_MovieSetFrame() {
	int m = vc->ResolveOperand();
	int f = vc->ResolveOperand();
	vc->MovieSetFrame(m,f);
}

void vc_GetObsPixel() {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	vc->vcreturn = vc->GetObsPixel(x,y);
}

void vc_GetObs()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	vc->vcreturn = vc->GetObs(x,y);
}

void vc_SetObs()
{
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int c = vc->ResolveOperand();
	vc->SetObs(x,y,c);
}

void vc_EntitySpawn() {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	std::string s = vc->ResolveString();
	vc->vcreturn = vc->EntitySpawn(x,y,s);
}

void vc_SetPlayer() { vc->SetPlayer(vc->ResolveOperand()); }
void vc_GetPlayer() { vc->vcreturn = vc->GetPlayer(); }

void vc_EntityStalk()
{
	int stalker = vc->ResolveOperand();
	int stalkee = vc->ResolveOperand();
	vc->EntityStalk(stalker,stalkee);
}

void vc_EntityMove() {
	int e = vc->ResolveOperand();
	std::string s = vc->ResolveString();
	vc->EntityMove(e,s);
}

void vc_PlayerMove()
{
	std::string s = vc->ResolveString();
	vc->PlayerMove(s);
}

void vc_ChangeCHR() {
	int e = vc->ResolveOperand();
	std::string c = vc->ResolveString();
	vc->ChangeCHR(e,c);
}
void vc_EntitySetWanderZone() { vc->EntitySetWanderZone(vc->ResolveOperand()); }
void vc_EntitySetWanderRect()
{
	int e = vc->ResolveOperand();
	int x1 = vc->ResolveOperand();
	int y1 = vc->ResolveOperand();
	int x2 = vc->ResolveOperand();
	int y2 = vc->ResolveOperand();
	vc->EntitySetWanderRect(e,x1,y1,x2,y2);
}
void vc_EntityStop() { vc->EntityStop(vc->ResolveOperand()); }
void vc_EntitySetWanderDelay()
{
	int e = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->EntitySetWanderDelay(e,d);
}
void vc_SetEntitiesPaused() { vc->SetEntitiesPaused(vc->ResolveOperand()); }

void vc_Render() { vc->Render(); }
void vc_RenderMap() {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int dest = vc->ResolveOperand();
	vc->RenderMap(x,y,dest);
}
void vc_GetSprite() { vc->vcreturn = vc->GetSprite(); }
void vc_ResetSprites() { vc->ResetSprites(); }

void vc_SetButtonKey() { 
	int b = vc->ResolveOperand();
	int k = vc->ResolveOperand();
	vc->SetButtonKey(b,k);
}
void vc_SetButtonJB() { 
	int b = vc->ResolveOperand();
	int jb = vc->ResolveOperand();
	vc->SetButtonJB(b,jb);
}

void vc_FunctionExists()
{
	std::string f = vc->ResolveString();
	vc->vcreturn = vc->FunctionExists(f.c_str());
}

void vc_atan2() {
	int y = vc->ResolveOperand();
	int x = vc->ResolveOperand();
	vc->vcreturn = vc->atan2(y,x);
}

void vc_fatan2()
{
	int y = vc->ResolveOperand();
	int x = vc->ResolveOperand();
	vc->vcreturn = vc->fatan2(y,x);
}

void vc_CopyImageToClipboard() { vc->CopyImageToClipboard(vc->ResolveOperand()); }
void vc_GetImageFromClipboard() { vc->vcreturn = vc->GetImageFromClipboard(); }

void vc_SetInt()
{
	std::string intname = vc->ResolveString();
	int value = vc->ResolveOperand();
	vc->SetInt(intname.c_str(), value);
}

void vc_GetInt()
{
	std::string intname = vc->ResolveString();
	vc->vcreturn = vc->GetInt(intname.c_str());
}

void vc_SetString()
{
	std::string strname = vc->ResolveString();
	std::string value = vc->ResolveString();
	vc->SetStr(strname.c_str(), value);
}

void vc_GetString()
{
	std::string strname = vc->ResolveString();
	vc->vcretstr = vc->GetStr(strname.c_str());
}

void vc_SetIntArray()
{
	std::string intname = vc->ResolveString();
	int index = vc->ResolveOperand();
	int value = vc->ResolveOperand();
	vc->SetIntArray(intname.c_str(), index, value);
}

void vc_GetIntArray()
{
	std::string intname = vc->ResolveString();
	int index = vc->ResolveOperand();
	vc->vcreturn = vc->GetIntArray(intname.c_str(), index);
}

void vc_SetStringArray()
{
	std::string strname = vc->ResolveString();
	int index = vc->ResolveOperand();
	std::string value = vc->ResolveString();
	vc->SetStrArray(strname.c_str(), index, value);
}

void vc_GetStringArray()
{
	std::string strname = vc->ResolveString();
	int index = vc->ResolveOperand();
	vc->vcretstr = vc->GetStrArray(strname.c_str(), index);
}

void vc_FlipBlit() {
	int x = vc->ResolveOperand();
	int y = vc->ResolveOperand();
	int fx = vc->ResolveOperand();
	int fy = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int d = vc->ResolveOperand();
	vc->FlipBlit(x, y, fx!=0, fy!=0, s, d);
}

void vc_Connect() { vc->vcreturn = vc->Connect(vc->ResolveString()); }
void vc_GetConnection() { vc->vcreturn = vc->GetConnection(); }
void vc_SocketConnected() { vc->vcreturn = vc->SocketConnected(vc->ResolveOperand())?1:0; }
void vc_SocketHasData()  { vc->vcreturn = vc->SocketHasData(vc->ResolveOperand())?1:0; }
void vc_SocketGetString() { vc->vcretstr = vc->SocketGetString(vc->ResolveOperand()); }

void vc_SocketSendString() {
	int sh = vc->ResolveOperand();
	std::string str = vc->ResolveString();
	vc->SocketSendString(sh,str);
}

void vc_SocketClose() { vc->SocketClose(vc->ResolveOperand()); }

void vc_SetCustomColorFilter()
{
	int c1 = vc->ResolveOperand();
	int c2 = vc->ResolveOperand();
	vc->SetCustomColorFilter(c1, c2);
}

void vc_SocketSendInt() { 
	int sh = vc->ResolveOperand();
	int i = vc->ResolveOperand();
	vc->SocketSendInt(sh,i);
}

void vc_SocketGetInt() { vc->vcreturn = vc->SocketGetInt(vc->ResolveOperand()); }
void vc_GetUrlText() { vc->vcretstr = vc->GetUrlText(vc->ResolveString()); }
void vc_GetUrlImage() { vc->vcreturn = vc->GetUrlImage(vc->ResolveString()); }

void vc_SocketSendFile() {
	int sh = vc->ResolveOperand();
	std::string fn = vc->ResolveString();
	vc->SocketSendFile(sh,fn);
}

void vc_SocketGetFile() {
	int sh = vc->ResolveOperand();
	std::string override = vc->ResolveString();
	vc->vcretstr = vc->SocketGetFile(sh,override); 
}

void vc_ListFilePattern() { vc->vcretstr = vc->ListFilePattern(vc->ResolveString());}

void vc_ImageValid() { vc->vcreturn = vc->ImageValid(vc->ResolveOperand()); }
void vc_Asc() { vc->vcreturn = vc->Asc(vc->ResolveString()); }

void vc_DictNew() {
	dict *d = new dict();
	vc->vcreturn = HandleForDict(d);
}

void vc_DictFree() {
	int handle = vc->ResolveOperand();
	dict *d = DictForHandle(handle);
	FreeDictHandle(handle);
	delete d;
}

void vc_DictGetString() {
	dict *d = DictForHandle(vc->ResolveOperand());
	vc->vcretstr = d->GetString(vc->ResolveString());
}

void vc_DictSetString() {
	dict *d = DictForHandle(vc->ResolveOperand());
	std::string key = vc->ResolveString();
	std::string value = vc->ResolveString();
	d->SetString(key, value);
}

void vc_DictContainsString() {
	dict *d = DictForHandle(vc->ResolveOperand());
	vc->vcreturn = d->ContainsString(vc->ResolveString());
}

void vc_DictSize() {
	dict *d = DictForHandle(vc->ResolveOperand());
	vc->vcreturn = d->Size();
}

void vc_DictGetInt() {
	dict *d = DictForHandle(vc->ResolveOperand());
	vc->vcreturn = atoi(d->GetString(vc->ResolveString()).c_str());
}

void vc_DictSetInt() {
	dict *d = DictForHandle(vc->ResolveOperand());
	std::string key = vc->ResolveString();
	int value = vc->ResolveOperand();
	d->SetString(key, va("%d", value));
}

void vc_DictRemove() {
	dict *d = DictForHandle(vc->ResolveOperand());
	std::string key = vc->ResolveString();
	d->RemoveString(key);
}

// Overkill: 2007-06-20
void vc_DictListKeys() {
	dict *d = DictForHandle(vc->ResolveOperand());
	std::string separator = vc->ResolveString();
	vc->vcretstr = d->ListKeys(separator);
}

// Overkill: 12/18/05
void vc_max()
{
	int a = vc->ResolveOperand();
	int b = vc->ResolveOperand();
	vc->vcreturn = (a < b ? b : a);
}

// Overkill: 12/18/05
void vc_min()
{
	int a = vc->ResolveOperand();
	int b = vc->ResolveOperand();
	vc->vcreturn = (a > b ? b : a);
}

// Overkill: 12/18/05
void vc_abs()
{
	int a = vc->ResolveOperand();
	vc->vcreturn = (a < 0 ? -a : a);
}

// Overkill: 12/19/05
void vc_sgn()
{
	int a = vc->ResolveOperand();
	vc->vcreturn = (((a) < 0) ? -1 : ((a) > 0) ? 1 : 0);
}

// Overkill 2006-02-04
void vc_RectVGrad()
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
void vc_RectHGrad()
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
void vc_RectRGrad()
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
void vc_Rect4Grad()
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
void vc_cbrt()
{
	vc->vcreturn = (int) (float) pow((float) vc->ResolveOperand(), (float) 1 / 3);
}

// Overkill (2006-06-30): Gets the contents of the key buffer.
// TODO: Implement for other platforms.
void vc_GetKeyBuffer()
{
	#ifdef __WIN32__
		vc->vcretstr = keybuffer;
	#else 
		err("The function GetKeyBuffer() is not defined for this platform.");
	#endif
}

// Overkill (2006-06-30): Clears the contents of the key buffer.
// TODO: Implement for other platforms.
void vc_FlushKeyBuffer()
{
	#ifdef __WIN32__
		FlushKeyBuffer();
	#else 
		err("The function FlushKeyBuffer() is not defined for this platform.");
	#endif
}

// Overkill (2006-06-30): Sets the delay in centiseconds before key repeat.
// TODO: Implement for other platforms.
void vc_SetKeyDelay()
{
	int d = vc->ResolveOperand();
	if (d <= 0)
	{
		d = 0;
	}
	#ifdef __WIN32__
		key_input_delay = d;
	#else 
		err("The function SetKeyDelay() is not defined for this platform.");
	#endif
}
// Overkill (2006-07-20):
// Saves a CHR file, using an open file handle, saving the specified entity.
void vc_FileWriteCHR() {
	int handle = vc->ResolveOperand();
	int ent = vc->ResolveOperand();
	vc->FileWriteCHR(handle,ent);
}

// Overkill (2006-07-20):
// Saves a MAP file, using an open file handle, saving the current map.
void vc_FileWriteMAP() {
	int handle = vc->ResolveOperand();
	vc->FileWriteMAP(handle);
}
// Overkill (2006-07-20):
// Saves a VSP file, using an open file handle, saving the current map's VSP.
void vc_FileWriteVSP() {
	int handle = vc->ResolveOperand();
	vc->FileWriteVSP(handle);
}

// Overkill (2006-07-20):
// Compiles the specified MAP filename.
void vc_CompileMap()
{
	if(releasemode)
	{
		vc->vcerr("vc_CompileMap() - Can't compile map in release mode!");
	}
	std::string filename = vc->ResolveString();
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
}

void vc_ListStructMembers()
{
	std::string structname = vc->ResolveString();
	std::vector<std::string> result = vc->ListStructMembers(structname.c_str());
	vc->vcretstr = "";

	for(std::vector<std::string>::iterator i = result.begin();
		i != result.end();
		i++)
	{
		vc->vcretstr += *i + "|";
	}
}

void vc_CopyArray()
{
	std::string src = vc->ResolveString();
	std::string dest = vc->ResolveString();
	bool result = vc->CopyArray(src.c_str(), dest.c_str());
	vc->vcreturn = result;
}

// Overkill (2006-11-20)
void vc_SoundIsPlaying() { vc->vcreturn = ScriptEngine::SoundIsPlaying(vc->ResolveOperand()); }

// Overkill (2007-05-04)
void vc_GetH()
{
	vc->vcreturn = ScriptEngine::GetH(vc->ResolveOperand());
}

// Overkill (2007-05-04)
void vc_GetS()
{
	vc->vcreturn = ScriptEngine::GetS(vc->ResolveOperand());
}

// Overkill (2007-05-04)
void vc_GetV()
{
	vc->vcreturn = ScriptEngine::GetV(vc->ResolveOperand());
}

// Overkill (2007-05-04)
void vc_HSV()
{
	int h = vc->ResolveOperand();
	int s = vc->ResolveOperand();
	int v = vc->ResolveOperand();
	vc->vcreturn = HSVtoColor(h, s, v);
}

// Overkill (2007-05-04)
void vc_HueReplace()
{
	int hue_find = vc->ResolveOperand();
	int hue_tolerance = vc->ResolveOperand();
	int hue_replace = vc->ResolveOperand();
	int dest = vc->ResolveOperand();
	ScriptEngine::HueReplace(hue_find, hue_tolerance, hue_replace, dest);
}

// Overkill (2007-05-04)
void vc_ColorReplace()
{
	int find = vc->ResolveOperand();
	int replace = vc->ResolveOperand();
	int dest = vc->ResolveOperand();
	ScriptEngine::ColorReplace(find, replace, dest);
}

void vc_ListBuiltinFunctions()
{
	vc->vcretstr = "";
	for (int i = 0; i < NUM_LIBFUNCS; i++)
	{
		vc->vcretstr += std::string(libfuncs[i][1]) + "|";
	}
}

void vc_ListBuiltinVariables()
{
	vc->vcretstr = "";
	for (int i = 0; i < NUM_HVARS; i++)
	{
		vc->vcretstr += std::string(libvars[i][1]) + "|";
	}
}

void vc_ListBuiltinDefines()
{
	vc->vcretstr = "";
	for (int i = 0; i < NUM_HDEFS; i++)
	{
		vc->vcretstr += std::string(hdefs[i][0]) + "|";
	}
}

void vc_GetUserSystemVcFunctionCount()
{
	vc->vcreturn = 0;
	vc->vcreturn = vc->userfuncs[CIMAGE_SYSTEM].size();
}

void vc_GetUserSystemVcFunctionByIndex()
{
	vc->vcretstr = "";
	int index = vc->ResolveOperand();
	int maxSize = vc->userfuncs[CIMAGE_SYSTEM].size();

	if( index < 0 || index > maxSize )
	{
		vc->vcerr("VC Execution error: Invalid offset: (%d).  Valid range: (0-%d)", index, maxSize );
	}

	std::string myString = (vc->userfuncs[CIMAGE_SYSTEM].at(index))->name;
	vc->vcretstr = myString;
}

// Overkill (2008-04-17): Socket port can be switched to something besides 45150.
void vc_SetConnectionPort()
{
	int port = vc->ResolveOperand();
	vc->SetConnectionPort(port);
}

// Overkill (2008-04-17): Sockets can send and receive raw length-delimited strings
void vc_SocketGetRaw()
{
	int sh = vc->ResolveOperand();
	int len = vc->ResolveOperand();
	vc->vcretstr = vc->SocketGetRaw(sh, len);
}

// Overkill (2008-04-17): Sckets can send and receive raw length-delimited strings
void vc_SocketSendRaw()
{
	int sh = vc->ResolveOperand();
	std::string str = vc->ResolveString();
	vc->SocketSendRaw(sh, str);
}

// Overkill (2008-04-20): Peek at how many bytes are in buffer. Requested by ustor.
void vc_SocketByteCount()
{
	int sh = vc->ResolveOperand();
	vc->vcreturn = vc->SocketByteCount(sh);
}

// ===================== End VC Standard Function Library =====================

void VCCore::HandleLibFunc()
{
	// Overkill (2006-06-07): Now functions past 255 work.
	// Yay! We'll probably never reach the 65535 mark,
	// so we're safe again.
	word c = currentvc->GrabW();
	switch (c)
	{
		case 0: vc_Exit(); break;
		case 1: vc_Log(); break;
		case 2: vc_NewImage(); break;
		case 3: vc_MakeColor(); break;
		case 4: vc_SetLucent(); break;
		case 5: vc_SetClip(); break;
		case 6: vc_LoadImage(); break;
		case 7: vc_LoadImage0(); break;
		case 8: vc_ShowPage(); break;
		case 9: vc_UpdateControls(); break;
		case 10: vc_Blit(); break;
		case 11: vc_TBlit(); break;
		case 12: vc_AdditiveBlit(); break;
		case 13: vc_TAdditiveBlit(); break;
		case 14: vc_SubtractiveBlit(); break;
		case 15: vc_TSubtractiveBlit(); break;
		case 16: vc_WrapBlit(); break;
		case 17: vc_TWrapBlit(); break;
		case 18: vc_ScaleBlit(); break;
		case 19: vc_TScaleBlit(); break;
		case 20: vc_RGB(); break;
		case 21: vc_SetPixel(); break;
		case 22: vc_GetPixel(); break;
		case 23: vc_Line(); break;
		case 24: vc_Rect(); break;
		case 25: vc_RectFill(); break;
		case 26: vc_Circle(); break;
		case 27: vc_CircleFill(); break;
		case 28: vc_GetR(); break;
		case 29: vc_GetG(); break;
		case 30: vc_GetB(); break;
		case 31: vc_RotScale(); break;
		case 32: vc_FreeImage(); break;
		case 33: vc->vcreturn = vc->LoadSong(vc->ResolveString()); break;
		case 34: vc->PlaySong(vc->ResolveOperand()); break;
		case 35: vc->StopSong(vc->ResolveOperand()); break;
		case 36: vc->PlayMusic(vc->ResolveString()); break;
		case 37: vc->StopMusic(); break;
		case 38: vc->StopSound(vc->ResolveOperand()); break;
		case 39: vc->FreeSong(vc->ResolveOperand()); break;
		case 40: vc->vcerr("mask"); break;
		case 41: vc_Silhouette(); break;
		case 42: vc_GrabRegion(); break;
		case 43: vc_TGrabRegion(); break;
		case 44: vc_Mosaic(); break;
		case 45: vc_DuplicateImage(); break;
		case 46: vc_Triangle(); break;
		case 47: vc_BlitTile(); break;
		case 48: vc_TBlitTile(); break;
		case 49: vc->vcerr("horzflip"); break;
		case 50: vc->vcerr("vertflip"); break;
		case 51: vc_ImageWidth(); break;
		case 52: vc_ImageHeight(); break;
		case 53: vc_LoadFont(); break;
		case 54: vc_EnableVariableWidth(); break;
		case 55: vc_PrintString(); break;
		case 56: vc_PrintRight(); break;
		case 57: vc_PrintCenter(); break;
		case 58: vc_TextWidth(); break;
		case 59: vc_FreeFont(); break;
		case 60: vc_Random(); break;
		case 61: vc_len(); break;
		case 62: vc_val(); break;
		case 63: vc_Unpress(); break;
		case 64: DebugBreakpoint("User breakpoint"); break;
		case 65: vc_FileOpen(); break;
		case 66: vc_FileClose(); break;
		case 67: vc_FileWrite(); break;
		case 68: vc_FileWriteln(); break;
		case 69: vc_FileReadln(); break;
		case 70: vc_FileReadToken(); break;
		case 71: vc_FileSeekLine(); break;
		case 72: vc_LoadSound(); break;
		case 73: vc_FreeSound(); break;
		case 74: vc_PlaySound(); break;
		case 75: vc_CallFunction(); break;
		case 76: vc_AssignArray(); break;
		case 77: vc_FileSeekPos(); break;
		case 78: vc_FileCurrentPos(); break;
		case 79: vc_FileWriteByte(); break;
		case 80: vc_FileWriteWord(); break;
		case 81: vc_FileWriteQuad(); break;
		case 82: vc_FileWriteString(); break;
		case 83: vc_FileReadByte(); break;
		case 84: vc_FileReadWord(); break;
		case 85: vc_FileReadQuad(); break;
		case 86: vc_FileReadString(); break;
		case 87: vc_sqrt(); break;
		case 88: vc_pow(); break;
		case 89: vc_SetAppName(); break;
		case 90: vc_SetResolution(); break;
		case 91: vc_BlitLucent(); break;
		case 92: vc_TBlitLucent(); break;
		case 93: vc_Map(); break;
		case 94: vc_strcmp(); break;
		case 95: vc_strdup(); break;
		case 96: vc_HookTimer(); break;
		case 97: vc_HookRetrace(); break;
		case 98: vc_HookEntityRender(); break;
		case 99: vc_HookKey(); break;
		case 100: vc_HookButton(); break;
		case 101: vc_BlitEntityFrame(); break;
		case 102: vc_SetEntitiesPaused(); break;
		case 103: vc_GetObsPixel(); break;
		case 104: vc_GetTile(); break;
		case 105: vc_SetTile(); break;
		case 106: vc_GetZone(); break;
		case 107: vc_SetZone(); break;
		case 108: showMessageBox(vc->ResolveString()); break;
		case 109: vc_sin(); break;
		case 110: vc_cos(); break;
		case 111: vc_tan(); break;
		case 112: vc_SuperSecretThingy(); break;
		case 113: vc_BlitWrap(); break;
		case 114: vc_ColorFilter(); break;
		case 115: vc_ImageShell(); break;
		case 116: vc_Malloc(); break;
		case 117: vc_MemFree(); break;
		case 118: vc_MemCopy(); break;
		case 119: vc_asin(); break;
		case 120: vc_acos(); break;
		case 121: vc_atan(); break;
		case 122: vc_AlphaBlit(); break;
		case 123: vc_WindowCreate(); break;
		case 124: vc_WindowGetImage(); break;
		case 125: vc_WindowClose(); break;
		case 126: vc_WindowSetSize(); break;
		case 127: vc_WindowSetResolution(); break;
		case 128: vc_WindowSetPosition(); break;
		case 129: vc_WindowSetTitle(); break;
		case 130: vc_WindowHide(); break;
		case 131: vc_WindowShow(); break;
		case 132: vc_WindowGetXRes(); break;
		case 133: vc_WindowGetYRes(); break;
		case 134: vc_WindowGetWidth(); break;
		case 135: vc_WindowGetHeight(); break;
		case 136: vc_WindowPositionCommand(); break;
		case 137: vc_SetSongPaused(); break;
		case 138: vc_SetSongVolume(); break;
		case 139: vc->vcreturn = vc->GetSongVolume(vc->ResolveOperand()); break;
		case 140: vc->vcreturn = vc->GetSongPos(vc->ResolveOperand()); break;
		case 141: vc_SetSongPos(); break;
		case 142: vc_TokenCount(); break;
		case 143: vc_GetToken(); break;
		case 144: vc_ToUpper(); break;
		case 145: vc_ToLower(); break;
		case 146: vc_LoadFontEx(); break;
		case 147: vc_FontHeight(); break;
		case 148: vc_MixColor(); break;
		case 149: vc_CHR(); break;
		case 150: vc_PlayMovie(); break;
		case 151: vc_MovieLoad(); break;
		case 152: vc_MoviePlay(); break;
		case 153: vc_MovieGetImage(); break;
		case 154: vc_MovieRender(); break;
		case 155: vc_MovieClose(); break;
		case 156: vc_MovieGetCurrFrame(); break;
		case 157: vc_MovieGetFramerate(); break;
		case 158: vc_MovieNextFrame(); break;
		case 159: vc_MovieSetFrame(); break;
		case 160: vc_Render(); break;
		case 161: vc_GetObs(); break;
		case 162: vc_SetObs(); break;
		case 163: vc_EntitySpawn(); break;
		case 164: vc_SetPlayer(); break;
		case 165: vc_EntityStalk(); break;
		case 166: vc_EntityMove(); break;
		case 167: vc_SetMusicVolume(); break;
		case 168: vc_PlayerMove(); break;
		case 169: vc_ChangeCHR(); break;
		case 170: vc_EntitySetWanderZone(); break;
		case 171: vc_EntitySetWanderRect(); break;
		case 172: vc_EntityStop(); break;
		case 173: vc_EntitySetWanderDelay(); break;
		case 174: vc->SetRandSeed(vc->ResolveOperand()); break;
		case 175: vc_ResetSprites(); break;
		case 176: vc_GetSprite(); break;
		case 177: vc_RenderMap(); break;
		case 178: vc_SetButtonKey(); break;
		case 179: vc_SetButtonJB(); break;
		case 180: vc_FunctionExists(); break;
		case 181: vc_atan2(); break;
		case 182: vc_CopyImageToClipboard(); break;
		case 183: vc_GetImageFromClipboard(); break;
		case 184: vc_SetInt(); break;
		case 185: vc_GetInt(); break;
		case 186: vc_SetString(); break;
		case 187: vc_GetString(); break;
		case 188: vc_SetIntArray(); break;
		case 189: vc_GetIntArray(); break;
		case 190: vc_SetStringArray(); break;
		case 191: vc_GetStringArray(); break;
		case 192: vc_FlipBlit(); break;
		case 193: vc_Connect(); break;
		case 194: vc_GetConnection(); break;
		case 195: vc_SocketConnected(); break;
		case 196: vc_SocketHasData(); break;
		case 197: vc_SocketGetString(); break;
		case 198: vc_SocketSendString(); break;
		case 199: vc_SocketClose(); break;
		case 200: Sleep(vc->ResolveOperand()); break;
		case 201: vc_SetCustomColorFilter(); break;
		case 202: vc_SocketSendInt(); break;
		case 203: vc_SocketGetInt(); break;
		case 204: // sendfloat
		case 205: // getfloat
		case 206: vc_GetUrlText(); break;
		case 207: vc_GetUrlImage(); break;
		case 208: vc_SocketSendFile(); break;
		case 209: vc_SocketGetFile(); break;
		case 210: vc_ListFilePattern(); break;
		case 211: vc_ImageValid(); break;
		case 212: vc_Asc(); break;
		case 213: vc_FileEOF(); break;
		case 214: vc_DictNew(); break;
		case 215: vc_DictFree(); break;
		case 216: vc_DictGetString(); break;
		case 217: vc_DictSetString(); break;
		case 218: vc_DictContainsString(); break;
		case 219: vc_DictSize(); break;
		case 220: vc_DictGetInt(); break;
		case 221: vc_DictSetInt(); break;
		case 222: vc_DictRemove(); break;
		case 223: vc_LoadImage8(); break;
		case 224: vc_AbortMovie(); break; //mbg 12/11/05
		case 225: vc_max(); break; // Overkill: 12/28/05
		case 226: vc_min(); break; // Overkill: 12/28/05
		case 227: vc_abs(); break; // Overkill: 12/28/05
		case 228: vc_sgn(); break; // Overkill: 12/28/05
		case 229: vc_GetTokenPos(); break; // Overkill: 12/28/05
		case 230: vc_TokenLeft(); break; // Overkill: 12/28/05
		case 231: vc_TokenRight(); break; // Overkill: 12/28/05
		case 232: vc_strpos(); break; // Overkill: 12/28/05
		case 233: vc_strovr(); break; // Overkill: 12/28/05
		case 234: vc_WrapText(); break; // Overkill: 12/28/05
		case 235: vc_RectVGrad(); break; // Overkill: 02/04/06, 06/25/06
		case 236: vc_RectHGrad(); break; // Overkill: 02/04/06, 06/25/06
		case 237: vc_cbrt(); break; // Overkill: 06/25/06
		case 238: vc_GetKeyBuffer(); break; // Overkill: 06/29/06
		case 239: vc_FlushKeyBuffer(); break; // Overkill: 06/29/06
		case 240: vc_SetKeyDelay(); break; // Overkill: 06/30/06
		case 241: vc_FileWriteCHR(); break; // Overkill: 05/20/06
		case 242: vc_FileWriteMAP(); break; // Overkill: 05/21/06
		case 243: vc_FileWriteVSP(); break; // Overkill: 05/21/06
		case 244: vc_CompileMap(); break; // Overkill: 05/21/06
		case 245: vc_RectRGrad(); break; // janus: 07/22/06
		case 246: vc_Rect4Grad(); break; // janus: 07/22/06
		case 247: vc_ListStructMembers(); break; // Overkill (2006-08-16)
		case 248: vc_CopyArray(); break; // Overkill (2006-08-16)
		case 249: vc_SoundIsPlaying(); break; // Overkill (2006-11-20)
		case 250: vc_GetH(); break; // Overkill: (2007-05-04)
		case 251: vc_GetS(); break; // Overkill: (2007-05-04)
		case 252: vc_GetV(); break; // Overkill: (2007-05-04)
		case 253: vc_HSV(); break; // Overkill: (2007-05-04)
		case 254: vc_HueReplace(); break; // Overkill: (2007-05-04)
		case 255: vc_ColorReplace(); break; // Overkill: (2007-05-04)
		case 256: vc_fatan(); break;
		case 257: vc_fatan2(); break;
		case 258: vc_fasin(); break;
		case 259: vc_facos(); break;
		case 260: vc_fsin(); break;
		case 261: vc_fcos(); break;
		case 262: vc_ftan(); break;
		case 263: vc_SetCharacterWidth(); break; //mbg (2007-06-18)
		case 264: vc_DictListKeys(); break; // Overkill: (2007-06-20)
		case 265: vc_ListBuiltinFunctions(); break;
		case 266: vc_ListBuiltinVariables(); break;
		case 267: vc_ListBuiltinDefines(); break;
		case 268: vc_GetPlayer(); break; // Kildorf (2007-10-13)
		case 269: vc_GetUserSystemVcFunctionCount(); break; // grue 
		case 270: vc_GetUserSystemVcFunctionByIndex(); break; //grue
		case 271: vc_SetConnectionPort(); break; // Overkill (2008-04-17)
		case 272: vc_SocketGetRaw(); break; // Overkill (2008-04-17)
		case 273: vc_SocketSendRaw(); break; // Overkill (2008-04-17)
		case 274: vc_SocketByteCount(); break; // Overkill (2008-04-20)
		default:
			vc->vcerr("VC Execution error: Invalid STDLIB index. (%d)", (int) c);
	}
}

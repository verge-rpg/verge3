#ifndef _G_SCRIPT
#define _G_SCRIPT

void InitScriptEngine();
void HookTimer();
void HookRetrace();
image *ImageForHandle(int handle);
void FreeImageHandle(int handle);
void SetHandleImage(int handle, image *img);
int HandleForImage(image *img);

class ScriptEngine;
extern ScriptEngine *se;

class MapScriptCompiler {
public:
	virtual bool CompileMap(const char *fname) = 0;
};

struct argument_t
{
	byte type_id;
	int int_value;
	StringRef string_value;
};

class ScriptEngine {
public:
	int vcreturn;
	StringRef vcretstr;

	void Error(const char *s, ...);

	virtual bool ExecuteFunctionString(CStringRef script) = 0;
	virtual bool FunctionExists(CStringRef func) = 0;
	virtual void ExecAutoexec() = 0;
	virtual void LoadMapScript(VFILE *f, CStringRef filename) = 0;
	virtual void DisplayError(CStringRef msg) = 0;
	virtual int ResolveOperand() = 0;
	virtual StringRef ResolveString() = 0;
	virtual bool CheckForVarargs() = 0;
	virtual void ReadVararg(std::vector<argument_t>& vararg) = 0;
	void vcerr(char *str, ...) {
	  	va_list argptr;
		char msg[256];

		va_start(argptr, str);
		vsprintf(msg, str, argptr);
		va_end(argptr);
		Error("%s",msg);
	}

	// Varargs support!
	std::vector<argument_t> argument_pass_list;

	void ArgumentPassAddInt(int value);
	void ArgumentPassAddString(StringRef value);
	void ArgumentPassClear();

	//script services
	//VI.a. General Utility Functions
	//TODO(CallFunction);
	static void Exit(CStringRef message);
	//TODO(FunctionExists);
	//TODO(GetInt);
	//TODO(GetIntArray);
	//TODO(GetString);
	//TODO(GetStringArray);
	static void HookButton(int b, CStringRef s);
	static void HookKey(int k, CStringRef s);
	static void HookTimer(CStringRef s);
	static void HookRetrace(CStringRef s);
	static void Log(CStringRef s);
	static void MessageBox(CStringRef msg);
	static int Random(int min, int max);
	static void SetAppName(CStringRef s);
	static void SetButtonJB(int b, int jb);
	static void SetButtonKey(int b, int k);
	//TODO(SetInt);
	//TODO(SetIntArray);
	static void SetRandSeed(int seed);
	static void SetResolution(int v3_xres, int v3_yres);
	//TODO(SetString);
	//TODO(SetStringArray);
	static void Unpress(int n);
	static void UpdateControls();
	//VI.b. String Functions
	static int Asc(CStringRef s);
	static StringRef Chr(int c);
	static StringRef GetToken(CStringRef s, CStringRef d, int i);
	static StringRef Left(CStringRef str, int len);
	static int Len(CStringRef s);
	static StringRef Mid(CStringRef str, int pos, int len);
	static StringRef Right(CStringRef str, int len);
	static StringRef Str(int d);
	static int Strcmp(CStringRef s1, CStringRef s2);
	static StringRef Strdup(CStringRef s, int times);
	static int TokenCount(CStringRef s, CStringRef d);
	static StringRef ToLower(CStringRef str);
	static StringRef ToUpper(CStringRef str);
	static int Val(CStringRef s);
	//VI.c. Dictionary Functions
	//TODO(DictContains);
	//TODO(DictFree);
	//TODO(DictGetInt);
	//TODO(DictGetString);
	//TODO(DictNew);
	//TODO(DictRemove);
	//TODO(DictSetInt);
	//TODO(DictSetString);
	//TODO(DictSize);
	//VI.d. Map Functions
	static int GetObs(int x, int y);
	static int GetObsPixel(int x, int y);
	static int GetTile(int x, int y, int i);
	static int GetZone(int x, int y);
	static void Map(CStringRef map);
	static void Render();
	static void RenderMap(int x, int y, int d);
	static void SetObs(int x, int y, int c);
	static void SetTile(int x, int y, int i, int z);
	static void SetZone(int x, int y, int z);
	//VI.e. Entity Functions
	static void ChangeCHR(int e, CStringRef c);
	static void EntityMove(int e, CStringRef s);
	static void EntitySetWanderDelay(int e, int d);
	static void EntitySetWanderRect(int e, int x1, int y1, int x2, int y2);
	static void EntitySetWanderZone(int e);
	static int EntitySpawn(int x, int y, CStringRef s);
	static void EntityStalk(int stalker, int stalkee);
	static void EntityStop(int e);
	static void HookEntityRender(int i, CStringRef s);
	static void PlayerMove(CStringRef s);
	static void PlayerEntityMoveCleanup();
	static void SetEntitiesPaused(int i);
	static void SetPlayer(int e);
	static int GetPlayer();
	//VI.f. Graphics Functions
	static void AdditiveBlit(int x, int y, int src, int dst);
	static void AlphaBlit(int x, int y, int src, int alpha, int dst);
	static void Blit(int x, int y, int src, int dst);
	static void BlitEntityFrame(int x, int y, int e, int f, int dst);
	static void BlitLucent(int x, int y, int lucent, int src, int dst);
	static void BlitTile(int x, int y, int t, int dst);
	static void BlitWrap(int x, int y, int src, int dst);
	static void Circle(int x1, int y1, int xr, int yr, int c, int dst);
	static void CircleFill(int x1, int y1, int xr, int yr, int c, int dst);
	static void ColorFilter(int filter, int dst);
	static void CopyImageToClipboard(int s);
	static int DuplicateImage(int s);
	static void FlipBlit(int x, int y, bool fx, bool fy, int src, int dst);
	static void FreeImage(int handle);
	static int GetB(int c);
	static int GetG(int c);
	static int GetImageFromClipboard();
	static int GetPixel(int x, int y, int src);
	static int GetR(int c);
	static void GrabRegion(int sx1, int sy1, int sx2, int sy2, int dx, int dy, int src, int dst);
	static int ImageHeight(int src);
	static int ImageShell(int x, int y, int w, int h, int src);
	static int ImageValid(int handle);
	static int ImageWidth(int src);
	static void Line(int x1, int y1, int x2, int y2, int c, int dst);
	static int LoadImage(CStringRef fn);
	static int LoadImage0(CStringRef fn);
	static int LoadImage8(CStringRef fn);
	static int MakeColor(int r, int g, int b);
	static int MixColor(int c1, int c2, int p);
	static void Mosaic(int xgran, int ygran, int dst);
	static int NewImage(int xsize, int ysize);
	static void Rect(int x1, int y1, int x2, int y2, int c, int dst);
	static void RectFill(int x1, int y1, int x2, int y2, int c, int dst);
	static int rgb(int r, int g, int b) { return MakeColor(r,g,b); }
	static void RotScale(int x, int y, int angle, int scale, int src, int dst);
	static void ScaleBlit(int x, int y, int dw, int dh, int src, int dst);
	static void SetClip(int x1, int y1, int x2, int y2, int img);
	static void SetCustomColorFilter(int c1, int c2);
	static void SetLucent(int p);
	static void SetPixel(int x, int y, int c, int dst);
	static void ShowPage();
	static void Silhouette(int x, int y, int c, int src, int dst);
	static void SubtractiveBlit(int x, int y, int src, int dst);
	static void SuperSecretThingy(int xskew, int yofs, int y, int src, int dst);
	static void TAdditiveBlit(int x, int y, int src ,int dst);
	static void TBlit(int x, int y, int src, int dst);
	static void TBlitLucent(int x, int y, int lucent, int src, int dst);
	static void TBlitTile(int x, int y, int t, int dst);
	static void TGrabRegion(int sx1, int sy1, int sx2, int sy2, int dx, int dy, int src, int dst);
	static void Triangle(int x1, int y1, int x2, int y2, int x3, int y3, int c, int dst);
	static void TScaleBlit(int x, int y, int dw, int dh, int src, int dst);
	static void TSubtractiveBlit(int x, int y, int src, int dst);
	static void TWrapBlit(int x, int y, int src, int dst);
	static void WrapBlit(int x, int y, int src, int dst);
	//VI.g. Sprite Functions
	static int GetSprite();
	static void ResetSprites();
	//VI.h. Sound/Music Functions
	static void FreeSong(int handle);
	static void FreeSound(int slot);
	static int GetSongPos(int handle);
	static int GetSongVolume(int handle);
	static int LoadSong(CStringRef fn);
	static int LoadSound(CStringRef fn);
	static void PlayMusic(CStringRef fn);
	static void PlaySong(int handle);
	static int PlaySound(int slot, int volume);
	static void SetMusicVolume(int v);
	static void SetSongPaused(int h, int p);
	static void SetSongPos(int h, int p);
	static void SetSongVolume(int h, int v);
	static void StopMusic();
	static void StopSong(int handle);
	static void StopSound(int chan);
	//VI.i. Font Functions
	static void EnableVariableWidth(int fh);
	static int FontHeight(int f);
	static void FreeFont(int f);
	static int LoadFont(CStringRef filename, int width, int height);
	static int LoadFontEx(CStringRef filename);
	static void PrintCenter(int x, int y, int d, int fh, CStringRef text);
	static void PrintRight(int x, int y, int d, int fh, CStringRef text);
	static void PrintString(int x, int y, int d, int fh, CStringRef text);
	static int TextWidth(int fh, CStringRef text);
	//VI.j. Math Functions
	static int acos(int val);
	static int facos(int val);
	static int asin(int val);
	static int fasin(int val);
	static int atan(int val);
	static int fatan(int val);
	static int atan2(int y, int x);
	static int fatan2(int y, int x);
	static int cos(int val);
	static int fcos(int val);
	static int pow(int a, int b);
	static int sin(int val);
	static int fsin(int val);
	static int sqrt(int val);
	static int tan(int val);
	static int ftan(int val);
	//VI.k. File Functions
	static void FileClose(int handle);
	static int FileCurrentPos(int handle);
	static bool FileEOF(int handle);
	static int FileOpen(CStringRef fname, int filemode);
	static int FileReadByte(int handle);
	static StringRef FileReadln(int handle);
	static int FileReadQuad(int handle);
	static StringRef FileReadString(int handle);
	static StringRef FileReadToken(int handle);
	static int FileReadWord(int handle);
	static void FileSeekLine(int handle, int line);
	static void FileSeekPos(int handle, int offset, int mode);
	static void FileWrite(int handle, CStringRef s);
	static void FileWriteByte(int handle, int var);
	static void FileWriteln(int handle, CStringRef s);
	static void FileWriteQuad(int handle, int var);
	static void FileWriteString(int handle, CStringRef s);
	static void FileWriteWord(int handle, int var);
	static StringRef ListFilePattern(CStringRef pattern);
	static void FileWriteCHR(int handle, int ent);
	static void FileWriteMAP(int handle);
	static void FileWriteVSP(int handle);
	//VI.l. Window Managment Functions
	static void WindowClose(int win);
	static int WindowCreate(int x, int y, int w, int h, CStringRef s);
	static int WindowGetHeight(int win);
	static int WindowGetImage(int win);
	static int WindowGetWidth(int win);
	static int WindowGetXRes(int win);
	static int WindowGetYRes(int win);
	static void WindowHide(int win);
	static void WindowPositionCommand(int win, int command, int arg1, int arg2);
	static void WindowSetPosition(int win, int x, int y);
	static void WindowSetResolution(int win, int x, int y);
	static void WindowSetSize(int win, int x, int y);
	static void WindowSetTitle(int win, CStringRef s);
	static void WindowShow(int win);
	//VI.m. Movie Playback Functions
	static void AbortMovie();
	static void MovieClose(int m);
	static int MovieGetCurrFrame(int m);
	static int MovieGetFramerate(int m);
	static int MovieGetImage(int m);
	static int MovieLoad(CStringRef s, bool mute);
	static void MovieNextFrame(int m);
	static void MoviePlay(int m, bool loop);
	static void MovieRender(int m);
	static void MovieSetFrame(int m, int f);
	static int PlayMovie(CStringRef s);
	//VI.n. Netcode Functions
	static void SetConnectionPort(int port);
	static int Connect(CStringRef ip);
	static int GetConnection();
	static int GetUrlImage(CStringRef url);
	static StringRef GetUrlText(CStringRef url);
	static void SocketClose(int sh);
	static bool SocketConnected(int sh);
	static StringRef SocketGetFile(int sh, CStringRef override);
	static int SocketGetInt(int sh);
	static StringRef SocketGetString(int sh);
	static bool SocketHasData(int sh);
	static void SocketSendFile(int sh, CStringRef fn);
	static void SocketSendInt(int sh, int i);
	static void SocketSendString(int sh, CStringRef str);
	static StringRef SocketGetRaw(int sh, int len);
	static void SocketSendRaw(int sh, CStringRef str);
	static int SocketByteCount(int sh); // Overkill (2008-04-20): Peek at how many bytes are in buffer. Requested by ustor.
	//XX: unsorted functions and variables, mostly newly added and undocumented
	static CStringRef Get_EntityChr(int arg);
	static void Set_EntityChr(int arg, CStringRef chr);
	static int Get_EntityFrameW(int ofs);
	static int Get_EntityFrameH(int ofs);
	static CStringRef Get_EntityDescription(int arg);
	static void Set_EntityDescription(int arg, CStringRef val);
	static void Set_EntityActivateScript(int arg, CStringRef val);
	static bool SoundIsPlaying(int chn);
	static void RectVGrad(int x1, int y1, int x2, int y2, int c, int c2, int d);
	static void RectHGrad(int x1, int y1, int x2, int y2, int c, int c2, int d);
	static void RectRGrad(int x1, int y1, int x2, int y2, int c, int c2, int d);
	static void Rect4Grad(int x1, int y1, int x2, int y2, int c1, int c2, int c3, int c4, int d);
	static StringRef strovr(CStringRef rep, CStringRef source, int offset);
	static StringRef WrapText(int wt_font, CStringRef wt_s, int wt_linelen);
	static int strpos(CStringRef sub, CStringRef source, int start);
	static int HSV(int h, int s, int v);
	static int GetH(int col);
	static int GetS(int col);
	static int GetV(int col);
	static void HueReplace(int hue_find, int hue_tolerance, int hue_replace, int image);
	static void ColorReplace(int find, int replace, int image);
	static StringRef GetKeyBuffer();
	static void FlushKeyBuffer();
	static void SetKeyDelay(int d);
};



#endif
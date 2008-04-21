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

class ScriptEngine {
public:
	void Error(const char *s, ...);

	virtual bool ExecuteFunctionString(const std::string &script) = 0;
	virtual bool FunctionExists(const std::string &func) = 0;
	virtual void ExecAutoexec() = 0;
	virtual void LoadMapScript(VFILE *f) = 0;
	virtual void DisplayError(const std::string &msg) = 0;

	//script services
	//VI.a. General Utility Functions
	//TODO(CallFunction);
	static void Exit(std::string message);
	//TODO(FunctionExists);
	//TODO(GetInt);
	//TODO(GetIntArray);
	//TODO(GetString);
	//TODO(GetStringArray);
	static void HookButton(int b, std::string s);
	static void HookKey(int k, std::string s);
	static void HookTimer(std::string s);
	static void HookRetrace(std::string s);
	static void Log(std::string s);
	static void MessageBox(std::string msg);
	static int Random(int min, int max);
	static void SetAppName(std::string s);
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
	static int Asc(std::string s);
	static std::string Chr(int c);
	static std::string GetToken(std::string s, std::string d, int i);
	static std::string Left(std::string str, int len);
	static int Len(std::string s);
	static std::string Mid(std::string str, int pos, int len);
	static std::string Right(std::string str, int len);
	static std::string Str(int d);
	static int Strcmp(std::string s1, std::string s2);
	static std::string Strdup(std::string s, int times);
	static int TokenCount(std::string s, std::string d);
	static std::string ToLower(std::string str);
	static std::string ToUpper(std::string str);
	static int Val(std::string s);
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
	static void Map(std::string map);
	static void Render();
	static void RenderMap(int x, int y, int d);
	static void SetObs(int x, int y, int c);
	static void SetTile(int x, int y, int i, int z);
	static void SetZone(int x, int y, int z);
	//VI.e. Entity Functions
	static void ChangeCHR(int e, std::string c);
	static void EntityMove(int e, std::string s);
	static void EntitySetWanderDelay(int e, int d);
	static void EntitySetWanderRect(int e, int x1, int y1, int x2, int y2);
	static void EntitySetWanderZone(int e);
	static int EntitySpawn(int x, int y, std::string s);
	static void EntityStalk(int stalker, int stalkee);
	static void EntityStop(int e);
	static void HookEntityRender(int i, std::string s);
	static void PlayerMove(std::string s);
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
	static int LoadImage(std::string fn);
	static int LoadImage0(std::string fn);
	static int LoadImage8(std::string fn);
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
	static int LoadSong(std::string fn);
	static int LoadSound(std::string fn);
	static void PlayMusic(std::string fn);
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
	static int LoadFont(std::string filename, int width, int height);
	static int LoadFontEx(std::string filename);
	static void PrintCenter(int x, int y, int d, int fh, std::string text);
	static void PrintRight(int x, int y, int d, int fh, std::string text);
	static void PrintString(int x, int y, int d, int fh, std::string text);
	static int TextWidth(int fh, std::string text);
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
	static int FileOpen(std::string fname, int filemode);
	static int FileReadByte(int handle);
	static std::string FileReadln(int handle);
	static int FileReadQuad(int handle);
	static std::string FileReadString(int handle);
	static std::string FileReadToken(int handle);
	static int FileReadWord(int handle);
	static void FileSeekLine(int handle, int line);
	static void FileSeekPos(int handle, int offset, int mode);
	static void FileWrite(int handle, std::string s);
	static void FileWriteByte(int handle, int var);
	static void FileWriteln(int handle, std::string s);
	static void FileWriteQuad(int handle, int var);
	static void FileWriteString(int handle, std::string s);
	static void FileWriteWord(int handle, int var);
	static std::string ListFilePattern(std::string pattern);
	static void FileWriteCHR(int handle, int ent);
	static void FileWriteMAP(int handle);
	static void FileWriteVSP(int handle);
	//VI.l. Window Managment Functions
	static void WindowClose(int win);
	static int WindowCreate(int x, int y, int w, int h, std::string s);
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
	static void WindowSetTitle(int win, std::string s);
	static void WindowShow(int win);
	//VI.m. Movie Playback Functions
	static void AbortMovie();
	static void MovieClose(int m);
	static int MovieGetCurrFrame(int m);
	static int MovieGetFramerate(int m);
	static int MovieGetImage(int m);
	static int MovieLoad(std::string s, bool mute);
	static void MovieNextFrame(int m);
	static void MoviePlay(int m, bool loop);
	static void MovieRender(int m);
	static void MovieSetFrame(int m, int f);
	static int PlayMovie(std::string s);
	//VI.n. Netcode Functions
	static void SetConnectionPort(int port);
	static int Connect(std::string ip);
	static int GetConnection();
	static int GetUrlImage(std::string url);
	static std::string GetUrlText(std::string url);
	static void SocketClose(int sh);
	static bool SocketConnected(int sh);
	static std::string SocketGetFile(int sh, std::string override);
	static int SocketGetInt(int sh);
	static std::string SocketGetString(int sh);
	static bool SocketHasData(int sh);
	static void SocketSendFile(int sh, std::string fn);
	static void SocketSendInt(int sh, int i);
	static void SocketSendString(int sh, std::string str);
	static std::string SocketGetRaw(int sh, int len);
	static void SocketSendRaw(int sh, std::string str);
	static int SocketByteCount(int sh); // Overkill (2008-04-20): Peek at how many bytes are in buffer. Requested by ustor.
	//XX: unsorted functions and variables, mostly newly added and undocumented
	static std::string Get_EntityChr(int arg);
	static void Set_EntityChr(int arg, std::string chr);
	static int Get_EntityFrameW(int ofs);
	static int Get_EntityFrameH(int ofs);
	static std::string Get_EntityDescription(int arg);
	static void Set_EntityDescription(int arg, std::string val);
	static void Set_EntityActivateScript(int arg, std::string val);
	static bool SoundIsPlaying(int chn);
	static void RectVGrad(int x1, int y1, int x2, int y2, int c, int c2, int d);
	static void RectHGrad(int x1, int y1, int x2, int y2, int c, int c2, int d);
	static void RectRGrad(int x1, int y1, int x2, int y2, int c, int c2, int d);
	static void Rect4Grad(int x1, int y1, int x2, int y2, int c1, int c2, int c3, int c4, int d);
	static std::string strovr(std::string rep, std::string source, int offset);
	static std::string WrapText(int wt_font, std::string wt_s, int wt_linelen);
	static int strpos(std::string sub, std::string source, int start);
	static int HSV(int h, int s, int v);
	static int GetH(int col);
	static int GetS(int col);
	static int GetV(int col);
	static void HueReplace(int hue_find, int hue_tolerance, int hue_replace, int image);
	static void ColorReplace(int find, int replace, int image);
};



#endif
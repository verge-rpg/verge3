extern char playingsng[];
extern bool UseSound;
extern FMUSIC_MODULE *curmod;
extern FSOUND_STREAM *curstream;

void InitSound();

// Dummy music interface
void PlayMusic(char *sng);
void StopMusic();
void SetMusicVolume(int v);

// Sound effect interface
FSOUND_SAMPLE* LoadSample(char *fn);
void PlaySample(FSOUND_SAMPLE* s, int vol);
void StopSound(int chan);
void FreeSample(FSOUND_SAMPLE* s);

int  LoadSong(char *fn);
void PlaySong(int h);
void StopSong(int h);
void SetPaused(int h, int p);
int  GetSongPos(int h);
void SetSongPos(int h, int p);
void SetSongVol(int h, int v);
int  GetSongVol(int h);
void FreeSong(int h);

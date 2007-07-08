/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


class SoundEngine_Fmod : public SoundEngine {
public:
	bool init();
	void shutdown();

	void PlayMusic(const char *sng);
	void StopMusic();
	void SetMusicVolume(int v);

	void *LoadSample(const char *fn);
	int PlaySample(void *sample, int vol);
	void StopSound(int chan);
	int SoundIsPlaying(int chan); // Overkill (2006-11-20)
	void FreeSample(void *sample);


	int LoadSong(const char *fn);
	void PlaySong(int h);
	void StopSong(int h);
	void SetPaused(int h, int p);
	int GetSongPos(int h);
	void SetSongPos(int h, int p);
	void SetSongVol(int h, int v);
	int GetSongVol(int h);
	void FreeSong(int h);
};


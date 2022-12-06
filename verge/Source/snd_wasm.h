/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.

#include <vector>

class SoundEngine_Wasm : public SoundEngine {
    public:
        bool init() override;
        void shutdown() override;

        void PlayMusic(const std::string& filename) override;
        void StopMusic() override;
        void SetMusicVolume(int vol) override;

        void* LoadSample(const char* filename) override;
        int PlaySample(void* sample, int vol) override;
        void StopSound(int chan) override;
        bool SoundIsPlaying(int chan) override;
        void FreeSample(void* sample) override;

        int LoadSong(const std::string& filename) override;
        void PlaySong(int song) override;
        void StopSong(int song) override;
        void SetPaused(int song, int paused) override;
        int GetSongPos(int song) override;
        void SetSongPos(int song, int pos) override;
        void SetSongVol(int song, int vol) override;
        int GetSongVol(int song) override;
        void FreeSong(int song) override;

    private:
        std::string currentMusicName;
        std::vector<std::string> sounds;
};
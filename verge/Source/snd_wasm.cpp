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
 * verge3: win_sound.cpp                                          *
 * copyright (c) 2004 vecna                                       *
 ******************************************************************/

#include <stdint.h>

#include "xerxes.h"
#include "snd_wasm.h"
#include "garlick.h"

// TODO: test stream music support
// TODO: test multiple song support
// TODO: test song get/set position support
//
// TODO: better sfx support (certain sfx never play / load)
// TODO: song pause/resume support
// TODO: Garlick support (not sure I have projects to test that use this functionality atm)

#ifdef SND_USE_WASM

EM_JS(void, wasm_initSound, (), {
    window.verge.audioContext = new AudioContext();
    window.verge.mainSong = null;

    window.verge.songs = {};
    window.verge.songDatas = {};
    window.verge.songRefs = {};

    window.verge.sounds = {};
    window.verge.soundRefs = {};
    window.verge.soundChannels = {};
    window.verge.soundChannelNextID = 1;

    window.verge.initMpt = () => {
        const ctx = window.verge.audioContext;
        if (ctx.audioWorklet) {
            window.verge.mptInited = ctx.audioWorklet.addModule('mpt-worklet.js').then(() => {
                window.verge.mainSong = window.verge.createSong();
            }).catch(err => console.log('failed to init audio worklet', err));
        } else {
            console.warn("AudioWorklet is not supported in this browser.  No music.  Sorry!");
            window.verge.mptInited = Promise.resolve();
        }
    };

    window.verge.createMptNode = () => {
        const ctx = window.verge.audioContext;
        return new AudioWorkletNode(ctx, 'libopenmpt-processor', {
            numberOfInputs: 0,
            numberOfOutputs: 1,
            outputChannelCount: [2],
        });
    };

    window.verge.createSong = () => {
        const ctx = window.verge.audioContext;
        const gainNode = ctx.createGain();
        gainNode.connect(ctx.destination);

        const streamAudio = new Audio();
        const streamNode = ctx.createMediaElementSource(streamAudio);
        streamNode.connect(gainNode);

        const mptNode = window.verge.createMptNode();
        mptNode.connect(gainNode);


        return {
            gainNode: gainNode,
            streamAudio: streamAudio,
            streamNode: streamNode,
            mptNode: mptNode,
            activeSourceNode: node,
            lastGetPosition: 0,
        }
    };

    window.verge.freeSong = (song) => {
        song.streamNode.disconnect();
        song.mptNode.disconnect();
    };

    window.verge.playSong = (song, filename, songData) => {
        window.verge.stopSong(song);

        window.verge.mptInited.then(() => {
            console.log('window.verge.playSong: playing ', filename, songData);  

            if (!song || !filename || !songData) {
                return;
            }            

            if (filename.endsWith('mp3')
            || filename.endsWith('ogg')
            || filename.endsWith('wav')) {
                const songBlob = new Blob(buffer);
                const songBlobURL = URL.createObjectURL(songBlob);

                console.log('window.verge.playSong: streaming', filename);

                song.streamAudio.srcObject = songBlobURL;
                song.streamAudio.play();
                song.activeSourceNode = song.streamNode;
            } else {
                console.log('window.verge.playSong: sequenced', filename);

                song.mptNode.port.postMessage({
                    songData: songData,
                    setRepeatCount: -1
                });
                song.activeSourceNode = song.mptNode;
            }
        });
    };

    window.verge.stopSong = (song) => {
        window.verge.mptInited.then(() => {
            console.log('window.verge.stopSong');

            if (song.activeSourceNode == song.streamAudio) {
                song.streamAudio.pause();
                song.streamAudio.currentTime = 0;
            } else if (song.activeSourceNode == song.mptNode) {
                song.mptNode.port.postMessage({
                    songData: new ArrayBuffer(0)
                });
            }

            song.activeSourceNode = null;
        });
    };

    // TODO: pause/resume (still needs to be implemented in mpt-worklet)

    window.verge.getSongPosition = (song) => {
        if (song.activeSourceNode == song.streamAudio) {
            return Math.floor(song.streamAudio.currentTime * 1000);
        } else if (song.activeSourceNode == song.mptNode) {
            response = await song.mptNode.port.postMessage({
                getPosition: true,
            });
            return Math.floor(response.position * 1000);
        } else {
            return 0;
        }
    }

    window.verge.setSongPosition = (song, position) => {
        if (song.activeSourceNode == song.streamAudio) {
            song.streamAudio.currentTime = position / 1000;
        } else if (song.activeSourceNode == song.mptNode) {
            song.mptNode.port.postMessage({
                setPosition: position / 1000
            });
        } else {
            return 0;
        }
    }

    window.verge.getSongVolume = (song) => song.gainNode.gain.value;

    window.verge.setSongVolume = (song, volume) => {
        //console.log('window.verge.setSongVolume', volume);
        song.gainNode.gain.setValueAtTime(volume / 100, window.verge.audioContext.currentTime);
    };

    window.verge.initMpt();
});

EM_JS(bool, wasm_loadSound, (const char* filename, const void* data, int length), {
    const name = UTF8ToString(filename);
    const audioData = HEAPU8.buffer.slice(data, data + length);

    // Sleep until the sound is loaded, since calling code expects this to be synchronous/blocking.
    return Asyncify.handleSleep(wakeUp => {
        if (window.verge.sounds[name]) {
            window.verge.soundRefs[name]++;
            wakeUp(true);
        } else {
            window.verge.audioContext.decodeAudioData(
                audioData,
                decoded => {
                    console.log("wasm_loadSound: Loaded sound ", name);
                    window.verge.sounds[name] = decoded;
                    window.verge.soundRefs[name] = (window.verge.soundRefs[name] || 0) + 1;
                    wakeUp(true);
                },
                () => {
                    console.log("wasm_loadSound: Unable to load sound data for ", name); // fixme
                    wakeUp(false);
                }
            );
        }
    });
});

EM_JS(int, wasm_freeSound, (const char* filename), {
    const name = UTF8ToString(filename);
    const sound = window.verge.sounds[name];
    if (!sound) {
        console.error("wasm_freeSound: Unknown sound ", name);
        return;
    }

    if (--window.verge.soundRefs[name] <= 0) {
        delete window.verge.sounds[name];
        delete window.verge.soundRefs[name];
    }
});

EM_JS(int, wasm_playSound, (const char* filename, int volume), {
    const name = UTF8ToString(filename);
    const sound = window.verge.sounds[name];
    if (!sound) {
        console.error("wasm_playSound: Unknown sound ", name);
        return;
    }

    const ctx = window.verge.audioContext;
    const channelID = window.verge.soundChannelNextID++;
    let destination = ctx.destination;

    const gainNode = volume < 100 ? ctx.createGain() : null;
    if (gainNode != null) {
        gainNode.gain.value = volume / 100;
        gainNode.connect(destination);
        destination = gainNode;
    }

    const source = ctx.createBufferSource();
    source.connect(destination);
    source.buffer = sound;
    source.addEventListener('ended', () => {
        delete window.verge.soundChannels[channelID];
        if (gainNode != null) {
            gainNode.disconnect();
        }
    });
    source.start(0);

    window.verge.soundChannels[channelID] = source;
});

EM_JS(int, wasm_stopSound, (int channelID), {
    const source = window.verge.soundChannels[channelID];
    if (source) {
        source.stop();
    }
});

EM_JS(bool, wasm_isSoundPlaying, (int channelID), {
    const source = window.verge.soundChannels[channelID];
    return typeof(source) !== 'undefined';
});

EM_JS(void, wasm_playMusic, (const char* filename, const void* data, int length), {
    const name = UTF8ToString(filename);
    const buffer = Module.HEAP8.buffer.slice(data, data + length);
    const songData = new Uint8Array(buffer);

    window.verge.mptInited.then(() => {
        window.verge.playSong(window.verge.mainSong, name, songData);
    });
});

EM_JS(void, wasm_stopMusic, (), {
    window.verge.mptInited.then(() => {
        window.verge.stopSong(window.verge.mainSong);
    });
})

EM_JS(void, wasm_setMusicVolume, (int volume), {
    window.verge.mptInited.then(() => {
        window.verge.setSongVolume(window.verge.mainSong, volume);
    });
});

EM_JS(bool, wasm_loadSong, (const char* filename, const void* data, int length), {
    const name = UTF8ToString(filename);
    const buffer = Module.HEAP8.buffer.slice(data, data + length);
    const songData = new Uint8Array(buffer);    

    return Asyncify.handleSleep(wakeUp => {
        window.verge.mptInited.then(() => {
            if (window.verge.songs[name]) {
                window.verge.songRefs[name]++;
                wakeUp(true);
            } else {
                window.verge.songs[name] = window.verge.createSong();
                window.verge.songDatas[name] = songData;
                window.verge.songRefs[name] = (window.verge.songRefs[name] || 0) + 1;

                console.log("wasm_loadSong: Load song for ", name);
                wakeUp(true);
            }
        });
    });
});

EM_JS(void, wasm_freeSong, (const char* filename), {
    const name = UTF8ToString(filename);
    const song = window.verge.songs[name];
    if (!song) {
        console.error("wasm_freeSong: Unknown song ", name);
        return;
    }

    if (--window.verge.songRefs[name] <= 0) {
        window.verge.freeSong(song);

        delete window.verge.songs[name];
        delete window.verge.songDatas[name];
        delete window.verge.songRefs[name];
    }
});

EM_JS(void, wasm_playSong, (const char* filename), {
    const name = UTF8ToString(filename);

    window.verge.mptInited.then(() => {
        const song = window.verge.songs[name];
        if (!song) {
            console.error("wasm_playSong: Unknown song ", name);
            return;
        }

        window.verge.playSong(song, name, window.verge.songDatas[name]);
    });
});

EM_JS(void, wasm_stopSong, (const char* filename), {
    const name = UTF8ToString(filename);
    const song = window.verge.songs[name];
    if (!song) {
        console.error("wasm_stopSong: Unknown song ", name);
        return;
    }

    window.verge.mptInited.then(() => {
        window.verge.stopSong(song);
    });
})

EM_JS(int, wasm_getSongPosition, (const char* filename), {
    const name = UTF8ToString(filename);
    const song = window.verge.songs[name];
    if (!song) {
        console.error("wasm_getSongPosition: Unknown song ", name);
        return;
    }    

    window.verge.mptInited.then(() => {
        window.verge.getSongPosition(song);
    });
})

EM_JS(void, wasm_setSongPosition, (const char* filename, int position), {
    const name = UTF8ToString(filename);
    const song = window.verge.songs[name];
    if (!song) {
        console.error("wasm_setSongPosition: Unknown song ", name);
        return;
    }

    window.verge.mptInited.then(() => {
        window.verge.setSongPosition(song, position);
    });
})

EM_JS(int, wasm_getSongVolume, (const char* filename), {
    const name = UTF8ToString(filename);
    const song = window.verge.songs[name];
    if (!song) {
        console.error("wasm_getSongVolume: Unknown song ", name);
        return;
    }    

    window.verge.mptInited.then(() => {
        window.verge.getSongVolume(song);
    });
})

EM_JS(void, wasm_setSongVolume, (const char* filename, int volume), {
    const name = UTF8ToString(filename);
    const song = window.verge.songs[name];
    if (!song) {
        console.error("wasm_setSongVolume: Unknown song ", name);
        return;
    }

    window.verge.mptInited.then(() => {
        window.verge.setSongVolume(song, volume);
    });
})

bool SoundEngine_Wasm::init() {
    wasm_initSound();
    log("SoundEngine_Wasm::init: initialized wasm sound");

    return true;
}

void SoundEngine_Wasm::shutdown() {}

void SoundEngine_Wasm::PlayMusic(const std::string& filename) {
    log("SoundEngine_Wasm::PlayMusic: playing \"%s\"", filename.c_str());
    if (currentMusicName == filename) {
        log("SoundEngine_Wasm::PlayMusic: music already playing");
        return;
    }

    // ovk(2022-12-07): This current implementation is probably really bad for streaming files,
    // since we need to allocate multiple file-sized copies in order to play + decode.
    // Detect format here, avoid buffer copies if streaming audio and not from a packfile?
    // Could manifest.txt support detect mp3s/oggs/etc over a certain size and skip loading them? (with some flag to override if necessary)
    // Dunno how you avoid overhead for packfiles though, it needs to allocate a buffer to pass to an <audio> tag or decoder worklet...
    // Would be ideal for bandwidth + client memory footprint to load music tracks on-demand, unlike usually-smaller sfx.

    std::vector<byte> buf;
    if (vreadfile(filename.c_str(), buf)) {
        log("SoundEngine_Wasm::PlayMusic: playing \"%s\"", filename.c_str());
        wasm_playMusic(filename.c_str(), buf.data(), buf.size());
        currentMusicName = filename;
    } else {
        log("SoundEngine_Wasm::PlayMusic: failed to read %s", filename.c_str());
    }
}

void SoundEngine_Wasm::StopMusic() {
    log("SoundEngine_Wasm::StopMusic");

    wasm_stopMusic();
    currentMusicName = "";
}

void SoundEngine_Wasm::SetMusicVolume(int vol) {
    wasm_setMusicVolume(vol);
}

void* SoundEngine_Wasm::LoadSample(const char* filename) {
    std::vector<byte> buf;

    //log("LoadSample: load %s", filename);
    if (!vreadfile(filename, buf)) {
        return 0;
    } else if (!wasm_loadSound(filename, buf.data(), buf.size())) {
        return 0;
    } else {
        if (unusedSoundHandles.size() > 0) {
            const auto i = unusedSoundHandles.back();
            unusedSoundHandles.pop_back();

            sounds[i - 1] = filename;
            return reinterpret_cast<void*>(static_cast<uintptr_t>(i));
        }

        sounds.push_back(filename);
        return reinterpret_cast<void*>(static_cast<uintptr_t>(sounds.size()));
    }
}

int SoundEngine_Wasm::PlaySample(void* sample, int vol) {
    const auto handle = reinterpret_cast<uintptr_t>(sample);
    if (handle > 0 && handle <= static_cast<uintptr_t>(sounds.size())) {
        return wasm_playSound(sounds[handle - 1].c_str(), vol);
    } else {
        return 0;
    }
}

void SoundEngine_Wasm::StopSound(int chan) {
    wasm_stopSound(chan);
}

bool SoundEngine_Wasm::SoundIsPlaying(int chan) {
    return wasm_isSoundPlaying(chan);
}

void SoundEngine_Wasm::FreeSample(void* sample) {
    const auto handle = reinterpret_cast<uintptr_t>(sample);
    if (handle > 0 && handle <= static_cast<uintptr_t>(sounds.size())) {
        wasm_freeSound(sounds[handle - 1].c_str());
        sounds[handle - 1] = "";
        unusedSoundHandles.push_back(static_cast<size_t>(handle));
    }
}

int SoundEngine_Wasm::LoadSong(const std::string& filename) {
    std::vector<byte> buf;

    if (!vreadfile(filename.c_str(), buf)) {
        return 0;
    } else if (!wasm_loadSong(filename.c_str(), buf.data(), buf.size())) {
        return 0;
    } else {
        if (unusedSongHandles.size() > 0) {
            const auto i = unusedSongHandles.back();
            unusedSongHandles.pop_back();

            songs[i - 1] = filename;
            return i;
        }

        songs.push_back(filename);
        return static_cast<int>(songs.size());
    }
}

void SoundEngine_Wasm::PlaySong(int song) {
    if (song > 0 && song <= static_cast<int>(songs.size())) {
        return wasm_playSong(songs[song - 1].c_str());
    }
}

void SoundEngine_Wasm::StopSong(int song) {
    if (song > 0 && song <= static_cast<int>(songs.size())) {
        return wasm_stopSong(songs[song - 1].c_str());
    }
}

void SoundEngine_Wasm::SetPaused(int song, int paused) {
    if (song > 0 && song <= static_cast<int>(songs.size())) {
        //return wasm_setSongPaused(songs[song - 1].c_str());
    }
}

int SoundEngine_Wasm::GetSongPos(int song) {
    if (song > 0 && song <= static_cast<int>(songs.size())) {
        return wasm_getSongPosition(songs[song - 1].c_str());
    } else {
        return 0;
    }
}

void SoundEngine_Wasm::SetSongPos(int song, int pos) {
    if (song > 0 && song <= static_cast<int>(songs.size())) {
        return wasm_setSongPosition(songs[song - 1].c_str(), pos);
    }
}

int SoundEngine_Wasm::GetSongVol(int song) {
    if (song > 0 && song <= static_cast<int>(songs.size())) {
        return wasm_getSongVolume(songs[song - 1].c_str());
    } else {
        return 0;
    }
}

void SoundEngine_Wasm::SetSongVol(int song, int vol) {
    if (song > 0 && song <= static_cast<int>(songs.size())) {
        return wasm_setSongVolume(songs[song - 1].c_str(), vol);
    }
}

void SoundEngine_Wasm::FreeSong(int song) {
    if (song > 0 && song <= static_cast<int>(songs.size())) {
        wasm_freeSong(songs[song - 1].c_str());
        songs[song - 1] = "";
        unusedSongHandles.push_back(static_cast<size_t>(song));
    }
}

#endif
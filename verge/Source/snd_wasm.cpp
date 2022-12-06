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

// TODO: stream music support
// TODO: better sfx support (certain sfx never play / load)
// TODO: multiple song support
// TODO: song get/set position support
// TODO: song pause/resume support
// TODO: Garlick support (not sure I have projects to test that use this functionality atm)

#ifdef SND_USE_WASM

EM_JS(void, wasm_initSound, (), {
    const ctx = new AudioContext();

    const musicGainNode = ctx.createGain();
    musicGainNode.connect(ctx.destination);

    window.verge.audioContext = ctx;
    window.verge.musicGainNode = musicGainNode;
    window.verge.sounds = {};
    window.verge.soundChannels = {};
    window.verge.soundChannelNextID = 1;

    if (ctx.audioWorklet) {
        window.verge.mptInited = ctx.audioWorklet.addModule('mpt-worklet.js').then(() => {
            window.verge.mptNode = new AudioWorkletNode(ctx, 'libopenmpt-processor', {
                numberOfInputs: 0,
                numberOfOutputs: 1,
                outputChannelCount: [2],
            });

            window.verge.mptNode.connect(musicGainNode);
        }).catch(err => console.log('failed to init audio worklet', err));
    } else {
        console.warn("AudioWorklet is not supported in this browser.  No music.  Sorry!");
        window.verge.mptInited = Promise.resolve();
    }
});

EM_JS(void, wasm_loadSound, (const char* filename, const void* data, int length), {
    const name = UTF8ToString(filename);
    const audioData = HEAPU8.buffer.slice(data, data + length);

    // Sleep until the sound is loaded, since calling code expects this to be synchronous/blocking.
    return Asyncify.handleSleep(wakeUp => {
        window.verge.audioContext.decodeAudioData(
            audioData,
            decoded => {
                // console.log("Loaded ", name, " ok!");
                window.verge.sounds[name] = decoded;
                wakeUp();
            },
            () => {
                console.log("Unable to load sound data for ", name); // fixme
                wakeUp();
            }
        );
    });
});

EM_JS(int, wasm_playSound, (const char* filename, int volume), {
    const name = UTF8ToString(filename);
    const sound = window.verge.sounds[name];
    if (!sound) {
        console.error("Unknown sound ", name);
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

EM_JS(int, wasm_freeSound, (const char* filename), {
    const name = UTF8ToString(filename);
    const sound = window.verge.sounds[name];
    if (!sound) {
        console.error("Unknown sound ", name);
        return;
    }

    delete window.verge.sounds[name];
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

EM_JS(void, wasm_playMusic, (const void* data, int length), {
    const buffer = Module.HEAP8.buffer.slice(data, data + length);
    const songData = new Uint8Array(buffer);

    window.verge.mptInited.then(() => {
        if (!window.verge.mptNode) {
            return;
        }

        window.verge.mptNode.port.postMessage({
            songData: songData,
            setRepeatCount: -1
        });

        //console.log('wasm_playMusic', songData, length);
    });
});

EM_JS(void, wasm_stopMusic, (), {
    window.verge.mptInited.then(() => {
        window.verge.mptNode.port.postMessage({
            songData: new ArrayBuffer(0)
        });

        //console.log('wasm_stopMusic', data, length);
    });
})

EM_JS(void, wasm_setMusicVolume, (int volume), {
    console.log('setvolume', volume);
    window.verge.musicGainNode.gain.setValueAtTime(volume / 100, window.verge.audioContext.currentTime);
});

bool SoundEngine_Wasm::init() {
    wasm_initSound();
    log("initialized wasm sound");

    return true;
}

void SoundEngine_Wasm::shutdown() {}

void SoundEngine_Wasm::PlayMusic(const std::string& filename) {
    if (currentMusicName == filename) {
        return;
    }

    std::vector<byte> buf;
    if (vreadfile(filename.c_str(), buf)) {
        //log("PlayMusic: playing %s", filename.c_str());
        wasm_playMusic(buf.data(), buf.size());
        currentMusicName = filename;
    } else {
        //log("PlayMusic: failed to read %s", filename.c_str());
    }
}

void SoundEngine_Wasm::StopMusic() {
    wasm_stopMusic();
}

void SoundEngine_Wasm::SetMusicVolume(int vol) {
    wasm_setMusicVolume(vol);
}

void* SoundEngine_Wasm::LoadSample(const char* filename) {
    std::vector<byte> buf;
    if (vreadfile(filename, buf)) {
        wasm_loadSound(filename, buf.data(), buf.size());

        sounds.push_back(filename);
        return reinterpret_cast<void*>(static_cast<uintptr_t>(sounds.size() - 1));
    }

    return 0;
}

int SoundEngine_Wasm::PlaySample(void* sample, int vol) {
    if (sample != nullptr) {
        const auto index = reinterpret_cast<uintptr_t>(sample);
        return wasm_playSound(sounds[index].c_str(), vol);
    }

    return 0;
}

void SoundEngine_Wasm::StopSound(int chan) {
    wasm_stopSound(chan);
}

bool SoundEngine_Wasm::SoundIsPlaying(int chan) {
    return wasm_isSoundPlaying(chan);
}

void SoundEngine_Wasm::FreeSample(void* sample) {
    if (sample != nullptr) {
        const auto index = reinterpret_cast<uintptr_t>(sample);
        wasm_freeSound(sounds[index].c_str());
    }
}

int SoundEngine_Wasm::LoadSong(const std::string& filename) {
    return 0;
}

void SoundEngine_Wasm::PlaySong(int song) {}

void SoundEngine_Wasm::StopSong(int song) {}

void SoundEngine_Wasm::SetPaused(int song, int paused) {}

int SoundEngine_Wasm::GetSongPos(int song) {
    return 0;
}

void SoundEngine_Wasm::SetSongPos(int song, int pos) {}

void SoundEngine_Wasm::SetSongVol(int song, int vol) {}

int SoundEngine_Wasm::GetSongVol(int song) {
    return 0;
}

void SoundEngine_Wasm::FreeSong(int song) {}

#endif
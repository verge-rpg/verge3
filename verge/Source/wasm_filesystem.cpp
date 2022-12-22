#include <vector>
#include <memory>
#include <string_view>

#include <emscripten.h>
#include <emscripten/fetch.h>

#include "wasm_filesystem.h"

std::string wasm_gameRoot;

EM_JS(void, wasm_initFileSystem, (const char* c), {
    let sgr = UTF8ToString(c);
    if (sgr.endsWith('/'))
        sgr = sgr.substr(0, sgr.length - 1);
    FS.mkdir("persist");
    FS.mkdir(sgr);
    FS.mkdir("persist/" + sgr);
    // Then mount with IDBFS type
    FS.mount(IDBFS, {}, "persist/" + sgr);

    // Then sync
    FS.syncfs(true, function (err) {
        // Error
        if (err) {
            console.error('wasm_initFileSystem failed!', err);
        } else {
            //console.log("wasm_initFileSystem ok");
        }
    });
});

EM_JS(void, wasm_syncFileSystem, (), {
    //console.log("wasm_syncFileSystem");
    FS.syncfs(false, err => {
        if (err) {
            console.error("wasm_syncFileSystem failed!!", err);
        } else {
            //console.log("wasm_syncFileSystem ok");
        }
    });
});

EM_JS(void, wasm_downloadAll, (const char** manifest), {
    return Asyncify.handleSleep(resume => {
        let promises = [];
        let count = 0;

        function download(pathPtr) {
            const path = UTF8ToString(pathPtr);
            //console.log('fetching' + path);

            return fetch(path).then(response => {
                if (!response.ok) {
                    console.error('wasm_fetchSync failed', path);
                    HEAP32[size >> 2] = 0;
                    HEAP32[data >> 2] = 0;
                    throw('wasm_fetchSync failed: ' + path);
                }
                return response.blob();
            }).then(blob =>
                blob.arrayBuffer()
            ).then(array => {
                const bytes = new Uint8Array(array);

                const idx = path.lastIndexOf('/');
                if (idx != -1) {
                    const dir = path.substr(0, idx).toLowerCase();
                    FS.mkdirTree(dir);
                }

                //console.log('Writing', path.toLowerCase(), '(' + bytes.length + ' bytes)');
                FS.writeFile(path.toLowerCase(), bytes);
                //console.log('Wrote', path.toLowerCase(), '(' + bytes.length + ' bytes)');

                ++count;
                verge.setLoadingProgress((100 * count / promises.length) | 0)
            }).catch(e => {
                console.error('wasm_fetchSync failed', path, e);
                throw('wasm_fetchSync failed: ' + path + ' ' + e);
            });
        }

        while (true) {
            let pathPtr = HEAPU32[manifest >> 2];
            if (pathPtr == 0) {
                break;
            }
            manifest += 4;
            promises.push(download(pathPtr));
        }

        Promise.all(promises).then(() => { resume(); });
    });
});

EM_JS(void, wasm_fetchSync, (const char* pathPtr, size_t* size, char** data), {
    return Asyncify.handleSleep(resume => {
        const path = UTF8ToString(pathPtr);
        return fetch(path).then(response => {
            if (!response.ok) {
                console.error('wasm_fetchSync failed', path);
                HEAP32[size >> 2] = 0;
                HEAP32[data >> 2] = 0;
                resume();
                return;
            }
            return response.blob();
        }).then(blob =>
            blob.arrayBuffer()
        ).then(array => {
            const bytes = new Uint8Array(array);
            HEAP32[size >> 2] = bytes.length;
            const dataPtr = _malloc(bytes.length);
            HEAP32[data >> 2] = dataPtr;
            HEAP8.set(bytes, dataPtr);
            resume();
        }).catch(e => {
            console.error('wasm_fetchSync failed', path, e);
            throw('wasm_fetchSync failed: ' + path + ' ' + e);
        });
    });
});

struct FreeDelete { void operator()(char* p) { free(p); } };
using Deleter = std::unique_ptr<char, FreeDelete>;

void downloadGame() {
    std::string manifestPath = wasm_gameRoot + "manifest.txt";
    char* manifestPtr;
    size_t manifestLength;
    wasm_fetchSync(manifestPath.c_str(), &manifestLength, &manifestPtr);
    Deleter hello{ manifestPtr };

    std::string_view manifest{ manifestPtr, manifestLength };

    std::vector<std::string> files;
    auto append = [&](std::string_view name) {
        if (name.empty())
            return;

        if (name[name.size() - 1] == '\r')
            name.remove_suffix(1);

        files.push_back(wasm_gameRoot + std::string{ name });
    };

    while (!manifest.empty()) {
        auto pos = manifest.find('\n');
        if (pos == std::string::npos) {
            append(std::string{ manifest });
            break;
        }
        append(std::string{ manifest.substr(0, pos) });
        manifest.remove_prefix(pos + 1);
    }

    std::vector<const char*> stuff;
    stuff.reserve(files.size() + 1);
    for (const std::string& s: files) {
        stuff.push_back(s.c_str());
    }
    stuff.push_back(nullptr);

    wasm_downloadAll((const char**)stuff.data());

    EM_ASM({
        window.verge.setLoadingProgress(100);
    });
}

void initWasmFileSystem()
{
    wasm_initFileSystem(wasm_gameRoot.c_str());

    downloadGame();
}

void syncWasmFileSystem()
{
    wasm_syncFileSystem();
}
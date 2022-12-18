#ifndef WASM_FILESYSTEM_H
#define WASM_FILESYSTEM_H

#include <string>

#ifdef __EMSCRIPTEN__

extern std::string wasm_gameRoot;

void initFileSystem();
#endif

#endif

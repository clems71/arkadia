#pragma once

#include <stdint.h>
#include <string>
#include <vector>

// Core loading
void coreInit(const std::string & corePath);
void coreClose();

// ROM loading
void coreLoadGame(const std::string & romPath);

// Core updating
void coreUpdate();

// Bits
// 31 . . . . . . . . . . . . . . . . . . 0
// AAAA AAAA RRRR RRRR GGGG GGGG BBBB BBBB
const std::vector<uint32_t> & coreVideoData(size_t & width, size_t & height);

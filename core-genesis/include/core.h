#pragma once

#include <stdint.h>

#include "core_export.h"
#include "surface.h"

enum Buttons {
  Button_Up,
  Button_Down,
  Button_Left,
  Button_Right,
  Button_A,
  Button_B,
  Button_C,
  Button_Start,
  Button_COUNT
};

extern "C" {

// Opaque type
struct Core;

// Creation functions
CORE_EXPORT Core * coreCreate(const char * romPath);
CORE_EXPORT void coreDelete(Core * core);

// Emulation functions
CORE_EXPORT void coreUpdate(Core * core);

// Button functions
CORE_EXPORT void coreButtonPressed(Core * core, uint32_t player, uint32_t b);
CORE_EXPORT void coreButtonReleased(Core * core, uint32_t player, uint32_t b);

// Copy video data into 'data'.
// 'data' should be sufficiently large to host the video buffer (width * height * 4 bytes).
CORE_EXPORT void coreVideoData(Core * core, char * data);
CORE_EXPORT void coreVideoSize(Core * core, uint32_t * width, uint32_t * height);

CORE_EXPORT uint32_t coreAudioData(Core * core, short * data=NULL);

CORE_EXPORT const char * coreRomInfo(Core * core);

CORE_EXPORT double coreFrameInterval(Core * core);

}

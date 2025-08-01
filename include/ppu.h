#pragma once

#include <SDL3/SDL_stdinc.h>
#include <stdint.h>

#include "fetcher.h"
#include "mmio.h"
#include "types.h"

typedef struct {
  MMIO *mmio;
  Fetcher *fetcher;
  PPUStateType state;

  uint16_t dots;
  uint32_t frameCount;
  
  bool intLineUp;
  bool incrWLY;
  bool windowInit;
  bool windowTrigger;
  uint8_t dropPixels;
  Uint64 startTime;
  Uint64 freq;

} PPU;

PPU *createPPU(MMIO *);
void destroyPPU(PPU *);

void ppuTick(PPU *);

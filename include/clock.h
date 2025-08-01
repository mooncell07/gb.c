#pragma once
#include "mmio.h"
#include "ppu.h"

typedef struct {
  MMIO *mmio;
  PPU *ppu;
} Clock;

Clock *createClock(PPU *, MMIO *);
void destroyClock(Clock *);
void tickComponents(Clock *, int);

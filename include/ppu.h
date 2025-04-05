#pragma once

#include "fetcher.h"
#include "mmio.h"
#include "types.h"
#include <stdint.h>

typedef struct {
    MMIO *mmio;
    Fetcher *fetcher;
    PPUStateType state;

    uint16_t dots;

    bool intLineUp;
    bool incrWLY;
    bool windowInit;
    bool windowTrigger;
    uint8_t dropPixels;

} PPU;

PPU *createPPU(MMIO *);
void destroyPPU(PPU *);

void ppuTick(PPU *);


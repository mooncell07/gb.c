#pragma once

#include "fifo.h"
#include "mmio.h"
#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    MMIO *mmio;
    uint8_t fetcherState;
    uint8_t spriteState;

    uint16_t internalX;
    uint8_t tileNum;
    uint16_t tileDataAddr;
    uint8_t tileLo;
    uint8_t tileSlice[8];

    bool firstInstance;
    bool isWindowVisible;
    bool tickingSprite;
    uint8_t staticLine;

    uint16_t spriteDataAddr;
    uint8_t spriteLo;
    uint8_t spriteSlice[8];
    Sprite currSprite;

    PixelFIFO tFIFO;
    PixelFIFO sFIFO;
    SpriteBuffer sprites;

} Fetcher;

Fetcher *createFetcher(MMIO *);
void destroyFetcher(Fetcher *);

uint8_t getColorIndex(Fetcher *, uint8_t, bool, bool);
SDL_Color searchColor(uint8_t);
void tileTick(Fetcher *);
void spriteTick(Fetcher *);
void fetchMain(Fetcher *, uint16_t);
void fetchBackground(Fetcher *);
void fetchWindow(Fetcher *);
void clearSpriteFetcher(Fetcher *);

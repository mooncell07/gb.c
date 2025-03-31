#include "ppu.h"

#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "fetcher.h"
#include "fifo.h"
#include "lcd.h"
#include "logger.h"
#include "mmio.h"
#include "types.h"
#include "utils.h"
#include "vbus.h"

PPU *createPPU(MMIO *mmio) {
    PPU *ppu = calloc(1, sizeof(PPU));
    ppu->fetcher = createFetcher(mmio);
    ppu->windowInit = true;
    ppu->mmio = mmio;
    return ppu;
}

void destroyPPU(PPU *self) {
    destroyFetcher(self->fetcher);
    free(self);
}

void sortSprites(PPU *self) {
    // An implementation of isort3.
    int i = 1;
    while (i < FIFOLen(&self->fetcher->sprites.base)) {
        Sprite next =
            getEntryAt(self->fetcher->sprites, &self->fetcher->sprites, i);
        int j = i;
        while (j > 0 && getEntryAt(self->fetcher->sprites,
                                   &self->fetcher->sprites, j - 1)
                                .x > next.x) {
            setEntryAt(self->fetcher->sprites, j,
                       getEntryAt(self->fetcher->sprites,
                                  &self->fetcher->sprites, j - 1));
            j = j - 1;
        }
        setEntryAt(self->fetcher->sprites, j, next);
        i = i + 1;
    }
}

void searchSprites(PPU *self) {
    for (int i = 0; i < 40; i++) {
        if (FIFOLen(&self->fetcher->sprites.base) == 10) {
            break;
        }

        uint8_t spriteY = oam[i * 4];
        uint8_t spriteX = oam[i * 4 + 1];
        uint8_t tileNum = oam[i * 4 + 2];
        uint8_t flags = oam[i * 4 + 3];
        uint8_t spriteHeight = 8;

        if (getLCDC(self->mmio, OBJSIZE)) {
            spriteHeight = 16;
        }

        if (((self->mmio->LY + 16) >= spriteY) &&
            ((self->mmio->LY + 16) < (spriteY + spriteHeight)) &&
            (spriteX > 0)) {
            Sprite spr = {spriteX, spriteY, tileNum, flags, spriteHeight};
            pushEntry(self->fetcher->sprites, spr);
        }
    }
    sortSprites(self);
}

void isWindowEnabled(PPU *self) {
    if (self->mmio->LY == self->mmio->WY) {
        self->windowTrigger = true;
    }
    self->fetcher->isWindowVisible =
        getLCDC(self->mmio, WINEN) && self->windowTrigger &&
        (self->mmio->LX + 7 >= self->mmio->WX) && BOUND(self->mmio->WY, 0, 143);
}

SDL_Color getTileColor(PPU *self, uint8_t colorCode) {
    return searchColor(getLCDC(self->mmio, BGANDWINEN) ? colorCode : 0);
}

bool checkStatInt(PPU *self, uint8_t v) { return BT(self->mmio->STAT, v); }

void sendStatInt(PPU *self) {
    bool oldState = self->intLineUp;
    self->intLineUp = true;

    if (!oldState && self->intLineUp) {
        sendIntReq(self->mmio, INTSTAT);
    }
}

void handleCoincidence(PPU *self, bool quirk) {
    uint8_t qLY = self->mmio->LY;
    if (quirk)
        qLY = 0;

    if (qLY == self->mmio->LYC) {
        setBit(self->mmio->STAT, 2);
        checkStatInt(self, 6) ? sendStatInt(self)
                              : clearBit(self->mmio->STAT, 2);
    }
}

void nextLine(PPU *self) {
    self->mmio->LY++;
    if (self->incrWLY)
        self->mmio->WLY++;
    handleCoincidence(self, false);
}

void switchMode(PPU *self, PPUStateType t) {
    clearBit(self->mmio->STAT, 0);
    clearBit(self->mmio->STAT, 1);
    self->mmio->STAT = self->mmio->STAT | t;
    self->state = t;

    if (self->state != PIXELTRANSFER && checkStatInt(self, t + 3))
        sendStatInt(self);
}

void ppuTick(PPU *self) {
    if (!getLCDC(self->mmio, LCDPPUEN)) {
        switchMode(self, HBLANK);
        self->mmio->LY = 0;
        return;
    }

    self->dots++;

    switch (self->state) {
    case OAMSEARCH:
        if (self->dots == 80) {
            fetchBackground(self->fetcher);
            self->dropPixels = self->mmio->SCX % 8;
            searchSprites(self);
            switchMode(self, PIXELTRANSFER);
        }
        break;

    case PIXELTRANSFER:
        if (self->fetcher->tickingSprite) {
            spriteTick(self->fetcher);
        } else {
            if (FIFOLen(&self->fetcher->sprites.base) != 0) {
                Sprite spr = getEntryAt(self->fetcher->sprites,
                                        &self->fetcher->sprites, 0);
                if (spr.x <= (self->mmio->LX + 8)) {
                    self->fetcher->currSprite = spr;
                    self->fetcher->tickingSprite = true;
                    spriteTick(self->fetcher);
                }
            }
        }

        isWindowEnabled(self);

        if (self->fetcher->isWindowVisible) {
            self->incrWLY = true;
            if (self->windowInit) {
                fetchWindow(self->fetcher);
                self->windowInit = false;
                self->dropPixels = 0;
                if (self->mmio->WX < 7) {
                    self->dropPixels = 7 - self->mmio->WX;
                }
                return;
            }
        }
        if (!self->fetcher->tickingSprite) {
            tileTick(self->fetcher);
        }
        if ((FIFOLen(&self->fetcher->tFIFO.base) > 0) &&
            !self->fetcher->tickingSprite) {
            Pixel tilePixel =
                popEntry(self->fetcher->tFIFO, &self->fetcher->tFIFO);

            if (self->dropPixels > 0) {
                self->dropPixels -= 1;
                return;
            }

            uint8_t tilePixelColorIndex =
                getColorIndex(self->fetcher, tilePixel.colorCode, false, false);
            SDL_Color tilePixelColor = getTileColor(self, tilePixelColorIndex);

            SDL_Color finalColor;

            if (FIFOLen(&self->fetcher->sFIFO.base) > 0) {
                Pixel spritePixel =
                    popEntry(self->fetcher->sFIFO, &self->fetcher->sFIFO);
                uint8_t spritePixelColorIndex =
                    getColorIndex(self->fetcher, spritePixel.colorCode, true,
                                  spritePixel.palette1);
                SDL_Color spritePixelColor = searchColor(spritePixelColorIndex);

                if ((spritePixel.colorCode == 0) ||
                    (spritePixel.priority && (tilePixel.colorCode != 0))) {
                    finalColor = tilePixelColor;
                } else {
                    finalColor = getLCDC(self->mmio, OBJEN) ? spritePixelColor
                                                            : tilePixelColor;
                }
            } else
                finalColor = tilePixelColor;

            drawPixel(finalColor);
            self->mmio->LX++;
        }

        if (self->mmio->LX == 160) {
            int dotsConsumed = self->dots - 80;
            if ((dotsConsumed < 172) || (dotsConsumed > 289)) {
                char dotStr[35];
                snprintf(dotStr, sizeof(dotStr), "Mode 3 Under/Overrun: %d",
                         dotsConsumed);
                logState(DEBUG, dotStr);
            }
            switchMode(self, HBLANK);
        }
        break;

    case HBLANK:
        if (self->dots == 456) {
            nextLine(self);

            self->fetcher->firstInstance = true;
            self->windowInit = true;
            self->intLineUp = false;
            self->incrWLY = false;

            self->mmio->LX = 0;
            self->dots = 0;

            clearSpriteFetcher(self->fetcher);

            if (self->mmio->LY > 143) {
                renderFrame();
                switchMode(self, VBLANK);
                sendIntReq(self->mmio, INTVBLANK);
            } else {
                switchMode(self, OAMSEARCH);
            }
        }
        break;

    case VBLANK:
        // Line 153 Quirk: LY==LYC is checked at the start of 153rd scanline
        // after a 1 M-cycle delay, at this point LYC is already 0 and
        // expects LY to be 0 as well, since the ppu is still ticking the
        // 153rd scanline, directly writing 0 to LY causes a crash so, we
        // instead match LYC to a "virtual" LY value of 0.
        if ((self->mmio->LY == 153) && (self->dots == 4)) {
            handleCoincidence(self, true);
        }

        if (self->dots > 456) {
            self->dots = 0;
            nextLine(self);

            if (self->mmio->LY > 153) {
                self->mmio->LY = 0;
                self->mmio->WLY = 0;
                self->windowTrigger = false;
                switchMode(self, OAMSEARCH);
            }
        }
        break;

    default:
        logState(FATAL, "Invalid PPU state.");
        break;
    }
}

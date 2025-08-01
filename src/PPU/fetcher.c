#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "fetcher.h"
#include "fifo.h"
#include "log.h"
#include "mmio.h"
#include "utils.h"
#include "vbus.h"

const SDL_Color DEFAULT_PALETTE[4] = {
    {0xFF, 0xF6, 0xD3, 0xFF},  // 00 - WHITE
    {0xF9, 0xA8, 0x75, 0xFF},  // 01 - LIGHT GRAY
    {0xEB, 0x6B, 0x6F, 0xFF},  // 10 - DARK GRAY
    {0x7C, 0x3F, 0x58, 0xFF}   // 11 - BLACK
};

Fetcher *createFetcher(MMIO *mmio) {
  Fetcher *f = calloc(1, sizeof(Fetcher));
  PixelFIFO tFIFO = {{.maxSize = 8}, {{0}}};
  PixelFIFO sFIFO = {{.maxSize = 8}, {{0}}};
  SpriteBuffer sprites = {{.maxSize = 10}, {{0}}};

  f->mmio = mmio;
  f->tFIFO = tFIFO;
  f->sFIFO = sFIFO;
  f->sprites = sprites;

  return f;
}

void destroyFetcher(Fetcher *f) { free(f); }

uint16_t getTileDataOffset(Fetcher *self, uint8_t tileNum) {
  if (getLCDC(self->mmio, BGANDWINTILEDATAAREA)) {
    return getTileDataBase(self->mmio) + ((uint16_t)tileNum * 16);
  }
  return getTileDataBase(self->mmio) + ((uint16_t)((int8_t)tileNum) * 16);
}

uint16_t getTileRow(Fetcher *self) {
  if (!self->isWindowVisible) { return ((uint16_t)(self->staticLine % 8) * 2); }
  return (uint16_t)(self->mmio->WLY % 8) * 2;
}

uint16_t internalY(Fetcher *self) {
  if (!self->isWindowVisible) {
    return ((uint16_t)(((self->mmio->LY + self->mmio->SCY) & 0xFF) / 8) * 32);
  }
  return (uint16_t)(self->mmio->WLY / 8.0) * 32;
}

uint8_t getColorIndex(Fetcher *self, uint8_t code, bool obj, bool palette1) {
  int index = (int)code * 2;
  uint8_t palette;
  if (obj) {
    palette = palette1 ? self->mmio->OBP1 : self->mmio->OBP0;
  } else {
    palette = self->mmio->BGP;
  }
  return BEXTR(palette, index, index + 1);
}

SDL_Color searchColor(uint8_t colorIndex) {
  return DEFAULT_PALETTE[colorIndex];
}

void fetchPixelSlice(Fetcher *self, uint8_t lo, uint8_t hi, bool isSprite) {
  int revX = 0;
  uint8_t cc = 0;
  uint8_t *slice = isSprite ? self->spriteSlice : self->tileSlice;

  for (int x = 7; x >= 0; x--) {
    cc = (BT(hi, x) << 1) | BT(lo, x);
    slice[revX] = cc;
    revX++;
  }
}

void tileTick(Fetcher *self) {
  switch (self->fetcherState) {
    case 0:
      self->fetcherState = 1;
      break;

    case 1:
      self->staticLine = self->mmio->LY + self->mmio->SCY;
      self->tileNum =
          ppuReadByte(getTileMapBase(self->mmio, self->isWindowVisible) +
                          ((self->internalX + internalY(self)) & 0x3FF),
                      false);
      self->fetcherState = 2;
      break;

    case 2:
      self->fetcherState = 3;
      break;

    case 3:
      self->tileDataAddr =
          getTileDataOffset(self, self->tileNum) + getTileRow(self);
      self->tileLo = ppuReadByte(self->tileDataAddr, false);
      self->fetcherState = 4;
      break;

    case 4:
      self->fetcherState = 5;
      break;

    case 5:
      fetchPixelSlice(self, self->tileLo,
                      ppuReadByte(self->tileDataAddr + 1, false), false);
      self->fetcherState = 6;
      if (self->firstInstance) {
        self->fetcherState = 0;
        self->firstInstance = false;
        return;
      }
      break;

    case 6:
      if (FIFOLen(&self->tFIFO.base) != 0) return;

      for (int i = 0; i < 8; i++) {
        Pixel px = {self->tileSlice[i], false, false};
        pushEntry(self->tFIFO, px);
      }
      self->internalX = (self->internalX + 1) % 32;
      self->fetcherState = 0;
      break;

    default:
      log_fatal("Invalid Tile Fetcher State.");
      break;
  }
}

void flipX(Fetcher *f) {
  int i = 7, j = 0;
  while (i > j) {
    uint8_t cc = f->spriteSlice[i];
    f->spriteSlice[i] = f->spriteSlice[j];
    f->spriteSlice[j] = cc;
    i--;
    j++;
  }
}

void spriteTick(Fetcher *self) {
  switch (self->spriteState) {
    case 0:
      // Ignore LSb of tall sprites.
      if (self->currSprite.height == 16) self->currSprite.tileNum &= 0xFE;

      uint16_t yOff =
          (uint16_t)self->mmio->LY - (uint16_t)self->currSprite.y + 16;

      // Vertical flip.
      if (BT(self->currSprite.flags, 6))
        yOff = self->currSprite.height - 1 - yOff;

      self->spriteDataAddr =
          0x8000 + ((uint16_t)self->currSprite.tileNum * 16) + (yOff * 2);
      self->spriteLo = ppuReadByte(self->spriteDataAddr, false);
      self->spriteState = 1;
      break;

    case 1:
      fetchPixelSlice(self, self->spriteLo,
                      ppuReadByte(self->spriteDataAddr + 1, false), true);
      self->spriteState = 2;
      break;
    case 2:
      // Horizontal flip.
      int counter = 0;
      if (self->currSprite.x < 8) counter = 8 - self->currSprite.x;
      if (BT(self->currSprite.flags, 5)) flipX(self);

      // If the Sprite FIFO already has some residual pixels left then we
      // have to mix both the new and the residual pixel at position I
      // using a transparency check.
      uint8_t residualAmount = FIFOLen(&self->sFIFO.base);

      for (; counter < 8; counter++) {
        uint8_t cc = self->spriteSlice[counter];
        Pixel px = {cc, BT(self->currSprite.flags, 4),
                    BT(self->currSprite.flags, 7)};

        if (residualAmount >= (counter + 1)) {
          Pixel residualPixel = getEntryAt(self->sFIFO, &self->sFIFO, counter);

          if ((residualPixel.colorCode == 0) && cc != 0) {
            setEntryAt(self->sFIFO, counter, px);
          }
        } else {
          pushEntry(self->sFIFO, px);
        }
      }

      popEntry(self->sprites, &self->sprites);

      self->spriteState = 0;
      self->tickingSprite = false;
      break;

    default:
      log_fatal("Invaid Sprite Fetcher State.");
      break;
  }
}

void fetchMain(Fetcher *self, uint16_t xPos) {
  self->fetcherState = 0;
  self->internalX = xPos;
  clearFIFO(&self->tFIFO.base);
}

void fetchBackground(Fetcher *self) {
  fetchMain(self, ((uint16_t)(self->mmio->SCX / 8) & 0x1F));
}

void fetchWindow(Fetcher *self) {
  fetchMain(self, ((self->mmio->WLY / 8) * 32) & 0x1F);
}

void clearSpriteFetcher(Fetcher *self) {
  clearFIFO(&self->sprites.base);
  clearFIFO(&self->sFIFO.base);
}

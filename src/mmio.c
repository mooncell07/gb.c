#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "joypad.h"
#include "mmio.h"
#include "types.h"
#include "utils.h"

MMIO *createMMIO() {
  MMIO *mmio = calloc(1, sizeof(MMIO));
  if (mmio != NULL) { mmio->booting = true; }
  return mmio;
}

void destroyMMIO(MMIO *self) { free(self); }

// clang-format off
uint8_t getRegister(MMIO *self, uint8_t address) {
    switch (address) {
        case 0x00: setKeyMask(); return js.keyMask;
        case 0x01: return self->SB;
        case 0x02: return self->SC;
        case 0x04: return MSB(self->DIV);
        case 0x05: return self->TIMA;
        case 0x06: return self->TMA;
        case 0x07: return self->TAC;
        case 0x0F: return self->IF;
        case 0x40: return self->LCDC;
        case 0x41: return self->STAT;
        case 0x42: return self->SCY;
        case 0x43: return self->SCX;
        case 0x44: return self->LY;
        case 0x45: return self->LYC;
        case 0x46: return self->DMA;
        case 0x47: return self->BGP;
        case 0x48: return self->OBP0;
        case 0x49: return self->OBP1;
        case 0x4A: return self->WY;
        case 0x4B: return self->WX;
        default: return 0xFF;
    }
}

void setRegister(MMIO *self, uint16_t address, uint8_t data) {
    switch (address) {
        case 0x00: js.P1 = data; break;
        case 0x01: self->SB = data; break;
        case 0x02: self->SC = data; break;
        case 0x04: self->DIV = 0; break;
        case 0x05: self->TIMA = data; break;
        case 0x06: self->TMA = data; break;
        case 0x07: self->TAC = data; break;
        case 0x0F: self->IF = data; break;
        case 0x40: self->LCDC = data; break;
        case 0x41: self->STAT = data; break;
        case 0x42: self->SCY = data; break;
        case 0x43: self->SCX = data; break;
        case 0x44: self->LY = 0; break;
        case 0x45: self->LYC = data; break;
        case 0x46: self->DMA = data; break;
        case 0x47: self->BGP = data; break;
        case 0x48: self->OBP0 = data; break;
        case 0x49: self->OBP1 = data; break;
        case 0x4A: self->WY = data; break;
        case 0x4B: self->WX = data; break;
        case 0x50: self->booting = false; break;
        default: break;
    }
}
// clang-format on

bool getLCDC(MMIO *self, LCDCType lct) { return BT(self->LCDC, lct); }

bool getLCDS(MMIO *self, LCDSType lst) { return BT(self->STAT, lst); }

uint16_t getTileMapBase(MMIO *self, bool isWin) {
  bool flag =
      isWin ? getLCDC(self, WINTILEMAPAREA) : getLCDC(self, BGTILEMAPAREA);
  return flag ? 0x9C00 : 0x9800;
}

uint16_t getTileDataBase(MMIO *self) {
  return getLCDC(self, BGANDWINTILEDATAAREA) ? 0x8000 : 0x9000;
}

void sendIntReq(MMIO *self, IntType ISR) { setBit(self->IF, ISR); }

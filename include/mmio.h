#pragma once
#include <stdbool.h>
#include <stdint.h>

#include "types.h"

typedef struct {
    uint16_t DIV;

    uint8_t SB;
    uint8_t SC;
    uint8_t TIMA;
    uint8_t TMA;
    uint8_t TAC;

    uint8_t LCDC;
    uint8_t STAT;
    uint8_t SCY;
    uint8_t SCX;
    uint8_t LY;
    uint8_t LYC;
    uint8_t DMA;
    uint8_t BGP;
    uint8_t OBP0;
    uint8_t OBP1;
    uint8_t WY;
    uint8_t WX;

    uint8_t LX;
    uint8_t WLY;

    uint8_t IF;
    uint8_t IE;

    bool booting;
} MMIO;

MMIO *createMMIO();
void destroyMMIO(MMIO *);

uint8_t getRegister(MMIO *, uint8_t);
void setRegister(MMIO *, uint16_t, uint8_t);
bool getLCDC(MMIO *, LCDCType);
bool getLCDS(MMIO *, LCDSType);
uint16_t getTileMapBase(MMIO *, bool);
uint16_t getTileDataBase(MMIO *);
void sendIntReq(MMIO *, IntType);

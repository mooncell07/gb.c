#pragma once
#include "clock.h"
#include <cartridge.h>
#include <mmio.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct Bus {
    Clock *clock;
    Cartridge *cart;
    MMIO *mmio;
} Bus;

Bus *createBus(Clock *, MMIO *, Cartridge *);
void destroyBus(Bus *);

uint8_t readByte(Bus *, uint16_t, bool, bool);
void writeByte(Bus *, uint16_t, uint8_t, bool);
void writeWord(Bus *, uint16_t, uint16_t);
void internal(Bus *);

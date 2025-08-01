#pragma once
#include <cartridge.h>
#include <mmio.h>
#include <stdbool.h>
#include <stdint.h>

#include "clock.h"


typedef struct{
  uint16_t address;
  uint8_t data;
} Cycles;

typedef struct {
#ifdef DEBUG
uint8_t *mem;
Cycles cycles[0xF];
int cyclesPtr;
#else
  Clock *clock;
  Cartridge *cart;
  MMIO *mmio;
#endif
} Bus;

#ifdef DEBUG
Bus *createBus();
#else
Bus *createBus(Clock *, MMIO *, Cartridge *);
#endif
void destroyBus(Bus *);

uint8_t read8(Bus *, uint16_t, bool, bool);
void write8(Bus *, uint16_t, uint8_t, bool);
void write16(Bus *, uint16_t, uint16_t);
void internal(Bus *);

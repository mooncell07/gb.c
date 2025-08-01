#pragma once
#include <stdint.h>

#include "bus.h"

typedef union {
  struct {
    uint8_t lo;
    uint8_t hi;
  };
  uint16_t pair;
} RegisterPair;

typedef struct {
  RegisterPair AF;
  RegisterPair BC;
  RegisterPair DE;
  RegisterPair HL;
  uint16_t PC;
  uint16_t SP;
} Registers;

typedef struct {
  Bus *bus;
  Registers regs;
  uint8_t opcode;
  bool halted;
  bool IME;
  bool IMERising;
} State;

typedef void (*fnptr)(State *self);
typedef struct {
  fnptr UNPREFIXED[0x100];
  fnptr PREFIXED[0x100];
  State state;
} Backend;

void push(State *self, uint16_t data);
Backend createBackend(Bus *bus);
void backendTick(Backend *);

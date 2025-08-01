#pragma once
#include <backend.h>
#include <bus.h>
#include <stdbool.h>
#include <stdint.h>
#include "types.h"

typedef struct {
  Bus *bus;
  Backend backend;
} CPU;

CPU createCPU(Bus *);
void cpuTick(CPU *);

void serviceIRQ(Backend *, uint8_t *, IntType);
void handlePendingIRQs(Backend *, uint8_t *, uint8_t);
void checkPendingIRQs(Backend *, uint8_t *, uint8_t);

void handleInterrupts(Backend *, uint8_t *, uint8_t);

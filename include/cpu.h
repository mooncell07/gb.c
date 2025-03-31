#pragma once
#include <bus.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct CPU {
  Bus *bus;
  uint8_t opcode;
  bool halted;
  bool IME;
  bool IMERising;
} CPU;

CPU createCPU(Bus *);
void destroyCPU(CPU *);
void getSerialOutput(CPU *);
void checkPendingIRQs(CPU *);
void cpuTick(CPU *);

#include <stdbool.h>
#include <stdint.h>

#include "backend.h"
#include "bus.h"

#ifdef DEBUG

typedef struct {
  uint16_t address;
  uint8_t data;
} RAM;

typedef struct {
  bool initial;
  uint16_t PC;
  uint16_t SP;
  uint8_t A;
  uint8_t F;
  uint8_t B;
  uint8_t C;
  uint8_t D;
  uint8_t E;
  uint8_t H;
  uint8_t L;
  RAM ram[0xF];
  size_t ramSize;
} Config;

typedef struct {
  Cycles cycles[0xF];
  size_t count;
} ConfigCycles;

#define regs backend->state.regs

void loadConfig(Backend *backend, Bus *bus, Config initial) {
  regs.AF.hi = initial.A;
  regs.AF.lo = initial.F;
  regs.BC.hi = initial.B;
  regs.BC.lo = initial.C;
  regs.DE.hi = initial.D;
  regs.DE.lo = initial.E;
  regs.HL.hi = initial.H;
  regs.HL.lo = initial.L;
  regs.PC = initial.PC;
  regs.SP = initial.SP;

  for (size_t i = 0; i < initial.ramSize; i++) {
    RAM ramData = initial.ram[i];
    write8(bus, ramData.address, ramData.data, false);
  }
}

#define logReg8State(name, actual, expected) \
  printf("%s: %02X:%02X\n", name, actual, expected)
#define logReg16State(name, actual, expected) \
  printf("%s: %04X:%04X\n", name, actual, expected)
#define logMemState(name, actual, expected)                           \
  printf("%s: Address: %04X Data: %02X:%02X\n", name, actual.address, \
         actual.data, expected.data)

void dumpStates(Backend *backend, Bus *bus, Config final, ConfigCycles cycles) {
  logReg16State("PC", regs.PC, final.PC);
  logReg16State("SP", regs.SP, final.SP);
  logReg8State("A", regs.AF.hi, final.A);
  logReg8State("F", regs.AF.lo, final.F);
  logReg8State("B", regs.BC.hi, final.B);
  logReg8State("C", regs.BC.lo, final.C);
  logReg8State("D", regs.DE.hi, final.D);
  logReg8State("E", regs.DE.lo, final.E);
  logReg8State("H", regs.HL.hi, final.H);
  logReg8State("L", regs.HL.lo, final.L);

  for (size_t i = 0; i < final.ramSize; i++) {
    RAM expectedRam = final.ram[i];
    RAM actualRam = {.address = expectedRam.address,
                     .data = read8(bus, expectedRam.address, false, false)};
    logMemState("RAM", actualRam, expectedRam);
  }
  for (size_t i = 0; i < cycles.count; i++) {
    Cycles expectedCycles = cycles.cycles[i];
    Cycles actualCycles = bus->cycles[i];
    logMemState("Cycles", actualCycles, expectedCycles);
  }
}

bool matchStates(Backend *backend, Bus *bus, Config final,
                 ConfigCycles cycles) {
  bool regsTest = (regs.PC == final.PC) && (regs.AF.hi == final.A) &&
                  (regs.AF.lo == final.F) && (regs.BC.hi == final.B) &&
                  (regs.BC.lo == final.C) && (regs.DE.hi == final.D) &&
                  (regs.DE.lo == final.E) && (regs.HL.hi == final.H) &&
                  (regs.HL.lo == final.L) && (regs.SP == final.SP);

  bool ramTest = false;

  for (int i = 0; i < final.ramSize; i++) {
    RAM expectedRam = final.ram[i];
    RAM actualRam = {.address = expectedRam.address,
                     .data = read8(bus, expectedRam.address, false, false)};
    ramTest = (expectedRam.data == actualRam.data);
  }

  bool cyclesTest = false;

  for (int i = 0; i < cycles.count; i++) {
    Cycles expectedCycles = cycles.cycles[i];
    Cycles actualCycles = bus->cycles[i];

    cyclesTest = (expectedCycles.address == actualCycles.address) &&
                 (expectedCycles.data == actualCycles.data);
  }

  return regsTest && cyclesTest && ramTest;
}
#undef regs

int runTest(Config initial, Config final, ConfigCycles cycles) {
  Bus *bus = createBus();
  Backend backend = createBackend(bus);

  loadConfig(&backend, bus, initial);
  backendTick(&backend);

  int status = 0;
  if (!matchStates(&backend, bus, final, cycles)) {
    printf("%s\n", "---------Test Failed----------");
    dumpStates(&backend, bus, final, cycles);
    status = 1;
  }

  destroyBus(bus);
  return status;
}
#endif

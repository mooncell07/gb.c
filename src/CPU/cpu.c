#include <stdbool.h>
#include <stdint.h>

#include "backend.h"
#include "bus.h"
#include "cpu.h"
#include "log.h"
#include "utils.h"

#define state self->backend.state

CPU createCPU(Bus *bus) {
  Backend backend = createBackend(bus);
  CPU cpu = {
      .bus = bus,
      .backend = backend,
  };
  return cpu;
}

#define PC_READ_ONLY(offset) \
  read8(self->bus, state.regs.PC + offset, false, true)

void dumpState(CPU *self) {
  uint8_t aa = PC_READ_ONLY(0), bb = PC_READ_ONLY(1);
  uint8_t cc = PC_READ_ONLY(2), dd = PC_READ_ONLY(3);
  log_trace(
      "A:%02X F:%02X B:%02X C:%02X D:%02X E:%02X H:%02X L:%02X SP:%04X PC:%04X "
      "PCMEM:%02X,%02X,%02X,%02X",
      state.regs.AF.hi, state.regs.AF.lo, state.regs.BC.hi, state.regs.BC.lo,
      state.regs.DE.hi, state.regs.DE.lo, state.regs.HL.hi, state.regs.HL.lo,
      state.regs.SP, state.regs.PC, aa, bb, cc, dd);
}

void readSerialOutput(State *self) {
  if (read8(self->bus, 0xFF02, false, true) == 0x81) {
    printf("%c", (char)read8(self->bus, 0xFF01, false, true));
    write8(self->bus, 0xFF02, 0, false);
  }
}

void cpuTick(CPU *self) {
  backendTick(&self->backend);
  readSerialOutput(&state);
  // dumpState(self);
}

#undef state

const uint16_t IVT[5] = {0x40, 0x48, 0x50, 0x58, 0x60};

void serviceIRQ(Backend *backend, uint8_t *intFlag, IntType intT) {
  backend->state.IME = false;
  backend->state.IMERising = false;
  clearBit(*intFlag, intT);
  push(&backend->state, IVT[intT]);
  backend->state.regs.PC = IVT[intT];
}

void handlePendingIRQs(Backend *backend, uint8_t *intFlag, uint8_t intEn) {
  for (int i = 0; i < 5; i++) {
    if (BT(*intFlag, i) && BT(intEn, i)) {
      serviceIRQ(backend, intFlag, (IntType)i);
    }
  }
}

void checkPendingIRQs(Backend *backend, uint8_t *intFlag, uint8_t intEn) {
  if (*intFlag == 0) { return; }
  backend->state.halted = false;
  if (backend->state.IME) { handlePendingIRQs(backend, intFlag, intEn); }
}

void handleInterrupts(Backend *backend, uint8_t *intFlag, uint8_t intEn) {
  checkPendingIRQs(backend, intFlag, intEn);
  if (backend->state.IMERising == true) { backend->state.IME = true; }
}

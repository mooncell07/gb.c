#include <stdlib.h>

#include "bus.h"
#include "cartridge.h"
#include "clock.h"
#include "cpu.h"
#include "lcd.h"
#include "log.h"
#include "mmio.h"
#include "ppu.h"

typedef struct {
  bool *running;
  Cartridge *cartridge;
  Bus *bus;
  MMIO *mmio;
  PPU *ppu;
  Clock *clock;
  CPU cpu;
} Emulator;

void destroyEmulator(Emulator *self);

static inline void terminate(int status) {
  log_info("Exiting Now.");
  exit(status);
}

static void failIfNull(Emulator *self, void *component, char name[]) {
  if (component == NULL) {
    log_fatal("Failed to initialize `%s`.", name);
    destroyEmulator(self);
    terminate(EXIT_FAILURE);
  } else {
    log_info("Initialized `%s`.", name);
  }
}

Emulator createEmulator(char path[]) {
  if (initLCD()) {
    log_fatal("Failed to initialize `LCD`.");
    terminate(EXIT_FAILURE);
  }

  Emulator emu = {0};
  emu.cartridge = createCartridge(path);
  failIfNull(&emu, emu.cartridge, "Cartridge");
  emu.mmio = createMMIO();
  failIfNull(&emu, emu.mmio, "MMIO");
  emu.ppu = createPPU(emu.mmio);
  failIfNull(&emu, emu.ppu, "PPU");
  emu.clock = createClock(emu.ppu, emu.mmio);
  failIfNull(&emu, emu.clock, "Clock");
  emu.bus = createBus(emu.clock, emu.mmio, emu.cartridge);
  failIfNull(&emu, emu.bus, "Bus");
  emu.cpu = createCPU(emu.bus);

  return emu;
}

void destroyEmulator(Emulator *self) {
  if (self->cartridge != NULL) destroyCartridge(self->cartridge);
  if (self->bus != NULL) destroyBus(self->bus);
  if (self->mmio != NULL) destroyMMIO(self->mmio);
  if (self->ppu != NULL) destroyPPU(self->ppu);
  if (self->clock != NULL) destroyClock(self->clock);
  destroyLCD();
}

void step(Emulator *self) {
  if (!self->cpu.backend.state.halted) {
    cpuTick(&self->cpu);
  } else {
    tickComponents(self->clock, 1);
  }

  handleInterrupts(&self->cpu.backend, &self->bus->mmio->IF,
                   self->bus->mmio->IE);
}

int main(int argc, char **argv) {
  log_set_level(1);
  if (argc == 0) {
    log_fatal("Pass path to the rom file.");
    terminate(EXIT_FAILURE);
  }

  Emulator emu;

  emu = createEmulator(argv[1]);
  log_info("Initialization Complete.");
  while (running) { step(&emu); }
  destroyEmulator(&emu);
  terminate(EXIT_SUCCESS);
}

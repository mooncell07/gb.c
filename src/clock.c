#include <stdlib.h>

#include "clock.h"
#include "mmio.h"
#include "ppu.h"
#include "timer.h"

Clock *createClock(PPU *ppu, MMIO *mmio) {
  Clock *clock = malloc(sizeof(Clock));
  if (clock != NULL) {
    clock->mmio = mmio;
    clock->ppu = ppu;
  }
  return clock;
}

void destroyClock(Clock *self) { free(self); }

void tickComponents(Clock *self, int m) {
  for (int i = 0; i < (m * 4); i++) {
    timerTick(self->mmio);
    ppuTick(self->ppu);
  }
}

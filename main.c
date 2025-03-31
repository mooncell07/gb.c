#include "bus.h"
#include "cartridge.h"
#include "clock.h"
#include "cpu.h"
#include "lcd.h"
#include "logger.h"
#include "mmio.h"
#include "ppu.h"

typedef struct {
    Cartridge *cartridge;
    Bus *bus;
    MMIO *mmio;
    PPU *ppu;
    Clock *clock;
    CPU cpu;
} Emulator;

Emulator createEmulator(char path[]) {
    initLCD();

    Emulator emu = {0};

    emu.cartridge = createCartridge(path);
    emu.mmio = createMMIO();
    emu.ppu = createPPU(emu.mmio);
    emu.clock = createClock(emu.ppu, emu.mmio);

    emu.bus = createBus(emu.clock, emu.mmio, emu.cartridge);
    emu.cpu = createCPU(emu.bus);

    logState(INFO, "gb.c is ready.");
    return emu;
}

void destroyEmulator(Emulator *self) {
    logState(INFO, "bye");
    destroyCartridge(self->cartridge);
    destroyBus(self->bus);
    destroyMMIO(self->mmio);
    destroyPPU(self->ppu);
    destroyClock(self->clock);
    destroyLCD();
}

void step(Emulator *self) {
    if (!self->cpu.halted) {
        cpuTick(&self->cpu);
        // getSerialOutput(&self->cpu);
    } else {
        tickComponents(self->clock, 1);
    }

    checkPendingIRQs(&self->cpu);
    if (self->cpu.IMERising) {
        self->cpu.IME = true;
    }
}

int main(int argc, char **argv) {
    Emulator emu;

    emu = createEmulator(argv[1]);
    while (RUNNING) {
        step(&emu);
    }
    destroyEmulator(&emu);
    return 0;
}

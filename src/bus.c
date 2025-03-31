
#include "bus.h"
#include <stdbool.h>
#include <stdint.h>

#include "cartridge.h"
#include "clock.h"
#include "mappers.h"
#include "mmio.h"
#include "utils.h"
#include "vbus.h"
#include <stdlib.h>

typedef struct {
    bool active;
    uint8_t currentByte;
    uint8_t currentIndex;
    bool starting;
    uint8_t initialDelay;
} DMA;

void dma_tick(Bus *bus, DMA *d);
DMA dma = {0};

Bus *createBus(Clock *clock, MMIO *mmio, Cartridge *cart) {
    Bus *bus = malloc(sizeof(Bus));
    bus->clock = clock;
    bus->mmio = mmio;
    bus->cart = cart;

    return bus;
}

void destroyBus(Bus *bus) { free(bus); }

void busCycle(Bus *self, bool incr) {
    if (!incr) {
        return;
    }

    if (dma.active) {
        dma_tick(self, &dma);
    }

    tickComponents(self->clock, 1);

    if (dma.starting) {
        if (dma.initialDelay > 1) {
            dma.initialDelay -= 1;
        } else {
            dma.starting = false;
            dma.active = true;
        }
    }
}

uint8_t readByte(Bus *self, uint16_t address, bool incr, bool conflict) {
    uint8_t result = 0;
    if (dma.active && !conflict && address >= 0xFE00) {
        return self->cart->mapper.wram[(address - 0xFE00) + 0x1E00];
    }

    else if ((address <= 0x7FFF) || BOUND(address, 0xA000, 0xBFFF)) {
        if (self->mmio->booting && (address <= 0x00FF)) {

            result = self->cart->mapper.bootRom[address];
        } else {
            result = romRead(self->cart, address);
        }
    } else if (BOUND(address, 0x8000, 0x9FFF)) {
        result = ppuReadByte(address, false);
    } else if (BOUND(address, 0xC000, 0xDFFF)) {
        result = self->cart->mapper.wram[address - 0xC000];
    } else if (BOUND(address, 0xE000, 0xFDFF)) {
        result = self->cart->mapper.wram[address - 0xE000];
    } else if (BOUND(address, 0xFE00, 0xFE9F)) {
        if (dma.active && conflict) {
            result = 0xFF;
        } else {
            result = ppuReadByte(address, true);
        }
    } else if (BOUND(address, 0xFF00, 0xFF7F)) {
        result = getRegister(self->mmio, address & 0xFF);
    } else if (BOUND(address, 0xFF80, 0xFFFE)) {
        result = self->cart->mapper.hram[address - 0xFF80];
    } else if (address == 0xFFFF) {
        result = self->mmio->IE;
    }

    busCycle(self, incr);
    return result;
}

void writeByte(Bus *self, uint16_t address, uint8_t data, bool incr) {
    if ((address <= 0x7FFF) || BOUND(address, 0xA000, 0xBFFF)) {
        romWrite(self->cart, address, data);
    }

    else if (BOUND(address, 0x8000, 0x9FFF)) {
        ppuWriteByte(address, data, false);
    }

    else if (BOUND(address, 0xC000, 0xDFFF)) {
        self->cart->mapper.wram[address - 0xC000] = data;
    } else if (BOUND(address, 0xE000, 0xFDFF)) {
        self->cart->mapper.wram[address - 0xE000] = data;
    } else if (BOUND(address, 0xFE00, 0xFE9F)) {
        ppuWriteByte(address, data, true);
    }

    else if (BOUND(address, 0xFF00, 0xFF7F)) {
        setRegister(self->mmio, address & 0xFF, data);
        if (address == 0xFF46) {
            dma.starting = true;
            dma.initialDelay = 2;
        }
    } else if (BOUND(address, 0xFF80, 0xFFFE)) {
        self->cart->mapper.hram[address - 0xFF80] = data;
    } else if (address == 0xFFFF) {
        self->mmio->IE = data;
    }

    busCycle(self, incr);
}

void writeWord(Bus *self, uint16_t address, uint16_t data) {
    writeByte(self, address, LSB(data), true);
    writeByte(self, address + 1, MSB(data), true);
}

void internal(Bus *self) { busCycle(self, true); }

void dma_tick(Bus *bus, DMA *d) {
    uint16_t addr = (uint16_t)bus->mmio->DMA << 8;
    d->currentByte =
        readByte(bus, addr + (uint16_t)d->currentIndex, false, false);
    ppuWriteByte(0xFE00 + (uint16_t)d->currentIndex, d->currentByte, true);
    d->currentIndex++;

    if (d->currentIndex == 0xA0) {
        d->active = false;
        d->initialDelay = 0;
        d->currentIndex = 0;
    }
}

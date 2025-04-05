#pragma once
#include <mappers.h>
#include <stdint.h>

typedef struct CartridgeHeaders {
    char title[16];
    char licensee[8];
    char romType[16];
    uint8_t romVersion;
    uint8_t destinationCode;
    uint8_t checksum;
    int romSize;
} CartridgeHeaders;

typedef struct Cartridge {
    CartridgeHeaders cartridgeHeaders;
    Mapper mapper;

} Cartridge;

uint8_t romRead(Cartridge *, uint16_t);
void romWrite(Cartridge *, uint16_t, uint8_t);

Cartridge *createCartridge(char[]);
void destroyCartridge(Cartridge *);

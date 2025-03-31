#pragma once
#include <mappers.h>
#include <stdint.h>

typedef struct CartridgeHeaders {
    char title[60];
    char licensee[60];
    char romType[60];
    int romSize;
    uint8_t romVersion;
    uint8_t destinationCode;
    uint8_t checksum;
} CartridgeHeaders;

typedef struct Cartridge {
    CartridgeHeaders cartridgeHeaders;
    Mapper mapper;

} Cartridge;

uint8_t romRead(Cartridge *, uint16_t);
void romWrite(Cartridge *, uint16_t, uint8_t);

Cartridge *createCartridge(char[]);
void destroyCartridge(Cartridge *);

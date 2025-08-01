#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "cartridge.h"
#include "log.h"
#include "mappers.h"
#include "types.h"
#include "utils.h"

int readBinary(char filepath[], uint8_t **buffer) {
  FILE *file = fopen(filepath, "rb");

  if (!file) {
    log_fatal("Failed to open file.");
    return 1;
  }

  fseek(file, 0, SEEK_END);
  size_t fileSize = ftell(file);
  rewind(file);
  *buffer = (uint8_t *)malloc(fileSize);

  if (!*buffer) {
    log_fatal("Failed to allocate memory for file.");
    return 1;
  }
  size_t actualSize = fread(*buffer, 1, fileSize, file);

  if (actualSize != fileSize) {
    log_fatal("Failed to read file.");
    return 1;
  }

  fclose(file);
  return 0;
}

uint8_t romRead(Cartridge *self, uint16_t address) {
  return mapperRead(&self->mapper, address);
}

void romWrite(Cartridge *self, uint16_t address, uint8_t data) {
  mapperWrite(&self->mapper, address, data);
}

void runChecksum(Cartridge *self) {
  uint8_t checksum = 0;

  for (int i = 0x0134; i <= 0x014C; i++) {
    checksum = checksum - self->mapper.romData[i] - 1;
  }

  if ((LSB(checksum)) == self->mapper.romData[0x014D]) {
    self->cartridgeHeaders.checksum = checksum;
    log_info("Checksum Passed.");
  } else {
    log_error("Checksum Failed.");
  }
}

int loadMemory(Cartridge *self, char filepath[]) {
  log_info("Loading `ROM`.");
  int status_rom = readBinary(filepath, &self->mapper.romData);
  if (status_rom) { return status_rom; }
  log_info("Loading `Bootrom`.");
  int status_bootrom = readBinary("./roms/bootrom.gb", &self->mapper.bootRom);
  if (status_bootrom) { return status_bootrom; }
  return 0;
}

void parseHeaders(Cartridge *self) {
  int romSize = 32 << (int)self->mapper.romData[0x0148];
  self->mapper.romBankCount = (uint8_t)(romSize / 16);
  self->mapper.ramSizeCode = self->mapper.romData[0x0149];
}

const int RAMBANKS[6] = {0, 1, 2, 4, 16, 8};

bool buildExternalMemory(Cartridge *self) {
  int totalSize = RAMBANKS[self->mapper.ramSizeCode] * 0x2000;
  self->mapper.extRam = (uint8_t *)malloc(totalSize * sizeof(uint8_t));
  return self->mapper.extRam != NULL;
}

void freeMemory(Mapper *mapper) {
  if (mapper->romData != NULL) {
    free(mapper->romData);
    mapper->romData = NULL;
  }

  if (mapper->bootRom != NULL) {
    free(mapper->bootRom);
    mapper->bootRom = NULL;
  }

  if (mapper->extRam != NULL) {
    free(mapper->extRam);
    mapper->extRam = NULL;
  }
}

Cartridge *createCartridge(char filepath[]) {
  Cartridge *self = malloc(sizeof(Cartridge));
  if (self == NULL) { return NULL; }

  CartridgeHeaders cartHeaders = {0};
  self->cartridgeHeaders = cartHeaders;

  MBC1 mbc1 = {1, 1, false};
  self->mapper.mbc1 = mbc1;
  self->mapper.mbcType = MBCt1;

  if (loadMemory(self, filepath)) { return NULL; }

  parseHeaders(self);
  runChecksum(self);

  buildExternalMemory(self);

  return self;
}

void destroyCartridge(Cartridge *self) {
  freeMemory(&self->mapper);
  free(self);
}

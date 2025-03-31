
#include "cartridge.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logger.h"
#include "mappers.h"
#include "types.h"
#include "utils.h"

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

void fileError(char msg[]) {
  logState(FATAL, msg);
  // At this point in time only the romData and bootRom are allocated.
  exit(1);
}

int readBinary(char filepath[], uint8_t **buffer) {
  FILE *file = fopen(filepath, "rb");

  if (!file) {
    fileError("Failed to open file.");
    return -1;
  }

  fseek(file, 0, SEEK_END);
  size_t fileSize = ftell(file);
  rewind(file);
  *buffer = (uint8_t *)malloc(fileSize);

  if (!*buffer) {
    fileError("Failed to allocate memory for file.");
    return -1;
  }
  size_t actualSize = fread(*buffer, 1, fileSize, file);

  if (actualSize != fileSize) {
    fileError("Failed to read file.");
    return -1;
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

void _runChecksum(Cartridge *self) {
  uint8_t checksum = 0;

  for (int i = 0x0134; i <= 0x014C; i++) {
    checksum = checksum - self->mapper.romData[i] - 1;
  }

  if ((LSB(checksum)) == self->mapper.romData[0x014D]) {
    self->cartridgeHeaders.checksum = checksum;
    logState(INFO, "Checksum Passed.");
  } else {
    logState(FATAL, "Checksum Failed.");
  }
}

void _loadMemory(Cartridge *self, char filepath[]) {
  logState(INFO, "Loading ROM...");
  int result = readBinary(filepath, &self->mapper.romData);
  if (result != 0 || self->mapper.romData == NULL) {
    logState(FATAL, "Failed to load ROM.");
    return;
  }

  logState(INFO, "Loading Boot ROM...");
  int result2 = readBinary("../roms/bootrom.gb", &self->mapper.bootRom);

  if (result2 != 0 || self->mapper.bootRom == NULL) {
    logState(FATAL, "Failed to load Boot ROM.");
  }
}

void _parseHeaders(Cartridge *self) {
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

Cartridge *createCartridge(char filepath[]) {

  Cartridge *self = malloc(sizeof(Cartridge));

  CartridgeHeaders cartHeaders = {0};
  self->cartridgeHeaders = cartHeaders;

  MBC1 mbc1 = {1, 1, false};
  self->mapper.mbc1 = mbc1;
  self->mapper.mbcType = MBCt1;

  _loadMemory(self, filepath);
  _parseHeaders(self);
  _runChecksum(self);

  buildExternalMemory(self);

  return self;
}

void destroyCartridge(Cartridge *self) {
  freeMemory(&self->mapper);
  free(self);
}

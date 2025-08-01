#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <types.h>

typedef struct {
  uint8_t romBank;
  uint8_t ramBank;
  bool modeFlag;
} MBC1;

typedef struct Mapper {
  uint8_t wram[0x2000];
  uint8_t hram[0x80];

  uint8_t *bootRom;
  uint8_t *romData;
  uint8_t *extRam;

  MBC1 mbc1;
  uint8_t ramSizeCode;
  uint8_t romBankCount;
  bool extRamFlag;
  MBCType mbcType;
} Mapper;

uint8_t mapperRead(Mapper *, uint16_t);
void mapperWrite(Mapper *, uint16_t, uint8_t);

#include <stdbool.h>
#include <stdint.h>

#include "mappers.h"
#include "utils.h"

uint8_t _mbc1Read(Mapper *mapper, uint16_t address) {
  if (address <= 0x3FFF) {
    return mapper->romData[address];
  }

  else if (BOUND(address, 0x4000, 0x7FFF)) {
    uint16_t highBankNum = mapper->mbc1.romBank & (mapper->romBankCount - 1);
    uint32_t addr = 0x4000 * highBankNum + (address - 0x4000);
    return mapper->romData[addr];
  }

  else if (BOUND(address, 0xA000, 0xBFFF)) {
    if (mapper->extRamFlag) {
      if ((mapper->ramSizeCode > 0x02) && mapper->mbc1.modeFlag) {
        return mapper
            ->extRam[0x2000 * mapper->mbc1.ramBank + (address - 0xA000)];
      } else {
        return mapper->extRam[(address - 0xA000)];
      }
    } else {
      return 0xFF;
    }
  } else {
    return 0xFF;
  }
}

void _mbc1Write(Mapper *mapper, uint16_t address, uint8_t data) {
  if (address <= 0x1FFF) {
    mapper->extRamFlag = BEXTR(data, 0, 3) == 0xA;
  } else if (BOUND(address, 0x2000, 0x3FFF)) {
    uint8_t maskedData = data & 0x1F;
    mapper->mbc1.romBank = maskedData % mapper->romBankCount;
    if ((mapper->mbc1.romBank == 0) && (maskedData < mapper->romBankCount)) {
      mapper->mbc1.romBank = 1;
    }
  } else if (BOUND(address, 0x4000, 0x5FFF)) {
    mapper->mbc1.ramBank = BEXTR(data, 0, 1);
  } else if (BOUND(address, 0x6000, 0x7FFF)) {
    mapper->mbc1.modeFlag = BT(data, 0);
  } else if (BOUND(address, 0xA000, 0xBFFF)) {
    if (mapper->extRamFlag) {
      if ((mapper->ramSizeCode > 0x02) && mapper->mbc1.modeFlag) {
        mapper->extRam[0x2000 * mapper->mbc1.ramBank + (address - 0xA000)] =
            data;
      } else {
        mapper->extRam[(address - 0xA000)] = data;
      }
    }
  }
}

uint8_t mapperRead(Mapper *self, uint16_t address) {
  return _mbc1Read(self, address);
}

void mapperWrite(Mapper *self, uint16_t address, uint8_t data) {
  _mbc1Write(self, address, data);
}

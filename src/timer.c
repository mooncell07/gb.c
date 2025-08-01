#include <stdbool.h>
#include <stdint.h>

#include "mmio.h"
#include "timer.h"
#include "types.h"
#include "utils.h"
static uint8_t CLKSEL[4] = {9, 3, 5, 7};

void timerTick(MMIO *mmio) {
  uint16_t oldDIV = mmio->DIV;
  uint8_t freq = CLKSEL[mmio->TAC & 0x03];

  mmio->DIV++;

  if (BT(mmio->DIV, freq) && (!BT(oldDIV, freq)) && BT(mmio->TAC, 2)) {
    if (mmio->TIMA == 0xFF) {
      mmio->TIMA = mmio->TMA;
      sendIntReq(mmio, INTTIMER);
    }
    mmio->TIMA++;
  }
}

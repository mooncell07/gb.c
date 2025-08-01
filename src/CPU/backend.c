#define __AUTOGEN__

// This file is auto generated, do not modify it manually.
// All the changes go in `codegen` package and/or the `export.sh` script.

#include "backend.h"

#include <stdint.h>
#include <sys/types.h>

#include "utils.h"

#define AFp self->regs.AF.pair
#define BCp self->regs.BC.pair
#define DEp self->regs.DE.pair
#define HLp self->regs.HL.pair
#define SP self->regs.SP
#define PC self->regs.PC

#define A self->regs.AF.hi
#define F self->regs.AF.lo
#define B self->regs.BC.hi
#define C self->regs.BC.lo
#define D self->regs.DE.hi
#define E self->regs.DE.lo
#define H self->regs.HL.hi
#define L self->regs.HL.lo

#define COPY_FLAG(v, pos) F = (F & ~(1 << pos)) | (((v) & 1) << pos)

#define Cf(v) COPY_FLAG(v, ftC)
#define Hf(v) COPY_FLAG(v, ftH)
#define Nf(v) COPY_FLAG(v, ftN)
#define Zf(v) COPY_FLAG(v, ftZ)

#define getFlag(ft) BT(F, ft)

static inline uint8_t next8(State *self) {
  return read8(self->bus, PC++, true, true);
}

static inline uint16_t next16(State *self) {
  uint8_t lo = next8(self);
  uint8_t hi = next8(self);
  return JOIN(hi, lo);
}

static inline uint16_t pop(State *self) {
  uint8_t lo = read8(self->bus, SP++, true, true);
  uint8_t hi = read8(self->bus, SP++, true, true);
  return JOIN(hi, lo);
}

void push(State *self, uint16_t data) {
  write8(self->bus, --SP, MSB(data), true);
  write8(self->bus, --SP, LSB(data), true);
}

// --------------- OPCODE HANDLERS ---------------

static void opINC_BC(State *self) {
  uint16_t r1 = BCp, src = 1;
  uint16_t result = r1 + src;
  BCp = result;
  internal(self->bus);
}

static void opINC_B(State *self) {
  uint8_t r1 = B, src = 1;
  uint8_t result = r1 + src;
  B = result;

  Zf(!result);
  Nf(false);
  Hf((r1 & 0xF) + (src & 0xF) >= 0x10);
}

static void opDEC_B(State *self) {
  uint8_t r1 = B, src = 1;
  uint8_t result = r1 - src;
  B = result;

  Zf(!result);
  Nf(true);
  Hf((r1 & 0xF) < (src & 0xF));
}

static void opADD_HL_BC(State *self) {
  uint16_t r1 = HLp, src = BCp;
  uint16_t result = r1 + src;
  HLp = result;
  internal(self->bus);

  Nf(false);
  Hf((r1 & 0xFFF) + (src & 0xFFF) > 0xFFF);
  Cf(((uint32_t)r1 + (uint32_t)src) > 0xFFFF);
}

static void opDEC_BC(State *self) {
  uint16_t r1 = BCp, src = 1;
  uint16_t result = r1 - src;
  BCp = result;
  internal(self->bus);
}

static void opINC_C(State *self) {
  uint8_t r1 = C, src = 1;
  uint8_t result = r1 + src;
  C = result;

  Zf(!result);
  Nf(false);
  Hf((r1 & 0xF) + (src & 0xF) >= 0x10);
}

static void opDEC_C(State *self) {
  uint8_t r1 = C, src = 1;
  uint8_t result = r1 - src;
  C = result;

  Zf(!result);
  Nf(true);
  Hf((r1 & 0xF) < (src & 0xF));
}

static void opINC_DE(State *self) {
  uint16_t r1 = DEp, src = 1;
  uint16_t result = r1 + src;
  DEp = result;
  internal(self->bus);
}

static void opINC_D(State *self) {
  uint8_t r1 = D, src = 1;
  uint8_t result = r1 + src;
  D = result;

  Zf(!result);
  Nf(false);
  Hf((r1 & 0xF) + (src & 0xF) >= 0x10);
}

static void opDEC_D(State *self) {
  uint8_t r1 = D, src = 1;
  uint8_t result = r1 - src;
  D = result;

  Zf(!result);
  Nf(true);
  Hf((r1 & 0xF) < (src & 0xF));
}

static void opADD_HL_DE(State *self) {
  uint16_t r1 = HLp, src = DEp;
  uint16_t result = r1 + src;
  HLp = result;
  internal(self->bus);

  Nf(false);
  Hf((r1 & 0xFFF) + (src & 0xFFF) > 0xFFF);
  Cf(((uint32_t)r1 + (uint32_t)src) > 0xFFFF);
}

static void opDEC_DE(State *self) {
  uint16_t r1 = DEp, src = 1;
  uint16_t result = r1 - src;
  DEp = result;
  internal(self->bus);
}

static void opINC_E(State *self) {
  uint8_t r1 = E, src = 1;
  uint8_t result = r1 + src;
  E = result;

  Zf(!result);
  Nf(false);
  Hf((r1 & 0xF) + (src & 0xF) >= 0x10);
}

static void opDEC_E(State *self) {
  uint8_t r1 = E, src = 1;
  uint8_t result = r1 - src;
  E = result;

  Zf(!result);
  Nf(true);
  Hf((r1 & 0xF) < (src & 0xF));
}

static void opINC_HL(State *self) {
  uint16_t r1 = HLp, src = 1;
  uint16_t result = r1 + src;
  HLp = result;
  internal(self->bus);
}

static void opINC_H(State *self) {
  uint8_t r1 = H, src = 1;
  uint8_t result = r1 + src;
  H = result;

  Zf(!result);
  Nf(false);
  Hf((r1 & 0xF) + (src & 0xF) >= 0x10);
}

static void opDEC_H(State *self) {
  uint8_t r1 = H, src = 1;
  uint8_t result = r1 - src;
  H = result;

  Zf(!result);
  Nf(true);
  Hf((r1 & 0xF) < (src & 0xF));
}

static void opADD_HL_HL(State *self) {
  uint16_t r1 = HLp, src = HLp;
  uint16_t result = r1 + src;
  HLp = result;
  internal(self->bus);

  Nf(false);
  Hf((r1 & 0xFFF) + (src & 0xFFF) > 0xFFF);
  Cf(((uint32_t)r1 + (uint32_t)src) > 0xFFFF);
}

static void opDEC_HL(State *self) {
  uint16_t r1 = HLp, src = 1;
  uint16_t result = r1 - src;
  HLp = result;
  internal(self->bus);
}

static void opINC_L(State *self) {
  uint8_t r1 = L, src = 1;
  uint8_t result = r1 + src;
  L = result;

  Zf(!result);
  Nf(false);
  Hf((r1 & 0xF) + (src & 0xF) >= 0x10);
}

static void opDEC_L(State *self) {
  uint8_t r1 = L, src = 1;
  uint8_t result = r1 - src;
  L = result;

  Zf(!result);
  Nf(true);
  Hf((r1 & 0xF) < (src & 0xF));
}

static void opINC_SP(State *self) {
  uint16_t r1 = SP, src = 1;
  uint16_t result = r1 + src;
  SP = result;
  internal(self->bus);
}

static void opINC_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true), src = 1;
  uint8_t result = r1 + src;
  write8(self->bus, HLp, result, true);

  Zf(!result);
  Nf(false);
  Hf((r1 & 0xF) + (src & 0xF) >= 0x10);
}

static void opDEC_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true), src = 1;
  uint8_t result = r1 - src;
  write8(self->bus, HLp, result, true);

  Zf(!result);
  Nf(true);
  Hf((r1 & 0xF) < (src & 0xF));
}

static void opADD_HL_SP(State *self) {
  uint16_t r1 = HLp, src = SP;
  uint16_t result = r1 + src;
  HLp = result;
  internal(self->bus);

  Nf(false);
  Hf((r1 & 0xFFF) + (src & 0xFFF) > 0xFFF);
  Cf(((uint32_t)r1 + (uint32_t)src) > 0xFFFF);
}

static void opDEC_SP(State *self) {
  uint16_t r1 = SP, src = 1;
  uint16_t result = r1 - src;
  SP = result;
  internal(self->bus);
}

static void opINC_A(State *self) {
  uint8_t r1 = A, src = 1;
  uint8_t result = r1 + src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf((r1 & 0xF) + (src & 0xF) >= 0x10);
}

static void opDEC_A(State *self) {
  uint8_t r1 = A, src = 1;
  uint8_t result = r1 - src;
  A = result;

  Zf(!result);
  Nf(true);
  Hf((r1 & 0xF) < (src & 0xF));
}

static void opADD_A_B(State *self) {
  uint8_t r1 = A, src = B;
  uint8_t result = r1 + src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf((r1 & 0xF) + (src & 0xF) >= 0x10);
  Cf(r1 > result);
}

static void opADD_A_C(State *self) {
  uint8_t r1 = A, src = C;
  uint8_t result = r1 + src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf((r1 & 0xF) + (src & 0xF) >= 0x10);
  Cf(r1 > result);
}

static void opADD_A_D(State *self) {
  uint8_t r1 = A, src = D;
  uint8_t result = r1 + src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf((r1 & 0xF) + (src & 0xF) >= 0x10);
  Cf(r1 > result);
}

static void opADD_A_E(State *self) {
  uint8_t r1 = A, src = E;
  uint8_t result = r1 + src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf((r1 & 0xF) + (src & 0xF) >= 0x10);
  Cf(r1 > result);
}

static void opADD_A_H(State *self) {
  uint8_t r1 = A, src = H;
  uint8_t result = r1 + src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf((r1 & 0xF) + (src & 0xF) >= 0x10);
  Cf(r1 > result);
}

static void opADD_A_L(State *self) {
  uint8_t r1 = A, src = L;
  uint8_t result = r1 + src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf((r1 & 0xF) + (src & 0xF) >= 0x10);
  Cf(r1 > result);
}

static void opADD_A_deref_HL(State *self) {
  uint8_t r1 = A, src = read8(self->bus, HLp, true, true);
  uint8_t result = r1 + src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf((r1 & 0xF) + (src & 0xF) >= 0x10);
  Cf(r1 > result);
}

static void opADD_A_A(State *self) {
  uint8_t r1 = A, src = A;
  uint8_t result = r1 + src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf((r1 & 0xF) + (src & 0xF) >= 0x10);
  Cf(r1 > result);
}

static void opADC_A_B(State *self) {
  uint8_t r1 = A, src = B;
  uint8_t c = getFlag(ftC);
  uint8_t result = r1 + c + src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(((r1 & 0xF) + (src & 0xF) + c) > 0xF);
  Cf(((uint16_t)r1 + (uint16_t)src + (uint16_t)c) > 0xFF);
}

static void opADC_A_C(State *self) {
  uint8_t r1 = A, src = C;
  uint8_t c = getFlag(ftC);
  uint8_t result = r1 + c + src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(((r1 & 0xF) + (src & 0xF) + c) > 0xF);
  Cf(((uint16_t)r1 + (uint16_t)src + (uint16_t)c) > 0xFF);
}

static void opADC_A_D(State *self) {
  uint8_t r1 = A, src = D;
  uint8_t c = getFlag(ftC);
  uint8_t result = r1 + c + src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(((r1 & 0xF) + (src & 0xF) + c) > 0xF);
  Cf(((uint16_t)r1 + (uint16_t)src + (uint16_t)c) > 0xFF);
}

static void opADC_A_E(State *self) {
  uint8_t r1 = A, src = E;
  uint8_t c = getFlag(ftC);
  uint8_t result = r1 + c + src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(((r1 & 0xF) + (src & 0xF) + c) > 0xF);
  Cf(((uint16_t)r1 + (uint16_t)src + (uint16_t)c) > 0xFF);
}

static void opADC_A_H(State *self) {
  uint8_t r1 = A, src = H;
  uint8_t c = getFlag(ftC);
  uint8_t result = r1 + c + src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(((r1 & 0xF) + (src & 0xF) + c) > 0xF);
  Cf(((uint16_t)r1 + (uint16_t)src + (uint16_t)c) > 0xFF);
}

static void opADC_A_L(State *self) {
  uint8_t r1 = A, src = L;
  uint8_t c = getFlag(ftC);
  uint8_t result = r1 + c + src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(((r1 & 0xF) + (src & 0xF) + c) > 0xF);
  Cf(((uint16_t)r1 + (uint16_t)src + (uint16_t)c) > 0xFF);
}

static void opADC_A_deref_HL(State *self) {
  uint8_t r1 = A, src = read8(self->bus, HLp, true, true);
  uint8_t c = getFlag(ftC);
  uint8_t result = r1 + c + src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(((r1 & 0xF) + (src & 0xF) + c) > 0xF);
  Cf(((uint16_t)r1 + (uint16_t)src + (uint16_t)c) > 0xFF);
}

static void opADC_A_A(State *self) {
  uint8_t r1 = A, src = A;
  uint8_t c = getFlag(ftC);
  uint8_t result = r1 + c + src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(((r1 & 0xF) + (src & 0xF) + c) > 0xF);
  Cf(((uint16_t)r1 + (uint16_t)src + (uint16_t)c) > 0xFF);
}

static void opSUB_A_B(State *self) {
  uint8_t r1 = A, src = B;
  uint8_t result = r1 - src;
  A = result;

  Zf(!result);
  Nf(true);
  Hf((r1 & 0xF) < (src & 0xF));
  Cf(src > r1);
}

static void opSUB_A_C(State *self) {
  uint8_t r1 = A, src = C;
  uint8_t result = r1 - src;
  A = result;

  Zf(!result);
  Nf(true);
  Hf((r1 & 0xF) < (src & 0xF));
  Cf(src > r1);
}

static void opSUB_A_D(State *self) {
  uint8_t r1 = A, src = D;
  uint8_t result = r1 - src;
  A = result;

  Zf(!result);
  Nf(true);
  Hf((r1 & 0xF) < (src & 0xF));
  Cf(src > r1);
}

static void opSUB_A_E(State *self) {
  uint8_t r1 = A, src = E;
  uint8_t result = r1 - src;
  A = result;

  Zf(!result);
  Nf(true);
  Hf((r1 & 0xF) < (src & 0xF));
  Cf(src > r1);
}

static void opSUB_A_H(State *self) {
  uint8_t r1 = A, src = H;
  uint8_t result = r1 - src;
  A = result;

  Zf(!result);
  Nf(true);
  Hf((r1 & 0xF) < (src & 0xF));
  Cf(src > r1);
}

static void opSUB_A_L(State *self) {
  uint8_t r1 = A, src = L;
  uint8_t result = r1 - src;
  A = result;

  Zf(!result);
  Nf(true);
  Hf((r1 & 0xF) < (src & 0xF));
  Cf(src > r1);
}

static void opSUB_A_deref_HL(State *self) {
  uint8_t r1 = A, src = read8(self->bus, HLp, true, true);
  uint8_t result = r1 - src;
  A = result;

  Zf(!result);
  Nf(true);
  Hf((r1 & 0xF) < (src & 0xF));
  Cf(src > r1);
}

static void opSUB_A_A(State *self) {
  uint8_t r1 = A, src = A;
  uint8_t result = r1 - src;
  A = result;

  Zf(!result);
  Nf(true);
  Hf((r1 & 0xF) < (src & 0xF));
  Cf(src > r1);
}

static void opSBC_A_B(State *self) {
  uint8_t r1 = A, src = B;
  uint8_t c = getFlag(ftC);
  uint8_t result = r1 - c - src;
  A = result;

  Zf(!result);
  Nf(true);
  Hf(((src & 0xF) + c) > (r1 & 0xF));
  Cf(((uint16_t)src + c) > r1);
}

static void opSBC_A_C(State *self) {
  uint8_t r1 = A, src = C;
  uint8_t c = getFlag(ftC);
  uint8_t result = r1 - c - src;
  A = result;

  Zf(!result);
  Nf(true);
  Hf(((src & 0xF) + c) > (r1 & 0xF));
  Cf(((uint16_t)src + c) > r1);
}

static void opSBC_A_D(State *self) {
  uint8_t r1 = A, src = D;
  uint8_t c = getFlag(ftC);
  uint8_t result = r1 - c - src;
  A = result;

  Zf(!result);
  Nf(true);
  Hf(((src & 0xF) + c) > (r1 & 0xF));
  Cf(((uint16_t)src + c) > r1);
}

static void opSBC_A_E(State *self) {
  uint8_t r1 = A, src = E;
  uint8_t c = getFlag(ftC);
  uint8_t result = r1 - c - src;
  A = result;

  Zf(!result);
  Nf(true);
  Hf(((src & 0xF) + c) > (r1 & 0xF));
  Cf(((uint16_t)src + c) > r1);
}

static void opSBC_A_H(State *self) {
  uint8_t r1 = A, src = H;
  uint8_t c = getFlag(ftC);
  uint8_t result = r1 - c - src;
  A = result;

  Zf(!result);
  Nf(true);
  Hf(((src & 0xF) + c) > (r1 & 0xF));
  Cf(((uint16_t)src + c) > r1);
}

static void opSBC_A_L(State *self) {
  uint8_t r1 = A, src = L;
  uint8_t c = getFlag(ftC);
  uint8_t result = r1 - c - src;
  A = result;

  Zf(!result);
  Nf(true);
  Hf(((src & 0xF) + c) > (r1 & 0xF));
  Cf(((uint16_t)src + c) > r1);
}

static void opSBC_A_deref_HL(State *self) {
  uint8_t r1 = A, src = read8(self->bus, HLp, true, true);
  uint8_t c = getFlag(ftC);
  uint8_t result = r1 - c - src;
  A = result;

  Zf(!result);
  Nf(true);
  Hf(((src & 0xF) + c) > (r1 & 0xF));
  Cf(((uint16_t)src + c) > r1);
}

static void opSBC_A_A(State *self) {
  uint8_t r1 = A, src = A;
  uint8_t c = getFlag(ftC);
  uint8_t result = r1 - c - src;
  A = result;

  Zf(!result);
  Nf(true);
  Hf(((src & 0xF) + c) > (r1 & 0xF));
  Cf(((uint16_t)src + c) > r1);
}

static void opAND_A_B(State *self) {
  uint8_t r1 = A, src = B;
  uint8_t result = r1 & src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(true);
  Cf(false);
}

static void opAND_A_C(State *self) {
  uint8_t r1 = A, src = C;
  uint8_t result = r1 & src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(true);
  Cf(false);
}

static void opAND_A_D(State *self) {
  uint8_t r1 = A, src = D;
  uint8_t result = r1 & src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(true);
  Cf(false);
}

static void opAND_A_E(State *self) {
  uint8_t r1 = A, src = E;
  uint8_t result = r1 & src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(true);
  Cf(false);
}

static void opAND_A_H(State *self) {
  uint8_t r1 = A, src = H;
  uint8_t result = r1 & src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(true);
  Cf(false);
}

static void opAND_A_L(State *self) {
  uint8_t r1 = A, src = L;
  uint8_t result = r1 & src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(true);
  Cf(false);
}

static void opAND_A_deref_HL(State *self) {
  uint8_t r1 = A, src = read8(self->bus, HLp, true, true);
  uint8_t result = r1 & src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(true);
  Cf(false);
}

static void opAND_A_A(State *self) {
  uint8_t r1 = A, src = A;
  uint8_t result = r1 & src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(true);
  Cf(false);
}

static void opXOR_A_B(State *self) {
  uint8_t r1 = A, src = B;
  uint8_t result = r1 ^ src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(false);
}

static void opXOR_A_C(State *self) {
  uint8_t r1 = A, src = C;
  uint8_t result = r1 ^ src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(false);
}

static void opXOR_A_D(State *self) {
  uint8_t r1 = A, src = D;
  uint8_t result = r1 ^ src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(false);
}

static void opXOR_A_E(State *self) {
  uint8_t r1 = A, src = E;
  uint8_t result = r1 ^ src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(false);
}

static void opXOR_A_H(State *self) {
  uint8_t r1 = A, src = H;
  uint8_t result = r1 ^ src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(false);
}

static void opXOR_A_L(State *self) {
  uint8_t r1 = A, src = L;
  uint8_t result = r1 ^ src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(false);
}

static void opXOR_A_deref_HL(State *self) {
  uint8_t r1 = A, src = read8(self->bus, HLp, true, true);
  uint8_t result = r1 ^ src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(false);
}

static void opXOR_A_A(State *self) {
  uint8_t r1 = A, src = A;
  uint8_t result = r1 ^ src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(false);
}

static void opOR_A_B(State *self) {
  uint8_t r1 = A, src = B;
  uint8_t result = r1 | src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(false);
}

static void opOR_A_C(State *self) {
  uint8_t r1 = A, src = C;
  uint8_t result = r1 | src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(false);
}

static void opOR_A_D(State *self) {
  uint8_t r1 = A, src = D;
  uint8_t result = r1 | src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(false);
}

static void opOR_A_E(State *self) {
  uint8_t r1 = A, src = E;
  uint8_t result = r1 | src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(false);
}

static void opOR_A_H(State *self) {
  uint8_t r1 = A, src = H;
  uint8_t result = r1 | src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(false);
}

static void opOR_A_L(State *self) {
  uint8_t r1 = A, src = L;
  uint8_t result = r1 | src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(false);
}

static void opOR_A_deref_HL(State *self) {
  uint8_t r1 = A, src = read8(self->bus, HLp, true, true);
  uint8_t result = r1 | src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(false);
}

static void opOR_A_A(State *self) {
  uint8_t r1 = A, src = A;
  uint8_t result = r1 | src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(false);
}

static void opCP_A_B(State *self) {
  uint8_t r1 = A, src = B;
  uint8_t result = r1 - src;

  Zf(!result);
  Nf(true);
  Hf((r1 & 0xF) < (src & 0xF));
  Cf(src > r1);
}

static void opCP_A_C(State *self) {
  uint8_t r1 = A, src = C;
  uint8_t result = r1 - src;

  Zf(!result);
  Nf(true);
  Hf((r1 & 0xF) < (src & 0xF));
  Cf(src > r1);
}

static void opCP_A_D(State *self) {
  uint8_t r1 = A, src = D;
  uint8_t result = r1 - src;

  Zf(!result);
  Nf(true);
  Hf((r1 & 0xF) < (src & 0xF));
  Cf(src > r1);
}

static void opCP_A_E(State *self) {
  uint8_t r1 = A, src = E;
  uint8_t result = r1 - src;

  Zf(!result);
  Nf(true);
  Hf((r1 & 0xF) < (src & 0xF));
  Cf(src > r1);
}

static void opCP_A_H(State *self) {
  uint8_t r1 = A, src = H;
  uint8_t result = r1 - src;

  Zf(!result);
  Nf(true);
  Hf((r1 & 0xF) < (src & 0xF));
  Cf(src > r1);
}

static void opCP_A_L(State *self) {
  uint8_t r1 = A, src = L;
  uint8_t result = r1 - src;

  Zf(!result);
  Nf(true);
  Hf((r1 & 0xF) < (src & 0xF));
  Cf(src > r1);
}

static void opCP_A_deref_HL(State *self) {
  uint8_t r1 = A, src = read8(self->bus, HLp, true, true);
  uint8_t result = r1 - src;

  Zf(!result);
  Nf(true);
  Hf((r1 & 0xF) < (src & 0xF));
  Cf(src > r1);
}

static void opCP_A_A(State *self) {
  uint8_t r1 = A, src = A;
  uint8_t result = r1 - src;

  Zf(!result);
  Nf(true);
  Hf((r1 & 0xF) < (src & 0xF));
  Cf(src > r1);
}

static void opADD_A_U8(State *self) {
  uint8_t r1 = A, src = next8(self);
  uint8_t result = r1 + src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf((r1 & 0xF) + (src & 0xF) >= 0x10);
  Cf(r1 > result);
}

static void opADC_A_U8(State *self) {
  uint8_t r1 = A, src = next8(self);
  uint8_t c = getFlag(ftC);
  uint8_t result = r1 + c + src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(((r1 & 0xF) + (src & 0xF) + c) > 0xF);
  Cf(((uint16_t)r1 + (uint16_t)src + (uint16_t)c) > 0xFF);
}

static void opSUB_A_U8(State *self) {
  uint8_t r1 = A, src = next8(self);
  uint8_t result = r1 - src;
  A = result;

  Zf(!result);
  Nf(true);
  Hf((r1 & 0xF) < (src & 0xF));
  Cf(src > r1);
}

static void opSBC_A_U8(State *self) {
  uint8_t r1 = A, src = next8(self);
  uint8_t c = getFlag(ftC);
  uint8_t result = r1 - c - src;
  A = result;

  Zf(!result);
  Nf(true);
  Hf(((src & 0xF) + c) > (r1 & 0xF));
  Cf(((uint16_t)src + c) > r1);
}

static void opAND_A_U8(State *self) {
  uint8_t r1 = A, src = next8(self);
  uint8_t result = r1 & src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(true);
  Cf(false);
}

static void opXOR_A_U8(State *self) {
  uint8_t r1 = A, src = next8(self);
  uint8_t result = r1 ^ src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(false);
}

static void opOR_A_U8(State *self) {
  uint8_t r1 = A, src = next8(self);
  uint8_t result = r1 | src;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(false);
}

static void opCP_A_U8(State *self) {
  uint8_t r1 = A, src = next8(self);
  uint8_t result = r1 - src;

  Zf(!result);
  Nf(true);
  Hf((r1 & 0xF) < (src & 0xF));
  Cf(src > r1);
}

static void opLD_BC_U16(State *self) { BCp = next16(self); }

static void opLD_deref_BC_A(State *self) { write8(self->bus, BCp, A, true); }

static void opLD_B_U8(State *self) { B = next8(self); }

static void opLD_A_deref_BC(State *self) {
  A = read8(self->bus, BCp, true, true);
}

static void opLD_C_U8(State *self) { C = next8(self); }

static void opLD_DE_U16(State *self) { DEp = next16(self); }

static void opLD_deref_DE_A(State *self) { write8(self->bus, DEp, A, true); }

static void opLD_D_U8(State *self) { D = next8(self); }

static void opLD_A_deref_DE(State *self) {
  A = read8(self->bus, DEp, true, true);
}

static void opLD_E_U8(State *self) { E = next8(self); }

static void opLD_HL_U16(State *self) { HLp = next16(self); }

static void opLD_H_U8(State *self) { H = next8(self); }

static void opLD_L_U8(State *self) { L = next8(self); }

static void opLD_SP_U16(State *self) { SP = next16(self); }

static void opLD_deref_HL_U8(State *self) {
  write8(self->bus, HLp, next8(self), true);
}

static void opLD_A_U8(State *self) { A = next8(self); }

static void opLD_B_B(State *self) { B = B; }

static void opLD_B_C(State *self) { B = C; }

static void opLD_B_D(State *self) { B = D; }

static void opLD_B_E(State *self) { B = E; }

static void opLD_B_H(State *self) { B = H; }

static void opLD_B_L(State *self) { B = L; }

static void opLD_B_deref_HL(State *self) {
  B = read8(self->bus, HLp, true, true);
}

static void opLD_B_A(State *self) { B = A; }

static void opLD_C_B(State *self) { C = B; }

static void opLD_C_C(State *self) { C = C; }

static void opLD_C_D(State *self) { C = D; }

static void opLD_C_E(State *self) { C = E; }

static void opLD_C_H(State *self) { C = H; }

static void opLD_C_L(State *self) { C = L; }

static void opLD_C_deref_HL(State *self) {
  C = read8(self->bus, HLp, true, true);
}

static void opLD_C_A(State *self) { C = A; }

static void opLD_D_B(State *self) { D = B; }

static void opLD_D_C(State *self) { D = C; }

static void opLD_D_D(State *self) { D = D; }

static void opLD_D_E(State *self) { D = E; }

static void opLD_D_H(State *self) { D = H; }

static void opLD_D_L(State *self) { D = L; }

static void opLD_D_deref_HL(State *self) {
  D = read8(self->bus, HLp, true, true);
}

static void opLD_D_A(State *self) { D = A; }

static void opLD_E_B(State *self) { E = B; }

static void opLD_E_C(State *self) { E = C; }

static void opLD_E_D(State *self) { E = D; }

static void opLD_E_E(State *self) { E = E; }

static void opLD_E_H(State *self) { E = H; }

static void opLD_E_L(State *self) { E = L; }

static void opLD_E_deref_HL(State *self) {
  E = read8(self->bus, HLp, true, true);
}

static void opLD_E_A(State *self) { E = A; }

static void opLD_H_B(State *self) { H = B; }

static void opLD_H_C(State *self) { H = C; }

static void opLD_H_D(State *self) { H = D; }

static void opLD_H_E(State *self) { H = E; }

static void opLD_H_H(State *self) { H = H; }

static void opLD_H_L(State *self) { H = L; }

static void opLD_H_deref_HL(State *self) {
  H = read8(self->bus, HLp, true, true);
}

static void opLD_H_A(State *self) { H = A; }

static void opLD_L_B(State *self) { L = B; }

static void opLD_L_C(State *self) { L = C; }

static void opLD_L_D(State *self) { L = D; }

static void opLD_L_E(State *self) { L = E; }

static void opLD_L_H(State *self) { L = H; }

static void opLD_L_L(State *self) { L = L; }

static void opLD_L_deref_HL(State *self) {
  L = read8(self->bus, HLp, true, true);
}

static void opLD_L_A(State *self) { L = A; }

static void opLD_deref_HL_B(State *self) { write8(self->bus, HLp, B, true); }

static void opLD_deref_HL_C(State *self) { write8(self->bus, HLp, C, true); }

static void opLD_deref_HL_D(State *self) { write8(self->bus, HLp, D, true); }

static void opLD_deref_HL_E(State *self) { write8(self->bus, HLp, E, true); }

static void opLD_deref_HL_H(State *self) { write8(self->bus, HLp, H, true); }

static void opLD_deref_HL_L(State *self) { write8(self->bus, HLp, L, true); }

static void opLD_deref_HL_A(State *self) { write8(self->bus, HLp, A, true); }

static void opLD_A_B(State *self) { A = B; }

static void opLD_A_C(State *self) { A = C; }

static void opLD_A_D(State *self) { A = D; }

static void opLD_A_E(State *self) { A = E; }

static void opLD_A_H(State *self) { A = H; }

static void opLD_A_L(State *self) { A = L; }

static void opLD_A_deref_HL(State *self) {
  A = read8(self->bus, HLp, true, true);
}

static void opLD_A_A(State *self) { A = A; }

static void opLD_SP_HL(State *self) {
  SP = HLp;
  internal(self->bus);
}

static void opRLCA(State *self) {
  uint8_t r1 = A;
  uint8_t result = rotateLeftBits(r1, 1) | getFlag(ftC) >> 7;
  A = result;

  Zf(false);
  Nf(false);
  Hf(false);
  Cf(r1 >> 7);
}

static void opRRCA(State *self) {
  uint8_t r1 = A;
  uint8_t result = rotateRightBits(r1, 1) | (0x80 & getFlag(ftC));
  A = result;

  Zf(false);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opRLA(State *self) {
  uint8_t r1 = A;
  uint8_t result = (r1 << 1) | getFlag(ftC);
  A = result;

  Zf(false);
  Nf(false);
  Hf(false);
  Cf(r1 >> 7);
}

static void opRRA(State *self) {
  uint8_t r1 = A;
  uint8_t result = (r1 >> 1) | (getFlag(ftC) << 7);
  A = result;

  Zf(false);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opRLC_B(State *self) {
  uint8_t r1 = B;
  uint8_t result = rotateLeftBits(r1, 1) | getFlag(ftC) >> 7;
  B = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 >> 7);
}

static void opRLC_C(State *self) {
  uint8_t r1 = C;
  uint8_t result = rotateLeftBits(r1, 1) | getFlag(ftC) >> 7;
  C = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 >> 7);
}

static void opRLC_D(State *self) {
  uint8_t r1 = D;
  uint8_t result = rotateLeftBits(r1, 1) | getFlag(ftC) >> 7;
  D = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 >> 7);
}

static void opRLC_E(State *self) {
  uint8_t r1 = E;
  uint8_t result = rotateLeftBits(r1, 1) | getFlag(ftC) >> 7;
  E = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 >> 7);
}

static void opRLC_H(State *self) {
  uint8_t r1 = H;
  uint8_t result = rotateLeftBits(r1, 1) | getFlag(ftC) >> 7;
  H = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 >> 7);
}

static void opRLC_L(State *self) {
  uint8_t r1 = L;
  uint8_t result = rotateLeftBits(r1, 1) | getFlag(ftC) >> 7;
  L = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 >> 7);
}

static void opRLC_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true);
  uint8_t result = rotateLeftBits(r1, 1) | getFlag(ftC) >> 7;
  write8(self->bus, HLp, result, true);

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 >> 7);
}

static void opRLC_A(State *self) {
  uint8_t r1 = A;
  uint8_t result = rotateLeftBits(r1, 1) | getFlag(ftC) >> 7;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 >> 7);
}

static void opRRC_B(State *self) {
  uint8_t r1 = B;
  uint8_t result = rotateRightBits(r1, 1) | (0x80 & getFlag(ftC));
  B = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opRRC_C(State *self) {
  uint8_t r1 = C;
  uint8_t result = rotateRightBits(r1, 1) | (0x80 & getFlag(ftC));
  C = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opRRC_D(State *self) {
  uint8_t r1 = D;
  uint8_t result = rotateRightBits(r1, 1) | (0x80 & getFlag(ftC));
  D = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opRRC_E(State *self) {
  uint8_t r1 = E;
  uint8_t result = rotateRightBits(r1, 1) | (0x80 & getFlag(ftC));
  E = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opRRC_H(State *self) {
  uint8_t r1 = H;
  uint8_t result = rotateRightBits(r1, 1) | (0x80 & getFlag(ftC));
  H = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opRRC_L(State *self) {
  uint8_t r1 = L;
  uint8_t result = rotateRightBits(r1, 1) | (0x80 & getFlag(ftC));
  L = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opRRC_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true);
  uint8_t result = rotateRightBits(r1, 1) | (0x80 & getFlag(ftC));
  write8(self->bus, HLp, result, true);

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opRRC_A(State *self) {
  uint8_t r1 = A;
  uint8_t result = rotateRightBits(r1, 1) | (0x80 & getFlag(ftC));
  A = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opRL_B(State *self) {
  uint8_t r1 = B;
  uint8_t result = (r1 << 1) | getFlag(ftC);
  B = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 >> 7);
}

static void opRL_C(State *self) {
  uint8_t r1 = C;
  uint8_t result = (r1 << 1) | getFlag(ftC);
  C = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 >> 7);
}

static void opRL_D(State *self) {
  uint8_t r1 = D;
  uint8_t result = (r1 << 1) | getFlag(ftC);
  D = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 >> 7);
}

static void opRL_E(State *self) {
  uint8_t r1 = E;
  uint8_t result = (r1 << 1) | getFlag(ftC);
  E = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 >> 7);
}

static void opRL_H(State *self) {
  uint8_t r1 = H;
  uint8_t result = (r1 << 1) | getFlag(ftC);
  H = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 >> 7);
}

static void opRL_L(State *self) {
  uint8_t r1 = L;
  uint8_t result = (r1 << 1) | getFlag(ftC);
  L = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 >> 7);
}

static void opRL_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true);
  uint8_t result = (r1 << 1) | getFlag(ftC);
  write8(self->bus, HLp, result, true);

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 >> 7);
}

static void opRL_A(State *self) {
  uint8_t r1 = A;
  uint8_t result = (r1 << 1) | getFlag(ftC);
  A = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 >> 7);
}

static void opRR_B(State *self) {
  uint8_t r1 = B;
  uint8_t result = (r1 >> 1) | (getFlag(ftC) << 7);
  B = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opRR_C(State *self) {
  uint8_t r1 = C;
  uint8_t result = (r1 >> 1) | (getFlag(ftC) << 7);
  C = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opRR_D(State *self) {
  uint8_t r1 = D;
  uint8_t result = (r1 >> 1) | (getFlag(ftC) << 7);
  D = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opRR_E(State *self) {
  uint8_t r1 = E;
  uint8_t result = (r1 >> 1) | (getFlag(ftC) << 7);
  E = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opRR_H(State *self) {
  uint8_t r1 = H;
  uint8_t result = (r1 >> 1) | (getFlag(ftC) << 7);
  H = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opRR_L(State *self) {
  uint8_t r1 = L;
  uint8_t result = (r1 >> 1) | (getFlag(ftC) << 7);
  L = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opRR_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true);
  uint8_t result = (r1 >> 1) | (getFlag(ftC) << 7);
  write8(self->bus, HLp, result, true);

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opRR_A(State *self) {
  uint8_t r1 = A;
  uint8_t result = (r1 >> 1) | (getFlag(ftC) << 7);
  A = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opSLA_B(State *self) {
  uint8_t r1 = B;
  uint8_t result = r1 << 1;
  B = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 >> 7);
}

static void opSLA_C(State *self) {
  uint8_t r1 = C;
  uint8_t result = r1 << 1;
  C = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 >> 7);
}

static void opSLA_D(State *self) {
  uint8_t r1 = D;
  uint8_t result = r1 << 1;
  D = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 >> 7);
}

static void opSLA_E(State *self) {
  uint8_t r1 = E;
  uint8_t result = r1 << 1;
  E = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 >> 7);
}

static void opSLA_H(State *self) {
  uint8_t r1 = H;
  uint8_t result = r1 << 1;
  H = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 >> 7);
}

static void opSLA_L(State *self) {
  uint8_t r1 = L;
  uint8_t result = r1 << 1;
  L = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 >> 7);
}

static void opSLA_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true);
  uint8_t result = r1 << 1;
  write8(self->bus, HLp, result, true);

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 >> 7);
}

static void opSLA_A(State *self) {
  uint8_t r1 = A;
  uint8_t result = r1 << 1;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 >> 7);
}

static void opSRA_B(State *self) {
  uint8_t r1 = B;
  uint8_t result = (r1 >> 1) | (r1 & 0x80);
  B = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opSRA_C(State *self) {
  uint8_t r1 = C;
  uint8_t result = (r1 >> 1) | (r1 & 0x80);
  C = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opSRA_D(State *self) {
  uint8_t r1 = D;
  uint8_t result = (r1 >> 1) | (r1 & 0x80);
  D = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opSRA_E(State *self) {
  uint8_t r1 = E;
  uint8_t result = (r1 >> 1) | (r1 & 0x80);
  E = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opSRA_H(State *self) {
  uint8_t r1 = H;
  uint8_t result = (r1 >> 1) | (r1 & 0x80);
  H = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opSRA_L(State *self) {
  uint8_t r1 = L;
  uint8_t result = (r1 >> 1) | (r1 & 0x80);
  L = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opSRA_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true);
  uint8_t result = (r1 >> 1) | (r1 & 0x80);
  write8(self->bus, HLp, result, true);

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opSRA_A(State *self) {
  uint8_t r1 = A;
  uint8_t result = (r1 >> 1) | (r1 & 0x80);
  A = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opSWAP_B(State *self) {
  uint8_t r1 = B;
  uint8_t result = rotateLeftBits(r1, 4);
  B = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(false);
}

static void opSWAP_C(State *self) {
  uint8_t r1 = C;
  uint8_t result = rotateLeftBits(r1, 4);
  C = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(false);
}

static void opSWAP_D(State *self) {
  uint8_t r1 = D;
  uint8_t result = rotateLeftBits(r1, 4);
  D = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(false);
}

static void opSWAP_E(State *self) {
  uint8_t r1 = E;
  uint8_t result = rotateLeftBits(r1, 4);
  E = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(false);
}

static void opSWAP_H(State *self) {
  uint8_t r1 = H;
  uint8_t result = rotateLeftBits(r1, 4);
  H = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(false);
}

static void opSWAP_L(State *self) {
  uint8_t r1 = L;
  uint8_t result = rotateLeftBits(r1, 4);
  L = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(false);
}

static void opSWAP_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true);
  uint8_t result = rotateLeftBits(r1, 4);
  write8(self->bus, HLp, result, true);

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(false);
}

static void opSWAP_A(State *self) {
  uint8_t r1 = A;
  uint8_t result = rotateLeftBits(r1, 4);
  A = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(false);
}

static void opSRL_B(State *self) {
  uint8_t r1 = B;
  uint8_t result = r1 >> 1;
  B = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opSRL_C(State *self) {
  uint8_t r1 = C;
  uint8_t result = r1 >> 1;
  C = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opSRL_D(State *self) {
  uint8_t r1 = D;
  uint8_t result = r1 >> 1;
  D = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opSRL_E(State *self) {
  uint8_t r1 = E;
  uint8_t result = r1 >> 1;
  E = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opSRL_H(State *self) {
  uint8_t r1 = H;
  uint8_t result = r1 >> 1;
  H = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opSRL_L(State *self) {
  uint8_t r1 = L;
  uint8_t result = r1 >> 1;
  L = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opSRL_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true);
  uint8_t result = r1 >> 1;
  write8(self->bus, HLp, result, true);

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opSRL_A(State *self) {
  uint8_t r1 = A;
  uint8_t result = r1 >> 1;
  A = result;

  Zf(!result);
  Nf(false);
  Hf(false);
  Cf(r1 & 1);
}

static void opBIT_0_B(State *self) {
  uint8_t r1 = B;
  uint8_t result = BT(r1, 0);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_0_C(State *self) {
  uint8_t r1 = C;
  uint8_t result = BT(r1, 0);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_0_D(State *self) {
  uint8_t r1 = D;
  uint8_t result = BT(r1, 0);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_0_E(State *self) {
  uint8_t r1 = E;
  uint8_t result = BT(r1, 0);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_0_H(State *self) {
  uint8_t r1 = H;
  uint8_t result = BT(r1, 0);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_0_L(State *self) {
  uint8_t r1 = L;
  uint8_t result = BT(r1, 0);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_0_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true);
  uint8_t result = BT(r1, 0);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_0_A(State *self) {
  uint8_t r1 = A;
  uint8_t result = BT(r1, 0);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_1_B(State *self) {
  uint8_t r1 = B;
  uint8_t result = BT(r1, 1);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_1_C(State *self) {
  uint8_t r1 = C;
  uint8_t result = BT(r1, 1);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_1_D(State *self) {
  uint8_t r1 = D;
  uint8_t result = BT(r1, 1);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_1_E(State *self) {
  uint8_t r1 = E;
  uint8_t result = BT(r1, 1);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_1_H(State *self) {
  uint8_t r1 = H;
  uint8_t result = BT(r1, 1);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_1_L(State *self) {
  uint8_t r1 = L;
  uint8_t result = BT(r1, 1);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_1_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true);
  uint8_t result = BT(r1, 1);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_1_A(State *self) {
  uint8_t r1 = A;
  uint8_t result = BT(r1, 1);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_2_B(State *self) {
  uint8_t r1 = B;
  uint8_t result = BT(r1, 2);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_2_C(State *self) {
  uint8_t r1 = C;
  uint8_t result = BT(r1, 2);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_2_D(State *self) {
  uint8_t r1 = D;
  uint8_t result = BT(r1, 2);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_2_E(State *self) {
  uint8_t r1 = E;
  uint8_t result = BT(r1, 2);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_2_H(State *self) {
  uint8_t r1 = H;
  uint8_t result = BT(r1, 2);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_2_L(State *self) {
  uint8_t r1 = L;
  uint8_t result = BT(r1, 2);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_2_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true);
  uint8_t result = BT(r1, 2);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_2_A(State *self) {
  uint8_t r1 = A;
  uint8_t result = BT(r1, 2);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_3_B(State *self) {
  uint8_t r1 = B;
  uint8_t result = BT(r1, 3);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_3_C(State *self) {
  uint8_t r1 = C;
  uint8_t result = BT(r1, 3);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_3_D(State *self) {
  uint8_t r1 = D;
  uint8_t result = BT(r1, 3);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_3_E(State *self) {
  uint8_t r1 = E;
  uint8_t result = BT(r1, 3);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_3_H(State *self) {
  uint8_t r1 = H;
  uint8_t result = BT(r1, 3);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_3_L(State *self) {
  uint8_t r1 = L;
  uint8_t result = BT(r1, 3);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_3_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true);
  uint8_t result = BT(r1, 3);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_3_A(State *self) {
  uint8_t r1 = A;
  uint8_t result = BT(r1, 3);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_4_B(State *self) {
  uint8_t r1 = B;
  uint8_t result = BT(r1, 4);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_4_C(State *self) {
  uint8_t r1 = C;
  uint8_t result = BT(r1, 4);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_4_D(State *self) {
  uint8_t r1 = D;
  uint8_t result = BT(r1, 4);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_4_E(State *self) {
  uint8_t r1 = E;
  uint8_t result = BT(r1, 4);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_4_H(State *self) {
  uint8_t r1 = H;
  uint8_t result = BT(r1, 4);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_4_L(State *self) {
  uint8_t r1 = L;
  uint8_t result = BT(r1, 4);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_4_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true);
  uint8_t result = BT(r1, 4);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_4_A(State *self) {
  uint8_t r1 = A;
  uint8_t result = BT(r1, 4);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_5_B(State *self) {
  uint8_t r1 = B;
  uint8_t result = BT(r1, 5);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_5_C(State *self) {
  uint8_t r1 = C;
  uint8_t result = BT(r1, 5);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_5_D(State *self) {
  uint8_t r1 = D;
  uint8_t result = BT(r1, 5);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_5_E(State *self) {
  uint8_t r1 = E;
  uint8_t result = BT(r1, 5);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_5_H(State *self) {
  uint8_t r1 = H;
  uint8_t result = BT(r1, 5);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_5_L(State *self) {
  uint8_t r1 = L;
  uint8_t result = BT(r1, 5);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_5_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true);
  uint8_t result = BT(r1, 5);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_5_A(State *self) {
  uint8_t r1 = A;
  uint8_t result = BT(r1, 5);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_6_B(State *self) {
  uint8_t r1 = B;
  uint8_t result = BT(r1, 6);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_6_C(State *self) {
  uint8_t r1 = C;
  uint8_t result = BT(r1, 6);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_6_D(State *self) {
  uint8_t r1 = D;
  uint8_t result = BT(r1, 6);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_6_E(State *self) {
  uint8_t r1 = E;
  uint8_t result = BT(r1, 6);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_6_H(State *self) {
  uint8_t r1 = H;
  uint8_t result = BT(r1, 6);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_6_L(State *self) {
  uint8_t r1 = L;
  uint8_t result = BT(r1, 6);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_6_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true);
  uint8_t result = BT(r1, 6);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_6_A(State *self) {
  uint8_t r1 = A;
  uint8_t result = BT(r1, 6);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_7_B(State *self) {
  uint8_t r1 = B;
  uint8_t result = BT(r1, 7);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_7_C(State *self) {
  uint8_t r1 = C;
  uint8_t result = BT(r1, 7);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_7_D(State *self) {
  uint8_t r1 = D;
  uint8_t result = BT(r1, 7);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_7_E(State *self) {
  uint8_t r1 = E;
  uint8_t result = BT(r1, 7);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_7_H(State *self) {
  uint8_t r1 = H;
  uint8_t result = BT(r1, 7);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_7_L(State *self) {
  uint8_t r1 = L;
  uint8_t result = BT(r1, 7);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_7_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true);
  uint8_t result = BT(r1, 7);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opBIT_7_A(State *self) {
  uint8_t r1 = A;
  uint8_t result = BT(r1, 7);

  Zf(!result);
  Nf(false);
  Hf(true);
}

static void opRES_0_B(State *self) {
  uint8_t r1 = B;
  uint8_t result = clearBit(r1, 0);
  B = result;
}

static void opRES_0_C(State *self) {
  uint8_t r1 = C;
  uint8_t result = clearBit(r1, 0);
  C = result;
}

static void opRES_0_D(State *self) {
  uint8_t r1 = D;
  uint8_t result = clearBit(r1, 0);
  D = result;
}

static void opRES_0_E(State *self) {
  uint8_t r1 = E;
  uint8_t result = clearBit(r1, 0);
  E = result;
}

static void opRES_0_H(State *self) {
  uint8_t r1 = H;
  uint8_t result = clearBit(r1, 0);
  H = result;
}

static void opRES_0_L(State *self) {
  uint8_t r1 = L;
  uint8_t result = clearBit(r1, 0);
  L = result;
}

static void opRES_0_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true);
  uint8_t result = clearBit(r1, 0);
  write8(self->bus, HLp, result, true);
}

static void opRES_0_A(State *self) {
  uint8_t r1 = A;
  uint8_t result = clearBit(r1, 0);
  A = result;
}

static void opRES_1_B(State *self) {
  uint8_t r1 = B;
  uint8_t result = clearBit(r1, 1);
  B = result;
}

static void opRES_1_C(State *self) {
  uint8_t r1 = C;
  uint8_t result = clearBit(r1, 1);
  C = result;
}

static void opRES_1_D(State *self) {
  uint8_t r1 = D;
  uint8_t result = clearBit(r1, 1);
  D = result;
}

static void opRES_1_E(State *self) {
  uint8_t r1 = E;
  uint8_t result = clearBit(r1, 1);
  E = result;
}

static void opRES_1_H(State *self) {
  uint8_t r1 = H;
  uint8_t result = clearBit(r1, 1);
  H = result;
}

static void opRES_1_L(State *self) {
  uint8_t r1 = L;
  uint8_t result = clearBit(r1, 1);
  L = result;
}

static void opRES_1_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true);
  uint8_t result = clearBit(r1, 1);
  write8(self->bus, HLp, result, true);
}

static void opRES_1_A(State *self) {
  uint8_t r1 = A;
  uint8_t result = clearBit(r1, 1);
  A = result;
}

static void opRES_2_B(State *self) {
  uint8_t r1 = B;
  uint8_t result = clearBit(r1, 2);
  B = result;
}

static void opRES_2_C(State *self) {
  uint8_t r1 = C;
  uint8_t result = clearBit(r1, 2);
  C = result;
}

static void opRES_2_D(State *self) {
  uint8_t r1 = D;
  uint8_t result = clearBit(r1, 2);
  D = result;
}

static void opRES_2_E(State *self) {
  uint8_t r1 = E;
  uint8_t result = clearBit(r1, 2);
  E = result;
}

static void opRES_2_H(State *self) {
  uint8_t r1 = H;
  uint8_t result = clearBit(r1, 2);
  H = result;
}

static void opRES_2_L(State *self) {
  uint8_t r1 = L;
  uint8_t result = clearBit(r1, 2);
  L = result;
}

static void opRES_2_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true);
  uint8_t result = clearBit(r1, 2);
  write8(self->bus, HLp, result, true);
}

static void opRES_2_A(State *self) {
  uint8_t r1 = A;
  uint8_t result = clearBit(r1, 2);
  A = result;
}

static void opRES_3_B(State *self) {
  uint8_t r1 = B;
  uint8_t result = clearBit(r1, 3);
  B = result;
}

static void opRES_3_C(State *self) {
  uint8_t r1 = C;
  uint8_t result = clearBit(r1, 3);
  C = result;
}

static void opRES_3_D(State *self) {
  uint8_t r1 = D;
  uint8_t result = clearBit(r1, 3);
  D = result;
}

static void opRES_3_E(State *self) {
  uint8_t r1 = E;
  uint8_t result = clearBit(r1, 3);
  E = result;
}

static void opRES_3_H(State *self) {
  uint8_t r1 = H;
  uint8_t result = clearBit(r1, 3);
  H = result;
}

static void opRES_3_L(State *self) {
  uint8_t r1 = L;
  uint8_t result = clearBit(r1, 3);
  L = result;
}

static void opRES_3_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true);
  uint8_t result = clearBit(r1, 3);
  write8(self->bus, HLp, result, true);
}

static void opRES_3_A(State *self) {
  uint8_t r1 = A;
  uint8_t result = clearBit(r1, 3);
  A = result;
}

static void opRES_4_B(State *self) {
  uint8_t r1 = B;
  uint8_t result = clearBit(r1, 4);
  B = result;
}

static void opRES_4_C(State *self) {
  uint8_t r1 = C;
  uint8_t result = clearBit(r1, 4);
  C = result;
}

static void opRES_4_D(State *self) {
  uint8_t r1 = D;
  uint8_t result = clearBit(r1, 4);
  D = result;
}

static void opRES_4_E(State *self) {
  uint8_t r1 = E;
  uint8_t result = clearBit(r1, 4);
  E = result;
}

static void opRES_4_H(State *self) {
  uint8_t r1 = H;
  uint8_t result = clearBit(r1, 4);
  H = result;
}

static void opRES_4_L(State *self) {
  uint8_t r1 = L;
  uint8_t result = clearBit(r1, 4);
  L = result;
}

static void opRES_4_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true);
  uint8_t result = clearBit(r1, 4);
  write8(self->bus, HLp, result, true);
}

static void opRES_4_A(State *self) {
  uint8_t r1 = A;
  uint8_t result = clearBit(r1, 4);
  A = result;
}

static void opRES_5_B(State *self) {
  uint8_t r1 = B;
  uint8_t result = clearBit(r1, 5);
  B = result;
}

static void opRES_5_C(State *self) {
  uint8_t r1 = C;
  uint8_t result = clearBit(r1, 5);
  C = result;
}

static void opRES_5_D(State *self) {
  uint8_t r1 = D;
  uint8_t result = clearBit(r1, 5);
  D = result;
}

static void opRES_5_E(State *self) {
  uint8_t r1 = E;
  uint8_t result = clearBit(r1, 5);
  E = result;
}

static void opRES_5_H(State *self) {
  uint8_t r1 = H;
  uint8_t result = clearBit(r1, 5);
  H = result;
}

static void opRES_5_L(State *self) {
  uint8_t r1 = L;
  uint8_t result = clearBit(r1, 5);
  L = result;
}

static void opRES_5_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true);
  uint8_t result = clearBit(r1, 5);
  write8(self->bus, HLp, result, true);
}

static void opRES_5_A(State *self) {
  uint8_t r1 = A;
  uint8_t result = clearBit(r1, 5);
  A = result;
}

static void opRES_6_B(State *self) {
  uint8_t r1 = B;
  uint8_t result = clearBit(r1, 6);
  B = result;
}

static void opRES_6_C(State *self) {
  uint8_t r1 = C;
  uint8_t result = clearBit(r1, 6);
  C = result;
}

static void opRES_6_D(State *self) {
  uint8_t r1 = D;
  uint8_t result = clearBit(r1, 6);
  D = result;
}

static void opRES_6_E(State *self) {
  uint8_t r1 = E;
  uint8_t result = clearBit(r1, 6);
  E = result;
}

static void opRES_6_H(State *self) {
  uint8_t r1 = H;
  uint8_t result = clearBit(r1, 6);
  H = result;
}

static void opRES_6_L(State *self) {
  uint8_t r1 = L;
  uint8_t result = clearBit(r1, 6);
  L = result;
}

static void opRES_6_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true);
  uint8_t result = clearBit(r1, 6);
  write8(self->bus, HLp, result, true);
}

static void opRES_6_A(State *self) {
  uint8_t r1 = A;
  uint8_t result = clearBit(r1, 6);
  A = result;
}

static void opRES_7_B(State *self) {
  uint8_t r1 = B;
  uint8_t result = clearBit(r1, 7);
  B = result;
}

static void opRES_7_C(State *self) {
  uint8_t r1 = C;
  uint8_t result = clearBit(r1, 7);
  C = result;
}

static void opRES_7_D(State *self) {
  uint8_t r1 = D;
  uint8_t result = clearBit(r1, 7);
  D = result;
}

static void opRES_7_E(State *self) {
  uint8_t r1 = E;
  uint8_t result = clearBit(r1, 7);
  E = result;
}

static void opRES_7_H(State *self) {
  uint8_t r1 = H;
  uint8_t result = clearBit(r1, 7);
  H = result;
}

static void opRES_7_L(State *self) {
  uint8_t r1 = L;
  uint8_t result = clearBit(r1, 7);
  L = result;
}

static void opRES_7_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true);
  uint8_t result = clearBit(r1, 7);
  write8(self->bus, HLp, result, true);
}

static void opRES_7_A(State *self) {
  uint8_t r1 = A;
  uint8_t result = clearBit(r1, 7);
  A = result;
}

static void opSET_0_B(State *self) {
  uint8_t r1 = B;
  uint8_t result = setBit(r1, 0);
  B = result;
}

static void opSET_0_C(State *self) {
  uint8_t r1 = C;
  uint8_t result = setBit(r1, 0);
  C = result;
}

static void opSET_0_D(State *self) {
  uint8_t r1 = D;
  uint8_t result = setBit(r1, 0);
  D = result;
}

static void opSET_0_E(State *self) {
  uint8_t r1 = E;
  uint8_t result = setBit(r1, 0);
  E = result;
}

static void opSET_0_H(State *self) {
  uint8_t r1 = H;
  uint8_t result = setBit(r1, 0);
  H = result;
}

static void opSET_0_L(State *self) {
  uint8_t r1 = L;
  uint8_t result = setBit(r1, 0);
  L = result;
}

static void opSET_0_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true);
  uint8_t result = setBit(r1, 0);
  write8(self->bus, HLp, result, true);
}

static void opSET_0_A(State *self) {
  uint8_t r1 = A;
  uint8_t result = setBit(r1, 0);
  A = result;
}

static void opSET_1_B(State *self) {
  uint8_t r1 = B;
  uint8_t result = setBit(r1, 1);
  B = result;
}

static void opSET_1_C(State *self) {
  uint8_t r1 = C;
  uint8_t result = setBit(r1, 1);
  C = result;
}

static void opSET_1_D(State *self) {
  uint8_t r1 = D;
  uint8_t result = setBit(r1, 1);
  D = result;
}

static void opSET_1_E(State *self) {
  uint8_t r1 = E;
  uint8_t result = setBit(r1, 1);
  E = result;
}

static void opSET_1_H(State *self) {
  uint8_t r1 = H;
  uint8_t result = setBit(r1, 1);
  H = result;
}

static void opSET_1_L(State *self) {
  uint8_t r1 = L;
  uint8_t result = setBit(r1, 1);
  L = result;
}

static void opSET_1_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true);
  uint8_t result = setBit(r1, 1);
  write8(self->bus, HLp, result, true);
}

static void opSET_1_A(State *self) {
  uint8_t r1 = A;
  uint8_t result = setBit(r1, 1);
  A = result;
}

static void opSET_2_B(State *self) {
  uint8_t r1 = B;
  uint8_t result = setBit(r1, 2);
  B = result;
}

static void opSET_2_C(State *self) {
  uint8_t r1 = C;
  uint8_t result = setBit(r1, 2);
  C = result;
}

static void opSET_2_D(State *self) {
  uint8_t r1 = D;
  uint8_t result = setBit(r1, 2);
  D = result;
}

static void opSET_2_E(State *self) {
  uint8_t r1 = E;
  uint8_t result = setBit(r1, 2);
  E = result;
}

static void opSET_2_H(State *self) {
  uint8_t r1 = H;
  uint8_t result = setBit(r1, 2);
  H = result;
}

static void opSET_2_L(State *self) {
  uint8_t r1 = L;
  uint8_t result = setBit(r1, 2);
  L = result;
}

static void opSET_2_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true);
  uint8_t result = setBit(r1, 2);
  write8(self->bus, HLp, result, true);
}

static void opSET_2_A(State *self) {
  uint8_t r1 = A;
  uint8_t result = setBit(r1, 2);
  A = result;
}

static void opSET_3_B(State *self) {
  uint8_t r1 = B;
  uint8_t result = setBit(r1, 3);
  B = result;
}

static void opSET_3_C(State *self) {
  uint8_t r1 = C;
  uint8_t result = setBit(r1, 3);
  C = result;
}

static void opSET_3_D(State *self) {
  uint8_t r1 = D;
  uint8_t result = setBit(r1, 3);
  D = result;
}

static void opSET_3_E(State *self) {
  uint8_t r1 = E;
  uint8_t result = setBit(r1, 3);
  E = result;
}

static void opSET_3_H(State *self) {
  uint8_t r1 = H;
  uint8_t result = setBit(r1, 3);
  H = result;
}

static void opSET_3_L(State *self) {
  uint8_t r1 = L;
  uint8_t result = setBit(r1, 3);
  L = result;
}

static void opSET_3_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true);
  uint8_t result = setBit(r1, 3);
  write8(self->bus, HLp, result, true);
}

static void opSET_3_A(State *self) {
  uint8_t r1 = A;
  uint8_t result = setBit(r1, 3);
  A = result;
}

static void opSET_4_B(State *self) {
  uint8_t r1 = B;
  uint8_t result = setBit(r1, 4);
  B = result;
}

static void opSET_4_C(State *self) {
  uint8_t r1 = C;
  uint8_t result = setBit(r1, 4);
  C = result;
}

static void opSET_4_D(State *self) {
  uint8_t r1 = D;
  uint8_t result = setBit(r1, 4);
  D = result;
}

static void opSET_4_E(State *self) {
  uint8_t r1 = E;
  uint8_t result = setBit(r1, 4);
  E = result;
}

static void opSET_4_H(State *self) {
  uint8_t r1 = H;
  uint8_t result = setBit(r1, 4);
  H = result;
}

static void opSET_4_L(State *self) {
  uint8_t r1 = L;
  uint8_t result = setBit(r1, 4);
  L = result;
}

static void opSET_4_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true);
  uint8_t result = setBit(r1, 4);
  write8(self->bus, HLp, result, true);
}

static void opSET_4_A(State *self) {
  uint8_t r1 = A;
  uint8_t result = setBit(r1, 4);
  A = result;
}

static void opSET_5_B(State *self) {
  uint8_t r1 = B;
  uint8_t result = setBit(r1, 5);
  B = result;
}

static void opSET_5_C(State *self) {
  uint8_t r1 = C;
  uint8_t result = setBit(r1, 5);
  C = result;
}

static void opSET_5_D(State *self) {
  uint8_t r1 = D;
  uint8_t result = setBit(r1, 5);
  D = result;
}

static void opSET_5_E(State *self) {
  uint8_t r1 = E;
  uint8_t result = setBit(r1, 5);
  E = result;
}

static void opSET_5_H(State *self) {
  uint8_t r1 = H;
  uint8_t result = setBit(r1, 5);
  H = result;
}

static void opSET_5_L(State *self) {
  uint8_t r1 = L;
  uint8_t result = setBit(r1, 5);
  L = result;
}

static void opSET_5_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true);
  uint8_t result = setBit(r1, 5);
  write8(self->bus, HLp, result, true);
}

static void opSET_5_A(State *self) {
  uint8_t r1 = A;
  uint8_t result = setBit(r1, 5);
  A = result;
}

static void opSET_6_B(State *self) {
  uint8_t r1 = B;
  uint8_t result = setBit(r1, 6);
  B = result;
}

static void opSET_6_C(State *self) {
  uint8_t r1 = C;
  uint8_t result = setBit(r1, 6);
  C = result;
}

static void opSET_6_D(State *self) {
  uint8_t r1 = D;
  uint8_t result = setBit(r1, 6);
  D = result;
}

static void opSET_6_E(State *self) {
  uint8_t r1 = E;
  uint8_t result = setBit(r1, 6);
  E = result;
}

static void opSET_6_H(State *self) {
  uint8_t r1 = H;
  uint8_t result = setBit(r1, 6);
  H = result;
}

static void opSET_6_L(State *self) {
  uint8_t r1 = L;
  uint8_t result = setBit(r1, 6);
  L = result;
}

static void opSET_6_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true);
  uint8_t result = setBit(r1, 6);
  write8(self->bus, HLp, result, true);
}

static void opSET_6_A(State *self) {
  uint8_t r1 = A;
  uint8_t result = setBit(r1, 6);
  A = result;
}

static void opSET_7_B(State *self) {
  uint8_t r1 = B;
  uint8_t result = setBit(r1, 7);
  B = result;
}

static void opSET_7_C(State *self) {
  uint8_t r1 = C;
  uint8_t result = setBit(r1, 7);
  C = result;
}

static void opSET_7_D(State *self) {
  uint8_t r1 = D;
  uint8_t result = setBit(r1, 7);
  D = result;
}

static void opSET_7_E(State *self) {
  uint8_t r1 = E;
  uint8_t result = setBit(r1, 7);
  E = result;
}

static void opSET_7_H(State *self) {
  uint8_t r1 = H;
  uint8_t result = setBit(r1, 7);
  H = result;
}

static void opSET_7_L(State *self) {
  uint8_t r1 = L;
  uint8_t result = setBit(r1, 7);
  L = result;
}

static void opSET_7_deref_HL(State *self) {
  uint8_t r1 = read8(self->bus, HLp, true, true);
  uint8_t result = setBit(r1, 7);
  write8(self->bus, HLp, result, true);
}

static void opSET_7_A(State *self) {
  uint8_t r1 = A;
  uint8_t result = setBit(r1, 7);
  A = result;
}

static void opPOP_BC(State *self) { BCp = pop(self); }

static void opPUSH_BC(State *self) {
  internal(self->bus);
  push(self, BCp);
}

static void opPOP_DE(State *self) { DEp = pop(self); }

static void opPUSH_DE(State *self) {
  internal(self->bus);
  push(self, DEp);
}

static void opPOP_HL(State *self) { HLp = pop(self); }

static void opPUSH_HL(State *self) {
  internal(self->bus);
  push(self, HLp);
}

static void opPOP_AF(State *self) {
  uint16_t result = pop(self);
  A = MSB(result);
  F = result & 0xFFF0;
}

static void opPUSH_AF(State *self) {
  internal(self->bus);
  push(self, AFp);
}

static void opJR_I8(State *self) {
  uint16_t address = (uint16_t)(int8_t)next8(self);
  PC += address;

  internal(self->bus);
}

static void opJR_NZ_I8(State *self) {
  uint16_t address = (uint16_t)(int8_t)next8(self);

  if (!getFlag(ftZ)) {
    PC += address;
    internal(self->bus);
  }
}

static void opJR_Z_I8(State *self) {
  uint16_t address = (uint16_t)(int8_t)next8(self);

  if (getFlag(ftZ)) {
    PC += address;
    internal(self->bus);
  }
}

static void opJR_NC_I8(State *self) {
  uint16_t address = (uint16_t)(int8_t)next8(self);

  if (!getFlag(ftC)) {
    PC += address;
    internal(self->bus);
  }
}

static void opJR_C_I8(State *self) {
  uint16_t address = (uint16_t)(int8_t)next8(self);

  if (getFlag(ftC)) {
    PC += address;
    internal(self->bus);
  }
}

static void opRET_NZ(State *self) {
  if (!getFlag(ftZ)) {
    PC = pop(self);
    internal(self->bus);
  }

  internal(self->bus);
}

static void opJP_NZ_U16(State *self) {
  uint16_t address = next16(self);

  if (!getFlag(ftZ)) {
    PC = address;
    internal(self->bus);
  }
}

static void opJP_U16(State *self) {
  uint16_t address = next16(self);
  PC = address;

  internal(self->bus);
}

static void opRET_Z(State *self) {
  if (getFlag(ftZ)) {
    PC = pop(self);
    internal(self->bus);
  }

  internal(self->bus);
}

static void opRET(State *self) {
  PC = pop(self);

  internal(self->bus);
}

static void opJP_Z_U16(State *self) {
  uint16_t address = next16(self);

  if (getFlag(ftZ)) {
    PC = address;
    internal(self->bus);
  }
}

static void opRET_NC(State *self) {
  if (!getFlag(ftC)) {
    PC = pop(self);
    internal(self->bus);
  }

  internal(self->bus);
}

static void opJP_NC_U16(State *self) {
  uint16_t address = next16(self);

  if (!getFlag(ftC)) {
    PC = address;
    internal(self->bus);
  }
}

static void opRET_C(State *self) {
  if (getFlag(ftC)) {
    PC = pop(self);
    internal(self->bus);
  }

  internal(self->bus);
}

static void opJP_C_U16(State *self) {
  uint16_t address = next16(self);

  if (getFlag(ftC)) {
    PC = address;
    internal(self->bus);
  }
}

static void opCALL_NZ_U16(State *self) {
  uint16_t address = next16(self);

  if (!getFlag(ftZ)) {
    push(self, PC);
    internal(self->bus);
    PC = address;
  }
}

static void opRST_00H(State *self) {
  internal(self->bus);
  uint16_t address = 0x00;

  push(self, PC);
  PC = address;
}

static void opCALL_Z_U16(State *self) {
  uint16_t address = next16(self);

  if (getFlag(ftZ)) {
    push(self, PC);
    internal(self->bus);
    PC = address;
  }
}

static void opCALL_U16(State *self) {
  internal(self->bus);
  uint16_t address = next16(self);

  push(self, PC);
  PC = address;
}

static void opRST_08H(State *self) {
  internal(self->bus);
  uint16_t address = 0x08;

  push(self, PC);
  PC = address;
}

static void opCALL_NC_U16(State *self) {
  uint16_t address = next16(self);

  if (!getFlag(ftC)) {
    push(self, PC);
    internal(self->bus);
    PC = address;
  }
}

static void opRST_10H(State *self) {
  internal(self->bus);
  uint16_t address = 0x10;

  push(self, PC);
  PC = address;
}

static void opCALL_C_U16(State *self) {
  uint16_t address = next16(self);

  if (getFlag(ftC)) {
    push(self, PC);
    internal(self->bus);
    PC = address;
  }
}

static void opRST_18H(State *self) {
  internal(self->bus);
  uint16_t address = 0x18;

  push(self, PC);
  PC = address;
}

static void opRST_20H(State *self) {
  internal(self->bus);
  uint16_t address = 0x20;

  push(self, PC);
  PC = address;
}

static void opRST_28H(State *self) {
  internal(self->bus);
  uint16_t address = 0x28;

  push(self, PC);
  PC = address;
}

static void opRST_30H(State *self) {
  internal(self->bus);
  uint16_t address = 0x30;

  push(self, PC);
  PC = address;
}

static void opRST_38H(State *self) {
  internal(self->bus);
  uint16_t address = 0x38;

  push(self, PC);
  PC = address;
}

static void opNOP(State *_) { return; }

static void opLD_deref_U16_SP(State *self) {
  write16(self->bus, (next16(self)), SP);
}

static void opSTOP(State *self) { self->halted = true; }

static void opLD_deref_HLi_A(State *self) {
  write8(self->bus, (HLp++), A, true);
}

static void opDAA(State *self) {
  uint8_t a = A;

  if (!getFlag(ftN)) {
    if (getFlag(ftH) | ((A & 0x0F) > 9)) { a += 6; }

    if (getFlag(ftC) | (A > 0x99)) {
      a += 0x60;

      Cf(true);
    }

  } else {
    if (getFlag(ftC)) { a -= 0x60; }

    if (getFlag(ftH)) { a -= 6; }
  }

  Zf(a == 0);

  Hf(false);

  A = a;
}

static void opLD_A_deref_HLi(State *self) {
  A = read8(self->bus, (HLp++), true, true);
}

static void opCPL(State *self) {
  A = ~A;

  Nf(true);

  Hf(true);
}

static void opLD_deref_HLd_A(State *self) {
  write8(self->bus, (HLp--), A, true);
}

static void opSCF(State *self) {
  Nf(false);

  Hf(false);

  Cf(true);
}

static void opLD_A_deref_HLd(State *self) {
  A = read8(self->bus, (HLp--), true, true);
}

static void opCCF(State *self) {
  Nf(false);

  Hf(false);

  Cf(~getFlag(ftC));
}

static void opHALT(State *self) { self->halted = true; }

static void opRETI(State *self) {
  self->IMERising = true;

  opRET(self);
}

static void opLD_deref_FF00_U8_A(State *self) {
  write8(self->bus, (0xFF00 + next8(self)), A, true);
}

static void opLD_deref_FF00_C_A(State *self) {
  write8(self->bus, (0xFF00 + C), A, true);
}

static void opADD_SP_I8(State *self) {
  uint16_t r1 = SP, src = (uint16_t)(int8_t)(next8(self));

  uint16_t result = r1 + src;

  SP = result;

  internal(self->bus);

  Zf(false);

  Nf(false);

  Hf((r1 & 0xF) + (src & 0xF) >= 0x10);

  Cf((r1 & 0xFF) + (src & 0xFF) > 0xFF);

  internal(self->bus);
}

static void opJP_HL(State *self) { PC = HLp; }

static void opLD_deref_U16_A(State *self) {
  write8(self->bus, (next16(self)), A, true);
}

static void opLD_A_deref_FF00_U8(State *self) {
  A = read8(self->bus, (0xFF00 + next8(self)), true, true);
}

static void opLD_A_deref_FF00_C(State *self) {
  A = read8(self->bus, (0xFF00 + C), true, true);
}

static void opDI(State *self) { self->IME = false; }

static void opLD_HL_SP_I8(State *self) {
  uint16_t r1 = SP, src = (uint16_t)(int8_t)(next8(self));

  uint16_t result = r1 + src;

  HLp = result;

  internal(self->bus);

  Zf(false);

  Nf(false);

  Hf((r1 & 0xF) + (src & 0xF) >= 0x10);

  Cf((r1 & 0xFF) + (src & 0xFF) > 0xFF);
}

static void opLD_A_deref_U16(State *self) {
  A = read8(self->bus, (next16(self)), true, true);
}

static void opEI(State *self) { self->IMERising = true; }

// --------------- Lookup Tables -----------------

void attachPrefixed(Backend *self) {
  fnptr *LUT = self->PREFIXED;
  LUT[0x0] = opRLC_B;
  LUT[0x1] = opRLC_C;
  LUT[0x2] = opRLC_D;
  LUT[0x3] = opRLC_E;
  LUT[0x4] = opRLC_H;
  LUT[0x5] = opRLC_L;
  LUT[0x6] = opRLC_deref_HL;
  LUT[0x7] = opRLC_A;
  LUT[0x8] = opRRC_B;
  LUT[0x9] = opRRC_C;
  LUT[0xa] = opRRC_D;
  LUT[0xb] = opRRC_E;
  LUT[0xc] = opRRC_H;
  LUT[0xd] = opRRC_L;
  LUT[0xe] = opRRC_deref_HL;
  LUT[0xf] = opRRC_A;
  LUT[0x10] = opRL_B;
  LUT[0x11] = opRL_C;
  LUT[0x12] = opRL_D;
  LUT[0x13] = opRL_E;
  LUT[0x14] = opRL_H;
  LUT[0x15] = opRL_L;
  LUT[0x16] = opRL_deref_HL;
  LUT[0x17] = opRL_A;
  LUT[0x18] = opRR_B;
  LUT[0x19] = opRR_C;
  LUT[0x1a] = opRR_D;
  LUT[0x1b] = opRR_E;
  LUT[0x1c] = opRR_H;
  LUT[0x1d] = opRR_L;
  LUT[0x1e] = opRR_deref_HL;
  LUT[0x1f] = opRR_A;
  LUT[0x20] = opSLA_B;
  LUT[0x21] = opSLA_C;
  LUT[0x22] = opSLA_D;
  LUT[0x23] = opSLA_E;
  LUT[0x24] = opSLA_H;
  LUT[0x25] = opSLA_L;
  LUT[0x26] = opSLA_deref_HL;
  LUT[0x27] = opSLA_A;
  LUT[0x28] = opSRA_B;
  LUT[0x29] = opSRA_C;
  LUT[0x2a] = opSRA_D;
  LUT[0x2b] = opSRA_E;
  LUT[0x2c] = opSRA_H;
  LUT[0x2d] = opSRA_L;
  LUT[0x2e] = opSRA_deref_HL;
  LUT[0x2f] = opSRA_A;
  LUT[0x30] = opSWAP_B;
  LUT[0x31] = opSWAP_C;
  LUT[0x32] = opSWAP_D;
  LUT[0x33] = opSWAP_E;
  LUT[0x34] = opSWAP_H;
  LUT[0x35] = opSWAP_L;
  LUT[0x36] = opSWAP_deref_HL;
  LUT[0x37] = opSWAP_A;
  LUT[0x38] = opSRL_B;
  LUT[0x39] = opSRL_C;
  LUT[0x3a] = opSRL_D;
  LUT[0x3b] = opSRL_E;
  LUT[0x3c] = opSRL_H;
  LUT[0x3d] = opSRL_L;
  LUT[0x3e] = opSRL_deref_HL;
  LUT[0x3f] = opSRL_A;
  LUT[0x40] = opBIT_0_B;
  LUT[0x41] = opBIT_0_C;
  LUT[0x42] = opBIT_0_D;
  LUT[0x43] = opBIT_0_E;
  LUT[0x44] = opBIT_0_H;
  LUT[0x45] = opBIT_0_L;
  LUT[0x46] = opBIT_0_deref_HL;
  LUT[0x47] = opBIT_0_A;
  LUT[0x48] = opBIT_1_B;
  LUT[0x49] = opBIT_1_C;
  LUT[0x4a] = opBIT_1_D;
  LUT[0x4b] = opBIT_1_E;
  LUT[0x4c] = opBIT_1_H;
  LUT[0x4d] = opBIT_1_L;
  LUT[0x4e] = opBIT_1_deref_HL;
  LUT[0x4f] = opBIT_1_A;
  LUT[0x50] = opBIT_2_B;
  LUT[0x51] = opBIT_2_C;
  LUT[0x52] = opBIT_2_D;
  LUT[0x53] = opBIT_2_E;
  LUT[0x54] = opBIT_2_H;
  LUT[0x55] = opBIT_2_L;
  LUT[0x56] = opBIT_2_deref_HL;
  LUT[0x57] = opBIT_2_A;
  LUT[0x58] = opBIT_3_B;
  LUT[0x59] = opBIT_3_C;
  LUT[0x5a] = opBIT_3_D;
  LUT[0x5b] = opBIT_3_E;
  LUT[0x5c] = opBIT_3_H;
  LUT[0x5d] = opBIT_3_L;
  LUT[0x5e] = opBIT_3_deref_HL;
  LUT[0x5f] = opBIT_3_A;
  LUT[0x60] = opBIT_4_B;
  LUT[0x61] = opBIT_4_C;
  LUT[0x62] = opBIT_4_D;
  LUT[0x63] = opBIT_4_E;
  LUT[0x64] = opBIT_4_H;
  LUT[0x65] = opBIT_4_L;
  LUT[0x66] = opBIT_4_deref_HL;
  LUT[0x67] = opBIT_4_A;
  LUT[0x68] = opBIT_5_B;
  LUT[0x69] = opBIT_5_C;
  LUT[0x6a] = opBIT_5_D;
  LUT[0x6b] = opBIT_5_E;
  LUT[0x6c] = opBIT_5_H;
  LUT[0x6d] = opBIT_5_L;
  LUT[0x6e] = opBIT_5_deref_HL;
  LUT[0x6f] = opBIT_5_A;
  LUT[0x70] = opBIT_6_B;
  LUT[0x71] = opBIT_6_C;
  LUT[0x72] = opBIT_6_D;
  LUT[0x73] = opBIT_6_E;
  LUT[0x74] = opBIT_6_H;
  LUT[0x75] = opBIT_6_L;
  LUT[0x76] = opBIT_6_deref_HL;
  LUT[0x77] = opBIT_6_A;
  LUT[0x78] = opBIT_7_B;
  LUT[0x79] = opBIT_7_C;
  LUT[0x7a] = opBIT_7_D;
  LUT[0x7b] = opBIT_7_E;
  LUT[0x7c] = opBIT_7_H;
  LUT[0x7d] = opBIT_7_L;
  LUT[0x7e] = opBIT_7_deref_HL;
  LUT[0x7f] = opBIT_7_A;
  LUT[0x80] = opRES_0_B;
  LUT[0x81] = opRES_0_C;
  LUT[0x82] = opRES_0_D;
  LUT[0x83] = opRES_0_E;
  LUT[0x84] = opRES_0_H;
  LUT[0x85] = opRES_0_L;
  LUT[0x86] = opRES_0_deref_HL;
  LUT[0x87] = opRES_0_A;
  LUT[0x88] = opRES_1_B;
  LUT[0x89] = opRES_1_C;
  LUT[0x8a] = opRES_1_D;
  LUT[0x8b] = opRES_1_E;
  LUT[0x8c] = opRES_1_H;
  LUT[0x8d] = opRES_1_L;
  LUT[0x8e] = opRES_1_deref_HL;
  LUT[0x8f] = opRES_1_A;
  LUT[0x90] = opRES_2_B;
  LUT[0x91] = opRES_2_C;
  LUT[0x92] = opRES_2_D;
  LUT[0x93] = opRES_2_E;
  LUT[0x94] = opRES_2_H;
  LUT[0x95] = opRES_2_L;
  LUT[0x96] = opRES_2_deref_HL;
  LUT[0x97] = opRES_2_A;
  LUT[0x98] = opRES_3_B;
  LUT[0x99] = opRES_3_C;
  LUT[0x9a] = opRES_3_D;
  LUT[0x9b] = opRES_3_E;
  LUT[0x9c] = opRES_3_H;
  LUT[0x9d] = opRES_3_L;
  LUT[0x9e] = opRES_3_deref_HL;
  LUT[0x9f] = opRES_3_A;
  LUT[0xa0] = opRES_4_B;
  LUT[0xa1] = opRES_4_C;
  LUT[0xa2] = opRES_4_D;
  LUT[0xa3] = opRES_4_E;
  LUT[0xa4] = opRES_4_H;
  LUT[0xa5] = opRES_4_L;
  LUT[0xa6] = opRES_4_deref_HL;
  LUT[0xa7] = opRES_4_A;
  LUT[0xa8] = opRES_5_B;
  LUT[0xa9] = opRES_5_C;
  LUT[0xaa] = opRES_5_D;
  LUT[0xab] = opRES_5_E;
  LUT[0xac] = opRES_5_H;
  LUT[0xad] = opRES_5_L;
  LUT[0xae] = opRES_5_deref_HL;
  LUT[0xaf] = opRES_5_A;
  LUT[0xb0] = opRES_6_B;
  LUT[0xb1] = opRES_6_C;
  LUT[0xb2] = opRES_6_D;
  LUT[0xb3] = opRES_6_E;
  LUT[0xb4] = opRES_6_H;
  LUT[0xb5] = opRES_6_L;
  LUT[0xb6] = opRES_6_deref_HL;
  LUT[0xb7] = opRES_6_A;
  LUT[0xb8] = opRES_7_B;
  LUT[0xb9] = opRES_7_C;
  LUT[0xba] = opRES_7_D;
  LUT[0xbb] = opRES_7_E;
  LUT[0xbc] = opRES_7_H;
  LUT[0xbd] = opRES_7_L;
  LUT[0xbe] = opRES_7_deref_HL;
  LUT[0xbf] = opRES_7_A;
  LUT[0xc0] = opSET_0_B;
  LUT[0xc1] = opSET_0_C;
  LUT[0xc2] = opSET_0_D;
  LUT[0xc3] = opSET_0_E;
  LUT[0xc4] = opSET_0_H;
  LUT[0xc5] = opSET_0_L;
  LUT[0xc6] = opSET_0_deref_HL;
  LUT[0xc7] = opSET_0_A;
  LUT[0xc8] = opSET_1_B;
  LUT[0xc9] = opSET_1_C;
  LUT[0xca] = opSET_1_D;
  LUT[0xcb] = opSET_1_E;
  LUT[0xcc] = opSET_1_H;
  LUT[0xcd] = opSET_1_L;
  LUT[0xce] = opSET_1_deref_HL;
  LUT[0xcf] = opSET_1_A;
  LUT[0xd0] = opSET_2_B;
  LUT[0xd1] = opSET_2_C;
  LUT[0xd2] = opSET_2_D;
  LUT[0xd3] = opSET_2_E;
  LUT[0xd4] = opSET_2_H;
  LUT[0xd5] = opSET_2_L;
  LUT[0xd6] = opSET_2_deref_HL;
  LUT[0xd7] = opSET_2_A;
  LUT[0xd8] = opSET_3_B;
  LUT[0xd9] = opSET_3_C;
  LUT[0xda] = opSET_3_D;
  LUT[0xdb] = opSET_3_E;
  LUT[0xdc] = opSET_3_H;
  LUT[0xdd] = opSET_3_L;
  LUT[0xde] = opSET_3_deref_HL;
  LUT[0xdf] = opSET_3_A;
  LUT[0xe0] = opSET_4_B;
  LUT[0xe1] = opSET_4_C;
  LUT[0xe2] = opSET_4_D;
  LUT[0xe3] = opSET_4_E;
  LUT[0xe4] = opSET_4_H;
  LUT[0xe5] = opSET_4_L;
  LUT[0xe6] = opSET_4_deref_HL;
  LUT[0xe7] = opSET_4_A;
  LUT[0xe8] = opSET_5_B;
  LUT[0xe9] = opSET_5_C;
  LUT[0xea] = opSET_5_D;
  LUT[0xeb] = opSET_5_E;
  LUT[0xec] = opSET_5_H;
  LUT[0xed] = opSET_5_L;
  LUT[0xee] = opSET_5_deref_HL;
  LUT[0xef] = opSET_5_A;
  LUT[0xf0] = opSET_6_B;
  LUT[0xf1] = opSET_6_C;
  LUT[0xf2] = opSET_6_D;
  LUT[0xf3] = opSET_6_E;
  LUT[0xf4] = opSET_6_H;
  LUT[0xf5] = opSET_6_L;
  LUT[0xf6] = opSET_6_deref_HL;
  LUT[0xf7] = opSET_6_A;
  LUT[0xf8] = opSET_7_B;
  LUT[0xf9] = opSET_7_C;
  LUT[0xfa] = opSET_7_D;
  LUT[0xfb] = opSET_7_E;
  LUT[0xfc] = opSET_7_H;
  LUT[0xfd] = opSET_7_L;
  LUT[0xfe] = opSET_7_deref_HL;
  LUT[0xff] = opSET_7_A;
}

void attachUnPrefixed(Backend *self) {
  fnptr *LUT = self->UNPREFIXED;
  LUT[0x0] = opNOP;
  LUT[0x1] = opLD_BC_U16;
  LUT[0x2] = opLD_deref_BC_A;
  LUT[0x3] = opINC_BC;
  LUT[0x4] = opINC_B;
  LUT[0x5] = opDEC_B;
  LUT[0x6] = opLD_B_U8;
  LUT[0x7] = opRLCA;
  LUT[0x8] = opLD_deref_U16_SP;
  LUT[0x9] = opADD_HL_BC;
  LUT[0xa] = opLD_A_deref_BC;
  LUT[0xb] = opDEC_BC;
  LUT[0xc] = opINC_C;
  LUT[0xd] = opDEC_C;
  LUT[0xe] = opLD_C_U8;
  LUT[0xf] = opRRCA;
  LUT[0x10] = opSTOP;
  LUT[0x11] = opLD_DE_U16;
  LUT[0x12] = opLD_deref_DE_A;
  LUT[0x13] = opINC_DE;
  LUT[0x14] = opINC_D;
  LUT[0x15] = opDEC_D;
  LUT[0x16] = opLD_D_U8;
  LUT[0x17] = opRLA;
  LUT[0x18] = opJR_I8;
  LUT[0x19] = opADD_HL_DE;
  LUT[0x1a] = opLD_A_deref_DE;
  LUT[0x1b] = opDEC_DE;
  LUT[0x1c] = opINC_E;
  LUT[0x1d] = opDEC_E;
  LUT[0x1e] = opLD_E_U8;
  LUT[0x1f] = opRRA;
  LUT[0x20] = opJR_NZ_I8;
  LUT[0x21] = opLD_HL_U16;
  LUT[0x22] = opLD_deref_HLi_A;
  LUT[0x23] = opINC_HL;
  LUT[0x24] = opINC_H;
  LUT[0x25] = opDEC_H;
  LUT[0x26] = opLD_H_U8;
  LUT[0x27] = opDAA;
  LUT[0x28] = opJR_Z_I8;
  LUT[0x29] = opADD_HL_HL;
  LUT[0x2a] = opLD_A_deref_HLi;
  LUT[0x2b] = opDEC_HL;
  LUT[0x2c] = opINC_L;
  LUT[0x2d] = opDEC_L;
  LUT[0x2e] = opLD_L_U8;
  LUT[0x2f] = opCPL;
  LUT[0x30] = opJR_NC_I8;
  LUT[0x31] = opLD_SP_U16;
  LUT[0x32] = opLD_deref_HLd_A;
  LUT[0x33] = opINC_SP;
  LUT[0x34] = opINC_deref_HL;
  LUT[0x35] = opDEC_deref_HL;
  LUT[0x36] = opLD_deref_HL_U8;
  LUT[0x37] = opSCF;
  LUT[0x38] = opJR_C_I8;
  LUT[0x39] = opADD_HL_SP;
  LUT[0x3a] = opLD_A_deref_HLd;
  LUT[0x3b] = opDEC_SP;
  LUT[0x3c] = opINC_A;
  LUT[0x3d] = opDEC_A;
  LUT[0x3e] = opLD_A_U8;
  LUT[0x3f] = opCCF;
  LUT[0x40] = opLD_B_B;
  LUT[0x41] = opLD_B_C;
  LUT[0x42] = opLD_B_D;
  LUT[0x43] = opLD_B_E;
  LUT[0x44] = opLD_B_H;
  LUT[0x45] = opLD_B_L;
  LUT[0x46] = opLD_B_deref_HL;
  LUT[0x47] = opLD_B_A;
  LUT[0x48] = opLD_C_B;
  LUT[0x49] = opLD_C_C;
  LUT[0x4a] = opLD_C_D;
  LUT[0x4b] = opLD_C_E;
  LUT[0x4c] = opLD_C_H;
  LUT[0x4d] = opLD_C_L;
  LUT[0x4e] = opLD_C_deref_HL;
  LUT[0x4f] = opLD_C_A;
  LUT[0x50] = opLD_D_B;
  LUT[0x51] = opLD_D_C;
  LUT[0x52] = opLD_D_D;
  LUT[0x53] = opLD_D_E;
  LUT[0x54] = opLD_D_H;
  LUT[0x55] = opLD_D_L;
  LUT[0x56] = opLD_D_deref_HL;
  LUT[0x57] = opLD_D_A;
  LUT[0x58] = opLD_E_B;
  LUT[0x59] = opLD_E_C;
  LUT[0x5a] = opLD_E_D;
  LUT[0x5b] = opLD_E_E;
  LUT[0x5c] = opLD_E_H;
  LUT[0x5d] = opLD_E_L;
  LUT[0x5e] = opLD_E_deref_HL;
  LUT[0x5f] = opLD_E_A;
  LUT[0x60] = opLD_H_B;
  LUT[0x61] = opLD_H_C;
  LUT[0x62] = opLD_H_D;
  LUT[0x63] = opLD_H_E;
  LUT[0x64] = opLD_H_H;
  LUT[0x65] = opLD_H_L;
  LUT[0x66] = opLD_H_deref_HL;
  LUT[0x67] = opLD_H_A;
  LUT[0x68] = opLD_L_B;
  LUT[0x69] = opLD_L_C;
  LUT[0x6a] = opLD_L_D;
  LUT[0x6b] = opLD_L_E;
  LUT[0x6c] = opLD_L_H;
  LUT[0x6d] = opLD_L_L;
  LUT[0x6e] = opLD_L_deref_HL;
  LUT[0x6f] = opLD_L_A;
  LUT[0x70] = opLD_deref_HL_B;
  LUT[0x71] = opLD_deref_HL_C;
  LUT[0x72] = opLD_deref_HL_D;
  LUT[0x73] = opLD_deref_HL_E;
  LUT[0x74] = opLD_deref_HL_H;
  LUT[0x75] = opLD_deref_HL_L;
  LUT[0x76] = opHALT;
  LUT[0x77] = opLD_deref_HL_A;
  LUT[0x78] = opLD_A_B;
  LUT[0x79] = opLD_A_C;
  LUT[0x7a] = opLD_A_D;
  LUT[0x7b] = opLD_A_E;
  LUT[0x7c] = opLD_A_H;
  LUT[0x7d] = opLD_A_L;
  LUT[0x7e] = opLD_A_deref_HL;
  LUT[0x7f] = opLD_A_A;
  LUT[0x80] = opADD_A_B;
  LUT[0x81] = opADD_A_C;
  LUT[0x82] = opADD_A_D;
  LUT[0x83] = opADD_A_E;
  LUT[0x84] = opADD_A_H;
  LUT[0x85] = opADD_A_L;
  LUT[0x86] = opADD_A_deref_HL;
  LUT[0x87] = opADD_A_A;
  LUT[0x88] = opADC_A_B;
  LUT[0x89] = opADC_A_C;
  LUT[0x8a] = opADC_A_D;
  LUT[0x8b] = opADC_A_E;
  LUT[0x8c] = opADC_A_H;
  LUT[0x8d] = opADC_A_L;
  LUT[0x8e] = opADC_A_deref_HL;
  LUT[0x8f] = opADC_A_A;
  LUT[0x90] = opSUB_A_B;
  LUT[0x91] = opSUB_A_C;
  LUT[0x92] = opSUB_A_D;
  LUT[0x93] = opSUB_A_E;
  LUT[0x94] = opSUB_A_H;
  LUT[0x95] = opSUB_A_L;
  LUT[0x96] = opSUB_A_deref_HL;
  LUT[0x97] = opSUB_A_A;
  LUT[0x98] = opSBC_A_B;
  LUT[0x99] = opSBC_A_C;
  LUT[0x9a] = opSBC_A_D;
  LUT[0x9b] = opSBC_A_E;
  LUT[0x9c] = opSBC_A_H;
  LUT[0x9d] = opSBC_A_L;
  LUT[0x9e] = opSBC_A_deref_HL;
  LUT[0x9f] = opSBC_A_A;
  LUT[0xa0] = opAND_A_B;
  LUT[0xa1] = opAND_A_C;
  LUT[0xa2] = opAND_A_D;
  LUT[0xa3] = opAND_A_E;
  LUT[0xa4] = opAND_A_H;
  LUT[0xa5] = opAND_A_L;
  LUT[0xa6] = opAND_A_deref_HL;
  LUT[0xa7] = opAND_A_A;
  LUT[0xa8] = opXOR_A_B;
  LUT[0xa9] = opXOR_A_C;
  LUT[0xaa] = opXOR_A_D;
  LUT[0xab] = opXOR_A_E;
  LUT[0xac] = opXOR_A_H;
  LUT[0xad] = opXOR_A_L;
  LUT[0xae] = opXOR_A_deref_HL;
  LUT[0xaf] = opXOR_A_A;
  LUT[0xb0] = opOR_A_B;
  LUT[0xb1] = opOR_A_C;
  LUT[0xb2] = opOR_A_D;
  LUT[0xb3] = opOR_A_E;
  LUT[0xb4] = opOR_A_H;
  LUT[0xb5] = opOR_A_L;
  LUT[0xb6] = opOR_A_deref_HL;
  LUT[0xb7] = opOR_A_A;
  LUT[0xb8] = opCP_A_B;
  LUT[0xb9] = opCP_A_C;
  LUT[0xba] = opCP_A_D;
  LUT[0xbb] = opCP_A_E;
  LUT[0xbc] = opCP_A_H;
  LUT[0xbd] = opCP_A_L;
  LUT[0xbe] = opCP_A_deref_HL;
  LUT[0xbf] = opCP_A_A;
  LUT[0xc0] = opRET_NZ;
  LUT[0xc1] = opPOP_BC;
  LUT[0xc2] = opJP_NZ_U16;
  LUT[0xc3] = opJP_U16;
  LUT[0xc4] = opCALL_NZ_U16;
  LUT[0xc5] = opPUSH_BC;
  LUT[0xc6] = opADD_A_U8;
  LUT[0xc7] = opRST_00H;
  LUT[0xc8] = opRET_Z;
  LUT[0xc9] = opRET;
  LUT[0xca] = opJP_Z_U16;
  LUT[0xcc] = opCALL_Z_U16;
  LUT[0xcd] = opCALL_U16;
  LUT[0xce] = opADC_A_U8;
  LUT[0xcf] = opRST_08H;
  LUT[0xd0] = opRET_NC;
  LUT[0xd1] = opPOP_DE;
  LUT[0xd2] = opJP_NC_U16;
  LUT[0xd4] = opCALL_NC_U16;
  LUT[0xd5] = opPUSH_DE;
  LUT[0xd6] = opSUB_A_U8;
  LUT[0xd7] = opRST_10H;
  LUT[0xd8] = opRET_C;
  LUT[0xd9] = opRETI;
  LUT[0xda] = opJP_C_U16;
  LUT[0xdc] = opCALL_C_U16;
  LUT[0xde] = opSBC_A_U8;
  LUT[0xdf] = opRST_18H;
  LUT[0xe0] = opLD_deref_FF00_U8_A;
  LUT[0xe1] = opPOP_HL;
  LUT[0xe2] = opLD_deref_FF00_C_A;
  LUT[0xe5] = opPUSH_HL;
  LUT[0xe6] = opAND_A_U8;
  LUT[0xe7] = opRST_20H;
  LUT[0xe8] = opADD_SP_I8;
  LUT[0xe9] = opJP_HL;
  LUT[0xea] = opLD_deref_U16_A;
  LUT[0xee] = opXOR_A_U8;
  LUT[0xef] = opRST_28H;
  LUT[0xf0] = opLD_A_deref_FF00_U8;
  LUT[0xf1] = opPOP_AF;
  LUT[0xf2] = opLD_A_deref_FF00_C;
  LUT[0xf3] = opDI;
  LUT[0xf5] = opPUSH_AF;
  LUT[0xf6] = opOR_A_U8;
  LUT[0xf7] = opRST_30H;
  LUT[0xf8] = opLD_HL_SP_I8;
  LUT[0xf9] = opLD_SP_HL;
  LUT[0xfa] = opLD_A_deref_U16;
  LUT[0xfb] = opEI;
  LUT[0xfe] = opCP_A_U8;
  LUT[0xff] = opRST_38H;
}

#undef AFp
#undef BCp
#undef DEp
#undef HLp
#undef SP
#undef PC
#undef A
#undef F
#undef B
#undef C
#undef D
#undef E
#undef H
#undef L
#undef COPY_FLAG
#undef Cf
#undef Hf
#undef Nf
#undef Zf
#undef getFlag

Backend createBackend(Bus *bus) {
  Registers regs;
  regs.AF.hi = 0x01;
  regs.AF.lo = 0xB0;
  regs.BC.hi = 0x00;
  regs.BC.lo = 0x13;
  regs.DE.hi = 0x00;
  regs.DE.lo = 0xD8;
  regs.HL.hi = 0x01;
  regs.HL.lo = 0x4D;
  regs.SP = 0xFFFE;
  State state = {.bus = bus,
                 .regs = regs,
                 .opcode = 0x00,
                 .halted = false,
                 .IME = false,
                 .IMERising = false};

  Backend backend = {.state = state};
  attachPrefixed(&backend);
  attachUnPrefixed(&backend);
  return backend;
}

void backendTick(Backend *self) {
  self->state.opcode = next8(&self->state);
  if (self->state.opcode == 0xCB) {
    uint8_t next = next8(&self->state);
    self->PREFIXED[next](&self->state);

  } else {
    self->UNPREFIXED[self->state.opcode](&self->state);
  }
}

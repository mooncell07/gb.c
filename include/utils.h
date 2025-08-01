#pragma once
#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdint.h>

#define LSB(v) (v & 0xFF)
#define MSB(v) (v >> 8)
#define JOIN(hi, lo) ((hi << 8) | lo)
#define BT(v, pos) (v & (1 << pos) ? true : false)

#define setBit(v, pos) (v |= (1 << pos))
#define clearBit(v, pos) (v &= ~(1 << pos))

#define checkHalfCarry(a, b) ((a & 0xF) + (b & 0xF) >= 0x10)
#define checkHalfBorrow(a, b) ((a & 0xF) < (b & 0xF))

static inline uint8_t rotateLeftBits(uint8_t value, int count) {
  return (value << count) | (value >> (8 - count));
}

static inline uint8_t rotateRightBits(uint8_t value, int count) {
  return (value >> count) | (value << (8 - count));
}

static inline uint8_t BEXTR(uint8_t v, int start, int end) {
  uint8_t len = (end - start + 1);
  return (v >> start) & ((1 << len) - 1);
}

#define BOUND(address, lower, upper) (lower <= address && address <= upper)


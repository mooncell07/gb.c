#pragma once
#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint8_t P1;
  uint8_t keyState;
  uint8_t keyMask;
} JoypadState;

extern JoypadState js;

void unsetKeyBit(SDL_Keycode);
void setKeyBit(SDL_Keycode);
void setKeyMask();

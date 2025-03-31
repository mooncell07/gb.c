#include "joypad.h"

#include <SDL3/SDL.h>
#include <stdint.h>

#include "utils.h"

JoypadState js = {0};

#define toggleBit(pos) setBit(js.keyState, pos);
#define resetBit(pos) clearBit(js.keyState, pos);

void unsetKeyBit(SDL_Keycode key) {
    switch (key) {
    case SDLK_Z:
        resetBit(0);
        break;
    case SDLK_X:
        resetBit(1);
        break;
    case SDLK_RSHIFT:
        resetBit(2);
        break;
    case SDLK_RETURN:
        resetBit(3);
        break;

    case SDLK_RIGHT:
        resetBit(4);
        break;
    case SDLK_LEFT:
        resetBit(5);
        break;
    case SDLK_UP:
        resetBit(6);
        break;
    case SDLK_DOWN:
        resetBit(7);
        break;
    default:
        break;
    }
}

void setKeyBit(SDL_Keycode key) {
    switch (key) {
    case SDLK_Z:
        toggleBit(0);
        break;
    case SDLK_X:
        toggleBit(1);
        break;
    case SDLK_RSHIFT:
        toggleBit(2);
        break;
    case SDLK_RETURN:
        toggleBit(3);
        break;

    case SDLK_RIGHT:
        toggleBit(4);
        break;
    case SDLK_LEFT:
        toggleBit(5);
        break;
    case SDLK_UP:
        toggleBit(6);
        break;
    case SDLK_DOWN:
        toggleBit(7);
        break;
    default:
        break;
    }
}

void setKeyMask() {
    uint8_t controlBits = BEXTR(js.P1, 4, 5);
    js.keyMask = 0xCF;
    uint8_t states = 0x00;

    switch (controlBits) {
    case 0b00: {
        states = 0xFF;
        break;
    }
    case 0b01: {
        states = BEXTR(js.keyState, 0, 3);
        break;
    }
    case 0b10: {
        states = BEXTR(js.keyState, 4, 7);
        break;
    }
    }
    js.keyMask &= ~states;
}

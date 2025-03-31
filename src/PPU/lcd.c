#include "lcd.h"

#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "joypad.h"
#include "logger.h"
#include "types.h"
#include <SDL3/SDL.h>
#include <stdint.h>
#include <stdlib.h>

const int WIDTH = 160;
const int HEIGHT = 144;

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;
SDL_Event userEvent;
const SDL_PixelFormatDetails *pixelFmt;
uint32_t *pixelBuffer;
int bufferIndex;

void destroyLCD() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_DestroyTexture(texture);
  if (pixelBuffer != NULL) {
    free(pixelBuffer);
    pixelBuffer = NULL;
  }

  SDL_Quit();
}

void handleInput() {
  while (SDL_PollEvent(&userEvent)) {
    if (userEvent.type == SDL_EVENT_KEY_DOWN) {
      setKeyBit(userEvent.key.key);
    }
    if (userEvent.type == SDL_EVENT_KEY_UP) {
      unsetKeyBit(userEvent.key.key);
    }
    if (userEvent.type == SDL_EVENT_QUIT) {
      RUNNING = false;
    }
  }
}

bool initLCD() {
  if (SDL_Init(SDL_INIT_VIDEO) == 0) {
    logState(FATAL, "Failed to initialize SDL.");
    return false;
  }

  pixelBuffer = (uint32_t *)malloc(WIDTH * HEIGHT * sizeof(uint32_t));

  SDL_CreateWindowAndRenderer("gb.c", WIDTH * 3, HEIGHT * 3, 0, &window,
                              &renderer);
  SDL_SetRenderVSync(renderer, 1);
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                              SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

  SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
  pixelFmt = SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_ARGB8888);
  SDL_SetRenderLogicalPresentation(renderer, WIDTH, HEIGHT,
                                   SDL_LOGICAL_PRESENTATION_DISABLED);
  return true;
}

void drawPixel(SDL_Color col) {
  pixelBuffer[bufferIndex] =
      SDL_MapRGBA(pixelFmt, NULL, col.r, col.g, col.b, col.a);
  bufferIndex += 1;
}

void renderFrame() {
  handleInput();
  bufferIndex = 0;
  SDL_UpdateTexture(texture, NULL, pixelBuffer, WIDTH * sizeof(uint32_t));
  SDL_RenderTexture(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);
}

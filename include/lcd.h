#pragma once
#include <SDL3/SDL.h>
#include <stdbool.h>

void destroyLCD();
bool initLCD();
void drawPixel(SDL_Color);
void renderFrame();

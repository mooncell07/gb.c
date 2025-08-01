#pragma once
#include <SDL3/SDL.h>
#include <stdbool.h>

extern bool running;

void destroyLCD();
int initLCD();
void drawPixel(SDL_Color);
void renderFrame();

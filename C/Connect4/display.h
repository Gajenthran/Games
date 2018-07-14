#ifndef DISPLAY_H___
#define DISPLAY_H___

#include "sdl_driver.h"
#include "game.h"

void displayGrid(Game *g, Driver *dr);
void clear(Driver *dr);
void update(Driver *dr);

#endif
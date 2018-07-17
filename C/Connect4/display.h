#ifndef DISPLAY_H___
#define DISPLAY_H___

#include "sdl_driver.h"
#include "game.h"

#define SZ 32
enum { DX = 6, DY = 4 };

void displayMenu(Game *g, Driver *dr);
void displayGrid(Game *g, Driver *dr);
void clear(Driver *dr);
void update(Driver *dr);

#endif
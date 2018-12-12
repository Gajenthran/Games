#ifndef DISPLAY_H___
#define DISPLAY_H___

#include "sdl_driver.h"
#include "game.h"
#include "grid.h"

void	displayMenu(Game *g, Driver *dr);
void	displayGrid(Game *g, Driver *dr);
void	displayPlayerRound(Game *g, Driver *dr);
void	displayGame(Game *g, Driver *dr);
void	displayEnd(Game *g, Driver *dr);
void	clear(Driver *dr);
void	update(Driver *dr);

#endif
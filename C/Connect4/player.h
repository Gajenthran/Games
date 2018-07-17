#ifndef PLAYER_H___
#define PLAYER_H___

#define NPLAYER 2
#include "display.h"
#include "grid.h"
#include "game.h"
#include "sdl_driver.h"

void	initPlayer(Game *g);
int 	chooseColumn(Game *g, Driver *dr);
int 	putDisc(Game *g, Driver *dr, int color);

#endif
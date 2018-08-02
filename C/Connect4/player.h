#ifndef PLAYER_H___
#define PLAYER_H___

#include "display.h"
#include "grid.h"
#include "game.h"
#include "sdl_driver.h"

void	initPlayer(Game *g);
int 	chooseColumn(Game *g, Driver *dr);
int 	putDisc(Game *g, Driver *dr, int pId);
void 	iaMove(Game *g, int depth);
int 	Min(Game *g, int depth);
int 	Max(Game *g, int depth);
int 	eval(Game *g, int coeff);

#endif
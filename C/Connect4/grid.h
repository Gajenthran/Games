#ifndef GRID_H___
#define GRID_H___

#define NCOUNT 4
#define NALIGN 4

#include "game.h"

void	initGrid(Game *g);
int 	fullGrid(Game *g);
int 	validPosition(Game *g, int x, int y);
int 	checkAlignment(Game *g, int color, int x, int y, int dx, int dy);
int 	checkWinner(Game *g);

enum verif { HORIZONTAL, VERTICAL, DIAG_GAUCHE, DIAG_DROITE, NVERIF };
enum dir { GAUCHE, HAUT, DROITE, BAS, NDIR };
enum discs { EMPTY, RED, YELLOW, NDISCS };


#endif

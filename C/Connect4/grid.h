#ifndef GRID_H___
#define GRID_H___

#include "game.h"

#define NCOUNT 4
#define NALIGN 4

enum verif { HORIZONTAL, VERTICAL, DIAG_GAUCHE, DIAG_DROITE, NVERIF };
enum dir { GAUCHE, HAUT, DROITE, BAS, NDIR };
enum discs { EMPTY, RED, YELLOW, NDISCS };

void	initGrid(Game *g);
int 	fullGrid(Game *g);
int 	validPosition(Game *g, int x, int y);
int 	checkAlignment(Game *g, int color, int x, int y, int dx, int dy);
int 	checkWinner(Game *g);



#endif

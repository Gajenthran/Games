#ifndef GRID_H___
#define GRID_H___

#include "game.h"

#define NCOUNT 4
#define NALIGN 4
#define DRAW 3

enum verif { HORIZONTAL, VERTICAL, DIAG_GAUCHE, DIAG_DROITE, NVERIF };
enum dir { GAUCHE, HAUT, DROITE, BAS, NDIR };
enum disc { GREEN, RED, EMPTY, NDISCS};

void	initGrid(Game *g);
int 	fullGrid(Game *g);
int 	validPosition(Game *g, int x, int y);
void 	alignment(Game *g, int color, int x, int y, int dx, int dy, int *p, int n);
int 	countDiscs(Game *g);
void 	countAlignments(Game *g, int *p, int n);
int 	checkWinner(Game *g);



#endif

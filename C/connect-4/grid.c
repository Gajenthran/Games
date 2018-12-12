#include <stdio.h>
#include <stdlib.h>
#include "grid.h"

/*! \brief initialize the grid, i.e. all the cells are EMPTY. */
void initGrid(Game *g) {
	int i;
	for(i = 0; i < g->rows * g->cols; i++)
		g->grid[i] = EMPTY;
}

/*! \brief check if all the cells are filled. */
int fullGrid(Game *g) {
	int i;
	for(i = 0; i < g->rows * g->cols; i++)
		if(g->grid[i] == EMPTY)
			return 0;
	return 1;
}

int countDiscs(Game *g) {
	int i, n = 0;
	for(i = 0; i < g->rows * g->cols; i++)
		if(g->grid[i] != EMPTY)
			n++;

	return n;
}

/*! \brief check if the coordonate in parameter are valid. */
int validPosition(Game *g, int x, int y) {
	return (y >= 0 && y < g->rows &&
			x >= 0 && x < g->cols);
}

void alignment(Game *g, int color, int x, int y, int dx, int dy, int *p, int n) {
	int i, count = 0, tmp;
	while(validPosition(g, x, y)) {
		if(g->grid[y * g->cols + x] == color)
			count++;
		else
			return;

		if(count == n) {
			p[color]++;
			return;
		}

		x += dx;
		y += dy;

	}
}

void countAlignments(Game *g, int *p, int n) {
	const int dirx[] = {-1,  0, 1, 0};
	const int diry[] = { 0, -1, 0, 1};
	const int verif[NVERIF][2] = {
		{dirx[HAUT], diry[HAUT]},
		{dirx[DROITE], diry[DROITE]},
		{dirx[GAUCHE], diry[HAUT]},
		{dirx[DROITE], diry[HAUT]}
	};

	int r, c, align;
	for(r = 0; r < g->rows; r++) {
		for(c = 0; c < g->cols; c++) {
			for(align = 0; align < NVERIF; align++) {
				if(g->grid[r * g->cols + c] != EMPTY) {
					alignment(
						g,
						g->grid[r * g->cols + c], 
						c, r, 
						verif[align][0], verif[align][1], p, n);
				}
			}
		}
	}
}

/*! \brief check if there is a winner using the function checkAlignment. */
int checkWinner(Game *g) {
	int p[NPLAYER] = {0}, i;
	countAlignments(g, p, NCOUNT);
	for(i = 0; i < NPLAYER; i++) {
		if(p[i]) {
			g->player[i].won = 1;
			return i;
		}
	}

	if(fullGrid(g)) return DRAW;

	return -1;
}


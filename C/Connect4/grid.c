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

/*! \brief check if the coordonate in parameter are valid. */
int validPosition(Game *g, int x, int y) {
	return (y >= 0 && y < g->rows &&
			x >= 0 && x < g->cols);
}

/*! \brief check if there is a correct alignment, i.e. there is a horizontal,
 * vertical or diagonal series (NCOUNT) of same color tokens. */
int checkAlignment(Game *g, int color, int x, int y, int dx, int dy) {
	int i, count = 0;
	while(validPosition(g, x, y)) {
		if(g->grid[y * g->cols + x] == color)
			count++;
		else
			return 0;

		if(count == NCOUNT)
			return 1;

		x += dx;
		y += dy;
	}
	return 0;
}

/*! \brief check if there is a winner using the function checkAlignment. */
int checkWinner(Game *g) {
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
			for(align = 0; align < NVERIF; align++) 
				if(	g->grid[r * g->cols + c] != EMPTY &&
						checkAlignment(
							g,
							g->grid[r * g->cols + c], 
							c, r, 
							verif[align][0], verif[align][1]))
					return 1;
		}
	}

	return 0;
}
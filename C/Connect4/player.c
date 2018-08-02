#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "player.h"

/*! \brief initialize player's data. */
void initPlayer(Game *g) {
	int pId;
	for(pId = 0; pId < NPLAYER; pId++) {
		g->player[pId].color = pId;
		g->player[pId].score = 0;
		g->player[pId].won = 0;
		g->player[pId].ia = (pId == RED ? 1 : 0);
	}
}

/*! \brief return the column clicked by the user. Otherwise return -1. */
int chooseColumn(Game *g, Driver *dr) {
	int r, c;
	for(r = DY; r < g->rows + DY; r++) {
		for(c = DX; c < g->cols + DX; c++) {
			SDL_Rect dst = { .x = c * SZ, .y = r * SZ, .w = SZ, .h = SZ };
			if(elementClicked(dr, dst.x, 0, dst.x + dst.w, dr->windowHeight)) {
				return c - DX;
			}
		}
	}

	return -1;
}

/*! \brief put the token of the current player by verifying if it's possible. */
int putDisc(Game *g, Driver *dr, int pId) {
	int r, c, col, row;
	unsigned int nbRows = dr->windowHeight/SZ, nbCols = dr->windowWidth/SZ;

	col = chooseColumn(g, dr);
	for(r = DY; r < g->rows + DY; r++) {
		for(c = DX; c < g->cols + DX; c++) {
			row = ((g->rows-1) - (r - DY));
			if(row < g->rows && g->grid[((g->rows-1) - (r - DY)) * g->cols + col] == EMPTY && col >= 0) {
				g->grid[((g->rows-1) - (r - DY)) * g->cols + col] = g->player[pId].color;
				return 1;
			}
		}
	}
	return 0;
}


void iaMove(Game *g, int depth) {
	int max = -1000;
	int tmp, col, row;
	int r, c;

	for(r = g->rows-1; r >= 0; r--) {
		for(c = 0; c < g->cols; c++) {
			if(g->grid[r * g->cols + c] == EMPTY) {
				// ((r+1) <= (g->rows-1) && g->grid[(r+1) * g->cols + c] != EMPTY))
			 	g->grid[r * g->cols + c] = RED;
			 	tmp = Min(g, depth-1);

			 	if(tmp > max) {
			 		max = tmp;
			 		col = c;
			 		row = r;
			 	}
				
				g->grid[r * g->cols + c] = EMPTY;
			}
		}
	}

	// printf("%d - %d\n", row, col);
	g->grid[row * g->cols + col] = RED;
}

int Max(Game *g, int depth) {
	if(depth == 0 || checkWinner(g) >= 0)
		return eval(g);

	int max = -1000;
	int r, c, tmp;

	for(r = g->rows-1; r >= 0; r--) {
		for(c = 0; c < g->cols; c++) {
			if(g->grid[r * g->cols + c] == EMPTY) {
				// (r == (g->rows-1) ||
				// ((r+1) <= (g->rows-1) && g->grid[(r+1) * g->cols + c] != EMPTY)) )
			 	g->grid[r * g->cols + c] = GREEN;
			 	tmp = Min(g, depth-1);

			 	if(tmp > max)
			 		max = tmp;
			 	g->grid[r * g->cols + c] = EMPTY;
			}
		}
	}
	return max;
}

int Min(Game *g, int depth) {
	if(depth == 0 || checkWinner(g) >= 0)
		return eval(g);

	int min = 1000;
	int r, c, tmp;

	for(r = g->rows-1; r >= 0; r--) {
		for(c = 0; c < g->cols; c++) {
			if( g->grid[r * g->cols + c] == EMPTY) {
				// (r == (g->rows-1) ||
				// ((r+1) <= (g->rows-1) && g->grid[(r+1) * g->cols + c] != EMPTY)) )
			 	g->grid[r * g->cols + c] = RED;
			 	tmp = Max(g, depth-1);

			 	if(tmp < min)
			 		min = tmp;
			 	g->grid[r * g->cols + c] = EMPTY;
			}
		}
	}
	return min;
}

int eval(Game *g) {
	int status, nbDiscs;
	nbDiscs = countDiscs(g);

	if((status = checkWinner(g)) >= 0) {
		switch(status) {
			case RED:
				return 1000 - nbDiscs;
			case GREEN:
				return -1000 + nbDiscs;
			default:
				return 0;
		}
	}

	int p[NPLAYER] = {0};
	countAlignments(g, p, NCOUNT-1);

	printf("%d - %d\n", p[RED], p[GREEN]);
	return p[RED] - p[GREEN];
}
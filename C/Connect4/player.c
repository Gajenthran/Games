#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "player.h"

/*! \brief initialize player's data. */
void initPlayer(Game *g) {
	int pId;
	for(pId = 0; pId < NPLAYER; pId++) {
		g->player[pId].color = pId+1;
		g->player[pId].score = 0;
		g->player[pId].won = 0;
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
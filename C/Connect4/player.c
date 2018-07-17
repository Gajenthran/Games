#include <stdio.h>
#include <stdlib.h>
#include "player.h"

void initJoueur(Game *g) {
	int pId;

	for(pId = 0; pId < NPLAYER; pId++) {
		g->player[pId].color = pId;
		g->player[pId].score = 0;	
	}
}

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
int putDisc(Game *g, Driver *dr, int color) {
	int r, c, col, row;
	unsigned int nbRows = dr->windowHeight/SZ, nbCols = dr->windowWidth/SZ;
	
	col = chooseColumn(g, dr);
	for(r = DY; r < g->rows + DY; r++) {
		for(c = DX; c < g->cols + DX; c++) {
			row = ((g->rows-1) - (r - DY));
			if(row < g->rows && g->grid[((g->rows-1) - (r - DY)) * g->cols + col] == EMPTY && col >= 0) {
				g->grid[((g->rows-1) - (r - DY)) * g->cols + col] = color;
				return 1;
			}
		}
	}

	return 0;
}
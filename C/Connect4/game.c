#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "grid.h"
#include "display.h"
#include "player.h"

static int _grid[ROWS * COLS];
static Player _player[2];
static int _gameState = MENU;

void initGame(Game *g, Driver *dr) {
	g->rows = ROWS;
	g->cols = COLS;
	g->grid = _grid;
	g->player = _player;
	g->state = _gameState;

	initSDL(dr, 640, 480);
	initPlayer(g);
	initGrid(g);
	callback(g, dr);
}


void menu(Game *g, Driver *dr) {
	int i;
	for(i = 2; i < NmTEX; i++) {
		if(elementClicked(dr, dr->mTexCoord[i].x, 
			dr->mTexCoord[i].y, 
			dr->mTexCoord[i].w + dr->mTexCoord[i].x, 
			dr->mTexCoord[i].h + dr->mTexCoord[i].y))
			g->state = i-1;
	}
}

void play(Game *g, Driver* dr) {
	static int pId = 0;
	if(putDisc(g, dr, g->player[pId].color))
		pId = (pId + 1) % NPLAYER;
	if(fullGrid(g)) g->state = END;
	if(checkWinner(g)) g->state = END;
}


void callback(Game* g, Driver* dr) {
	int i = 0;
	while(!dr->in.quit){
		clear(dr);
		switch(g->state) {
			case MENU:
			case H_SCORE:
				menu(g, dr);
				displayMenu(g, dr);
				break;
			case GAME:
				play(g, dr);
				displayGrid(g, dr);
				break;
			case END:
				exit(0);
		}
		updateEvents(dr);
		update(dr); 
	}
}
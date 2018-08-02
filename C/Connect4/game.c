#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "grid.h"
#include "display.h"
#include "player.h"

/*! \brief game grid (size : ROWS * COLS). */
static int _grid[ROWS * COLS];
/*! \brief all the player in the game (= 2). */
static Player _player[NPLAYER];
/*! \brief the actual state of the game : menu, playing or end of the game. */
static int _gameState = MENU;
static int _pId = 0;

/*! \brief initialize the game, ie. initialize the SDL's parameters 
 * the player's data and the grid. Morever, call the function callback. */
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

/*! \brief the interface of the menu  with 3 options : play, scoreboard and quit. */
void menu(Game *g, Driver *dr) {
	int i;
	for(i = TEX_PLAY; i < NmTEX; i++) {
		if(elementClicked(dr, dr->mTexCoord[i].x, 
			dr->mTexCoord[i].y, 
			dr->mTexCoord[i].w + dr->mTexCoord[i].x, 
			dr->mTexCoord[i].h + dr->mTexCoord[i].y))
			g->state = i-1;
	}

	if(g->state == END)
		exit(0);
}

/*! \brief the interface of the game with the display of the grid and the current
 * player to play. Call the function putDisc, fullGrid and checkWinner. */
void play(Game *g, Driver* dr) {
	int status;

	if(g->player[_pId].ia) {
		g->round++;
		iaMove(g, 2);
		_pId = (_pId + 1) % NPLAYER;
	}

	if(putDisc(g, dr, _pId)) {
		g->round++;
		status = checkWinner(g);
		if(status >= 0) {
			g->state = END;
			return;
		}
		_pId = (_pId + 1) % NPLAYER;
	}
}

/*! \brief the interface of the end of the game with 2 option. Restart the game 
 * or quit the game. */
void end(Game *g, Driver *dr) {
	int i;
	if(elementClicked(dr, dr->eTexCoord[TEX_RESTART].x, 
		dr->eTexCoord[TEX_RESTART].y, 
		dr->eTexCoord[TEX_RESTART].w + dr->eTexCoord[TEX_RESTART].x, 
		dr->eTexCoord[TEX_RESTART].h + dr->eTexCoord[TEX_RESTART].y)) {
		g->state = MENU;
		initPlayer(g);
		initGrid(g);
		_pId = 0;
		g->round = 0;
	}

	if(elementClicked(dr, dr->eTexCoord[TEX_END].x, 
		dr->eTexCoord[TEX_END].y, 
		dr->eTexCoord[TEX_END].w + dr->eTexCoord[TEX_END].x, 
		dr->eTexCoord[TEX_END].h + dr->eTexCoord[TEX_END].y))
		dr->in.quit = 1;
}

/*! \brief the principal function which updates events, the displays of the elements. */
void callback(Game* g, Driver* dr) {
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
				displayGame(g, dr);
				break;
			case END:
				end(g, dr);
				displayGame(g, dr);
				displayEnd(g, dr);
				break;
		}
		updateEvents(dr);
		update(dr); 
	}
}
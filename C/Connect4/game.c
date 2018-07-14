#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "grid.h"

static int _grid[ROWS * COLS];
static Player _player[2];
static int _gameState = MENU;

void initGame(Game *g, Driver *dr) {
	g->rows = ROWS;
	g->cols = COLS;
	g->grid = _grid;
	g->player = _player;

	initSDL(dr);
	callback(g);
}

void callback(Game *g) {

}
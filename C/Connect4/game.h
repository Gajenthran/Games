#ifndef GAME_H___
#define GAME_H___

#include "sdl_driver.h"

#define NPLAYER 2
#define ROWS 7
#define COLS 8

enum states { MENU, GAME, H_SCORE, END, NSTATES };

typedef struct Game Game;
typedef struct Player Player;

struct Player {
	int color;
	int score;
	int ia;
	char* name;
	int won;
};

struct Game {
	int rows;
	int cols;
	int round;
	int *grid;
	Player *player;
	int state;
};

void	initGame(Game *g, Driver *dr);
void	menu(Game *g, Driver *dr);
void	play(Game *g, Driver* dr);
void	end(Game *g, Driver *dr);
void	callback(Game *g, Driver *dr);

#endif	

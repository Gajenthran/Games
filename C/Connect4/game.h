#ifndef GAME_H___
#define GAME_H___

#include "sdl_driver.h"

#define ROWS 7
#define COLS 8

enum states { MENU, GAME, H_SCORE, END, NSTATES };

typedef struct Game Game;
typedef struct Player Player;

void initGame(Game *g, Driver *dr);
void callback(Game *g, Driver *dr);

struct Player {
	char* name;		
	int color;	
	int score;
};

struct Game {
	int rows;		
	int cols;		
	int *grid;	
	Player *player;
	int state;
};

#endif	

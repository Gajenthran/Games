#include "sdl_driver.h"
#include "game.h"

int main(void) {
	Game game;
	Driver driver;
	initGame(&game, &driver);
	return 0;
}
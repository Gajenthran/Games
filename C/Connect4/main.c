/**
 * \file main.c
 * \brief Connect 4
 * \author PANCHALINGAMOORTHY Gajenthran
 * \date 17 July 2018
 */
#include "sdl_driver.h"
#include "game.h"

int main(void) {
	Game game;
	Driver driver;
	initGame(&game, &driver);
	return 0;
}
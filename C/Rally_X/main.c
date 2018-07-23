/**
 * \file main.c
 * \brief Rally-X with SDL's driver
 * \author PANCHALINGAMOORTHY Gajenthran
 * \date 8 July 2018
 */
#include "game.h"

extern Driver sdl_driver;

int main() {
    init_game(&sdl_driver);
    return 0;
}



/**
 * \file main2.c
 * \brief Rally-X with Ncurses' driver
 * \author PANCHALINGAMOORTHY Gajenthran
 * \date 8 July 2018
 */
#include "game.h"

extern Driver ncurses_driver;

int main() {
    init_game(&ncurses_driver);
    return 0;
}



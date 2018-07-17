#include <stdio.h>
#include <stdlib.h>
#include "menu.h"

void menu(Game *g, Driver *dr) {
	int i;
	for(i = 2; i < NmTEX; i++) {
		if(elementClicked(dr, dr->mTexCoord[i].x, dr->mTexCoord[i].y, dr->mTexCoord[i].w + dr->mTexCoord[i].x, dr->mTexCoord[i].h + dr->mTexCoord[i].y))
			g->state = i-1;
	}
}
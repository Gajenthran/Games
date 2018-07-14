#include "display.h"

void displayGrid(Game *g, Driver *dr) {
	SDL_Rect dst = { .x = 0, .y = 0, .w = 300, .h = 300 };
	SDL_RenderCopy(dr->ren, dr->tex[0], NULL, &dst);
}

void clear(Driver *dr) {
	SDL_RenderClear(dr->ren);
}

void update(Driver *dr) {
	SDL_RenderPresent(dr->ren);
}

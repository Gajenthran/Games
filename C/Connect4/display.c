#include "display.h"

void displayBackground(Game* g, Driver* dr) {
	int r, c;
	unsigned int nbRows = dr->windowHeight/SZ, nbCols = dr->windowWidth/SZ;
	for(r = 0; r < nbRows; r++) {
		for(c = 0; c < nbCols; c++) {
			SDL_Rect dst = {.x = c * SZ, .y = r * SZ, .w = SZ, .h = SZ };
			SDL_RenderCopy(dr->ren, dr->mTex[TEX_MBG], NULL, &dst);
		}
	}
}

void displayMenu(Game *g, Driver *dr) {
	int i;
	displayBackground(g, dr);
	for(i = 1; i < NmTEX; i++) {
		SDL_SetRenderDrawColor(dr->ren, 0, 255, 255, SDL_ALPHA_OPAQUE);
		if(elementTargeted(dr, dr->mTexCoord[i].x, dr->mTexCoord[i].y, dr->mTexCoord[i].w + dr->mTexCoord[i].x, dr->mTexCoord[i].h + dr->mTexCoord[i].y))
				SDL_SetRenderDrawColor(dr->ren, 0, 250, 255, SDL_ALPHA_OPAQUE);
		SDL_RenderCopy(dr->ren, dr->mTex[i], NULL, &dr->mTexCoord[i]);
	}
}

int displayHighlightColumn(Game *g, Driver *dr, int col) {
	return (elementTargeted(dr, col, 0, col + SZ, dr->windowHeight));
}

void displayGrid(Game *g, Driver *dr) {
	int r, c, srcY;
	unsigned int nbRows = dr->windowHeight/SZ, nbCols = dr->windowWidth/SZ;
	displayBackground(g, dr);
	
	for(r = DY; r < g->rows + DY; r++) {
		for(c = DX; c < g->cols + DX; c++) {
			SDL_Rect dst = { .x = c * SZ, .y = r * SZ, .w = SZ, .h = SZ };
			if(displayHighlightColumn(g, dr, dst.x)) srcY = (SZ * 2); else srcY = 0;
			SDL_Rect src = { .x = g->grid[(r-DY) * g->cols + (c-DX)] * (SZ * 2) - 1, .y = srcY, .w = SZ, .h = SZ };
			SDL_RenderCopy(dr->ren, dr->gTex[TEX_DISCS], &src, &dst);
		}
	}
} 

void clear(Driver *dr) {
	SDL_SetRenderDrawColor(dr->ren, 255, 255, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(dr->ren);
}

void update(Driver *dr) {
	SDL_RenderPresent(dr->ren);
}

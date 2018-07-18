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

void displayPlayerName(Game *g, Driver *dr) {
	int round = g->round % NPLAYER, r, c;
	SDL_SetTextureAlphaMod(dr->gTex[TEX_DISCS], 255);
	int y = g->rows + DY - 3, x = g->cols + DX + 2;
	SDL_Rect dst = { .x = x * SZ, .y = y * SZ, .w = SZ * 3, .h = SZ};
	SDL_RenderCopy(dr->ren, dr->textTex[round], NULL, &dst);
}

void displayMenu(Game *g, Driver *dr) {
	int i;
	SDL_Rect src = { .x = 0, .y = 0, .w = 1059, .h = 344 };
	displayBackground(g, dr);
	for(i = 1; i < NmTEX; i++) {
		src.y = 0;
		if(elementTargeted(dr, dr->mTexCoord[i].x, dr->mTexCoord[i].y, dr->mTexCoord[i].w + dr->mTexCoord[i].x, dr->mTexCoord[i].h + dr->mTexCoord[i].y) && i != 1)
			src.y = src.h;
		SDL_RenderCopy(dr->ren, dr->mTex[i], &src, &dr->mTexCoord[i]);
	}
}

int displayHighlightColumn(Game *g, Driver *dr, int col) {
	return (elementTargeted(dr, col, 0, col + SZ, dr->windowHeight));
}

void displayPlayerRound(Game *g, Driver *dr) {
	int round = g->round % NPLAYER, r, c;
	SDL_SetTextureAlphaMod(dr->gTex[TEX_DISCS], 255);
	for(r = g->rows + DY - 2; r < g->rows + DY; r++) {
		for(c = g->cols + DX + 2; c <  g->cols + DX + 4; c++) {
			SDL_Rect dst = { .x = c * SZ, .y = r * SZ, .w = SZ, .h = SZ };
			SDL_Rect src = { .x = g->player[round].color * (SZ * 2), .y = 0, .w = SZ, .h = SZ };
			SDL_RenderCopy(dr->ren, dr->gTex[TEX_DISCS], &src, &dst);
		}
	}
}

void displayWinner(Game *g, Driver *dr) {
	SDL_Rect dst = { 
		.x = dr->eTexCoord[TEX_NAME].x + 5, 
		.y = dr->eTexCoord[TEX_NAME].y + 5,
		.w = SZ * 8,
		.h = SZ * 3
	};

	int player = (g->round + 1) % NPLAYER;
	SDL_RenderCopy(dr->ren, dr->textTex[player], NULL, &dst);
}

void displayEnd(Game *g, Driver *dr) {
	int i;
	SDL_Rect r = { 0, 0, dr->windowWidth, dr->windowHeight };
	SDL_SetRenderDrawBlendMode(dr->ren, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(dr->ren, 0, 0, 0, 100);
	SDL_RenderFillRect(dr->ren, &r);

	SDL_SetRenderDrawColor(dr->ren, 250, 250, 250, 150);

	SDL_RenderFillRect(dr->ren, &dr->eTexCoord[TEX_SQUARE]);

	SDL_SetRenderDrawColor(dr->ren, 0, 0, 0, 150);
	SDL_RenderDrawRect(dr->ren, &dr->eTexCoord[TEX_NAME]);

	for(i = TEX_RESTART; i < NeTEX; i++) {
		SDL_SetTextureAlphaMod(dr->eTex[i], 150);
		SDL_RenderCopy(dr->ren, dr->eTex[i], NULL, &dr->eTexCoord[i]);
	} 

	displayWinner(g, dr);
}

void displayGame(Game *g, Driver *dr) {
	displayBackground(g, dr);
	displayPlayerRound(g, dr);
	displayPlayerName(g, dr);
	displayGrid(g, dr);
}

void displayGrid(Game *g, Driver *dr) {
	int r, c;
	unsigned int nbRows = dr->windowHeight/SZ, nbCols = dr->windowWidth/SZ;
	for(r = DY; r < g->rows + DY; r++) {
		for(c = DX; c < g->cols + DX; c++) {
			SDL_SetTextureAlphaMod(dr->gTex[TEX_DISCS], 150);
			SDL_Rect dst = { .x = c * SZ, .y = r * SZ, .w = SZ, .h = SZ };
			if(displayHighlightColumn(g, dr, dst.x) && g->state == GAME)
				SDL_SetTextureAlphaMod(dr->gTex[TEX_DISCS], 255);
			SDL_Rect src = { .x = g->grid[(r-DY) * g->cols + (c-DX)] * (SZ * 2), .y = 0, .w = SZ, .h = SZ };
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

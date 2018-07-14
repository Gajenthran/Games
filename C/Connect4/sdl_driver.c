#include "sdl_driver.h"

static char *files[NTEX] = {
	"files/grid.png",
	"files/discs.png",
};

int initSDL(Driver* driver) {
	int i;
	if(SDL_Init(SDL_INIT_VIDEO) != 0){
		printf("SDL_Init Error: %s\n",  SDL_GetError());
		return 1;
	}

	if(TTF_Init() < 0) {
		printf("TTF_Init Error: %s\n", SDL_GetError());
		return 1;
	}

	driver->win = SDL_CreateWindow("Connect_4", 0, 0, 640, 480, SDL_WINDOW_SHOWN);
	if(driver->win == NULL) {
		printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	driver->ren = SDL_CreateRenderer(driver->win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(driver->ren == NULL){
 		SDL_DestroyWindow(driver->win);
		printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	if(loadFiles(driver))
		return 1;

	return 0;
}

static int loadFiles(Driver *driver) {
	int i;
	SDL_Surface *png;
	for(i = 0; i < NTEX; ++i) {
		png = IMG_Load(files[i]);
		if(png == NULL){
			SDL_DestroyRenderer(driver->ren);
			SDL_DestroyWindow(driver->win);
			printf("Error: %s\n", SDL_GetError());
			SDL_Quit();
			return 1;
		}

		driver->tex[i] = SDL_CreateTextureFromSurface(driver->ren, png);
		SDL_FreeSurface(png);
	}
	return 0;
}

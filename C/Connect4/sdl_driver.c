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

	memset(&driver->in, 0, sizeof(driver->in));
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

void updateEvents(Driver *dr) {
	unsigned int t0, t = SDL_GetTicks();
	SDL_Event ev;
	srand((unsigned int)time(NULL));
	while(SDL_PollEvent(&ev) != 0) {
		switch(ev.type) {
			case SDL_QUIT:
				dr->in.quit = 1;
				break;
			case SDL_KEYDOWN:
				if(ev.key.keysym.sym == SDLK_LEFT) 
					dr->in.key = -1;
				else if(ev.key.keysym.sym == SDLK_LEFT)
					dr->in.key = 1;
				break;
			case SDL_MOUSEBUTTONDOWN:
				if(ev.button.button == SDL_BUTTON_LEFT) {
					dr->in.mbx = ev.button.x;
					dr->in.mby = ev.button.y;
				}
			case SDL_MOUSEMOTION:
				dr->in.mmx = ev.button.x;
				dr->in.mmy = ev.button.y;
				break;
			default:
				break;
		}

		t0 = SDL_GetTicks() - t;

		if(t0 < 20)
			SDL_Delay(20 - t0);
	}
}
#include "sdl_driver.h"

/*! \brief all the files about the menu. */
static char* menuFiles[NmTEX] = {
	"files/background.png",
	"files/title.png",
	"files/play.png",
	"files/highscore.png",
	"files/quit.png"
};

/*! \brief all the files about the game. */
static char* gameFiles[NgTEX] = {
	"files/background.png",
	"files/grid.png",
	"files/discs.png"
};

/*! \brief all the files about the end of the game. */
static char* endFiles[NeTEX] = {
	"files/background.png",
	"files/grid.png",
	"files/restart.png",
	"files/end.png"
};

/*! \brief all the texts in the game. */
static const char* texts[NTEXT] = {
    "player 1 ",
    "player 2 ",
    "draw"
};

/*! \brief initialize all the SDL settings. */
int initSDL(Driver* driver, int windowWidth, int windowHeight) {
	int i;
	driver->windowWidth = windowWidth;
	driver->windowHeight = windowHeight;

	if(SDL_Init(SDL_INIT_VIDEO) != 0){
		printf("SDL_Init Error: %s\n",  SDL_GetError());
		return 1;
	}

	if(TTF_Init() < 0) {
		printf("TTF_Init Error: %s\n", SDL_GetError());
		return 1;
	}

	driver->win = SDL_CreateWindow("Connect_4", 0, 0, driver->windowWidth, driver->windowHeight, SDL_WINDOW_SHOWN);
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
	if(loadmFiles(driver) || loadgFiles(driver) || loadeFiles(driver) || loadTexts(driver))
		return 1;

	initTexCoord(driver);
	return 0;
}

/*! \brief load all the texts, i.e. transform all the texts into textures. */
static int loadTexts(Driver *dr) {
	int i;
	SDL_Color black = { 0, 0, 0, 255 };
	TTF_Font * font = TTF_OpenFont("files/LearningCurve.ttf", 32);
	SDL_Surface *t;
	for(i = 0; i < NTEXT; ++i) {
		t = TTF_RenderText_Solid(font, texts[i], black);
		if(t == NULL) {
			SDL_DestroyRenderer(dr->ren);
			SDL_DestroyWindow(dr->win);
			printf("Error: %s\n", SDL_GetError());
			SDL_Quit();
			return 1;
		}
		dr->textTex[i] = SDL_CreateTextureFromSurface(dr->ren, t);
		SDL_FreeSurface(t);
	}
	return 0;
}

/*! \brief load all the menu files, i.e. transform all the menu files into textures. */
static int loadmFiles(Driver *driver) {
	int i;
	SDL_Surface *png;
	for(i = 0; i < NmTEX; ++i) {
		png = IMG_Load(menuFiles[i]);
		if(png == NULL){
			SDL_DestroyRenderer(driver->ren);
			SDL_DestroyWindow(driver->win);
			printf("Error: %s\n", SDL_GetError());
			SDL_Quit();
			return 1;
		}

		driver->mTex[i] = SDL_CreateTextureFromSurface(driver->ren, png);
		SDL_FreeSurface(png);
	}
	return 0;
}

/*! \brief load all the game files, i.e. transform all the game files into textures. */
static int loadgFiles(Driver *driver) {
	int i;
	SDL_Surface *png;
	for(i = 0; i < NgTEX; ++i) {
		png = IMG_Load(gameFiles[i]);
		if(png == NULL){
			SDL_DestroyRenderer(driver->ren);
			SDL_DestroyWindow(driver->win);
			printf("Error: %s\n", SDL_GetError());
			SDL_Quit();
			return 1;
		}

		driver->gTex[i] = SDL_CreateTextureFromSurface(driver->ren, png);
		SDL_FreeSurface(png);
	}
	return 0;
}


/*! \brief load all the end of the game files, i.e. transform all the end of the game 
 * files into textures. */
static int loadeFiles(Driver *driver) {
	int i;
	SDL_Surface *png;
	for(i = 0; i < NeTEX; ++i) {
		png = IMG_Load(endFiles[i]);
		if(png == NULL){
			SDL_DestroyRenderer(driver->ren);
			SDL_DestroyWindow(driver->win);
			printf("Error: %s\n", SDL_GetError());
			SDL_Quit();
			return 1;
		}

		driver->eTex[i] = SDL_CreateTextureFromSurface(driver->ren, png);
		SDL_FreeSurface(png);
	}

	return 0;
}

/*! \brief initialize the coordinates of all textures. */
static void initTexCoord(Driver *dr) {
	int SZw = 270, SZh = 30, i;

	dr->mTexCoord[TEX_TITLE].x = 180; dr->mTexCoord[TEX_TITLE].y = 0; 
	dr->mTexCoord[TEX_TITLE].w = SZw; dr->mTexCoord[TEX_TITLE].h = SZh * 4; 
	for(i = 2; i < NmTEX; i++) {
		dr->mTexCoord[i].x = dr->mTexCoord[TEX_TITLE].x;
 		dr->mTexCoord[i].y = 126 * i - 137; 
		dr->mTexCoord[i].w = SZw; dr->mTexCoord[i].h = SZh * 4; 
	}

	dr->eTexCoord[TEX_SQUARE].x = dr->windowWidth/4; 
	dr->eTexCoord[TEX_SQUARE].y = dr->windowHeight/4;
	dr->eTexCoord[TEX_SQUARE].w = dr->windowWidth/2; 
	dr->eTexCoord[TEX_SQUARE].h = dr->windowHeight/2;

	dr->eTexCoord[TEX_NAME].x = dr->windowWidth/3 - 20; 
	dr->eTexCoord[TEX_NAME].y = dr->windowHeight/4 + 20;
	dr->eTexCoord[TEX_NAME].w = 250; 
	dr->eTexCoord[TEX_NAME].h = 100;

	dr->eTexCoord[TEX_RESTART].x = dr->windowWidth/3 - 20; 
	dr->eTexCoord[TEX_RESTART].y = dr->windowHeight/4 + dr->eTexCoord[TEX_NAME].h + 30;
	dr->eTexCoord[TEX_RESTART].w = dr->eTexCoord[TEX_NAME].w / 2.5; 
	dr->eTexCoord[TEX_RESTART].h = dr->eTexCoord[TEX_NAME].h / 1.5;

	dr->eTexCoord[TEX_END].x = dr->eTexCoord[TEX_NAME].w + dr->eTexCoord[TEX_NAME].x - dr->eTexCoord[TEX_RESTART].w; 
	dr->eTexCoord[TEX_END].y = dr->eTexCoord[TEX_RESTART].y;
	dr->eTexCoord[TEX_END].w = dr->eTexCoord[TEX_RESTART].w; 
	dr->eTexCoord[TEX_END].h = dr->eTexCoord[TEX_RESTART].h; 
}

/*! \brief update mouse events and keyboard events */
void updateEvents(Driver *dr) {
	SDL_Event ev;
	srand((unsigned int)time(NULL));
	int t0, t = SDL_GetTicks();
	while(SDL_PollEvent(&ev) != 0) {
		switch(ev.type) {
			case SDL_QUIT:
				dr->in.quit = 1;
				break;
			case SDL_KEYDOWN:
				if(ev.key.keysym.sym == SDLK_LEFT)
					dr->in.key = -1;
				else if(ev.key.keysym.sym == SDLK_RIGHT)
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

/*! \brief check if the mouse cursor is in the rectangle (coordinates are passed in 
 * parameter) */
int elementTargeted(Driver *dr, int x0, int y0, int x1, int y1) {
	return (dr->in.mmx > x0 && dr->in.mmy > y0 && dr->in.mmx < x1 && dr->in.mmy < y1);
}

/*! \brief check if the user clicks in the rectangle (coordinates are passed in 
 * parameter) */
int elementClicked(Driver *dr, int x0, int y0, int x1, int y1) {
	if(dr->in.mbx > x0 && dr->in.mby > y0 && dr->in.mbx < x1 && dr->in.mby < y1) {
		dr->in.mbx = dr->in.mby = 0;
		return 1;
	}
	return 0;
}
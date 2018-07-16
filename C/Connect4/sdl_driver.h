#ifndef SDL_DRIVER_H___
#define SDL_DRIVER_H___

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

enum { TEX_GRID, TEX_DISCS, NTEX };
typedef struct Driver Driver;
typedef struct Input Input;

struct Input {
	int key;
	int mbx, mby;
	int mmx, mmy;
	char quit;
};

struct Driver {
	SDL_Window* win;
	SDL_Renderer* ren;
	SDL_Texture* tex[NTEX];
	Input in;
};

int initSDL(Driver* driver);
static int loadFiles(Driver *driver);
void updateEvents(Driver *dr);

#endif
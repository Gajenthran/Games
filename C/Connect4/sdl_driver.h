#ifndef SDL_DRIVER_H___
#define SDL_DRIVER_H___

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

enum { TEX_GRID, TEX_DISCS, NTEX };
typedef struct driver Driver;

struct driver {
	SDL_Window* win;
	SDL_Renderer* ren;
	SDL_Texture* tex[NTEX];
};

int initSDL(Driver* driver);
static int loadFiles(Driver *driver);

#endif
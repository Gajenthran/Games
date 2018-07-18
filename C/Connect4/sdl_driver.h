#ifndef SDL_DRIVER_H___
#define SDL_DRIVER_H___

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

enum options { TITLE, PLAY, HIGH_SCORE, QUIT, NOPTION };
enum gameTextures { TEX_GBG, TEX_GRID, TEX_DISCS, NgTEX };
enum menuTextures { TEX_MBG, TEX_TITLE, TEX_PLAY, TEX_HIGHSCORE, TEX_QUIT, NmTEX };

typedef struct Driver Driver;
typedef struct Input Input;
typedef struct Rect Rect;

struct Input {
	int key;
	int mbx, mby;
	int mmx, mmy;
	char quit;
};

struct Driver {
	int windowWidth, windowHeight;
	SDL_Window* win;
	SDL_Renderer* ren;
	SDL_Texture* gTex[NgTEX];
	SDL_Rect gTexCoord[NgTEX];
	SDL_Texture* mTex[NmTEX];
	SDL_Rect mTexCoord[NmTEX];
	Input in;
};


int initSDL(Driver* driver, int windowWidth, int windowHeight);
void updateEvents(Driver *dr);
static int loadmFiles(Driver *driver);
static int loadgFiles(Driver *driver);
static void initTexCoord(Driver *dr);
extern int elementTargeted(Driver *dr, int x0, int y0, int x1, int y1);
extern int elementClicked(Driver *dr, int x0, int y0, int x1, int y1);

#endif
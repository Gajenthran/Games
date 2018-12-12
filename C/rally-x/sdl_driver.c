#include "game.h"
#include "driver.h"
#include <stdio.h>
#include <unistd.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

static int      init(const Game* game);
static void     start(void(*callback)(void*));
static int      get_move(void);
static void     draw_bg(void);
static void     draw_entity(int ent_id);
static void     update(void);
static void     lose_game(void);
static void     quit_game(void);

/*! \brief elements of the SDL driver. */
Driver sdl_driver = {
	.game = NULL,
	.init = init,
	.start = start,
	.get_move = get_move,
	.draw_bg = draw_bg,
	.draw_entity = draw_entity,
	.update = update,
	.lose_game = lose_game,
	.quit_game = quit_game
};

#define GAME (sdl_driver.game)

/*! \brief all the files of the tiles. */
static char* tiles_files[] = { 
	"files/map.png", 
	"files/title.png"
};

/*! \brief all the files of the sprites. */
static const char* sprites_files[] = { 
	"files/player.png", 
	"files/enemy.png", 
	"files/enemy.png",
	"files/enemy.png" 
};

/*! \brief all the texts in the game. */
static const char* texts[] = {
	"Fuel : ",
	"Score : ",
	"Highscore : ",
	"Life : "
};

/*! \brief number of tiles. */
#define ntiles_files (sizeof(tiles_files)/ sizeof(*tiles_files))
/*! \brief number of sprites. */
#define nsprites_files (sizeof(sprites_files)/ sizeof(*sprites_files))
/*! \brief number of texts. */
#define ntexts (sizeof(texts)/ sizeof(*texts))

/*! \brief window of the game. */
static SDL_Window *win;
/*! \brief renderer of the game. */
static SDL_Renderer *ren;
/*! \brief all the textures of the tiles. */
static SDL_Texture* tiles[ntiles_files];
/*! \brief all the textures of the sprites. */
static SDL_Texture* sprites[nsprites_files];
/*! \brief all the textures of texts. */
static SDL_Texture* textTex[ntexts];
/*! \brief white color. */
static SDL_Color white = { 255, 255, 255, 200 };
/*! \brief font used for the texts. */
static TTF_Font * font = NULL;
/*! \brief informations about the player (score, fuel...). */
static int dataOption = 0;

enum { 
	SZ = 38,
	FPS = 15,
	TILES_W = 50,
	TILES_H = 50,
	ENT_W = 74,
	ENT_H = 70
};

/*! \brief load all the texts, i.e. transform all the texts into textures. */
static int load_texts(void) {
	int i;
	font = TTF_OpenFont("files/Arial.ttf", 30);
	SDL_Surface *t;
	for(i = 0; i < ntexts; ++i) {
		t = TTF_RenderText_Solid(font, texts[i], white);
		if(t == NULL) {
			SDL_DestroyRenderer(ren);
			SDL_DestroyWindow(win);
			printf("Error: %s\n", SDL_GetError());
			SDL_Quit();
			return 1;
		}

		textTex[i] = SDL_CreateTextureFromSurface(ren, t);
		SDL_FreeSurface(t);
	}
	return 0;
}

/*! \brief load all the sprites, i.e. transform all the sprites into textures. */
static int load_sprites(void) {
	int i;
	SDL_Surface *png;
	for(i = 0; i < NEntity; ++i) {
		png = IMG_Load(sprites_files[i]);
		if(png == NULL) {
			SDL_DestroyRenderer(ren);
			SDL_DestroyWindow(win);
			printf("Error: %s\n", SDL_GetError());
			SDL_Quit();
			return 1;
		}

		sprites[i] = SDL_CreateTextureFromSurface(ren, png);
		SDL_FreeSurface(png);
	}
	return 0;
}

/*! \brief load all the tiles, i.e. transform all the tiles into textures. */
static int load_tiles(void) {
	int i;
	SDL_Surface *png;
	for(i = 0; i < ntiles_files; ++i) {
		png = IMG_Load(tiles_files[i]);
		if(png == NULL){
			SDL_DestroyRenderer(ren);
			SDL_DestroyWindow(win);
			printf("Error: %s\n", SDL_GetError());
			SDL_Quit();
			return 1;
		}
		tiles[i] = SDL_CreateTextureFromSurface(ren, png);
		SDL_FreeSurface(png);
	}
	return 0;
}

/*! \brief initialize all the SDL settings. */
static int init(const Game* game) {
	int i;
	GAME = game;
	if(SDL_Init(SDL_INIT_VIDEO) != 0){
		printf("SDL_Init Error: %s\n",  SDL_GetError());
		return 1;
	}

	if(TTF_Init() < 0) {
		printf("TTF_Init Error: %s\n", SDL_GetError());
		return 1;
	}

	win = SDL_CreateWindow("Rally_X", 0, 0, GAME->screen_w * SZ, GAME->screen_h * SZ, SDL_WINDOW_SHOWN);
	if(win == NULL) {
		printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(ren == NULL){
		SDL_DestroyWindow(win);
		printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	if(load_tiles())
		return 1;
	if(load_sprites())
		return 1;
	if(load_texts())
		return 1;

	return 0;
}

/*! \brief Call the function callback. */
static void start(void(*callback)(void*)) {
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg(callback, &sdl_driver, FPS, 0);
#else
	for(;;) {
		callback(&sdl_driver);
		usleep(135000);
	}
#endif
}

/*! \brief update keyboard events and return the player's next move. */
static int get_move(void) {
	static int last_move = Nothing;

	last_move = GAME->entity[Player].dir;
	SDL_PumpEvents();
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	if(state[SDL_SCANCODE_ESCAPE])
		quit_game();
	if(state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP])
		last_move = Up;
	if(state[SDL_SCANCODE_K]) {
		if(GAME->entity[Player].fuel > SmFuel * -1) {
			GAME->entity[Player].smoke = On;
			GAME->entity[Player].fuel += SmFuel;
		}
	}
	if(state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT])
		last_move = Left;
	if(state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT])
		last_move = Right;
	if(state[SDL_SCANCODE_Z] || state[SDL_SCANCODE_DOWN])
		last_move = Down;

	if(state[SDL_SCANCODE_P])
		dataOption = !dataOption;

	return last_move;
}

/*! \brief return the highest score since the creation of the game. */
static int high_score(void) {
	static int score = -1;

	if(score < 0) {
		FILE * file = fopen("files/highscore_sdl.txt", "r");
		fscanf(file, "%d", &score);
		fclose(file);
	}
	if(score < GAME->entity[Player].score)
		score = GAME->entity[Player].score;
	if((GAME->entity[Player].life <= 0 || GAME->entity[Player].level > NLevel) && score <= GAME->entity[Player].score) {
		FILE *file = fopen("files/highscore_sdl.txt", "w");
		fprintf(file, "%d", score);
		fclose(file);
	}

	return score;
}

/*! \brief display the player's informations (score, fuel, life). */
static void draw_data(void) {
	int i;
	SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
	const int ty = 10;
	SDL_Surface *t;
	SDL_Texture *texScore;

	char wr_score[256], wr_high_score[256];
	sprintf(wr_score, "%d", GAME->entity[Player].score);
	sprintf(wr_high_score, "%d", high_score());

	SDL_Rect rect = { .x = 0, .y = 0, .w = GAME->screen_w * SZ, .h = 50};
	SDL_SetRenderDrawColor(ren, 0, 0, 0, 200);
	SDL_RenderFillRect(ren, &rect);

	SDL_Rect fuel = { .x = GAME->screen_w * SZ / 10, .y = ty + 5, .w = GAME->screen_w *((GAME->entity[Player].fuel*200)/Fuel)/24, .h = 20};
	SDL_SetRenderDrawColor(ren, 255, GAME->entity[Player].fuel*(255)/Fuel, 0, 200);
	SDL_RenderFillRect(ren, &fuel);
	SDL_Rect tfuel = { .x = GAME->screen_w * SZ / 10 - SZ, .y = ty, .w = 40, .h = 30 };
	SDL_RenderCopy(ren, textTex[TFuel], NULL, &tfuel);

	SDL_Rect tscore = { .x = GAME->screen_w * SZ / 2.5 - SZ, .y = ty, .w = 40, .h = 30};
	SDL_RenderCopy(ren, textTex[TScore], NULL, &tscore);
	SDL_Rect twr_score = { .x = GAME->screen_w * SZ / 2.5, .y = ty - 5, .w = 40, .h = 40};
	t = TTF_RenderText_Solid(font, wr_score, white);
	texScore = SDL_CreateTextureFromSurface(ren, t);
	SDL_RenderCopy(ren, texScore, NULL, &twr_score);
	SDL_FreeSurface(t);

	SDL_Rect thigh_score = { .x = GAME->screen_w * SZ / 1.5 - SZ * 1.5, .y = ty, .w = 60, .h = 30};
	SDL_RenderCopy(ren, textTex[THighScore], NULL, &thigh_score);
	SDL_Rect twr_high_score = { .x = GAME->screen_w * SZ / 1.5, .y = ty - 5, .w = 40, .h = 40};
	t = TTF_RenderText_Solid(font, wr_high_score, white);
	texScore = SDL_CreateTextureFromSurface(ren, t);
	SDL_RenderCopy(ren, texScore, NULL, &twr_high_score);
	SDL_FreeSurface(t);

	for(i = 0; i < GAME->entity[Player].life; i++) {
		SDL_Rect life = { .x = GAME->screen_w * SZ / 1 - SZ * (i+1), .y = ty, .w = 30, .h = 30};
		SDL_Rect src = { .x = TILES_W * Life, .y = 0, .w = 10, .h = 10};
		SDL_RenderCopy(ren, tiles[Map], &src, &life);
	}
}

/*! \brief display the background of the game. */
static void draw_bg(void) {
	SDL_RenderClear(ren);
	int y, x, typecell, l, c, is_flag = Off;
	SDL_Rect dst = {.x = 0, .y = 0, .w = SZ, .h = SZ };
	SDL_Rect dstSc = {.x = 0, .y = 0, .w = SZ, .h = SZ };
	SDL_Rect src = {.x = 0, .y =(GAME->entity[Player].level - 1) * TILES_H, .w = TILES_W, .h = TILES_H};
	SDL_Rect srcSc = {.x = Score * src.w, .y =(GAME->entity[Player].coeff - 2) * 8, .w = 20, .h = 9};
	SDL_Rect srcBo = {.x = Score * src.w + 18, .y =(GAME->entity[Player].bonus -1) * 8, .w = 20, .h = 8};

	int move_x =(GAME->entity[Player].x - GAME->screen_w / 2);
	int move_y =(GAME->entity[Player].y - GAME->screen_h / 2);

	if(move_y < 0)
		move_y = 0;

	if(move_x < 0)
		move_x = 0;

	int limit_y = GAME->screen_h + move_y;
	int limit_x = GAME->screen_w + move_x;
	int diff_y = 0;
	int diff_x = 0;

	if(limit_y >= GAME->h) {
		diff_y = limit_y - GAME->h;
		limit_y = GAME->h;
	}

	if(limit_x >= GAME->w) {
		diff_x = limit_x - GAME->w;
		limit_x = GAME->w;
	}
	
	for(y = move_y - diff_y, l = 0; y < limit_y; ++y, ++l) {
		for(x = move_x - diff_x, c = 0; x < limit_x; ++x, ++c) {
			dst.x = c * SZ;
			dst.y = l * SZ;
			typecell = GAME->background[y * GAME->w + x];

			if(typecell == Score) {
				src.x = 0 * src.w;
				SDL_RenderCopy(ren, tiles[Map], &src, &dst);
				dstSc.x = dst.x;
				dstSc.y = dst.y;
				is_flag = On;
			}

			else {
				src.x = typecell * src.w;
				SDL_RenderCopy(ren, tiles[Map], &src, &dst);
			}
		}
	}

	if(is_flag == On) {
		SDL_RenderCopy(ren, tiles[Map], &srcSc, &dstSc);
		dstSc.x += srcSc.w; 
		SDL_RenderCopy(ren, tiles[Map], &srcBo, &dstSc);
	}

	if(dataOption) draw_data();
}

/*! \brief display all the entities (player and enemies) of the game. */
static void draw_entity(int ent_id) {
	static int sp[NEntity];
	static int cur_dir[NEntity];

	int move_x =(GAME->entity[Player].x - GAME->screen_w/2);
	int move_y =(GAME->entity[Player].y - GAME->screen_h/2);
	SDL_Rect src = {.x = 0, .y = 0, .w = ENT_W, .h = ENT_H };
	SDL_Rect dst = {.x = SZ *(GAME->entity[ent_id].x - move_x), .y = SZ *(GAME->entity[ent_id].y - move_y), .w = SZ, .h = SZ };

	if(move_y < 0)
		dst.y = SZ * GAME->entity[ent_id].y;
	if(move_x < 0) 
		dst.x = SZ * GAME->entity[ent_id].x;

	int limit_y = GAME->screen_h + move_y;
	int limit_x = GAME->screen_w + move_x;

	if(limit_y >= GAME->h)
		dst.y = SZ *(GAME->entity[ent_id].y - move_y +(limit_y - GAME->h));
	if(limit_x >= GAME->w)
		dst.x = SZ *(GAME->entity[ent_id].x - move_x +(limit_x - GAME->w));
	if(GAME->entity[ent_id].dir == Right) {
		src.y = ENT_H;
		if(cur_dir[ent_id] != Right) {
			cur_dir[ent_id] = Right;
			sp[ent_id] = 0;
		}
	}
	else if(GAME->entity[ent_id].dir == Up) {
		src.y = 0;
		if(cur_dir[ent_id] != Up) {
			cur_dir[ent_id] = Up;
			sp[ent_id] = 0;
		}
	}
	else if(GAME->entity[ent_id].dir == Left) {
		src.y = ENT_H * 3;
		if(cur_dir[ent_id] != Left) {
			cur_dir[ent_id] = Left;
			sp[ent_id] = 0;
		}
	}
	else if(GAME->entity[ent_id].dir == Down) {
		src.y = ENT_H * 2;
		if(cur_dir[ent_id] != Down) {
			cur_dir[ent_id] = Down;
			sp[ent_id] = 0;
		}
	}

	src.x = sp[ent_id] * ENT_W;
	SDL_RenderCopy(ren, sprites[ent_id], &src, &dst);
	if(sp[ent_id] < 2)
		++sp[ent_id];
}

/*! \brief display a message that shows the end of the game. */
static void lose_game(void) {
	int move_x =(GAME->entity[Player].x - GAME->screen_w/2);
	int move_y =(GAME->entity[Player].y - GAME->screen_h/2);
	SDL_Rect src = {.x = Bang * TILES_W, .y = 0, .w = TILES_W, .h = TILES_H };
	SDL_Rect dst = {.x = SZ *(GAME->entity[Player].x - move_x), .y = SZ *(GAME->entity[Player].y - move_y), .w = SZ, .h = SZ};
	SDL_RenderCopy(ren, tiles[Map], &src, &dst);
	SDL_RenderPresent(ren);
	usleep(600000);
}

/*! \brief quit the game. */
static void quit_game(void) {
	SDL_Quit();
	exit(0);
}

/*! \brief update the screen. */
static void update(void) {
	SDL_RenderPresent(ren);
}


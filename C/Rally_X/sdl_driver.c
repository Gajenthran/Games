#include "game.h"
#include "driver.h"
#include <stdio.h>
#include <unistd.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <SDL.h>
#include <SDL_image.h>

static int init(const Game* game);
static void start(void(*callback)(void*));
static int get_move(void);
static void draw_bg(void);
static void draw_entity(int ent_id);
static void update(void);
static void lose_game(void);
static void quit_game(void);

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

static char* tiles_files[] = { 
    "files/map.png", 
    "files/title.png"
};

static const char* sprites_files[] = { 
    "files/player.png", 
    "files/enemy.png", 
    "files/enemy.png",
    "files/enemy.png" 
};

#define ntiles_files (sizeof(tiles_files)/ sizeof(*tiles_files))
#define nsprites_files (sizeof(sprites_files)/ sizeof(*sprites_files))

static SDL_Window *win;
static SDL_Renderer *ren;
static SDL_Texture* tiles[ntiles_files];
static SDL_Texture* sprites[nsprites_files];

enum { 
    SZ = 38,
    FPS = 15,
    TILES_W = 50,
    TILES_H = 50,
    ENT_W = 74,
    ENT_H = 70
};

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

static int init(const Game* game) {
    int i;
    GAME = game;
    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        printf("SDL_Init Error: %s\n",  SDL_GetError());
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
    
    return 0;
}

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

    return last_move;
}

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

/* static void show_data(void) {
    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
    int data_w = 30 * GAME->screen_w/GAME->w, y, x, l, c, ent_id, forestH = 10, forestW = 24;

    DataC data = {
        .title_x =(GAME->screen_w - data_w) * SZ + SZ,
        .title_y = 1 * SZ,
        .fuel_x =(GAME->screen_w - data_w) * SZ + SZ,
        .fuel_y = 2 * SZ + 50,
        .map_x = 108 *(GAME->screen_w *(24 *(((30 * 24/90)* SZ)/48)/24)) /(24 *(GAME->screen_w *((data_w * SZ)/48)/24)),
        .map_y = 35 *(GAME->screen_h *(18 *(((30 * 24/90)* SZ)/48)/18)) /(18 *(GAME->screen_h *((data_w * SZ)/48)/18)),
        .life_x =(GAME->screen_w - data_w) * SZ + SZ,
        .life_y = GAME->screen_h *(14* SZ)/18, 
    };

    SDL_SetRenderDrawColor(ren, 0, 0, 0, 200);
    SDL_Rect dataRect = { .x =(GAME->screen_w - data_w) * SZ, .y = 0, .w = data_w * SZ, .h = GAME->screen_h * SZ };
    SDL_RenderFillRect(ren, &dataRect);

    SDL_Rect titleRect = { .x = data.title_x, .y = data.title_y, .w =(GAME->screen_w *150)/18, .h = 50 };
    SDL_RenderCopy(ren, tiles[Title], NULL, &titleRect);

    SDL_SetRenderDrawColor(ren, 255, 255, 255, 200);
    SDL_Rect rulerRect = { .x = data.fuel_x, .y = data.fuel_y, .w = GAME->screen_w *((Fuel*200)/Fuel)/24, .h = 20 };
    SDL_RenderDrawRect(ren, &rulerRect);

    SDL_SetRenderDrawColor(ren, 255, 255, 0, 200);
    SDL_Rect fuelRect = { .x = data.fuel_x, .y = data.fuel_y, .w = GAME->screen_w *((GAME->entity[Player].fuel*200)/Fuel)/24, .h = 20 };
    SDL_SetRenderDrawColor(ren, 255, GAME->entity[Player].fuel*(255)/Fuel, 0, 200);
    SDL_RenderFillRect(ren, &fuelRect);

    SDL_SetRenderDrawColor(ren, 5, 0, 255, 200);
    SDL_Rect entRect = {.x = 0, .y = 0, .w = GAME->screen_w *(dataRect.w/48)/24, .h = GAME->screen_w *(dataRect.w/48)/24};
    SDL_RenderFillRect(ren, &entRect);

    for(y = forestH, l = data.map_y; y < GAME->h - forestH; y++, l++) {
        for(x = forestW, c = data.map_x; x < GAME->w -forestW; x++, c++) {
            entRect.x = c* entRect.w;
            entRect.y = l* entRect.h;

            int typecell = GAME->background[y * GAME->w + x];
            if(typecell == Checkpoint || typecell == SCheckpoint || typecell == LCheckpoint) {
                SDL_SetRenderDrawColor(ren, 255, 255, 0, 200);
                SDL_RenderFillRect(ren, &entRect);
            }

            else if(typecell != Forest) {
                SDL_SetRenderDrawColor(ren, 0, 0, 255, 200);
                SDL_RenderFillRect(ren, &entRect);
            }

            for(ent_id = 0; ent_id < NEntity; ent_id++) {
                if(GAME->entity[ent_id].x == x && GAME->entity[ent_id].y == y) {
                    if(ent_id >= FEnemy) {
                        SDL_SetRenderDrawColor(ren, 255, 0, 0, 200);
                        SDL_RenderFillRect(ren, &entRect);
                    }

                    else if(ent_id == Player) {
                        SDL_SetRenderDrawColor(ren, 255, 255, 255, 200);
                        SDL_RenderDrawRect(ren, &entRect);
                    }
                }
            }
        }
    }

    SDL_SetRenderDrawColor(ren, 255, 255, 0, 200); 
    SDL_Rect lifeRect = {.x = data.life_x +(c * 20), .y = data.life_y, .w = 20, .h = 20}; 
    for(c = 0; c < GAME->entity[Player].life; c++) {
        lifeRect.x = data.life_x +(c * lifeRect.w) + 10;
        SDL_RenderCopy(ren, tiles[Life], NULL, &lifeRect);
    }

    high_score();
} */

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

    // show_data();
}

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

static void lose_game(void) {
    int move_x =(GAME->entity[Player].x - GAME->screen_w/2);
    int move_y =(GAME->entity[Player].y - GAME->screen_h/2);
    SDL_Rect src = {.x = Bang * TILES_W, .y = 0, .w = TILES_W, .h = TILES_H };
    SDL_Rect dst = {.x = SZ *(GAME->entity[Player].x - move_x), .y = SZ *(GAME->entity[Player].y - move_y), .w = SZ, .h = SZ};
    SDL_RenderCopy(ren, tiles[Map], &src, &dst);
    SDL_RenderPresent(ren);
    usleep(600000);
}

static void quit_game(void) {
    SDL_Quit();
    exit(0);
}
static void update(void) {
    SDL_RenderPresent(ren);
}

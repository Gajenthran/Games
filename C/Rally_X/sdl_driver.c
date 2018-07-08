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

static SDL_Window *win;
static SDL_Renderer *ren;
static SDL_Texture* tiles[NCell];
static char* tiles_files[] = { 
    "files/road.png", 
    "files/tree.png", 
    "files/forest.png", 
    "files/checkpoint.png",
    "files/s_checkpoint.png",
    "files/l_checkpoint.png",
    "files/rock.png",
    "files/smoke.png",
    "files/score.png",
    "files/life.png",
    "files/map.png",
    "files/title.png",
    "files/bang.png"
};
static SDL_Texture* sprites[NSprite];
static const char* sprites_files[] = { 
    "files/player.png", 
    "files/enemy.png", 
    "files/enemy.png",
    "files/enemy.png" 
};

enum { 
    SZ = 32,
    FPS = 15,
    IMG_W = 25,
    IMG_H = 23
};

static int load_sprites(void) {
    int i;
    SDL_Surface *png;
    for(i = 0; i < NSprite; ++i) {
        png = IMG_Load(sprites_files[i]);
        if (png == NULL) {
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
    for(i = 0; i < NCell; ++i) {
        png = IMG_Load(tiles_files[i]);
        if (png == NULL){
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
    if (SDL_Init(SDL_INIT_VIDEO) != 0){
        printf("SDL_Init Error: %s\n",  SDL_GetError());
        return 1;
    }
    
    win = SDL_CreateWindow("Rally_X", 0, 0, GAME->screen_w * SZ, GAME->screen_h * SZ, SDL_WINDOW_SHOWN);
    if (win == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    
    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == NULL){
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
        usleep(145000);
    }
#endif
}

static int get_move (void) {
    static int lastMove = Nothing;

    lastMove = GAME->entity[0].dir;
    SDL_PumpEvents();
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_ESCAPE])
        quit_game();

    if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP])
        lastMove = Up;
    
    if (state[SDL_SCANCODE_K]) {
        if (GAME->entity[0].fuel > SmFuel * -1) {
            GAME->entity[0].smoke = On;
            GAME->entity[0].fuel += SmFuel;
        }
    }

    if (state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT])
        lastMove = Left;
    
    if (state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT])
        lastMove = Right;

    if (state[SDL_SCANCODE_Z] || state[SDL_SCANCODE_DOWN])
        lastMove = Down;

    return lastMove;
}

static int high_score (void) {
    static int score = -1;

    if (score < 0) {
        FILE * file = fopen("files/highscore_sdl.txt", "r");
        fscanf(file, "%d", &score);
        fclose(file);
    }

    if (score < GAME->entity[0].score)
        score = GAME->entity[0].score;

    if ((GAME->entity[0].life <= 0 || GAME->entity[0].level > NLevel) && score <= GAME->entity[0].score) {
        FILE *file = fopen("files/highscore_sdl.txt", "w");
        fprintf(file, "%d", score);
        fclose(file);
    }

    return score;
}

/* static void show_data (void) {
    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
    int data_w = 30 * GAME->screen_w/GAME->w, y, x, l, c, ent_id, forestH = 10, forestW = 24;

    DataC data = {
        .title_x = (GAME->screen_w - data_w) * SZ + SZ,
        .title_y = 1 * SZ,
        .fuel_x = (GAME->screen_w - data_w) * SZ + SZ,
        .fuel_y = 2 * SZ + 50,
        .map_x = 108 * (GAME->screen_w * (24 * (((30 * 24/90)* SZ)/48)/24)) / (24 * (GAME->screen_w * ((data_w * SZ)/48)/24)),
        .map_y = 35 * (GAME->screen_h * (18 * (((30 * 24/90)* SZ)/48)/18)) / (18 * (GAME->screen_h * ((data_w * SZ)/48)/18)),
        .life_x = (GAME->screen_w - data_w) * SZ + SZ,
        .life_y = GAME->screen_h * (14* SZ)/18, 
    };

    SDL_SetRenderDrawColor(ren, 0, 0, 0, 200);
    SDL_Rect dataRect = { .x = (GAME->screen_w - data_w) * SZ, .y = 0, .w = data_w * SZ, .h = GAME->screen_h * SZ };
    SDL_RenderFillRect(ren, &dataRect);

    SDL_Rect titleRect = { .x = data.title_x, .y = data.title_y, .w = (GAME->screen_w *150)/18, .h = 50 };
    SDL_RenderCopy(ren, tiles[Title], NULL, &titleRect);

    SDL_SetRenderDrawColor(ren, 255, 255, 255, 200);
    SDL_Rect rulerRect = { .x = data.fuel_x, .y = data.fuel_y, .w = GAME->screen_w * ((Fuel*200)/Fuel)/24, .h = 20 };
    SDL_RenderDrawRect(ren, &rulerRect);

    SDL_SetRenderDrawColor(ren, 255, 255, 0, 200);
    SDL_Rect fuelRect = { .x = data.fuel_x, .y = data.fuel_y, .w = GAME->screen_w * ((GAME->entity[0].fuel*200)/Fuel)/24, .h = 20 };
    SDL_SetRenderDrawColor(ren, 255, GAME->entity[0].fuel*(255)/Fuel, 0, 200);
    SDL_RenderFillRect(ren, &fuelRect);

    SDL_SetRenderDrawColor(ren, 5, 0, 255, 200);
    SDL_Rect entRect = {.x = 0, .y = 0, .w = GAME->screen_w * (dataRect.w/48)/24, .h = GAME->screen_w * (dataRect.w/48)/24};
    SDL_RenderFillRect(ren, &entRect);

    for(y = forestH, l = data.map_y; y < GAME->h - forestH; y++, l++) {
        for(x = forestW, c = data.map_x; x < GAME->w -forestW; x++, c++) {
            entRect.x = c* entRect.w;
            entRect.y = l* entRect.h;

            int typecell = GAME->background[y * GAME->w + x];
            if (typecell == Checkpoint || typecell == SCheckpoint || typecell == LCheckpoint) {
                SDL_SetRenderDrawColor(ren, 255, 255, 0, 200);
                SDL_RenderFillRect(ren, &entRect);
            }

            else if (typecell != Forest) {
                SDL_SetRenderDrawColor(ren, 0, 0, 255, 200);
                SDL_RenderFillRect(ren, &entRect);
            }

            for (ent_id = 0; ent_id < NSprite; ent_id++) {
                if (GAME->entity[ent_id].x == x && GAME->entity[ent_id].y == y) {
                    if (ent_id >= FEnemy) {
                        SDL_SetRenderDrawColor(ren, 255, 0, 0, 200);
                        SDL_RenderFillRect(ren, &entRect);
                    }

                    else if (ent_id == Player) {
                        SDL_SetRenderDrawColor(ren, 255, 255, 255, 200);
                        SDL_RenderDrawRect(ren, &entRect);
                    }
                }
            }
        }
    }

    SDL_SetRenderDrawColor(ren, 255, 255, 0, 200); 
    SDL_Rect lifeRect = {.x = data.life_x + (c * 20), .y = data.life_y, .w = 20, .h = 20}; 
    for (c = 0; c < GAME->entity[0].life; c++) {
        lifeRect.x = data.life_x + (c * lifeRect.w) + 10;
        SDL_RenderCopy(ren, tiles[Life], NULL, &lifeRect);
    }

    high_score();
} */

static void draw_bg(void) {
    SDL_RenderClear(ren);
    int y, x, typecell, l, c, isFlag = Off;
    SDL_Rect dst = {.x = 0, .y = 0, .w = SZ, .h = SZ };
    SDL_Rect dstSc = {.x = 0, .y = 0, .w = SZ, .h = SZ };
    SDL_Rect src = {.x = 0, .y = (GAME->entity[0].level - 1)* 15, .w = 16, .h = 15};
    SDL_Rect srcSc = {.x = Score * src.w, .y = (GAME->entity[0].coeff - 2)* 8, .w = 20, .h = 9};
    SDL_Rect srcBo = {.x = Score * src.w + srcSc.w, .y = (GAME->entity[0].bonus -1) * 8, .w = 12, .h = 8};

    int moveX = (GAME->entity[0].x - GAME->screen_w / 2);
    int moveY = (GAME->entity[0].y - GAME->screen_h / 2);

    if (moveY < 0)
        moveY = 0;

    if (moveX < 0)
        moveX = 0;

    int limitY = GAME->screen_h + moveY;
    int limitX = GAME->screen_w + moveX;
    int diffY = 0;
    int diffX = 0;

    if (limitY >= GAME->h) {
        diffY = limitY - GAME->h;
        limitY = GAME->h;
    }

    if (limitX >= GAME->w) {
        diffX = limitX - GAME->w;
        limitX = GAME->w;
    }
    
    for(y = moveY - diffY, l = 0; y < limitY; ++y, ++l) {
        for(x = moveX - diffX, c = 0; x < limitX; ++x, ++c) {
            dst.x = c * SZ;
            dst.y = l * SZ;
            typecell = GAME->background[y * GAME->w + x];

            if (typecell == Score) {
                src.x = 0 * src.w;
                SDL_RenderCopy(ren, tiles[Map], &src, &dst);
                dstSc.x = dst.x;
                dstSc.y = dst.y;
                isFlag = On;
            }

            else {
                src.x = typecell * src.w;
                SDL_RenderCopy(ren, tiles[Map], &src, &dst);
            }
        }
    }

    if (isFlag == On) {
        SDL_RenderCopy(ren, tiles[Map], &srcSc, &dstSc);
        dstSc.x += srcSc.w; 
        SDL_RenderCopy(ren, tiles[Map], &srcBo, &dstSc);
    }

    // show_data();
}

static void draw_entity(int ent_id) {
    static int sp[NSprite];
    static int curDir[NSprite];

    int moveX = (GAME->entity[0].x - GAME->screen_w/2);
    int moveY = (GAME->entity[0].y - GAME->screen_h/2);
    SDL_Rect src = {.x = 0, .y = 0, .w = IMG_W, .h = IMG_H };
    SDL_Rect dst = {.x = SZ * (GAME->entity[ent_id].x - moveX), .y = SZ * (GAME->entity[ent_id].y - moveY), .w = SZ, .h = SZ };

    if (moveY < 0)
        dst.y = SZ * GAME->entity[ent_id].y;

    if (moveX < 0) 
        dst.x = SZ * GAME->entity[ent_id].x;

    int limitY = GAME->screen_h + moveY;
    int limitX = GAME->screen_w + moveX;

    if (limitY >= GAME->h)
        dst.y = SZ * (GAME->entity[ent_id].y - moveY + (limitY - GAME->h));

    if (limitX >= GAME->w)
        dst.x = SZ * (GAME->entity[ent_id].x - moveX + (limitX - GAME->w));

    if (GAME->entity[ent_id].dir == Right) {
        src.y = IMG_H;
        if (curDir[ent_id] != Right) {
            curDir[ent_id] = Right;
            sp[ent_id] = 0;
        }
    }

    else if (GAME->entity[ent_id].dir == Up) {
        src.y = 0;
        if (curDir[ent_id] != Up) {
            curDir[ent_id] = Up;
            sp[ent_id] = 0;
        }
    }

    else if (GAME->entity[ent_id].dir == Left) {
        src.y = IMG_H * 3;
        if (curDir[ent_id] != Left) {
            curDir[ent_id] = Left;
            sp[ent_id] = 0;
        }
    }

    else if (GAME->entity[ent_id].dir == Down) {
        src.y = IMG_H * 2;
        if (curDir[ent_id] != Down) {
            curDir[ent_id] = Down;
            sp[ent_id] = 0;
        }
    }

    src.x = sp[ent_id] * IMG_W;
    SDL_RenderCopy(ren, sprites[ent_id], &src, &dst);

    if (sp[ent_id] < 2)
        ++sp[ent_id];
}

static void lose_game(void) {
    int moveX = (GAME->entity[0].x - GAME->screen_w/2);
    int moveY = (GAME->entity[0].y - GAME->screen_h/2);
    SDL_Rect src = {.x = 0, .y = 0, .w = 24, .h = 24 };
    SDL_Rect dst = {.x = SZ * (GAME->entity[0].x - moveX), .y = SZ * (GAME->entity[0].y - moveY), .w = SZ, .h = SZ};
    SDL_RenderCopy(ren, tiles[Bang], &src, &dst);
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


#include "game.h"
#include "driver.h"
#include "unistd.h"
#include <ncurses.h>
#include <stdlib.h>

static int init(const Game* game);
static void start(void(*callback)(void*));
static int get_move(void);
static void draw_bg(void);
static void draw_entity(int ent_id);
static void update(void);
static void lose_game(void);
static void quit_game(void);

Driver ncurses_driver = {
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


#define GAME (ncurses_driver.game)

static char tiles[NCell] = {' ', '.', '#', '+', '!', '%','O', 'Y'};
static const char* sprites[NSprite] = {
    "ooo@<<<@>>>@^^^@vvv@", 
    "ooo@<<<@>>>@^^^@vvv@", 
    "ooo@<<<@>>>@^^^@vvv@", 
    "ooo@<<<@>>>@^^^@vvv@"
};

enum { FPS = 15 };

static int init(const Game* game) {
    GAME = game;
    initscr();
    curs_set(0);
    noecho();
    halfdelay(1);
    return 0;
}

static void start(void(*callback)(void*)) {
    for(;;) {
        callback(&ncurses_driver);
        usleep(20000);
    }
}

static int get_move(void) {
    int car = getch();
    static int lastMove = Nothing;
    lastMove = GAME->entity[0].dir;

    switch(car) {
        case 'z':
            lastMove = Up;
            break;

        case 'q':
            lastMove = Left;
            break;

        case 'w':
            lastMove = Down;
            break;

        case 'd':
            lastMove = Right;
            break;

        case 'k':
            if (GAME->entity[0].fuel > SmFuel * -1) {
                GAME->entity[0].smoke = On;
                GAME->entity[0].fuel += SmFuel;

            }
            break;

        case 'p':
            quit_game();

        default:
            break;
    }

    return lastMove;
}

static int high_score (void) {
    static int score = -1;

    if (score < 0) {
        FILE * file = fopen("files/highscore_nc.txt", "r");
        fscanf(file, "%d", &score);
        fclose(file);
    }

    if (score < GAME->entity[0].score)
        score = GAME->entity[0].score;

    if (GAME->entity[0].life <= 0 || GAME->entity[0].level > NLevel) {
        FILE *file = fopen("files/highscore_nc.txt", "w");
        fprintf(file, "%d", score);
        fclose(file);
    }

    return score;
}
static void show_data (void) {
    int checkpt = 0, xData = GAME->screen_w, yData = 3, wData = 10, hData = 5;
    DataC data = {
        .fuel_x = xData + 10,
        .fuel_y = yData * 2,
        .map_x = xData + 10,
        .map_y = yData * 3,
        .score_x = xData + 10, 
        .score_y = yData * 1, 
        .life_x = xData + 10, 
        .life_y = yData * 4, 
        .level_x = xData + 10,
        .level_y = yData * 5,
        .hs_x = xData + 10,
        .hs_y = yData * 0
    };

    for (int i = 0; i < GAME->w * GAME->h; i++)
        if (GAME->background[i] == Checkpoint || 
            GAME->background[i] == LCheckpoint ||
            GAME->background[i] == SCheckpoint)
            checkpt++;
        
    mvprintw(data.map_y, data.map_x, "CH: %d ", checkpt);
    mvprintw(data.hs_y, data.hs_x, "HS: %d ", high_score());
    mvprintw(data.level_y, data.level_x, "LVL: %d ", GAME->entity[0].level);
    mvprintw(data.fuel_y, data.fuel_x, "FU: %d ", GAME->entity[0].fuel);
    mvprintw(data.score_y, data.score_x, "SC: %d ", GAME->entity[0].score);
    mvprintw(data.life_y, data.life_x, "LI: %d ", GAME->entity[0].life);
    mvprintw(GAME->screen_h + 2, 0, "LEFT : q", NULL);
    mvprintw(GAME->screen_h + 3, 0, "UP : z", NULL);
    mvprintw(GAME->screen_h + 4, 0, "RIGHT : d", NULL);
    mvprintw(GAME->screen_h + 5, 0, "DOWN : w", NULL);
    mvprintw(GAME->screen_h + 6, 0, "SMOKE : k (azerty)", NULL);

}
static void draw_bg(void) {

    int y, x, typecell, l, c;
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
            typecell = GAME->background[y * GAME->w + x];
            mvprintw(l, c, "%c", tiles[typecell]);
        }
    }

    show_data();
}

static void draw_entity(int ent_id) {

    int x0,y0;
    int moveX = (GAME->entity[0].x - GAME->screen_w / 2);
    int moveY = (GAME->entity[0].y - GAME->screen_h / 2);
    x0 = GAME->entity[ent_id].x - moveX;
    y0 = GAME->entity[ent_id].y - moveY;

    if (moveY < 0)
        y0 =GAME->entity[ent_id].y;

    if (moveX < 0) 
        x0 =GAME->entity[ent_id].x;

    int limitY = GAME->screen_h + moveY;
    int limitX = GAME->screen_w + moveX;

    if (limitY >= GAME->h)
        y0 = GAME->entity[ent_id].y - moveY + (limitY - GAME->h);

    if (limitX >= GAME->w)
        x0 = GAME->entity[ent_id].x - moveX + (limitX - GAME->w);

    static int anim = 0;
    if (ent_id == 0)
        mvprintw(y0, x0, "%c", sprites[ent_id][4 * GAME->entity[ent_id].dir + anim]);

    if (ent_id != 0 && GAME->entity[ent_id].y - moveY > 0 && GAME->entity[ent_id].y - moveY < GAME->screen_h &&
        GAME->entity[ent_id].x - moveX > 0 && GAME->entity[ent_id].x - moveX < GAME->screen_w)
        mvprintw(y0, x0, "%c", sprites[ent_id][4 * GAME->entity[ent_id].dir + anim]);
    anim = (anim + 1) % 4;
}


static void lose_game(void) {
    int moveX = (GAME->entity[0].x - GAME->screen_w / 2);
    int moveY = (GAME->entity[0].y - GAME->screen_h / 2);
    int x0 = GAME->entity[0].x - moveX;
    int y0 = GAME->entity[0].y - moveY;
    mvprintw(y0, x0, "BANG", sprites[0][4 * GAME->entity[0].dir]);
    refresh();
    usleep(530000);

}
static void quit_game(void) {
    endwin();
    exit(0);
}
static void update(void) {
    refresh();
}


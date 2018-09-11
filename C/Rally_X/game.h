typedef struct Game Game;
typedef struct Driver Driver;
#include "player.h"

struct Game {
    int w, h;
    int screen_w, screen_h;
    unsigned char* background;
    Entity* entity;
};

enum { TFuel, TScore, THighScore, TLife };


void init_game(const Driver*);
void init_value(void);


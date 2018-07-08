typedef struct Game Game;
typedef struct Driver Driver;
typedef struct Entity Entity;
typedef struct DataC DataC;

struct Entity {
    int x, y;
    int id;
    int dir;
    int smoke, standstill;
    int fuel;
    int life;
    int score;
    int coeff;
    int bonus;
    int level;
    int lose;
};

struct Game {
    int w, h;
    int screen_w, screen_h;
    unsigned char* background;
    Entity* entity;
};


/* struct DataC {
    int title_x;
    int title_y;
    int fuel_x;
    int fuel_y;
    int map_x;
    int map_y;
    int life_x;
    int life_y;
    int score_x;
    int score_y;
    int level_x;
    int level_y;
    int hs_x;
    int hs_y;

}; */

enum {
    Road,
    Tree,
    Forest,
    Checkpoint,
    SCheckpoint,
    LCheckpoint,
    Rock,
    Smoke,
    Score,
    Life,
    Map,
    Title,
    Bang,
    NCell
};

enum {
    Player,
    Random,
    Miner,
    Tank,
    NSprite
};

enum {
    Nothing, Left, Right, Up, Down
};

enum {
    Off, On
};

enum {
    Fuel = 2000,
    MFuel = -3,
    FFuel = 100,
    StFuel = 2,
    SmFuel = -10,
    scoreFlag = 100,
    NLevel = 3,
    NLife = 3
};

void init_game(const Driver*);
void init_value(void);


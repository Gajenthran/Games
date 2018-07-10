typedef struct Game Game;
typedef struct Driver Driver;
typedef struct Entity Entity;

struct Entity {
    int x, y, dir;
    int id;
    int smoke, standstill;
    int fuel;
    int life;
    int score, coeff, bonus;
    int level;
    int lose;
};

struct Game {
    int w, h;
    int screen_w, screen_h;
    unsigned char* background;
    Entity* entity;
};

enum game_info {
    H = 50,
    W = 90,
    ScH = 18, 
    ScW = 24,
    Fuel = 2000,
    MFuel = -3,
    FFuel = 100,
    StFuel = 2,
    SmFuel = -10,
    scoreFlag = 100,
    NLevel = 3,
    NLife = 3,
};

enum tiles {
    Road,
    Tree,
    Forest,
    Checkpoint,
    SCheckpoint,
    LCheckpoint,
    Rock,
    Smoke,
    Score,
    Bang,
    Life,
    NCell,
    Map = 0, 
    Title = 1
};

enum entity {
    Player,
    Random,
    Miner,
    Tank,
    NEntity
};

enum dir {
    Nothing, Left, Right, Up, Down, NDir
};

enum { Off, On };
enum { TFuel, TScore, THighScore, TLife };


void init_game(const Driver*);
void init_value(void);


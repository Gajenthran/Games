#ifndef PLAYER_H_
#define PLAYER_H_

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

enum ent {
    Player,
    Random,
    Miner,
    Tank,
    NEntity
};

enum dir { Nothing, Left, Right, Up, Down, NDir };
enum { Off, On };

void 	init_player(Entity *entity);
void 	init_enemy(Entity *entity);
int 	count_checkpoint(unsigned char* bg);
void 	player_loses(Entity *entity, unsigned char* bg);
int 	distance(int x_player, int y_player, int x_enemy, int y_enemy);
int 	get_random_move(int n);
int 	count_object(unsigned char* bg, int typecell);
void 	eval_fuel(Entity *entity);
void 	is_object(Entity *entity, unsigned char* bg);
int 	follow_player(Entity *entity, int ent_id, unsigned char* bg);
void 	player_attack(Entity * entity, unsigned char* bg);
void 	miner_attack(Entity *entity, int ent_id, unsigned char *bg);
void 	enemy_attack(Entity *entity, int ent_id, unsigned char *bg);
void 	enemy_move(Entity *entity, int ent_id, unsigned char *bg);
void 	move_auto(Entity *entity, unsigned char *bg);
void 	player_move(Entity *entity, int move, unsigned char *bg);
// void 	is_win(void);

#endif
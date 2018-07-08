#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include "game.h"
#include "driver.h"

static void init_player(void);

#define H 50
#define W 90

Game g;

static unsigned char bg[H * W];
static Entity entity[5];
static int dirx[] = {0, -1, 1, 0, 0 };
static int diry[] = {0, 0, 0, -1, 1 };

static char * load_level(void) {
    FILE * file = NULL;
    char *lbg = malloc(H * W * sizeof *lbg);
    assert(lbg);
    switch(entity[0].level) {
        case 1:
            file = fopen("files/level/level1.txt", "r+");
            break;
        case 2:
            file = fopen("files/level/level2.txt", "r+");
            break;
        case 3:
            file = fopen("files/level/level3.txt", "r+");
            break;
        default:
            file = fopen("files/level/level1.txt", "r+");
            break;
    }
    fgets(lbg, H*W, file);
    fclose(file);
    return lbg;
}

static void next_level(void) {
	int i;
	init_player();
	g.entity[0].score += 1000;
	g.entity[0].level++;
	char * level = load_level();
	for(i = 0; i < H * W; ++i)
		g.background[i] = level[i] - 'A';
}

static void player_loses(void) {
	int i;
	init_player();
    g.entity[0].life--;
	for(i = 0; i < H * W; ++i)
		if(g.background[i] == Smoke)
			g.background[i] = Road;
}

static int count_checkpoint(void) {
    int count = 0;
    for(int i = 0; i < H * W; ++i)
        if(bg[i] == Checkpoint || bg[i] == SCheckpoint || bg[i] == LCheckpoint)
            count++;
    return count;
}

static void isWin(void) {
    if(count_checkpoint() == 0)
        next_level();
}

static int distance(int xPlayer, int yPlayer, int xEnemy, int yEnemy) {
    int x = xPlayer - xEnemy;
    int y = yPlayer - yEnemy;
    return(x * x) +(y * y);
}

static void isObject(void) {
    int distMax = 8 * 8;
    switch(bg[entity[0].y * W + entity[0].x]) {
        case Checkpoint:
           bg[entity[0].y * W + entity[0].x] = Score;
           entity[0].score += (scoreFlag * entity[0].bonus * entity[0].coeff);
           entity[0].coeff++; 
           break;
        case SCheckpoint:
           bg[entity[0].y * W + entity[0].x] = Score;
           entity[0].score += (scoreFlag * entity[0].bonus * entity[0].coeff);
           entity[0].bonus++;
           entity[0].coeff++;
           break;
         case LCheckpoint:
           bg[entity[0].y * W + entity[0].x] = Score;
           entity[0].score += entity[0].fuel +(scoreFlag * entity[0].bonus * entity[0].coeff);
           entity[0].coeff++;
           break;
        case Rock:
            entity[0].lose = On;
            break;
    }
    for(int i = 0; i < H * W; i++) {
        if(bg[i] == Score && distance(entity[0].x, entity[0].y, i%W, i/W) > distMax)
            bg[i] = Road;
    }
}

static int follow_player(int ent_id) {
    int best = Nothing, mDist = H*W, dist;
    for(int m = Left; m <= Down; ++m) {
        dist = distance(entity[0].x, entity[0].y, entity[ent_id].x + dirx[m], entity[ent_id].y + diry[m]);
        if(bg[(entity[ent_id].y + diry[m]) * W +(entity[ent_id].x + dirx[m])] == Tree ||
            bg[(entity[ent_id].y + diry[m]) * W +(entity[ent_id].x + dirx[m])] == Forest ||
            bg[(entity[ent_id].y + diry[m]) * W +(entity[ent_id].x + dirx[m])] == Rock ||
            entity[ent_id].x + dirx[m] < 0 ||
            entity[ent_id].x + dirx[m] >= W ||
            entity[ent_id].y + diry[m] < 0 ||
            entity[ent_id].y + diry[m] >= H)
            dist = H * W;
        for(int i = Random; i < NSprite; i++) {
            if(i == ent_id)
                continue;
            if(entity[ent_id].x + dirx[m] == entity[i].x && entity[ent_id].y + diry[m] == entity[i].y)
                dist = H * W;
        }
        if(dist < mDist) {
            mDist = dist;
            best = m;
        }
    }
    return best;
}

static void player_attack(void) {
    int distMax = 8 * 8;
    int move = entity[0].dir;
    int smoke_x = entity[0].x - dirx[move], smoke_y = entity[0].y - diry[move];
    if(entity[0].smoke == On) {
        if(bg[smoke_y * W + smoke_x] == Road)
            bg[smoke_y * W + smoke_x] = Smoke;
        
        entity[0].smoke = Off;

    }
    for(int i = 0; i < H * W; i++) {
        if(bg[i] == Smoke && distance(entity[0].x, entity[0].y, i%W, i/W) > distMax)
            bg[i] = Road;
    }
    for(int ent_id = Random; ent_id < NSprite; ent_id++) {
        if(bg[entity[ent_id].y * W + entity[ent_id].x] == Smoke)
            entity[ent_id].standstill = On;
        else 
            entity[ent_id].standstill = Off;
    }   
}

static int get_random_move(int n) {
    return(int)(n *(rand()/(RAND_MAX + 1.0)));
}

static int count_object(int typecell) {
    int count = 0, i;
    for(i = 0; i < H * W; i++) {
        if(bg[i] == typecell)
            count++;
    }
    return count;
}

static void miner_attack(int ent_id) {
    if(count_object(Rock) >= 10)
        return;
    int move = entity[ent_id].dir, chance = 10, rock_x = get_random_move(W), rock_y = get_random_move(H);
    if(chance == get_random_move(200)) {
        if(bg[rock_y * W + rock_x] == Road &&(rock_x != W/2 && rock_y != H/2))
            bg[rock_y * W + rock_x] = Rock;
    }    
}

static void enemy_attack(int ent_id) {
    switch(entity[ent_id].id) {
        case Miner:
            miner_attack(ent_id);
            break;
    }
}

static void enemy_move(int ent_id) {
    if(entity[ent_id].standstill == On)
        return;

    int move;
    if(entity[ent_id].id == Random)
        move = get_random_move(5);
    else
        move = follow_player(ent_id);

    entity[ent_id].dir = move;
    int nextx = entity[ent_id].x + dirx[move];
    int nexty = entity[ent_id].y + diry[move];

    if(nextx < 0 || nextx >= W || nexty < 0 || nexty >= H || 
        bg[nexty * W + nextx] == Tree || 
        bg[nexty * W + nextx] == Forest ||
        bg[nexty * W + nextx] == Rock)
        return;

    entity[ent_id].x = nextx;
    entity[ent_id].y = nexty;

    if((entity[0].x == entity[ent_id].x) &&(entity[0].y == entity[ent_id].y))
        entity[0].lose = On;

}

static void eval_fuel(void) {
    if(entity[0].fuel <= 0)
        entity[0].standstill = On;
    else
        entity[0].standstill = Off;
}

static void move_auto(void) {
    int m;
    for(m = Left; m <= Down; m++) {
        if(entity[0].dir == m)
            continue;
        if((entity[0].dir == Left && m == Right) ||
           (entity[0].dir == Right && m == Left) ||
           (entity[0].dir == Up && m == Down) ||
           (entity[0].dir == Down && m == Up))
            continue;
        if(bg[(entity[0].y + diry[m]) * W +(entity[0].x + dirx[m])] != Tree &&
            bg[(entity[0].y + diry[m]) * W +(entity[0].x + dirx[m])] != Forest &&
            bg[(entity[0].y + diry[m]) * W +(entity[0].x + dirx[m])] != Rock) {
            entity[0].y += diry[m];
            entity[0].x += dirx[m];
            entity[0].dir = m;
            break;
        }
    }
}
static void player_move(int move) {

    eval_fuel();
    if(entity[0].standstill == On) {
        entity[0].fuel += StFuel;
        return;
    }

    int nextx = entity[0].x + dirx[move];
    int nexty = entity[0].y + diry[move];

    if((nextx < 0 || nextx >= W || nexty < 0 || nexty >= H || bg[nexty * W + nextx] == Tree || bg[nexty * W + nextx] == Forest))
        move_auto();

    else {
        entity[0].x = nextx;
        entity[0].y = nexty;
        entity[0].dir = move;
    }

    entity[0].fuel += MFuel;
    isObject();
    for(int ent_id = Random; ent_id < NSprite; ent_id++)
        if((entity[0].x == entity[ent_id].x) &&(entity[0].y == entity[ent_id].y))
            entity[0].lose = On;
}

static void callback(void* d) {
    int ent_id;
    const Driver* dr = (Driver*)d;

    int move = dr->get_move();
    player_move(move);
    for(ent_id = Random; ent_id < NSprite; ent_id++) {
        enemy_move(ent_id);
        enemy_attack(ent_id);
    }
    player_attack();
    dr->draw_bg();
    for(ent_id = 0; ent_id < NSprite; ent_id++)
        dr->draw_entity(ent_id);

    if(entity[0].lose == On) {
        dr->lose_game();
        player_loses();
        entity[0].lose = Off;
    }
    dr->update();
    if(entity[0].life <= 0 || entity[0].level > NLevel || entity[0].level <= 0)
        dr->quit_game();
    if(move != Nothing)
        isWin();
}

static void init_player(void) {
    g.entity[0].x = W/2;
    g.entity[0].y = H/2;
    g.entity[0].id = 0;
    g.entity[0].dir = Right;
    g.entity[0].smoke = g.entity[0].standstill = Off;
    g.entity[0].fuel = Fuel;
    g.entity[0].coeff = g.entity[0].bonus = 1;
}

void init_value(void) {
    int i, ent_id;
    g.h = H;
    g.w = W;
    g.screen_h = 18;
    g.screen_w = 24;
    g.background = bg;
    g.entity = entity;
    
    init_player();
	g.entity[0].score = 0;
    g.entity[0].life = NLife;
	g.entity[0].level = 1;

	char * level = load_level();
	for(i = 0; i < H * W; ++i)
		g.background[i] = level[i] - 'A';

    for(ent_id = Random; ent_id < NSprite; ent_id++) {
        if(ent_id > g.entity[0].level)
            continue;
        g.entity[ent_id].x = (10 * ent_id) + 20;
        g.entity[ent_id].y = 15;
        g.entity[ent_id].id = ent_id;
        g.entity[ent_id].dir = Right;
    }
}

void init_game(const Driver* dr) {
    init_value();
    dr->init(&g);
    dr->start(callback);
}


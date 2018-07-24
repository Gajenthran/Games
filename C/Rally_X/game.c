#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include "game.h"
#include "driver.h"

/*! \brief game data */
Game g;
/*! \brief background of the game (dimension : H * W) */
static unsigned char bg[H * W];
/*! \brief entities (player and enemies) of the game */
static Entity entity[NEntity];
/*! \brief direction X */
static int dirx[] = {0, -1, 1, 0, 0 };
/*! \brief direction Y */
static int diry[] = {0, 0, 0, -1, 1 };

/*! \brief initialize the player's data. */
static void init_player(void) {
	g.entity[Player].x = W/2;
	g.entity[Player].y = H/2;
	g.entity[Player].id = 0;
	g.entity[Player].dir = Right;
	g.entity[Player].smoke = g.entity[Player].standstill = Off;
	g.entity[Player].fuel = Fuel;
	g.entity[Player].coeff = g.entity[Player].bonus = 1;
}

/*! \brief initialize the enemies' data. */
static void init_enemy(void) {
	int ent_id;
	for(ent_id = Random; ent_id < NEntity; ent_id++) {
		if(ent_id > g.entity[Player].level)
			continue;
		g.entity[ent_id].x = (10 * ent_id) + 20;
		g.entity[ent_id].y = 15;
		g.entity[ent_id].id = ent_id;
		g.entity[ent_id].dir = Right;
	}
}

/*! \brief load the current level */
static char * load_level(void) {
	FILE * file = NULL;
	char *lbg = malloc(H * W * sizeof *lbg);
	assert(lbg);

	char level_file[256];
	sprintf(level_file, "level/level%d.txt", entity[Player].level);
	file = fopen(level_file, "r+");
	fgets(lbg, H*W, file);
	fclose(file);

	return lbg;
}

/*! \brief move to the next level, by updating the player's data
 * and loading the new level. */
static void next_level(void) {
	int i;

	init_player();
	g.entity[Player].score += 1000;
	g.entity[Player].level++;
	char * level = load_level();
	for(i = 0; i < H * W; ++i)
		g.background[i] = level[i] - 'A';
	init_enemy();
}

/*! \brief count the number of checkpoints */
static int count_checkpoint(void) {
	int count = 0;
	for(int i = 0; i < H * W; ++i)
		if(bg[i] == Checkpoint || bg[i] == SCheckpoint || bg[i] == LCheckpoint)
			count++;
	return count;
}

/*! \brief update the player's data, the enemies' data and 
 * the background when the player loses. */
static void player_loses(void) {
	int i;
	init_player();
	init_enemy();
	g.entity[Player].life--;
	for(i = 0; i < H * W; ++i)
		if(g.background[i] == Smoke)
			g.background[i] = Road;
}

/*! \brief call the function next_level when there is no checkpoint. */  
static void is_win(void) {
	if(count_checkpoint() == 0)
		next_level();
}

/*! \brief calculate the distance between the player and an enemy, by
 * using Manhattan distance. */
static int distance(int x_player, int y_player, int x_enemy, int y_enemy) {
	int x = x_player - x_enemy;
	int y = y_player - y_enemy;
	return(x * x) +(y * y);
}

/*! \brief return a random number beetwen 0 and N. */
static int get_random_move(int n) {
	return(int)(n *(rand()/(RAND_MAX + 1.0)));
}

/*! \brief count the number of objects. */
static int count_object(int typecell) {
	int count = 0, i;
	for(i = 0; i < H * W; i++) {
		if(bg[i] == typecell)
			count++;
	}
	return count;
}

/*! \brief evaluate the quantity of fuel. If it's negative, the player will
 * move slowly. */
static void eval_fuel(void) {
	if(entity[Player].fuel <= 0)
		entity[Player].standstill = On;
	else
		entity[Player].standstill = Off;
}

/*! \brief update player's data (score, lose...) depending on what object
 * is in the player's coordinates. */
static void is_object(void) {
	int dist_max = 8 * 8;
	switch(bg[entity[Player].y * W + entity[Player].x]) {
		case Checkpoint:
		   bg[entity[Player].y * W + entity[Player].x] = Score;
		   entity[Player].score += (scoreFlag * entity[Player].bonus * entity[Player].coeff);
		   entity[Player].coeff++; 
		   break;
		case SCheckpoint:
		   bg[entity[Player].y * W + entity[Player].x] = Score;
		   entity[Player].score += (scoreFlag * entity[Player].bonus * entity[Player].coeff);
		   entity[Player].bonus++;
		   entity[Player].coeff++;
		   break;
		 case LCheckpoint:
		   bg[entity[Player].y * W + entity[Player].x] = Score;
		   entity[Player].score += entity[Player].fuel +(scoreFlag * entity[Player].bonus * entity[Player].coeff);
		   entity[Player].coeff++;
		   break;
		case Rock:
			entity[Player].lose = On;
			break;
	}
	for(int i = 0; i < H * W; i++) {
		if(bg[i] == Score && distance(entity[Player].x, entity[Player].y, i%W, i/W) > dist_max)
			bg[i] = Road;
	}
}

/*! \brief the enemies follow the player, by using the function "distance". 
 * Return the best direction/move for the enemy. */
static int follow_player(int ent_id) {
	int best = Nothing, dist_min = H*W, dist, i;
	for(int m = Left; m <= Down; ++m) {
		dist = distance(entity[Player].x, entity[Player].y, entity[ent_id].x + dirx[m], entity[ent_id].y + diry[m]);
		if(bg[(entity[ent_id].y + diry[m]) * W +(entity[ent_id].x + dirx[m])] == Tree ||
			bg[(entity[ent_id].y + diry[m]) * W +(entity[ent_id].x + dirx[m])] == Forest ||
			bg[(entity[ent_id].y + diry[m]) * W +(entity[ent_id].x + dirx[m])] == Rock ||
			entity[ent_id].x + dirx[m] < 0 ||
			entity[ent_id].x + dirx[m] >= W ||
			entity[ent_id].y + diry[m] < 0 ||
			entity[ent_id].y + diry[m] >= H)
			dist = H * W;
		for(i = Random; i < NEntity; i++) {
			if(i == ent_id)
				continue;
			if(entity[ent_id].x + dirx[m] == entity[i].x && entity[ent_id].y + diry[m] == entity[i].y)
				dist = H * W;
		}
		if(dist < dist_min) {
			dist_min = dist;
			best = m;
		}
	}
	return best;
}

/*! \brief player attack by putting smoke on the road to divert the enemies. */
static void player_attack(void) {
	int dist_max = 8 * 8;
	int move = entity[Player].dir;
	int smoke_x = entity[Player].x - dirx[move], smoke_y = entity[Player].y - diry[move];
	if(entity[Player].smoke == On) {
		if(bg[smoke_y * W + smoke_x] == Road)
			bg[smoke_y * W + smoke_x] = Smoke;
		
		entity[Player].smoke = Off;

	}
	for(int i = 0; i < H * W; i++) {
		if(bg[i] == Smoke && distance(entity[Player].x, entity[Player].y, i%W, i/W) > dist_max)
			bg[i] = Road;
	}
	for(int ent_id = Random; ent_id < NEntity; ent_id++) {
		if(bg[entity[ent_id].y * W + entity[ent_id].x] == Smoke)
			entity[ent_id].standstill = On;
		else 
			entity[ent_id].standstill = Off;
	}   
}

/*! \brief miner's attack (one of the enemy's speciality). Miner put 
 * rock on the road to cause accidents. */
static void miner_attack(int ent_id) {
	if(count_object(Rock) >= 10)
		return;
	int move = entity[ent_id].dir, chance = 10, rock_x = get_random_move(W), rock_y = get_random_move(H);
	if(chance == get_random_move(200)) {
		if(bg[rock_y * W + rock_x] == Road && (rock_x != W/2 && rock_y != H/2))
			bg[rock_y * W + rock_x] = Rock;
	}    
}

/*! \brief all enemy attacks. At this moment, there is only miner's attack. */
static void enemy_attack(int ent_id) {
	switch(entity[ent_id].id) {
		case Miner:
			miner_attack(ent_id);
			break;
	}
}

/*! \brief all ennemy movements. Some enemies move randomly and others follow
 * the player. */
static void enemy_move(int ent_id) {
	if(entity[ent_id].standstill == On)
		return;

	int move;
	if(entity[ent_id].id == Random)
		move = get_random_move(NDir);
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

	if((entity[Player].x == entity[ent_id].x) && (entity[Player].y == entity[ent_id].y))
		entity[Player].lose = On;

}

/*! \brief player changes automatically direction when there is an obstacle 
 * in front of him. */
static void move_auto(void) {
	int m;
	for(m = Left; m <= Down; m++) {
		if(entity[Player].dir == m)
			continue;
		if((entity[Player].dir == Left && m == Right) ||
		   (entity[Player].dir == Right && m == Left) ||
		   (entity[Player].dir == Up && m == Down) ||
		   (entity[Player].dir == Down && m == Up))
			continue;
		if(bg[(entity[Player].y + diry[m]) * W +(entity[Player].x + dirx[m])] != Tree &&
		   bg[(entity[Player].y + diry[m]) * W +(entity[Player].x + dirx[m])] != Forest &&
		   bg[(entity[Player].y + diry[m]) * W +(entity[Player].x + dirx[m])] != Rock) {
			entity[Player].y += diry[m];
			entity[Player].x += dirx[m];
			entity[Player].dir = m;
			break;
		}
	}
}

/*! \brief the player's movement. Call the function eval_fuel, move_auto, is_object */
static void player_move(int move) {
	eval_fuel();
	if(entity[Player].standstill == On) {
		entity[Player].fuel += StFuel;
		return;
	}

	int nextx = entity[Player].x + dirx[move];
	int nexty = entity[Player].y + diry[move];
	if((nextx < 0 || nextx >= W || nexty < 0 || nexty >= H || bg[nexty * W + nextx] == Tree || bg[nexty * W + nextx] == Forest))
		move_auto();
	else {
		entity[Player].x = nextx;
		entity[Player].y = nexty;
		entity[Player].dir = move;
	}
	entity[Player].fuel += MFuel;
	is_object();
	for(int ent_id = Random; ent_id < NEntity; ent_id++)
		if((entity[Player].x == entity[ent_id].x) &&(entity[Player].y == entity[ent_id].y))
			entity[Player].lose = On;
}

/*! \brief the principal function which updates events, the displays of the elements */
static void callback(void* d) {
	int ent_id;
	const Driver* dr = (Driver*)d;

	int move = dr->get_move();
	player_move(move);
	for(ent_id = Random; ent_id < NEntity; ent_id++) {
		enemy_move(ent_id);
		enemy_attack(ent_id);
	}
	player_attack();
	dr->draw_bg();
	for(ent_id = 0; ent_id < NEntity; ent_id++)
		dr->draw_entity(ent_id);

	if(entity[Player].lose == On) {
		dr->lose_game();
		player_loses();
		entity[Player].lose = Off;
	}
	dr->update();
	if(entity[Player].life <= 0 || entity[Player].level > NLevel)
		dr->quit_game();
	if(move != Nothing)
		is_win();
}

/*! \brief initialize all the values/elements of the game, i.e player's data, enemies'
 * data, game data. */
void init_value(void) {
	int i;
	g.h = H;
	g.w = W;
	g.screen_h = ScH;
	g.screen_w = ScW;
	g.background = bg;
	g.entity = entity;

	init_player();
	g.entity[Player].score = 0;
	g.entity[Player].life = NLife;
	g.entity[Player].level = 1;
	char * level = load_level();
	for(i = 0; i < H * W; ++i)
		g.background[i] = level[i] - 'A';
	init_enemy();

}

/*! \brief initialize the game, i.e initialize all the values of the game, driver data and call
 * the function callback. */
void init_game(const Driver* dr) {
	init_value();
	dr->init(&g);
	dr->start(callback);
}


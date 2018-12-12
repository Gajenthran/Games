#include <stdio.h>
#include <stdlib.h>
#include "player.h"

/*! \brief direction X */
static int dirx[] = {0, -1, 1, 0, 0 };
/*! \brief direction Y */
static int diry[] = {0, 0, 0, -1, 1 };

/*! \brief initialize the player's data. */
void init_player(Entity *entity) {
	entity->x = W/2;
	entity->y = H/2;
	entity->id = 0;
	entity->dir = Right;
	entity->smoke = entity->standstill = Off;
	entity->fuel = Fuel;
	entity->coeff = entity->bonus = 1;
}

/*! \brief initialize the enemies' data. */
void init_enemy(Entity *entity) {
	int ent_id;
	for(ent_id = Random; ent_id < NEntity; ent_id++) {
		if(ent_id > entity[Player].level)
			continue;
		entity[ent_id].x = (10 * ent_id) + 20;
		entity[ent_id].y = 15;
		entity[ent_id].id = ent_id;
		entity[ent_id].dir = Right;
	}
}

/*! \brief count the number of checkpoints */
int count_checkpoint(unsigned char* bg) {
	int count = 0;
	for(int i = 0; i < H * W; ++i)
		if(bg[i] == Checkpoint || bg[i] == SCheckpoint || bg[i] == LCheckpoint)
			count++;
	return count;
}

/*! \brief update the player's data, the enemies' data and 
 * the background when the player loses. */
void player_loses(Entity *entity, unsigned char* bg) {
	int i;
	init_player(&entity[Player]);
	init_enemy(entity);
	entity[Player].life--;
	for(i = 0; i < H * W; ++i)
		if(bg[i] == Smoke)
			bg[i] = Road;
}

/*! \brief calculate the distance between the player and an enemy, by
 * using Manhattan distance. */
int distance(int x_player, int y_player, int x_enemy, int y_enemy) {
	int x = x_player - x_enemy;
	int y = y_player - y_enemy;
	return(x * x) +(y * y);
}

/*! \brief return a random number beetwen 0 and N. */
int get_random_move(int n) {
	return(int)(n *(rand()/(RAND_MAX + 1.0)));
}

/*! \brief count the number of objects. */
int count_object(unsigned char* bg, int typecell) {
	int count = 0, i;
	for(i = 0; i < H * W; i++) {
		if(bg[i] == typecell)
			count++;
	}
	return count;
}

/*! \brief evaluate the quantity of fuel. If it's negative, the player will
 * move slowly. */
void eval_fuel(Entity *entity) {
	if(entity->fuel <= 0)
		entity->standstill = On;
	else
		entity->standstill = Off;
}

/*! \brief update player's data (score, lose...) depending on what object
 * is in the player's coordinates. */
void is_object(Entity *entity, unsigned char* bg) {
	const int dist_max = 8 * 8;
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
int follow_player(Entity *entity, int ent_id, unsigned char* bg) {
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
void player_attack(Entity * entity, unsigned char* bg) {
	const int dist_max = 8 * 8;
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
void miner_attack(Entity *entity, int ent_id, unsigned char *bg) {
	if(count_object(bg, Rock) >= 10)
		return;
	int move = entity[ent_id].dir, chance = 10, rock_x = get_random_move(W), rock_y = get_random_move(H);
	if(chance == get_random_move(200)) {
		if(bg[rock_y * W + rock_x] == Road && (rock_x != W/2 && rock_y != H/2))
			bg[rock_y * W + rock_x] = Rock;
	}    
}

/*! \brief all enemy attacks. At this moment, there is only miner's attack. */
void enemy_attack(Entity *entity, int ent_id, unsigned char* bg) {
	switch(entity[ent_id].id) {
		case Miner:
			miner_attack(entity, ent_id, bg);
			break;
	}
}

/*! \brief all ennemy movements. Some enemies move randomly and others follow
 * the player. */
void enemy_move(Entity *entity, int ent_id, unsigned char *bg) {
	if(entity[ent_id].standstill == On)
		return;

	int move;
	if(entity[ent_id].id == Random)
		move = get_random_move(NDir);
	else
		move = follow_player(entity, ent_id, bg);

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
void move_auto(Entity *entity, unsigned char *bg) {
	int m;
	for(m = Left; m <= Down; m++) {
		if(entity->dir == m)
			continue;
		if((entity->dir == Left && m == Right) ||
		   (entity->dir == Right && m == Left) ||
		   (entity->dir == Up && m == Down) ||
		   (entity->dir == Down && m == Up))
			continue;

		if(bg[(entity->y + diry[m]) * W +(entity->x + dirx[m])] != Tree &&
		   bg[(entity->y + diry[m]) * W +(entity->x + dirx[m])] != Forest &&
		   bg[(entity->y + diry[m]) * W +(entity->x + dirx[m])] != Rock) {
			entity->y += diry[m];
			entity->x += dirx[m];
			entity->dir = m;
			break;
		}
	}
}

/*! \brief the player's movement. Call the function eval_fuel, move_auto, is_object */
void player_move(Entity *entity, int move, unsigned char *bg) {
	eval_fuel(&entity[Player]);
	if(entity[Player].standstill == On) {
		entity[Player].fuel += StFuel;
		return;
	}

	int nextx = entity[Player].x + dirx[move];
	int nexty = entity[Player].y + diry[move];
	if((nextx < 0 || nextx >= W || nexty < 0 || nexty >= H || bg[nexty * W + nextx] == Tree || bg[nexty * W + nextx] == Forest))
		move_auto(&entity[Player], bg);
	else {
		entity[Player].x = nextx;
		entity[Player].y = nexty;
		entity[Player].dir = move;
	}
	entity[Player].fuel += MFuel;
	is_object(entity, bg);
	for(int ent_id = Random; ent_id < NEntity; ent_id++)
		if((entity[Player].x == entity[ent_id].x) &&(entity[Player].y == entity[ent_id].y))
			entity[Player].lose = On;
}


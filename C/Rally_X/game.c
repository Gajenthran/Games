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
	if(count_checkpoint(g.background) != 0)
		return;

	int i;
	init_player(&g.entity[Player]);
	g.entity[Player].score += 1000;
	g.entity[Player].level++;
	char * level = load_level();
	for(i = 0; i < H * W; ++i)
		g.background[i] = level[i] - 'A';
	init_enemy(g.entity);
}

/*! \brief the principal function which updates events, the displays of the elements */
static void callback(void* d) {
	int ent_id;
	const Driver* dr = (Driver*)d;

	int move = dr->get_move();
	player_move(entity, move, g.background);
	for(ent_id = Random; ent_id < NEntity; ent_id++) {
		enemy_move(g.entity, ent_id, g.background);
		enemy_attack(g.entity, ent_id, g.background);
	}
	player_attack(g.entity, g.background);
	dr->draw_bg();
	for(ent_id = 0; ent_id < NEntity; ent_id++)
		dr->draw_entity(ent_id);

	if(entity[Player].lose == On) {
		dr->lose_game();
		player_loses(g.entity, g.background);
		entity[Player].lose = Off;
	}
	dr->update();
	if(entity[Player].life <= 0 || entity[Player].level > NLevel)
		dr->quit_game();
	if(move != Nothing)
		next_level();
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

	init_player(&g.entity[Player]);
	g.entity[Player].score = 0;
	g.entity[Player].life = NLife;
	g.entity[Player].level = 1;
	char * level = load_level();
	for(i = 0; i < H * W; ++i)
		g.background[i] = level[i] - 'A';
	init_enemy(g.entity);

}

/*! \brief initialize the game, i.e initialize all the values of the game, driver data and call
 * the function callback. */
void init_game(const Driver* dr) {
	init_value();
	dr->init(&g);
	dr->start(callback);
}


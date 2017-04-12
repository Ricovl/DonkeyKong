#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>
#include <debug.h>

// shared libraries
#include <graphx.h>
#include <keypadc.h>

// donkeykong stuff
#include "defines.h"
#include "bonus_scores.h"
#include "images.h"
#include "overlay.h"


const uint8_t bonus_score_value[] = { 1, 2, 3, 5, 8 };
bonus_score_t bonus_score[MAX_BONUS_SCORES];
uint8_t num_bonus_scores;

/*
 * Initialize all the variables for a new bonus_score number 
 * 0 = 100, 1 = 200, 2 = 300, 3 = 500, 4 = 800
 */
void spawn_bonus_score(uint8_t score, uint24_t x, uint8_t y) {
	bonus_score_t *this_bonus_score = &bonus_score[num_bonus_scores];
	
	this_bonus_score->x = x;
	this_bonus_score->y = y;
	this_bonus_score->counter = 64;
	this_bonus_score->sprite = bonus_score_sprite[score];
	this_bonus_score->background_data[0] = 15;
	this_bonus_score->background_data[1] = 7;

	game.score += bonus_score_value[score] * 100;
	draw_player_score();

	num_bonus_scores++;
}


/* Updates all the bonus score counters */
void update_bonus_scores(void) {
	uint8_t i;

	for (i = 0; i < num_bonus_scores; i++) {
		bonus_score_t *this_bonus_score = &bonus_score[i];

		this_bonus_score->counter--;
		if (this_bonus_score->counter == 0) {	//delete the bonusscore
			gfx_BlitArea(gfx_buffer, this_bonus_score->x, this_bonus_score->y, 15, 7);
			bonus_score[i] = bonus_score[--num_bonus_scores];
		}
	}
}
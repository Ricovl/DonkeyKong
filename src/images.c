#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>
#include <debug.h>

// shared libraries
#include <lib/ce/graphx.h>
#include <lib/ce/keypadc.h>
#include <decompress.h>

// donkeykong stuff
#include "images.h"
#include "gfx\sprites_gfx.h"
#include "gfx\kong_gfx.h"


// Jumpman
gfx_image_t *jumpman_sprite[2][15] =  { { jumpman_walk_left0 , jumpman_walk_left1 , jumpman_walk_left2 , jumpman_jump_left , jumpman_slide_left , jumpman_climb_left0 , jumpman_climb_left1 , jumpman_climb_left2 , jumpman_standing, jumpman_hammer_left0 , jumpman_hammer_left1 , jumpman_hammer_left2 , jumpman_hammer_left3 , jumpman_hammer_left4 , jumpman_hammer_left5 },
										{ jumpman_walk_right0, jumpman_walk_right1, jumpman_walk_right2, jumpman_jump_right, jumpman_slide_right, jumpman_climb_right0, jumpman_climb_right1, jumpman_climb_right2, jumpman_standing, jumpman_hammer_right0, jumpman_hammer_right1, jumpman_hammer_right2, jumpman_hammer_right3, jumpman_hammer_right4, jumpman_hammer_right5 }, };

gfx_image_t *jumpman_dead[6] =			{ jumpman_dead0, jumpman_dead1, jumpman_dead2, jumpman_dead3, jumpman_dead5, jumpman_dead4 };

// Barrels
gfx_image_t *rolling_barrel[2][6] =   { { barrel_rolling_N0, barrel_rolling_N1, barrel_rolling_N2, barrel_rolling_N3, barrel_ladder_N0, barrel_ladder_N1 },
										{ barrel_rolling_B0, barrel_rolling_B1, barrel_rolling_B2, barrel_rolling_B3, barrel_ladder_B0, barrel_ladder_B1 }, };

// Firefoxes
gfx_image_t *firefox_sprite[2][4] =   { { firefox_left0 , firefox_left1 , fireball_left0 , fireball_left1  },
										{ firefox_right0, firefox_right1, fireball_right0, fireball_right1 }, };

// Kong
gfx_image_t *kong_goofy;
gfx_image_t *kong_sprite[12];		 // { kong_standing, kong_left, kong_holding, kong_right, kong_arm_left, kong_arm_right, kong_climbing_left0, kong_climbing_right0, kong_climbing_left1, kong_climbing_right1, kong_onhead };

// Bonus sprites
gfx_image_t *bonus_score_sprite[5] =	{ num100, num200, num300, num500, num800 };

// Hammer sprites
gfx_image_t *hammer_sprite[2][2] =    { { hammer_left0, hammer_left1 },
										{ hammer_right0, hammer_right1 } };

// Flame on top of oilcan
gfx_image_t *fire[4] =					{ fire0, fire1, fire2, fire3 };

// Pauline
gfx_image_t *pauline_sprite[2][3] =   { { pauline_left0 , pauline_left1 , pauline_left2  },
										{ pauline_right0, pauline_right1, pauline_right2 } };

// Bouncers
gfx_image_t *bouncer_sprite[2] =		{ bouncer0, bouncer1 };

// Pauline's items
gfx_image_t *pauline_item[3] =			{ parasol, purse, hat };

// Pulleys
gfx_image_t *pulley_right[3] =			{ pulley_right0, pulley_right1, pulley_right2 };
gfx_image_t *pulley_left[3]  =			{ pulley_left0 , pulley_left2 , pulley_left1 };


void decompress_images(void) {
	gfx_image_t *tmp_ptr;

	tmp_ptr = gfx_MallocSprite(40, 32);
	dzx7_Standard(kong_standing_compressed, tmp_ptr);
	kong_sprite[0] = tmp_ptr;

	tmp_ptr = gfx_MallocSprite(43, 32);
	dzx7_Standard(kong_left_compressed, tmp_ptr);
	kong_sprite[1] = tmp_ptr;

	tmp_ptr = gfx_MallocSprite(40, 32);
	dzx7_Standard(kong_holding_compressed, tmp_ptr);
	kong_sprite[2] = tmp_ptr;

	tmp_ptr = gfx_MallocSprite(43, 32);
	dzx7_Standard(kong_right_compressed, tmp_ptr);
	kong_sprite[3] = tmp_ptr;

	tmp_ptr = gfx_MallocSprite(46, 32);
	dzx7_Standard(kong_arm_left_compressed, tmp_ptr);
	kong_sprite[4] = tmp_ptr;

	tmp_ptr = gfx_MallocSprite(46, 32);
	dzx7_Standard(kong_arm_right_compressed, tmp_ptr);
	kong_sprite[5] = tmp_ptr;

	tmp_ptr = gfx_MallocSprite(33, 36);
	dzx7_Standard(kong_climbing_left0_compressed, tmp_ptr);
	kong_sprite[6] = tmp_ptr;

	tmp_ptr = gfx_MallocSprite(33, 36);
	dzx7_Standard(kong_climbing_right0_compressed, tmp_ptr);
	kong_sprite[7] = tmp_ptr;

	tmp_ptr = gfx_MallocSprite(38, 36);
	dzx7_Standard(kong_climbing_left1_compressed, tmp_ptr);
	kong_sprite[8] = tmp_ptr;

	tmp_ptr = gfx_MallocSprite(43, 36);
	dzx7_Standard(kong_climbing_right1_compressed, tmp_ptr);
	kong_sprite[9] = tmp_ptr;

	tmp_ptr = gfx_MallocSprite(40, 32);
	dzx7_Standard(kong_onhead_compressed, tmp_ptr);
	kong_sprite[10] = tmp_ptr;

	tmp_ptr = gfx_MallocSprite(40, 32);
	dzx7_Standard(kong_knockedout_compressed, tmp_ptr);
	kong_sprite[11] = tmp_ptr;

	kong_goofy = gfx_MallocSprite(46, 32);
	dzx7_Standard(kong_goofy_compressed, kong_goofy);
}
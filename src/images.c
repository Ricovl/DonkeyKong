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
#include <decompress.h>

// donkeykong stuff
#include "images.h"
#include "gfx/kong_gfx.h"
#include "gfx/sprites_gfx.h"


// Jumpman
gfx_image_t *jumpman_sprite[2][18] =  { { jumpman_left_walking0  , jumpman_left_walking1  , jumpman_left_walking2  , jumpman_left_climbing0  , jumpman_left_climbing1  , jumpman_left_climbing2  , jumpman_standing , jumpman_left_hammer00  , jumpman_left_hammer01 , jumpman_left_hammer10  , jumpman_left_hammer11  , jumpman_left_hammer20  , jumpman_left_hammer21  , jumpman_left_jumping  , jumpman_left_landing , jumpman_left_dying0 , jumpman_left_dying1 , jumpman_left_dying2  },
										{ jumpman_right_walking0 , jumpman_right_walking1 , jumpman_right_walking2 , jumpman_right_climbing0 , jumpman_right_climbing1 , jumpman_right_climbing2 , jumpman_standing , jumpman_right_hammer00 , jumpman_right_hammer01, jumpman_right_hammer10 , jumpman_right_hammer11 , jumpman_right_hammer20 , jumpman_right_hammer21 , jumpman_right_jumping , jumpman_right_landing, jumpman_right_dying0, jumpman_right_dying1, jumpman_right_dying2 }, };

// Barrels
gfx_image_t *rolling_barrel[2][6] =   { { barrel_rolling_N0, barrel_rolling_N1, barrel_rolling_N2, barrel_rolling_N3, barrel_falling_N0, barrel_falling_N1 },
										{ barrel_rolling_B0, barrel_rolling_B1, barrel_rolling_B2, barrel_rolling_B3, barrel_falling_B0, barrel_falling_B1 }, };

// Firefoxes
gfx_image_t *firefox_sprite[2][4] =   { { fireball_left0 , fireball_left1 , firefox_left0 , firefox_left1  },
										{ fireball_right0, fireball_right1, firefox_right0, firefox_right1 }, };

// Kong
gfx_image_t *kong_goofy;
gfx_image_t *kong_sprite[13];		 // { kong_standing, kong_left, kong_holding, kong_right, kong_arm_left, kong_arm_right, kong_climbing_left0, kong_climbing_right0, kong_climbing_left1, kong_climbing_right1, kong_onhead, kong_knockedOut, kong_teeth };

gfx_image_t *kong_crazy_eye[2] =		{ kong_eye1, kong_eye0 };
gfx_image_t *kong_knockedout_sprite[2] ={ knockedout_sprite1, knockedout_sprite0 };

// Bonus sprites
gfx_image_t *bonus_score_sprite[5] =	{ num100, num200, num300, num500, num800 };

// Hammer sprites
gfx_image_t *hammer_sprite[2][4] =    { { hammer_left_N0 , hammer_left_N1 , hammer_left_Y0 , hammer_left_Y1  },
										{ hammer_right_N0, hammer_right_N1, hammer_right_Y0, hammer_right_Y1 } };

gfx_image_t *hammer_hit[4] =			{ circle0, circle1, circle2, burst };

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


//const uint8_t *kong_compressed_images[] = { kong_standing_compressed, kong_left_compressed, kong_holding_compressed, kong_right_compressed, kong_arm_right_compressed, kong_climbing_left0_compressed, kong_climbing_right0_compressed, kong_climbing_left1_compressed, kong_climbing_right1_compressed, kong_onhead_compressed, kong_knockedout_compressed };

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

	tmp_ptr = gfx_MallocSprite(40, 32);
	kong_sprite[12] = gfx_RotateSpriteHalf(kong_sprite[10], tmp_ptr);

	kong_goofy = gfx_MallocSprite(46, 32);
	dzx7_Standard(kong_goofy_compressed, kong_goofy);
}
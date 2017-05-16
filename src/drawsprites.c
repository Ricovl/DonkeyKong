// standard headers
#include <math.h>
#include <stdio.h>
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
#include "jumpman.h"
#include "elevators.h"
#include "barrels.h"
#include "firefoxes.h"
#include "conveyors.h"
#include "hammers.h"
#include "bonus_scores.h"
#include "drawsprites.h"
#include "images.h"


void disable_sprites(void) {
	num_retractable_ladders =
		num_bonus_scores =
		num_bonus_items =
		num_firefoxes =
//		num_elevators =
		num_bouncers =
		num_barrels =
		num_hammers =
		num_pies = 0;
	oilcan.onFire = false;
}

/* Draws all the moving sprites on the screen(I should write this in asm for more speed) */
#pragma optspeed
void update_screen(void) {
	uint8_t i;

	/* Get the background behind the moving sprites before they get drawn */
	for (i = 0; i < num_retractable_ladders; i++)
		gfx_GetSprite((gfx_image_t*)retractableLadder[i].background_data, retractableLadder[i].x, retractableLadder[i].y_old);

	if (jumpman.enabled)
		gfx_GetSprite((gfx_image_t*)jumpman.buffer_data, jumpman.x_old - 7, jumpman.y_old - 15);

	for (i = 0; i < num_hammers; i++)		// Hammers
		gfx_GetSprite((gfx_image_t*)hammer[i].background_data, hammer[i].x_old - 7, hammer[i].y_old - 9);

	for (i = 0; i < num_barrels; i++)		// Barrels
		gfx_GetSprite((gfx_image_t*)barrel[i].background_data, barrel[i].x_old - 7, barrel[i].y_old - 9);

	for (i = 0; i < num_pies; i++)			// Pies
		gfx_GetSprite((gfx_image_t*)pie[i].background_data, pie[i].x_old, pie[i].y_old);

	for (i = 0; i < num_bouncers; i++)		// Bouncers
		gfx_GetSprite((gfx_image_t*)bouncer[i].background_data, bouncer[i].x_old, bouncer[i].y_old);

	for (i = 0; i < num_firefoxes; i++)		// Firefoxes
		gfx_GetSprite((gfx_image_t*)firefox[i].background_data, firefox[i].x_old - 7, firefox[i].y_old - 15);

	for (i = 0; i < num_bonus_scores; i++)	// Bonus scores
		gfx_GetSprite((gfx_image_t*)bonus_score[i].background_data, bonus_score[i].x, bonus_score[i].y);

	
	/* Draw the moving sprites in the buffer */
	for (i = 0; i < num_retractable_ladders; i++)
		gfx_TransparentSprite_NoClip(retracting_ladder_sprite, retractableLadder[i].x, retractableLadder[i].y);

	if (jumpman.enabled)
		gfx_TransparentSprite_NoClip(jumpman_sprite[jumpman.dir][jumpman.sprite], jumpman.x - 7, jumpman.y - 15);

	for (i = 0; i < num_hammers; i++)		// Hammers
		gfx_TransparentSprite_NoClip(hammer_sprite[hammer[i].dir][hammer[i].sprite], hammer[i].x - 7, hammer[i].y - 9);

	for (i = 0; i < num_barrels; i++) {		// Barrels
		barrel_t *this_barrel = &barrel[i];
		gfx_TransparentSprite(rolling_barrel[this_barrel->isBlue][this_barrel->sprite], this_barrel->x - ((this_barrel->sprite & 4) ? 7 : 5), this_barrel->y - 9);	// draw new sprite
	}	

	for (i = 0; i < num_pies; i++)
		gfx_TransparentSprite(pie_sprite, pie[i].x, pie[i].y);

	for (i = 0; i < num_bouncers; i++)		// Bouncers
		gfx_TransparentSprite(bouncer_sprite[bouncer[i].sprite], bouncer[i].x, bouncer[i].y);

	for (i = 0; i < num_firefoxes; i++)		// Firefoxes
		gfx_TransparentSprite_NoClip(firefox_sprite[firefox[i].dir & 1][firefox[i].sprite], firefox[i].x - 7, firefox[i].actualY - 15);

	for (i = 0; i < num_elevators; i++)		// Elevator platforms
		gfx_TransparentSprite_NoClip(elevator_platform, elevator[i].x, elevator[i].y);

	if (game.stage == STAGE_ELEVATORS) {	// Elevator receptables
		gfx_TransparentSprite_NoClip(receptable_bottom, 80, 223);
		gfx_TransparentSprite_NoClip(receptable_bottom, 144, 223);
		gfx_TransparentSprite_NoClip(receptable_top, 80, 72);
		gfx_TransparentSprite_NoClip(receptable_top, 144, 72);
	}

	if (game.stage == STAGE_CONVEYORS) {	// Pulley's
		uint8_t *y = &pulleyTable_y[0];

		for (i = 0; i < NUM_CONVEYORS; i++) {
			gfx_TransparentSprite_NoClip(conveyor[i].sprite0, pulleyTable_leftX[i], *y);
			gfx_TransparentSprite_NoClip(conveyor[i].sprite1, pulleyTable_rightX[i], *y);
			y++;
		}
	}

	for (i = 0; i < num_bonus_items; i++)	// Bonus items
		gfx_TransparentSprite_NoClip(pauline_item[bonus_item[i].type], bonus_item[i].x, bonus_item[i].y);

	if(game.stage <= STAGE_CONVEYORS)		// Oilcan
		gfx_TransparentSprite_NoClip(oilcan_sprite, oilcan.x, oilcan.y);
	if (oilcan.onFire)
		gfx_TransparentSprite_NoClip(fire[oilcan.sprite], oilcan.x, oilcan.y - 16);

	for (i = 0; i < num_bonus_scores; i++)	// Bonus scores
		gfx_TransparentSprite_NoClip(bonus_score[i].sprite, bonus_score[i].x, bonus_score[i].y);


	/* Swaps the buffer with the screen.. */
	gfx_SwapDraw();


	/* Remove the moving sprites out of the current buffer and updates their old positions */
	for (i = 0; i < num_retractable_ladders; i++) {
		gfx_Sprite_NoClip((gfx_image_t*)retractableLadder[i].background_data, retractableLadder[i].x, retractableLadder[i].y_old);
		retractableLadder[i].y_old = retractableLadder[i].y;
	}
	
	if (jumpman.enabled) {
		gfx_Sprite_NoClip((gfx_image_t*)jumpman.buffer_data, jumpman.x_old - 7, jumpman.y_old - 15);
		jumpman.x_old = jumpman.x; jumpman.y_old = jumpman.y;
	}

	for (i = 0; i < num_hammers; i++) {		// Hammers
		hammer_t *this_hammer = &hammer[i];

		gfx_Sprite_NoClip((gfx_image_t*)this_hammer->background_data, this_hammer->x_old - 7, this_hammer->y_old - 9);
		this_hammer->x_old = this_hammer->x; this_hammer->y_old = this_hammer->y;
	}

	for (i = 0; i < num_barrels; i++) {		// Barrels
		barrel_t *this_barrel = &barrel[i];

		gfx_Sprite_NoClip((gfx_image_t*)this_barrel->background_data, this_barrel->x_old - 7, this_barrel->y_old - 9);
		this_barrel->x_old = this_barrel->x; this_barrel->y_old = this_barrel->y;
	}

	for (i = 0; i < num_pies; i++) {		// Bouncers
		pie_t *this_pie = &pie[i];

		gfx_Sprite((gfx_image_t*)this_pie->background_data, this_pie->x_old, this_pie->y_old);
		this_pie->x_old = this_pie->x; this_pie->y_old = this_pie->y;
	}

	for (i = 0; i < num_bouncers; i++) {	// Bouncers
		bouncer_t *this_bouncer = &bouncer[i];

		gfx_Sprite((gfx_image_t*)this_bouncer->background_data, this_bouncer->x_old, this_bouncer->y_old);
		this_bouncer->x_old = this_bouncer->x; this_bouncer->y_old = this_bouncer->y;
	}

	for (i = 0; i < num_firefoxes; i++) {	// Firefoxes
		firefox_t *this_firefox = &firefox[i];

		gfx_Sprite_NoClip((gfx_image_t*)this_firefox->background_data, this_firefox->x_old - 7, this_firefox->y_old - 15);
		this_firefox->x_old = this_firefox->x; this_firefox->y_old = this_firefox->actualY;
	}

	for (i = 0; i < num_elevators; i++) {	// Elevator platforms
		gfx_FillRectangle_NoClip(elevator[i].x, elevator[i].y - 1, 16, 10);
		gfx_SetColor(COLOR_RED_DARK);
		gfx_FillRectangle_NoClip(elevator[i].x + 7, elevator[i].y - 1, 2, 10);
		gfx_SetColor(COLOR_BACKGROUND);
	}

	if (oilcan.onFire)
		gfx_FillRectangle_NoClip(oilcan.x, oilcan.y - 16, 16, 16);

	for (i = 0; i < num_bonus_scores; i++)	// Bonus scores
		gfx_Sprite_NoClip((gfx_image_t*)bonus_score[i].background_data, bonus_score[i].x, bonus_score[i].y);
}
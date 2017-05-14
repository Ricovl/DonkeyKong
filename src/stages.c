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
#include "stages.h"
#include "jumpman.h"
#include "overlay.h"
#include "kong.h"
#include "bonus_scores.h"
#include "barrels.h"
#include "firefoxes.h"
#include "conveyors.h"
#include "rivets.h"
#include "hammers.h"
#include "images.h"
#include "elevators.h"


/* Draw the stage with overlay and initialize the begin variables */
void initialize_stage(uint8_t stage) {
	uint8_t i;

	gfx_FillScreen(COLOR_BACKGROUND);
	draw_stage(stage_data[game.stage - 1]);

	init_jumpman(95, 231);

	// Kong
	kong.x = 72;
	kong.y = 40;

	// Pauline
	pauline.x = 136;
	pauline.y = 18;
	pauline.dir = FACE_RIGHT;
	pauline.sprite = 0;

	// Conveyors
	memset(&conveyor, 0, sizeof(conveyor_t) * NUM_CONVEYORS);
	conveyor[Top].reverseCounter = 0x80;
	conveyor[Middle].reverseCounter = 0xC0;
	conveyor[Bottom].reverseCounter = 0xFF;
	for (i = 0; i < NUM_CONVEYORS; i++) {
		conveyor[i].direction = 0xFF;
		conveyor[i].sprite0 = pulley_left0;
		conveyor[i].sprite1 = pulley_right0;
	}
	conveyor[Top].direction = 0x01;
	/*conveyorVector_top =
	conveyorVector_middleLeft =
	conveyorVector_middleRight =
	conveyorVector_bottom = 0;*/

	// Retractable ladders
	retractableLadder[0].moveTimer = 0x00;
	retractableLadder[0].waitTimer = 1;
	retractableLadder[0].y_old =
		retractableLadder[0].y = 80;
	retractableLadder[0].x = 63;
	retractableLadder[0].background_data[0] = 10;
	retractableLadder[0].background_data[1] = 16;

	retractableLadder[1].moveTimer = 0x10;
	retractableLadder[1].waitTimer = 1;
	retractableLadder[1].y_old =
		retractableLadder[1].y = 80;
	retractableLadder[1].x = 247;
	retractableLadder[1].background_data[0] = 10;
	retractableLadder[1].background_data[1] = 16;

	// Pies
	pieTimer = 0;
	
	// Barrels
	barrelDeploymentTimer = 1;
	deployingBarrel = deployBarrel = false;

	// Oilcan
	oilcan.onFire = false;
	oilcan.fireRelease = 0;
	oilcan.updateTimer = 4;
	oilcan.releaseTimer = 0x10;

	releaseFirefox =
		releaseBouncer =
		releasePie = false;

	// Reset all entity nums
	num_retractable_ladders =
		num_bonus_scores =
		num_bonus_items =
		num_firefoxes =
		num_elevators =
		num_bouncers =
		num_barrels =
		num_hammers =
		num_pies = 0;

	// Hammers
	memset(&hammer, 0, sizeof(hammer_t) * 2);
	if (game.stage != STAGE_ELEVATORS) {
		for (i = 0; i < 2; i++) {
			uint8_t location = (game.stage >> 1) + i;
			hammer[i].x_old = hammer[i].x = hammer_locations_x[location];
			hammer[i].y_old = hammer[i].y = hammer_locations_y[location];
			hammer[i].background_data[0] = 13;
			hammer[i].background_data[1] = 10;
		}
		num_hammers = 2;
	}
	hammerActive = 0;
	wasItemHit = false;

	// Bonus items
	if (game.stage != STAGE_BARRELS) {
		for (i = 0; i < 3; i++) {
			uint8_t location = (game.stage - 2) * 3 + i;
			bonus_item[i].type = i;
			bonus_item[i].y = item_locations_y[location];
			bonus_item[i].x = item_locations_x[location];
			bonus_item[i].background_data[0] = *(uint8_t*)(pauline_item[i]);
			bonus_item[i].background_data[1] = bonus_item_height[i];
			gfx_GetSprite((gfx_image_t*)bonus_item[i].background_data, bonus_item[i].x, bonus_item[i].y);
		}
		num_bonus_items = 3;
	}

	if (stage == STAGE_BARRELS) {				// Stage barrels stuff
		// Initialize some varialbes
		oilcan.x = 64;
		oilcan.y = 216;

		kong.y = 36;

		// Draw some objects
		gfx_Sprite_NoClip(barrel_standing, 48, 51);
		gfx_Sprite_NoClip(barrel_standing, 58, 51);
		gfx_Sprite_NoClip(barrel_standing, 48, 35);
		gfx_Sprite_NoClip(barrel_standing, 58, 35);
	}
	else if (stage == STAGE_CONVEYORS) {		// Stage conveyors stuff
		// Initialize some varialbes
		oilcan.x = 152;
		oilcan.y = 112;
		oilcan.onFire = true;
		oilcan.fireRelease = 1;

		num_retractable_ladders = 2;

		// Change palette colors
		gfx_SetPalette(conveyors_palette, 6, 3);
	}
	else if (stage == STAGE_ELEVATORS) {		// Stage elevators stuff
		// Initalize some variables
		init_jumpman(54, 215);

		// Setup two firefoxes
		memset(firefox, 0, sizeof(firefox_t) * 2);
		firefox[0].x_old = firefox[0].x = 0x58 + 33;
		firefox[0].actualY = firefox[0].y_old = firefox[0].y = 0x80 - 9;
		firefox[0].background_data[0] = 16;
		firefox[0].background_data[1] = 16;

		firefox[1].x_old = firefox[1].x = 0xEB + 33;
		firefox[1].actualY = firefox[1].y_old = firefox[1].y = 0x60 - 9;
		firefox[1].background_data[0] = 16;
		firefox[1].background_data[1] = 16;
		num_firefoxes = 2;

		elevatorTimer = 0x34;
		num_elevators = 6;
		for (i = 0; i < 6; i++) {
			elevator[i].movingUp = (i < 3);
			elevator[i].y = elevator_y_table[i];
			elevator[i].x = (i < 3) ? 80 : 144;
		}

		// Draw the elevator pillars
		gfx_SetColor(COLOR_RED_DARK);
		gfx_Rectangle_NoClip(87, 88, 2, 136);
		gfx_Rectangle_NoClip(151, 88, 2, 136);
	}
	else if (stage == STAGE_RIVETS) {			// Stage rivets stuff
		// Initialize some variables
		kong.x = 140;

		pauline.x = 153;
		pauline.y = 10;

		memset(rivet_enabled, true, 8);
		num_rivets = 8;
		for (i = 71; i <= 191; i += 40) {
			gfx_Sprite_NoClip(rivet, 104, i);
			gfx_Sprite_NoClip(rivet, 208, i);
		}
		
		// Draw the pillars
		gfx_SetColor(22);
		gfx_Rectangle_NoClip(119, 40, 2, 32);
		gfx_Rectangle_NoClip(199, 40, 2, 32);

		// Change palette colors
		gfx_SetPalette(rivets_palette, 6, 3);
		gfx_palette[9] = gfx_palette[COLOR_COLLISION - 1];
	}	

	// Kong
	kong.sprite = 0;
	kong.x_old = kong.x;
	kong.y_old = kong.y;
	kong.background_data[0] = 40;
	kong.background_data[1] = 32;
	gfx_GetSprite((gfx_image_t*)kong.background_data, kong.x, kong.y);

	// Bonus timer stuff
	game.initialBonusValue = game_data.level * 10 + 40;
	if (game.initialBonusValue > 80)
		game.initialBonusValue = 80;
	game.blueBarrelCounter = game.bonusTimer = game.initialBonusValue;
	game.bonusDelay = game.initialBonusDelay = (220 - game.initialBonusValue * 2);
	game.difficultyTimer0 = game.difficultyTimer1 = 0;

	game.timeRanOut = false;
	frameCounter = rand() & 255;

	draw_overlay_full();
	draw_bonus_box();

	/* Copy the buffer to the screen, so they are the same */
	gfx_Blit(gfx_buffer);

	gfx_SetColor(COLOR_BACKGROUND);
	
	draw_kong();
	draw_pauline(false);
}

/* Draws the stage itself to the screen */
void draw_stage(uint8_t *array_b) {
	while (*(uint8_t*)array_b != 0xAA) {
		uint24_t x, x1;
		tile_t tile;

		memcpy(&tile, array_b, sizeof(tile_t));
		x1 = tile.x1 + 48;
		x = tile.x + 48;

		if (tile.type <= 1) {			// ladder and broken ladder
			uint8_t i = 0;

			if (game.stage == STAGE_CONVEYORS && tile.type == 1)
				tile.y -= 8;
			for (tile.y += 8 - (tile.y & 7); tile.y < tile.y1; tile.y += 8) {
				gfx_Sprite_NoClip(ladder, x, tile.y);
				if (tile.type && i == 0) {
					tile.y += 8;
					if (((tile.y1 - tile.y) & 7) == 0) // == 32? don't know if this will works for conveyors
						tile.y += 8;
				}
				i++;
			}
		}
		else if (tile.type == 2) {		// girder
			for (; x <= x1; x += 8) {
				gfx_Sprite_NoClip(girder, x, tile.y);
				if (tile.y != tile.y1 && (x & 15) == 8) {
					if (tile.y < tile.y1) { tile.y++; }
					else { tile.y--; }
				}
			}
		}
		else if (tile.type == 3) {		// conveyor
			for (; x <= x1; x += 8) {
				gfx_Sprite_NoClip(conveyor_tile, x, tile.y);
			}
		}
		else if (tile.type == 4) {		// erase girder
			gfx_FillRectangle_NoClip(x, tile.y, (x1 - x) + 16, 8);
		}
		else if (tile.type == 5) {		// circle girder used in rivets
			for (; x <= x1; x += 8) {
				gfx_Sprite_NoClip(girder_circle, x, tile.y);
			}
		}
		else if (tile.type == 6) {		// X tile
			for (; x <= x1; x += 8) {
				gfx_Sprite_NoClip(x_tile, x, tile.y);
			}
		}

		array_b += 5;
	}
}

/* Some lookup tables used above */
uint8_t  hammer_locations_y[] = { 91, 183, 132, 171, 91 , 131 };
uint24_t hammer_locations_x[] = { 68, 220, 66 , 156, 157, 60  };
uint8_t  item_locations_y[]	  =	{ 136, 222, 143, 104, 78, 183, 56, 222, 183 };
uint24_t item_locations_x[]   =	{ 224, 167, 109, 52, 255, 117, 76, 175, 245 };
uint16_t conveyors_palette[3] = { gfx_RGBTo1555(254, 104, 0),  gfx_RGBTo1555(254, 184, 84), gfx_RGBTo1555(255, 254, 255) };
uint16_t rivets_palette[3]	  =	{ gfx_RGBTo1555(0, 0, 255),	   gfx_RGBTo1555(0, 255, 255),	gfx_RGBTo1555(255, 184, 0)	 };
uint8_t	 stage_order[20] = {
	1, 4,
	1, 3, 4,
	1, 2, 3, 4,
	1, 2, 1, 3, 4,
	1, 2, 1, 3, 1, 4,
};


/* Data of all the stages */
uint8_t *stage_data[4] = { (uint8_t*)stage_barrels_data, (uint8_t*)stage_conveyors_data, (uint8_t*)stage_elevators_data, (uint8_t*)stage_rivets_data };

uint8_t stage_barrels_data[] = {
	0x00, 0xB8, 0x47, 0xB8, 0x5F,  // short ladder at top right
	0x00, 0xB8, 0x89, 0xB8, 0xA1,  // short ladder at center right
	0x00, 0xB8, 0xCB, 0xB8, 0xE3,  // short ladder at bottom right
	0x00, 0x50, 0x08, 0x50, 0x44,  // kong's ladder (right)
	0x01, 0x50, 0xC5, 0x50, 0xE8,  // bottom broken ladder
	0x00, 0x20, 0x68, 0x20, 0x80,  // short ladder at left side
	0x00, 0x20, 0xAA, 0x20, 0xC2,  // short ladder at left side above oil can
	0x00, 0x40, 0x08, 0x40, 0x44,  // kong's ladder (left)
	0x01, 0x40, 0x82, 0x40, 0xA8,  // second broken ladder from bottom
	0x00, 0x48, 0x66, 0x48, 0x82,  // longer ladder under the top left hammer
	0x00, 0x60, 0xA6, 0x60, 0xC6,  // longer ladder to left of bottom hammer
	0x00, 0x70, 0x85, 0x70, 0xA5,  // center longer ladder
	0x00, 0x80, 0x28, 0x80, 0x44,  // ladder leading to girl
	0x01, 0xA8, 0x60, 0xA8, 0x88,  // third broken ladder
	0x01, 0x58, 0x44, 0x58, 0x65,  // fourth broken ladder near kong

	0x02, 0x58, 0x28, 0x87, 0x28,  // top girder where girl sits
	0x02, 0x00, 0x44, 0x8F, 0x44,  // girder where kong sits
	0x02, 0x90, 0x45, 0xCF, 0x48,  // 1st slanted girder at top right
	0x02, 0x10, 0x69, 0xDF, 0x5D,  // 2nd slanted girder
	0x02, 0x00, 0x7E, 0xCF, 0x8A,  // 3rd slanted girder
	0x02, 0x10, 0xAB, 0xDF, 0x9F,  // 4th slanted girder
	0x02, 0x00, 0xC0, 0xCF, 0xCC,  // 5th slanted girder
	0x02, 0x70, 0xE7, 0xDF, 0xE1,  // bottom slanted girder
	0x02, 0x00, 0xE8, 0x6F, 0xE8,  // bottom flat girder where jumpman starts
	0xAA,
};

uint8_t stage_conveyors_data[] = {
	0x00, 0x50, 0x08, 0x50, 0x48,	// kong's ladder (right)
	0x00, 0x50, 0x70, 0x50, 0x98,	// center ladder to left of oil can fire
	0x00, 0x50, 0xC0, 0x50, 0xE8,	// bottom level ladder #2 of 4
	0x00, 0x40, 0x08, 0x40, 0x48,	// kong's ladder (left)
	0x00, 0x40, 0x98, 0x40, 0xC0,	// ladder under the hat
	0x00, 0x88, 0x70, 0x88, 0x98,	// center ladder to right of oil can fire
	0x00, 0x88, 0xC0, 0x88, 0xE8,	// bottom level ladder #3 of 4
	0x01, 0x10, 0x48, 0x10, 0x70,	// top broken ladder left side
	0x01, 0xC8, 0x48, 0xC8, 0x70,	// top broken ladder right side
	0x00, 0x18, 0x70, 0x18, 0x98,	// ladder on left platform with hammer
	0x00, 0xC0, 0x70, 0xC0, 0x98,	// ladder on right plantform with umbrella
	0x00, 0x90, 0x98, 0x90, 0xC0,	// ladder to right of bottom hammer
	0x00, 0x18, 0xC0, 0x18, 0xE8,	// bottom level ladder #1 of 4
	0x00, 0xC0, 0xC0, 0xC0, 0xE8,	// bottom level ladder #4 of 4
	0x00, 0x80, 0x28, 0x80, 0x48,	// ladder leading to girl

	0x06, 0x60, 0x80, 0x78, 0x80,	// central patch of XXX's
	0x06, 0x60, 0x88, 0x78, 0x88,	// central patch of XXX's		y's stay the same, so they are drawn horizontal
	0x06, 0x60, 0x90, 0x78, 0x90,	// central patch of XXX's

	0x02, 0x58, 0x28, 0x87, 0x28,	// girder where girl sits
	0x03, 0x08, 0x48, 0xD0, 0x48,	// top conveyor girder
	0x03, 0x80, 0x70, 0xDF, 0x70,	// top right conveyor next to oil can
	0x03, 0x00, 0x70, 0x5F, 0x70,	// top left conveyor next to oil can

	0x02, 0x40, 0x98, 0x97, 0x98,	// center ledge
	0x02, 0xA8, 0x98, 0xD7, 0x98,	// right center ledge
	0x02, 0x08, 0x98, 0x2F, 0x98,	// left center ledge(has hammer)

	0x03, 0x08, 0xC0, 0xD0, 0xC0,	// main lower conveyor girder(has hammer)
	0x02, 0x00, 0xE8, 0xDF, 0xE8,	// bottom level girder
	0xAA,
};

uint8_t stage_elevators_data[] = {
	0x00, 0x50, 0x08, 0x50, 0x48,	// kong's ladder (right)
	0x00, 0x50, 0x78, 0x50, 0xC0,	// center ladder right
	0x00, 0x40, 0x08, 0x40, 0x48,	// long's ladder (left)
	0x00, 0x40, 0x78, 0x40, 0xC0,	// center ladder left
	0x00, 0xD0, 0x58, 0xD0, 0x80,	// far top right ladder leading to purse
	0x00, 0xD0, 0xA8, 0xD0, 0xC0,	// far bottom right ladder
	0x00, 0xB8, 0x80, 0xB8, 0xA0,	// ladder leading to purse(lower level)
	0x00, 0xA0, 0x48, 0xA0, 0x68,	// ladder leading to kong's level
	0x00, 0x88, 0x70, 0x88, 0x90,	// ladder to right of top right elevator
	0x00, 0x80, 0x28, 0x80, 0x48,	// ladder leading up to girl
	0x00, 0x10, 0x78, 0x10, 0xB0,	// long ladder on left side
	0x00, 0x08, 0xB0, 0x08, 0xD8,	// bottom left ladder

	0x02, 0x58, 0x28, 0x87, 0x28,	// girder girl is on
	0x02, 0x00, 0x48, 0xA7, 0x48,	// kong's girder
	0x02, 0xD0, 0x58, 0xDF, 0x58,	// girder where purse is
	0x02 ,0xB8 ,0x60 ,0xC7 ,0x60,	// girder to left of purse
	0x02, 0xA0, 0x68, 0xAF, 0x68,	// girder holding ladder that leads up to kong's level
	0x02, 0x80, 0x70, 0x97, 0x70,	// girder to right of top right elevator
	0x02, 0x38, 0x78, 0x57, 0x78,	// top girder for central ladder section between elevators
	0x02, 0x00, 0x78, 0x17, 0x78,	// girder that holds the umbrella
	0x02, 0xB8, 0x80, 0xDF, 0x80,	// girder under the girder that has the purse
	0x02, 0x88, 0x90, 0x97, 0x90,	// bottom girder for section to right of top right elevator
	0x02, 0xA0, 0x98, 0xAF, 0x98,	// small floating girder
	0x02, 0xB8, 0xA0, 0xC7, 0xA0,	// small girder
	0x02, 0xD0, 0xA8, 0xDF, 0xA8,	// small girder
	0x02, 0x00, 0xB0, 0x17, 0xB0,	// girder just above jumpman start
	0x02, 0xC8, 0xC0, 0xDF, 0xC0,	// small girder on far right bottom
	0x02, 0x40, 0xC0, 0x57, 0xC0,	// bottom girder for central ladder section between elevators
	0x02, 0xB0, 0xC8, 0xBF, 0xC8,	// small girder
	0x02, 0x98, 0xD0, 0xA7, 0xD0,	// small girder
	0x02, 0x78, 0xD8, 0x8F, 0xD8,	// floating girder where the right side elevator gets off
	0x02, 0x00, 0xD8, 0x17, 0xD8,	// girder where jumpman starts
	0x02, 0x00, 0xE8, 0xDF, 0xE8,	// long bottom girder(jumpman dies if he gets that low)
	0xAA,
};

uint8_t stage_rivets_data[] = {
	0x00, 0x68, 0x70, 0x68, 0x98,	// center ladder level 3
	0x00, 0x68, 0xC0, 0x68, 0xE8,	// bottom center ladder
	0x00, 0x20, 0x48, 0x20, 0x70,	// top left ladder
	0x00, 0x40, 0x48, 0x40, 0x70,	// top left ladder(right side)
	0x00, 0x98, 0x48, 0x98, 0x70,	// top right ladder(left side)
	0x00, 0xB8, 0x48, 0xB8, 0x70,	// top right ladder
	0x00, 0x18, 0x70, 0x18, 0x98,	// level 3 ladder left side
	0x00, 0xC0, 0x70, 0xC0, 0x98,	// level 3 ladder right side
	0x00, 0x10, 0x98, 0x10, 0xC0,	// level 2 ladder left side
	0x00, 0x48, 0x98, 0x48, 0xC0,	// level 2 ladder #2 of 4
	0x00, 0x90, 0x98, 0x90, 0xC0,	// level 2 ladder #3 of 4
	0x00, 0xC8, 0x98, 0xC8, 0xC0,	// level 2 ladder right side
	0x00, 0x08, 0xC0, 0x08, 0xE8,	// bottom left ladder
	0x00, 0xD0, 0xC0, 0xD0, 0xE8,	// bottom right ladder

	0x05, 0x38, 0x20, 0xA7, 0x20,	// girder above kong 0x68
	0x05, 0x20, 0x48, 0xBF, 0x48,	// girder kong stands on
	0x05, 0x18, 0x70, 0xC7, 0x70,	// level 4 girder
	0x05, 0x10, 0x98, 0xCF, 0x98,	// level 3 girder
	0x05, 0x08, 0xC0, 0xD7, 0xC0,	// level 2 girder
	0x05, 0x00, 0xE8, 0xDF, 0x98,	// bottom level girder
	0xAA,
};


/* Data used for game intro */
uint8_t stage_barrels_intro_data[] = {
	0x00, 0x40, 0x08, 0x40, 0x44,   // kong's ladder (left)
	0x00, 0x50, 0x08, 0x50, 0x44,   // kong's ladder (right)
	0x00, 0x80, 0x28, 0x80, 0x44,   // ladder to reach girl

	0x02, 0x58, 0x28, 0x87, 0x28,   // top level where girl sits
	0x02, 0x10, 0x5D, 0xDF, 0x5D,   // 2nd girder down
	0x02, 0x00, 0x7E, 0xCF, 0x7E,   // 3rd girder down
	0x02, 0x10, 0x9F, 0xDF, 0x9F,   // 4th girder down
	0x02, 0x00, 0xC0, 0xCF, 0xC0,   // 5th girder down
	
	0x00, 0x70, 0x44, 0x70, 0xE1,   // long ladder (left)
	0x00, 0x80, 0x44, 0x80, 0xE1,   // long ladder (right)

	0x02, 0x00, 0x44, 0xCF, 0x44,   // kongs level girder
	0x02, 0x00, 0xE1, 0xDF, 0xE1,   // bottom girder
	0xAA,
};

uint8_t stage_barrels_slanted_top[] = {
	0x04, 0x90, 0x44, 0xCF, 0x44,   // clear right of kongs level girder
	0x02, 0x90, 0x45, 0xCF, 0x48,	// 1st slanted girder at top right
	0xAA
};

uint8_t stage_barrels_slanted[] = {
	0x04, 0x10, 0x5D, 0xDF, 0x5D,  // 2nd girder down
	0x02, 0x10, 0x69, 0xDF, 0x5D,  // 2nd slanted girder
	0xAA,

	0x04, 0x00, 0x7E, 0xCF, 0x7E,  // 3rd girder down
	0x02, 0x00, 0x7E, 0xCF, 0x8A,  // 3rd slanted girder
	0xAA,

	0x04, 0x10, 0x9F, 0xDF, 0x9F,  // 4th girder down
	0x02, 0x10, 0xAB, 0xDF, 0x9F,  // 4th slanted girder
	0xAA,

	0x04, 0x00, 0xC0, 0xCF, 0xC0,  // 5th girder down
	0x02, 0x00, 0xC0, 0xCF, 0xCC,  // 5th slanted girder
	0xAA,

	0x04, 0x00, 0xE1, 0xDF, 0xE1,  // bottom girder
	0x02, 0x70, 0xE7, 0xDF, 0xE1,  // bottom slanted girder
	0x02, 0x00, 0xE8, 0x6F, 0xE8,  // bottom flat girder where jumpman starts
	0xAA,
};
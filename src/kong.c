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

// kong stuff
#include "defines.h"
#include "jumpman.h"
#include "kong.h"
#include "conveyors.h"
#include "barrels.h"
#include "images.h"
#include "stages.h"
#include "overlay.h"

#include "screens.h"


/* Updates kong and pauline */
void update_kong(void) {
	if (game.stage == STAGE_CONVEYORS) {
		kong.x += conveyorVector_top;
		draw_kong();
	}

	if (frameCounter == 0 && !kong.timerOn)
		kong.timerOn = true;

	if (kong.timerOn) {
		kong.timer++;

		if (kong.timer == 0x80) {
			kong.timer = 0;
			kong.timerOn = false;

			draw_pauline(false);

			if (deployingBarrel)
				goto update_pauline;
			kong.sprite = 0;
		}
		else {
			if (!deployingBarrel && (kong.timer & 31) == 0) {
				if (((kong.timer >> 5) & 1) == 1)
					kong.sprite = 5;	// right arm up
				else
					kong.sprite = 4;	// left arm up
			}
			else goto update_pauline;
		}

		draw_kong();
	}

update_pauline:
	if (game.stage == STAGE_RIVETS) {
		if ((jumpman.x < 160 && pauline.dir != FACE_LEFT) || (pauline.dir != FACE_RIGHT && jumpman.x >= 160)) {
			draw_pauline(false);
			pauline.dir ^= 1;
			draw_pauline((kong.timer >= 64));
		}
	}

	if (((kong.timer >> 6) & 1) == 0)
		return;

	if ((kong.timer & 7) != 0)
		return;

	pauline.sprite ^= 1;
	draw_pauline(true);
}

kong_t kong;

/* Draw kong to the screen and the buffer(does not work if y position or height has changed) */
void draw_kong(void) {
	uint8_t width_old = kong.background_data[0];
	uint8_t width;
	uint24_t blitx, x = kong.x;

	if (kong.sprite) {
		if (kong.sprite == 1)	x -= 4;
		else if (kong.sprite == 3)	x++;
		else if (kong.sprite == 4)	x -= 4;
		else if (kong.sprite == 5)	x -= 2;
	}

	// Clear kong in buffer and store background
	gfx_Sprite_NoClip((gfx_image_t*)kong.background_data, kong.x_old, kong.y_old);
	kong.background_data[0] = *(uint8_t*)kong_sprite[kong.sprite];
	gfx_GetSprite((gfx_image_t*)kong.background_data, x, kong.y);

	// Draw kong
	gfx_TransparentSprite_NoClip(kong_sprite[kong.sprite], x, kong.y);

	// Copy the sprite to the buffer and the erased area ^
	if (kong.x_old < x) {
		blitx = kong.x_old;
		width = (x + kong.background_data[0]) - blitx;
		if (blitx + width < blitx + width_old)
			width += (blitx + width_old) - (blitx + width);
	}
	else {
		blitx = x;
		width = (kong.x_old + width_old) - blitx;
		if (blitx + width < x + kong.background_data[0])
			width = kong.background_data[0];
	}
	gfx_BlitRectangle(gfx_buffer, blitx, kong.y, width, 32);

	kong.x_old = x;
	kong.y_old = kong.y;
}

pauline_t pauline;

/* Draw pauline to the screen and buffer with help or without */
void draw_pauline(bool help) {
	uint24_t x = pauline.x;

	gfx_Sprite_NoClip(pauline_sprite[pauline.dir][pauline.sprite], pauline.x, pauline.y);
	gfx_BlitRectangle(gfx_buffer, pauline.x, pauline.y, 16, 22);

	if (pauline.dir == FACE_LEFT)
		x -= 24;
	else
		x += 17;

	if (help)
		gfx_Sprite_NoClip((pauline.dir) ? help_right : help_left, x, pauline.y - 2);
	else
		gfx_FillRectangle_NoClip(x, pauline.y - 2, 24, 8);

	gfx_BlitRectangle(gfx_buffer, x, pauline.y - 2, 24, 8);
}


/* Draws kong for use in cinematics */
void render_kong(void) {
	uint24_t x = kong.x;
	uint8_t y = kong.y;

	if (kong.sprite) {
		if (kong.sprite == 1)	x -= 4;
		else if (kong.sprite == 3 || kong.sprite == 7 || kong.sprite == 9)	x++;
		else if (kong.sprite == 4)	x -= 4;
		else if (kong.sprite == 5)	x -= 2;
		else if (kong.sprite == 6 || kong.sprite == 8)	x += 6;
		if (kong.sprite == 9) y += 4;
	}

	gfx_GetSprite((gfx_image_t*)kong.background_data, kong.x_old, kong.y_old - 32);
	gfx_TransparentSprite(kong_sprite[kong.sprite], x, y - 32);
	if (kong.sprite == 11) {
		if ((frameCounter % 3) != 0) {
			gfx_Sprite_NoClip(kong_crazy_eye[(frameCounter % 3) - 1], 155, 193);
		}
		gfx_TransparentSprite_NoClip(kong_knockedout_sprite[frameCounter & 1], 152, 195);
	}

	gfx_SwapDraw();
	gfx_Sprite((gfx_image_t*)kong.background_data, kong.x_old, kong.y_old - 32);

	kong.background_data[0] = *(uint8_t*)kong_sprite[kong.sprite];
	kong.background_data[1] = *(uint8_t*)((uint8_t*)kong_sprite[kong.sprite] + 1);
	kong.x_old = x;
	kong.y_old = y;
}

void kong_climb_ladder(void) {
	kong.y -= 4;
	render_kong();
	kong.sprite ^= 1;
	waitTicks(8);
}

void draw_heart(gfx_image_t* sprite, uint8_t x, uint8_t y) {
	gfx_Sprite_NoClip(sprite, x, y);
	gfx_BlitRectangle(gfx_buffer, x, y, 16, 13);	// Maybe use bool isbroken instead?
}

/* play end cinematic for all four stages */
void end_stage_cinematic(void) {
	uint8_t i;

	gfx_Blit(gfx_screen);
	gfx_Sprite_NoClip((gfx_image_t*)kong.background_data, kong.x_old, kong.y_old);
	kong.y_old = kong.y += 32;

	if (game.stage == STAGE_RIVETS) {			// Stage Rivets
		uint8_t x, y;
		pauline.sprite = 1;
		draw_pauline(false);
		kong.sprite = 0;
		render_kong();

		waitTicks(0x20);

		// Clear space
		gfx_FillRectangle_NoClip(112, 72, 96, 160);
		for (y = 200; y <= 224; y += 8)
			for (x = 112; x <= 200; x += 8)
				gfx_Sprite_NoClip(girder_circle, x, y);
		gfx_TransparentSprite_NoClip(jumpman_sprite[jumpman.dir][jumpman.sprite], jumpman.x - 7, jumpman.y - 15);
		gfx_BlitRectangle(gfx_buffer, 112, 72, 96, 160);

		// Kong beating chest
		kong.sprite = 4;
		for (i = 0; i < 15; i++) {
			waitTicks(8);
			kong.sprite ^= 1;
			render_kong();
		}
		waitTicks(8);

		kong.sprite = 0;
		render_kong();
		waitTicks(32);

		// Kong is falling
		kong.sprite = 10;
		kong.y += 32; // = might be smaller

		while (kong.y < 200) {
			render_kong();
			
			// If kong is one pixel above the ground, move girder underneath pauline down
			if (kong.y == 199) {	// maybe 198?
				gfx_FillRectangle_NoClip(104, 32, 112, 8);
				gfx_FillRectangle_NoClip(119, 40, 82, 32);
				for (x = 104; x <= 208; x += 8)
					gfx_Sprite_NoClip(girder_circle, x, 72);
				gfx_BlitRectangle(gfx_buffer, 104, 32, 112, 48);
			}

			kong.y++;
			waitTicks(1);
		}

		// move pauline down
		gfx_FillRectangle_NoClip(pauline.x, pauline.y, 16, 22);
		gfx_BlitRectangle(gfx_buffer, pauline.x, pauline.y, 16, 22);
		pauline.y = 50;
		pauline.dir = 0;
		draw_pauline(false);

		// Roll kongs eyes
		kong.sprite = 11;
		for (frameCounter = 0; frameCounter < 32; frameCounter++) {
			render_kong();
			
			if (frameCounter == 4) {
				gfx_Sprite_NoClip((gfx_image_t*)jumpman.buffer_data, jumpman.x_old - 7, jumpman.y_old - 15);
				gfx_BlitRectangle(gfx_buffer, jumpman.x_old - 7, jumpman.y_old - 15, 15, 16);
				gfx_TransparentSprite_NoClip(jumpman_right_walking0, 120, 56);
				gfx_BlitRectangle(gfx_buffer, 120, 56, 14, 16);
			}
			if (frameCounter == 8) {
				draw_heart(heart, 136, 42);
			}

			waitTicks(8);
		}

		game_data.level++;
		i = 0xE0;
	}
	else {										// Stage Barrels, Elevators or Conveyors
		// Step 1 of 6: update kong and draw heart(1 of 5 for conveyors)
		draw_pauline(false);
		draw_heart(heart, 154, 10);
		kong.sprite = 0;
		render_kong();

		if (game.stage == STAGE_CONVEYORS) {	// Stage Conveyors
			// Step 2 of 5: reverse conveyor if needed and move kong to ladder
			conveyor[Top].reverseCounter = 0;
			if (kong.x > 104 && conveyor[Top].direction < 127 || kong.x < 104 && conveyor[Top].direction < 127)
				conveyor[Top].reverseCounter = 1;

			while (kong.x != 104) {
				render_kong();
				handle_conveyor_dirs();
				kong.x += conveyorVector_top;
				frameCounter--;
			}
		}
		else {									// Stage Barrels or Elevators
			waitTicks(0x20);
			// Step 2 of 6: update kong to facing right
			kong.sprite = 3;
			render_kong();
			waitTicks(0x20);
			
			// Step 3 of 6: prepare kong for climbing
			kong.x = 104;
		}

		kong.sprite = 6;
		
		// Step 4 of 6: climb ladder without pauline(same as step 3 from conveyors)
		while (kong.y > 48) {
			kong_climb_ladder();
		}

		// step 5 of 6: climb ladder with pauline(same as step 4 from conveyors)
		kong.sprite = 8;
		draw_heart(heart_broken, 153, 10);
		gfx_FillRectangle_NoClip(pauline.x, pauline.y, 16, 22);
		gfx_BlitRectangle(gfx_buffer, pauline.x, pauline.y, 16, 22);

		while (kong.y >= 16) {
			kong_climb_ladder();
		}

		// step 6 of 6: determine next level and add bonus to score(same as step 5 from conveyors)
		i = 0x60;
	}

	game_data.round++;
	if (game_data.round > 19)
		game_data.round = 14;

	// Add bonusTimer value to score
	game_data.score += game.bonusTimer * 100;
	draw_player_score();
	waitTicks(i);
}

/* Removes a piece of ladder underneath kong */
void remove_ladder(uint8_t y) {
	gfx_TempSprite(background_data, 8, 8);
	gfx_GetSprite(background_data, 152, y);
	gfx_Sprite_NoClip(background_data, 160, y);
	gfx_Sprite_NoClip(background_data, 176, y);
	gfx_BlitRectangle(gfx_buffer, 160, y, 24, 8);
}

/* Handles kong climbing ladder */
void climb_ladder(void) {
	kong.climbCounter++;
	if ((kong.climbCounter & 7) == 0) {
		kong.y -= 4;
		kong.sprite ^= 1;
		render_kong();
	}
}

static uint8_t cinematicProgress = 0;

/* Play the intro cinematic */
void intro_cinematic(void) {
	switch (cinematicProgress) {
	case 0:												// Initialize some variales
		draw_overlay_full();
		draw_stage(&stage_barrels_intro_data);
		gfx_Blit(gfx_buffer);

		kong.climbCounter = 0;
		kong.jumpCounter = 0;

		waitTimer = 0x40;
		cinematicProgress++;
		break;
	case 1:												// Spawn kong
		handle_waitTimer();
		kong.sprite = 8;
		kong.x_old = kong.x = 152;
		kong.y_old = kong.y = 221;
		kong.background_data[0] = 40;
		kong.background_data[1] = 32;
		gfx_GetSprite((gfx_image_t*)kong.background_data, kong.x, kong.y);
		render_kong();
		cinematicProgress++;
		break;
	case 2:												// Kong climbing ladder with pauline
		climb_ladder();
		if ((kong.climbCounter & 15) == 0) {
			remove_ladder(kong.y + 15);
		}

		if (kong.y <= 97) {
			waitTimer = 0x20;
			cinematicProgress++;
		}
		break;
	case 4:												// Kong jumping up to girder
		if ((frameCounter & 1) == 0) {
			uint8_t i;

			// Kong is jumping up
			if (kong.jumpCounter < sizeof(kong_jumpup_table)) {
				kong.y += kong_jumpup_table[kong.jumpCounter];
				render_kong();
				kong.jumpCounter++;
				break;
			}

			kong.y += 5;
			kong.sprite = 0;
			render_kong();

			// Kong landed on girder; draw pauline, remove ladder and angle top girder
			pauline.x = 136;
			pauline.y = 18;
			pauline.dir = FACE_RIGHT;
			pauline.sprite = 0;
			draw_pauline(false);
			kong.jumpCounter = 0;
			kong.bounceCounter = 0;
			for (i = 76; i < 120; i += 8)
				remove_ladder(i);
			draw_stage(&stage_barrels_slanted_top);
			gfx_BlitLines(gfx_buffer, 68, 12);

			waitTimer = 0x20;
			cinematicProgress++;
		}
		break;
	case 6:												// Kong bouncing to left
		if ((frameCounter & 1) == 0) {
			if (kong.jumpCounter < sizeof(kong_jumpleft_table)) {
				kong.y += kong_jumpleft_table[kong.jumpCounter];
				kong.x--;
				render_kong();
				kong.jumpCounter++;
				break;
			}

			draw_stage(&stage_barrels_slanted[kong.bounceCounter * 11]);
			gfx_BlitLines(gfx_buffer, 93 + kong.bounceCounter * 33, 20 - 5 * (kong.bounceCounter == 4));	// This works, but there might be a better way

			kong.jumpCounter = 0;
			kong.bounceCounter++;
			if (kong.bounceCounter < 5)
				break;

			waitTimer = 0xB0;
			cinematicProgress++;
		}
		break;
	case 7:												// Kong standing and showing teeth
		if (waitTimer == 0x90) {		// Kong showing teeth
			kong.sprite = 12;
			render_kong();
		}
		else if (waitTimer == 0x18) {	// Kong's normal face
			kong.sprite = 0;
			render_kong();
		}

		handle_waitTimer();
		cinematicProgress = 0;
		game_state = pre_round_screen;
		break;
	default:
		handle_waitTimer();
		cinematicProgress++;
		break;
	}
}


// Lookup tables
uint8_t kong_jumpup_table[22] = { 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x01, 0x01, 0x01 };
uint8_t kong_jumpleft_table[16] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01 };
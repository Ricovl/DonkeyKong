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
#include <lib/ce/graphx.h>
#include <lib/ce/keypadc.h>

// kong stuff
#include "defines.h"
#include "kong.h"
#include "conveyors.h"
#include "barrels.h"
#include "images.h"


kong_t kong;

// before this way of drawing: 54255 bytes; after: 53956
/* Draw kong to the screen and the buffer(does not work if y position or height has changed) */
void draw_kong(void) {
	uint8_t width_old = kong.background_data[0];
	uint8_t width;
	uint24_t blitx, x = kong.x;

	if (kong.sprite) {
		if		(kong.sprite == 1)	x -= 4;
		else if (kong.sprite == 3)	x++;
		else if (kong.sprite == 4)	x -= 5;
		else if (kong.sprite == 5)	x += 5;
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
	}
	else {
		blitx = x;
		width = (kong.x_old + width_old) - blitx;
	}
	gfx_BlitRectangle(gfx_buffer, blitx, kong.y, width, 32);

	kong.x_old = x;
	kong.y_old = kong.y;
}

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


pauline_t pauline;

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




void waitTicks(uint8_t ticks) {
	while (--ticks) {
		while (!(timer_IntStatus & TIMER1_RELOADED));	// Wait until the timer has reloaded
		timer_IntStatus = TIMER1_RELOADED;				// Acknowledge the reload
	}
}

/* Draws kong for use in cinematics */
void render_kong(void) {
	uint24_t x = kong.x;
	if (kong.sprite) {
		if (kong.sprite == 1)	x -= 4;
		else if (kong.sprite == 3 || kong.sprite == 7 || kong.sprite == 9)	x++;
		else if (kong.sprite == 4)	x -= 5;
		else if (kong.sprite == 5)	x += 5;
		else if (kong.sprite == 6 || kong.sprite == 8)	x += 6;
	}

	gfx_GetSprite((gfx_image_t*)kong.background_data, kong.x_old, kong.y_old - 32);
	gfx_TransparentSprite(kong_sprite[kong.sprite], x, kong.y - 32);
	gfx_SwapDraw();
	gfx_Sprite((gfx_image_t*)kong.background_data, kong.x_old, kong.y_old - 32);

	kong.background_data[0] = *(uint8_t*)kong_sprite[kong.sprite];
	kong.background_data[1] = *(uint8_t*)((uint8_t*)kong_sprite[kong.sprite] + 1);
	kong.x_old = x;
	kong.y_old = kong.y;
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

void end_stage_cinematic(void) {
	gfx_Blit(gfx_screen);
	gfx_Sprite_NoClip((gfx_image_t*)kong.background_data, kong.x_old, kong.y_old);
	kong.y_old = kong.y += 32;

	if (game.stage == STAGE_RIVETS) {			// Stage Rivets
		waitTicks(8);
	}
	else {										// Stage Barrels, Elevators or Conveyors
		// Step 1 of 6: update kong and draw heart(1 of 5 for conveyors)
		draw_pauline(false);
		draw_heart(heart, 154, 16);
		kong.sprite = 0;
		render_kong();

		if (game.stage == STAGE_CONVEYORS) {	// Stage Conveyors
			// Step 2 of 5:
			conveyor[Top].reverseCounter = 0;
			if (kong.x > 104 && conveyor[Top].direction < 127 || kong.x < 104 < conveyor[Top].direction > 127)
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
		draw_heart(heart_broken, 153, 16);
		gfx_FillRectangle_NoClip(pauline.x, pauline.y, 16, 22);
		gfx_BlitRectangle(gfx_buffer, pauline.x, pauline.y, 16, 22);

		while (kong.y >= 16) {
			kong_climb_ladder();
		}
		// step 6 of 6: determine next level and add bonus to score(same as step 5 from conveyors)
		waitTicks(0x40);
	}
}
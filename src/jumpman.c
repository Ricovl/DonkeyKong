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
#include "barrels.h"
#include "firefoxes.h"
#include "bonus_scores.h"
#include "overlay.h"
#include "images.h"
#include "stages.h"
#include "elevators.h"
#include "drawsprites.h"


/* Initialize all the variables for the jumpman */
void init_jumpman(uint24_t x, uint8_t y) {
	memset(&jumpman, 0, sizeof(jumpman_t));

	jumpman.x_old = jumpman.x = x;
	jumpman.y_old = jumpman.y = y;

	jumpman.dir = FACE_RIGHT;
	jumpman.moveIndicator = 2;

	jumpman.isAlive = true;
	jumpman.buffer_data[0] = 15;
	jumpman.buffer_data[1] = 16;
}


void handle_jumping(jumpman_t *jumpman);
void handle_bouncing(jumpman_t *jumpman);

void move_jumpman(void) {
	kb_key_t key;
	kb_Scan();
	key = kb_Data[kb_group_7];

	// Jumpman is jumping
	if (jumpman.isJumping) {
		unsigned int testX = jumpman.x;
		uint8_t testY = jumpman.y;
		handle_jumping(&jumpman);

		// Check if jumpman hits the border of the screen or the area where kong is in elevators or barrels; if so bounce him the other way
		if (jumpman.jumpDirIndicator && ((jumpman.x >= 267 || jumpman.x <= 52) || ((game.stage & 1) && (jumpman.y < 73 && jumpman.x <= 142)))) {

			if (jumpman.x < 150) {	// Jumpman is on right side(jumping to left)
				jumpman.jumpDir = 0;
				jumpman.dir = FACE_RIGHT;
			}
			else {					// Jumpan is on left side(jumping to right)
				jumpman.jumpDir = 0xFF;
				jumpman.dir = FACE_LEFT;
			}
			jumpman.jumpDirIndicator = 0x80;	// don't need this here, right?

			// This makes jumpman bounce up when jumped from a slanted girder to the wall, so he can make his way up the girder when he is not falling too far
			if (!jumpman.fallingTooFar) {
				handle_bouncing(&jumpman);
				asm("push ix");
				asm("ld ix, _jumpman");
				asm("ld (ix+19),h");
				asm("ld (ix+18),l");
				asm("pop ix");

				jumpman.jumpCounter = 0;
			}

			handle_jumping(&jumpman);
		}
		
		if (game.stage == STAGE_ELEVATORS && collision_elevator())
			goto jumpman_collided;


		// Player's new y position is colliding with the ground; so move him up till there is no collision
		if (((gfx_GetPixel(jumpman.x + 3 + jumpman.dir, jumpman.y) <= COLOR_COLLISION || gfx_GetPixel(jumpman.x - 4 + jumpman.dir, jumpman.y) <= COLOR_COLLISION) && !(game.stage == STAGE_BARRELS && jumpman.y <= jumpman.originalY - 13))) {	// should get changed
			uint8_t test = 0;

			// Jumpman collision with side of girder detection test(doesn't work exactly like the original)
			if (testX - jumpman.x != 0) {	// jumpman is in girder and is moving horizontal
				dbg_sprintf(dbgout, "jumpman might be colliding with the side of a girder!\n");							// maybe don't stop jumpman if he is going up and y%7 is 0 or 1(dy > 1? && Y%7 < 1?)?
				dbg_sprintf(dbgout, "dx: %d, dy: %d, y&7: %d\n", testX - jumpman.x, testY - jumpman.y, jumpman.y & 7);	// Print some numbers that might be usefull to determine if he is colliding with side
				// not if jumpman.y & 7 == 0 and jumpman is going up?
				if ((jumpman.y & 7) != 0) {	// || ((jumpman.y & 7) == 1 && testY - jumpman.y == -2)
					if(!((jumpman.y & 7) == (testY - jumpman.y - 1)))	// I don't think this does anything
						dbg_sprintf(dbgout, "collided with side?\n");	// works better I think
				}
			}

			for (; gfx_GetPixel(jumpman.x + 3 + jumpman.dir, jumpman.y) <= COLOR_COLLISION || gfx_GetPixel(jumpman.x - 4 + jumpman.dir, jumpman.y) <= COLOR_COLLISION; jumpman.y--) { test++; }
			//dbg_sprintf(dbgout, "%d\n", test);

			// Other jumpman collision with side of girder detection test(doesn't work exactly like the original)
			if (gfx_GetPixel(jumpman.x - ((jumpman.dir - 1) | 1), jumpman.y + test) > COLOR_COLLISION && test > 1) {
				//dbg_sprintf(dbgout, "jumped in side\n");
				jumpman.y += test;
				jumpman.jumpDir =
					jumpman.jumpDirIndicator = 0;
				if (!jumpman.comingDown) {
					jumpman.comingDown = true;
					jumpman.movingUp =
						jumpman.velocityY =
						jumpman.jumpCounter = 0;
				}
				return;
			}

		jumpman_collided:

			jumpman.isAlive = jumpman.fallingTooFar ^ 1;

			jumpman.comingDown = 6;
			jumpman.isJumping = false;
		}
		else if (!jumpman.comingDown) {
			// Check if jumpman is at apex of jump
			if (jumpman.jumpCounter == 0x14) {
				jumpman.comingDown = true;

				check_jump_over();
				hammerActive = check_collision(num_hammers, &hammer[0].y, 4, 6, 4, 6, sizeof(hammer_t));
			}
			jumpman.sprite = 3;
		}
		else {
			if (jumpman.y - 14 >= jumpman.originalY) {
				jumpman.fallingTooFar = true;
			}
		}

		return;
	}

	// Jumpman is standing still on the ground after a jump.
	if (jumpman.comingDown) {
		jumpman.comingDown--;
		// Check if comingDown is 0, if so end jump
		if (jumpman.comingDown == 0) {
			jumpman.sprite = 0;

			if (hammerActive) {
				hammer[hammerActive - 1].active = true;
				hammer[hammerActive - 1].background_data[0] = 17;
			}
		}
		else
			jumpman.sprite = 4;

		return;
	}

	if (!hammerActive) {
		// Jumpman is climbing a ladder
		if (jumpman.onLadder) {
			climb_ladder(key);
			return;
		}

		// Player is pressing jump key, jumpman is about to jump
		if (kb_Data[kb_group_1] & kb_2nd) {
			// Initialize all the variables needed to make jumpman jump
			jumpman.isJumping = true;

			jumpman.jumpDir = 0;
			jumpman.jumpDirIndicator = 0x80;
			if (key & kb_Left) {
				jumpman.jumpDir = 0xFF;
			}
			else if (!(key & kb_Right)) {
				jumpman.jumpDirIndicator = 0;
			}

			jumpman.movingUp = true;
			jumpman.velocityY = 0x48;
			jumpman.jumpCounter =
				jumpman.jumpCounterX =
				jumpman.jumpCounterY =
				jumpman.comingDown =
				jumpman.fallingTooFar = 0;
			jumpman.originalY = jumpman.y;

			return;
		}
	}

	// If right or left key is pressed, move mario in the given direction
	if ((key & kb_Right || key & kb_Left) && !(key & kb_Right && key & kb_Left)) {

		// moveIndicator counts from 2 to zero, so jumpman's position gets updated every 2 out of 3 ticks
		if (jumpman.moveIndicator) {

			if (key & kb_Right) {		// Move player to the right if not at edge
				if (jumpman.x < 266) {
					jumpman.dir = FACE_RIGHT;
					jumpman.x++;
				}
				else return;
			} 
			else {						// Move player to the Left if not at edge or kong on stage barrels or elevators
				if (jumpman.x > 54 && !((game.stage & 1) && (jumpman.y < 73 && jumpman.x <= 142))) {
					jumpman.dir = FACE_LEFT;
					jumpman.x--;
				}
				else return;
			}

			// If stage is barrels, then check if the girder if going up or down
			if (game.stage == STAGE_BARRELS) {
				if ((jumpman.x & 15) == (jumpman.dir ^ 1) * 15) {
					if (gfx_GetPixel(jumpman.x, jumpman.y) <= COLOR_COLLISION)
						jumpman.y--;
					else if (gfx_GetPixel(jumpman.x, jumpman.y + 1) > COLOR_COLLISION)
						jumpman.y++;
				}
			}

			jumpman.moveIndicator--;
		}
		else {
			if (++jumpman.moveCounter > 3)
				jumpman.moveCounter = 0;
			jumpman.sprite = jumpman_walking_sprite_table[jumpman.moveCounter];

			jumpman.moveIndicator = 2;	// reset moveIndicator back to 2
		}

	}

	if (!hammerActive && ladder_in_range()) {
		climb_ladder(key);
	}
}


/* Handles jumpman climbing a ladder */
void climb_ladder(kb_key_t key) {
	if ((key & kb_Up || key & kb_Down) && !(key & kb_Up && key & kb_Down)) {
		if (!jumpman.moveIndicator) {

			if (key & kb_Down) {	// Player going down on ladder
				jumpman.moveIndicator = 3;
				jumpman.y += 2;
			}
			else {					// Player going up on ladder
				jumpman.moveIndicator = 4;	// Slower movement going up
				jumpman.y -= 2;
			}

			jumpman.ladderToggle ^= 1;
			if (!jumpman.ladderToggle) {
				uint8_t climbingHeight = jumpman.y - jumpman.ladderTop;

				// Check if player is at the top or bottem of ladder.
				if (jumpman.y == jumpman.ladderBottom || jumpman.y == jumpman.ladderTop) {
					jumpman.sprite = 8;
					jumpman.onLadder = false;
					return;
				}

				if (climbingHeight == 12) {
					jumpman.sprite = 6;
				}
				else if (climbingHeight == 8) {
					jumpman.sprite = 7;
				}
				else {
					jumpman.sprite = 5;
				}

			}
			else {
				jumpman.dir ^= 1;
			}

			jumpman.onLadder = true;
		}
		else {
			if (jumpman.brokenLadder) {
				if (jumpman.ladderBottom - 0x0C >= jumpman.y && key & kb_Up) {
					return;
				}
			}
			jumpman.moveIndicator--;
		}
	}
}

/* Check jumpman is in range of a ladder and make him ready to climb if there is one */
bool ladder_in_range(void) {
	if (kb_Data[kb_group_7] & kb_Up || kb_Data[kb_group_7] & kb_Down) {
		uint8_t *array = stage_data[game.stage - 1];
		
		while (*array < 2) {
			if (jumpman.x > *(array + 1) + 47 && jumpman.x < (*(array + 1) + 56)) {
				if ((kb_Data[kb_group_7] & kb_Up   && jumpman.y + 1 == *(array + 4)) || 
					(kb_Data[kb_group_7] & kb_Down && jumpman.y + 1 == *(array + 2) && !(*array))) {
					// There is a ladder nearby; initialize all variables for ladder movement
					jumpman.sprite = 8;
					jumpman.x = *(array + 1) + 51;
					jumpman.brokenLadder = *array;
					jumpman.ladderTop = *(array + 2) - 1;
					jumpman.ladderBottom = *(array + 4) - 1;

					return true;
				}
			}
			array += 5;
		}
	}

	return false;
}

/* Checks for jumps over items on girders */
void check_jump_over(void) {
	uint8_t i, numObstaclesJumped = 0;

	for (i = 0; i < num_barrels; i++) {
		barrel_t *this_barrel = &barrel[i];

		// Check if barrel is in between 17 pixels under jumpman
		if (this_barrel->y > jumpman.y && jumpman.y + 17 > this_barrel->y) {
			if (abs(jumpman.x - this_barrel->x) < 13)	// 13 is the width of a barrel
				numObstaclesJumped++;
		}
	}

	if (numObstaclesJumped) {
		// 0000		0010	 0100
		// 0 = 100, 2 = 300, 4 = 800
		// 0000		0001	 0010
		// 0001		0010	 0011
		numObstaclesJumped = (numObstaclesJumped - 1) << 1;
		spawn_bonus_score(numObstaclesJumped, jumpman.x - 6, jumpman.y + 9);
	}
}

const uint8_t jumpman_walking_sprite_table[] = { 0, 2, 0, 1 };


/* checks for collision */
uint8_t check_collision(uint8_t loop, uint8_t *structp, uint8_t width, uint8_t height, uint8_t offsetx, uint8_t offsety, uint8_t size) {
	int8_t distance;
	uint8_t i;

	for (i = 0; i < loop; i++) {
		// Check if y position is in range
		if (abs((jumpman.y - 5) - (*structp + offsety)) > height)
			goto check_next;

		// Check if x position is in range
		if (abs(jumpman.x - (*(uint24_t*)(structp + 2) + offsetx)) <= width)
			return i + 1;

	check_next:
		structp += size;
	}

	return false;
}


void jumpman_falling(void) {
	if (!jumpman.onLadder && !jumpman.isJumping && !jumpman.onElevator) {
		if (((jumpman.x & 7) == (4 - jumpman.dir)) && gfx_GetPixel(jumpman.x, jumpman.y + 2) == COLOR_BACKGROUND) {
		//if (gfx_GetPixel(jumpman.x - (((jumpman.dir - 1) ^ 2) | 1), jumpman.y + 2) == COLOR_BACKGROUND) {
			// jumpman is falling
			memset(&jumpman.jumpDir, 0, 7);

			jumpman.isJumping =
				jumpman.comingDown = true;

			jumpman.originalY = jumpman.y;
		}

	}

}


const uint8_t bonus_item_width[] = {7, 4, 6};
const uint8_t bonus_item_height[] = { 15, 9, 8 };

void bonus_item_picked_up(void) {
	uint8_t i;

	for (i = 0; i < num_bonus_items; i++) {
		bonus_item_t *this_bonus_item = &bonus_item[i];

		if (jumpman.x == this_bonus_item->x + bonus_item_width[this_bonus_item->type]) {
			if (jumpman.y == this_bonus_item->y + bonus_item_height[this_bonus_item->type]) {
				uint8_t score;
				gfx_Sprite_NoClip((gfx_image_t*)this_bonus_item->background_data, this_bonus_item->x, this_bonus_item->y);
				gfx_BlitRectangle(gfx_buffer, this_bonus_item->x, this_bonus_item->y, 16, bonus_item_height[this_bonus_item->type]);

				score = game.level + 1;
				if (score > 4)
					score = 4;

				spawn_bonus_score(score, jumpman.x - 6, jumpman.y - 11);
				bonus_item[i] = bonus_item[--num_bonus_items];
			}
			return;
		}
	}
}


/* Animate jumpman dead */
void animate_jumpman_dead(void) {
	uint8_t i, delay;

	gfx_SetDrawScreen();
	jumpman.dir ^= 1;

	for (i = 13 + 2; i > 0; i--) {
		gfx_BlitRectangle(gfx_buffer, jumpman.x - 7, jumpman.y - 15, 16, 16);
		gfx_TransparentSprite_NoClip(jumpman_dead[jumpman.sprite], jumpman.x - 7, jumpman.y - 15);

		for (delay = 0; delay < 12; delay++) {
			while (!(timer_IntStatus & TIMER1_RELOADED));	// Wait until the timer has reloaded
			timer_IntStatus = TIMER1_RELOADED;				// Acknowledge the reload
		}

		jumpman.sprite = (jumpman.sprite + ((jumpman.dir - 1) | 1)) & 3;

		if (i <= 3)			// laying on ground
			jumpman.sprite = jumpman.dir + 4;
	}

	if (game.lives)
		game.lives--;

	gfx_SetDrawBuffer();
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Hammer Stuff
////////////////////////////////////////////////////////////////////////////////////////////////

const uint16_t firefox_hammer_palette[] = { gfx_RGBTo1555(255, 0, 0), gfx_RGBTo1555(0, 255, 255), gfx_RGBTo1555(155, 155, 255) };

// if hammer active width = 17 height = 10
void hammer_stuff(void) {
	if (game.stage != STAGE_ELEVATORS && hammerActive) {
		hammer_t *this_hammer = &hammer[hammerActive - 1];

		if (this_hammer->active) {
			this_hammer->dir = jumpman.dir;

			if (jumpman.sprite < 4)
				jumpman.sprite += 9;

			if (hammerTimer == 0) {
				gfx_SetPalette(firefox_hammer_palette, 6, 6);
			}

			hammerTimer++;
			if (!(hammerTimer & 7)) {
				//this_hammer->inFront ^= 1;
				this_hammer->sprite ^= 1;

				if (hammerTimer == 0) {
					hammerLength++;
					if (hammerLength == 2) {
						hammerLength = 0;
						hammerTimer = 0;
						this_hammer->active = false;

						// Remove hammer sprite from screen
						gfx_SetDrawScreen();
						gfx_Sprite_NoClip((gfx_image_t*)this_hammer->background_data, this_hammer->x_old, this_hammer->y_old);
						gfx_SetDrawBuffer();
						
						// Delete the hammer's struct
						if (hammerActive == 1) {
							hammer[0] = hammer[1];
						}
						hammerActive = false;
						num_hammers--;

						// Restore the firefoxes' palette
						gfx_SetPalette(sprites_gfx_pal + 6, 6, 6);
						// Restore jumpmans sprite(Could maybe just set the sprite to 0)
						jumpman.sprite = 0;
						return;
					}
				}

				if (jumpman.sprite >= 12)
					jumpman.sprite -= 3;
				else
					jumpman.sprite += 3;

			}
						
		}

		// Move the hammer sprite to an offset from jumpman
		if ((this_hammer->sprite & 1) == 0) {
			this_hammer->x = jumpman.x - 3;
			this_hammer->y = jumpman.y - 25;
		}
		else {
			if (jumpman.dir == FACE_LEFT)
				this_hammer->x = jumpman.x - 24;
			else
				this_hammer->x = jumpman.x + 8;
			this_hammer->y = jumpman.y - 9;
		}

	}
}

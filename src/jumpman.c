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
#include "jumpman.h"
#include "barrels.h"
#include "bonus_scores.h"
#include "defines.h"
#include "drawsprites.h"
#include "elevators.h"
#include "firefoxes.h"
#include "hammers.h"
#include "images.h"
#include "kong.h"
#include "overlay.h"
#include "screens.h"
#include "stages.h"


/* Initialize all the variables for the jumpman */
void init_jumpman(uint24_t x, uint8_t y) {
	memset(&jumpman, 0, sizeof(jumpman_t));

	jumpman.x_old = jumpman.x = x;
	jumpman.y_old = jumpman.y = y;

	jumpman.dir = FACE_RIGHT;
	jumpman.moveIndicator = 1;

	jumpman.isAlive = true;
	jumpman.buffer_data[0] = 16;
	jumpman.buffer_data[1] = 16;
}


jumpman_t jumpman;
void handle_jumping(jumpman_t *jumpman);
void handle_bouncing(jumpman_t *jumpman);
uint8_t check_jump_over_item(void);

void move_jumpman(void) {
	kb_key_t key;
	kb_Scan();
	key = kb_Data[7];

	// Jumpman is jumping
	if (jumpman.isJumping) {
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

		if (girder_collision() || (game.stage == STAGE_ELEVATORS && collision_elevator())) {
			jumpman.isAlive = jumpman.fallingTooFar ^ 1;

			jumpman.sprite = 14;
			jumpman.comingDown = 4;
			jumpman.isJumping = false;
		}
		else if (!jumpman.comingDown) {
			// Check if jumpman is at apex of jump
			if (jumpman.jumpCounter == 0x14) {
				uint8_t NumObstaclesJumped, loop;
				
				jumpman.comingDown = true;
				NumObstaclesJumped = check_jump_over_item();

				if (NumObstaclesJumped) {
					spawn_bonus_score(NumObstaclesJumped - 1, jumpman.x - 6, jumpman.y + 9);
				}
				
				loop = num_hammers;
				while (loop--) {
					hammer_t *this_hammer = &hammer[loop];

					// Check if y position is in range
					if (abs((jumpman.y_old - 1) - this_hammer->y) <= 7) {
						// Check if x position is in range
						if (abs(jumpman.x_old - this_hammer->x) <= 7)
							hammerActive = loop + 1;
					}
				}
			}
			jumpman.sprite = 13;
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
				hammer[hammerActive - 1].background_data[0] = 16;
				hammerTimer = 0;
				hammerLength = 0;
			}
		}

		return;
	}

	if (!hammerActive) {
		// Jumpman is climbing a ladder
		if (jumpman.onLadder) {
			climb_ladder();
			return;
		}

		// Player is pressing jump key, jumpman is about to jump
		if (kb_Data[1] & kb_2nd) {
			// Initialize all the variables needed to make jumpman jump
			jumpman.isJumping = true;

			jumpman.jumpDir = 0;
			jumpman.jumpDirIndicator = 0x80;
			if (key & kb_Left)
				jumpman.jumpDir = 0xFF;
			else if (!(key & kb_Right))
				jumpman.jumpDirIndicator = 0;

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
			if (game.stage == STAGE_BARRELS && jumpman.x < 257 && jumpman.x > 62) {
				uint8_t pixel = gfx_GetPixel(jumpman.x, jumpman.y + 1);

				if (pixel == COLOR_COLLISION)
					jumpman.y--;
				else if (pixel > COLOR_COLLISION)
					jumpman.y++;
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
		climb_ladder();
	}
}

/* Handle if jumpmans next position is in a girder */
bool girder_collision(void) {
	uint24_t x = jumpman.x;

	if (game.stage == STAGE_BARRELS) {
		if (gfx_GetPixel(x, jumpman.y) > COLOR_COLLISION || jumpman.y <= jumpman.originalY - 13)
			return false;
	}
	else {
		x -= 3;
		if (gfx_GetPixel(x, jumpman.y) > COLOR_COLLISION) {
			x += 7;
			if (gfx_GetPixel(x, jumpman.y) > COLOR_COLLISION)
				return false;
		}

		// check if jumping in edge of girder
		if (gfx_GetPixel(x + (jumpman.x_old - jumpman.x), jumpman.y) == COLOR_BACKGROUND && (jumpman.y & 7) != 0) {
			if (jumpman.jumpDir == 0xFF)
				jumpman.x = (jumpman.x | 7) - 4;
			else
				jumpman.x = ((jumpman.x - 8) | 7) + 4;
			jumpman.startFalling = true;
			return false;
		}
	}

	while (gfx_GetPixel(x, jumpman.y) <= COLOR_COLLISION)
		jumpman.y--;
	return true;
}

/* Handles jumpman climbing a ladder */
void climb_ladder(void) {
	sk_key_t key = kb_Data[7];

	if ((key & kb_Up || key & kb_Down) && !(key & kb_Up && key & kb_Down)) {
		if (jumpman.moveIndicator == 0) {

			if (key & kb_Down) {	// Player going down on ladder
				jumpman.moveIndicator = 3;
				jumpman.y += 2;
			}
			else {					// Player going up on ladder
				jumpman.moveIndicator = 4;	// Slower movement going up
				jumpman.y -= 2;
			}

			jumpman.ladderToggle ^= 1;
			if (jumpman.ladderToggle == 0) {
				uint8_t climbingHeight = jumpman.y - jumpman.ladderTop;

				// Check if player is at the top or bottem of ladder.
				if (jumpman.y == jumpman.ladderBottom || jumpman.y == jumpman.ladderTop) {
					jumpman.sprite = 6;
					jumpman.onLadder = false;
					return;
				}

				if (climbingHeight == 12) {
					jumpman.sprite = 4;
				}
				else if (climbingHeight == 8) {
					jumpman.sprite = 5;
				}
				else {
					jumpman.sprite = 3;
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
	if (kb_Data[7] & kb_Up || kb_Data[7] & kb_Down) {
		uint8_t *array = stage_data[game.stage - 1];

		while (*array < 2) {
			if (jumpman.x > *(array + 1) + 47 && jumpman.x < (*(array + 1) + 56)) {
				if ((kb_Data[7] & kb_Up   && jumpman.y + 1 == *(array + 4)) ||
					(kb_Data[7] & kb_Down && jumpman.y + 1 == *(array + 2) && !(*array))) {
					// There is a ladder nearby; initialize all variables for ladder movement
					jumpman.sprite = 6;
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


const uint8_t jumpman_walking_sprite_table[] = { 0, 2, 0, 1 };


/* Make jumpman fall when there is nothing under him */
void check_jumpman_falling(void) {
	if (!jumpman.onLadder && !jumpman.isJumping && !jumpman.onElevator) {
		if (gfx_GetPixel(jumpman.x + 4, jumpman.y + 4) == COLOR_BACKGROUND) {
			if(gfx_GetPixel(jumpman.x - 3, jumpman.y + 4) == COLOR_BACKGROUND)
				jumpman.startFalling = true;
		}
	}
}

/* Initialize variables to make jumpman fall */
void handle_jumpman_falling(void) {
	if (jumpman.startFalling) {
		jumpman.startFalling = false;
		memset(&jumpman.jumpDir, 0, 7);

		jumpman.isJumping = true;
		jumpman.comingDown = true;

		jumpman.originalY = jumpman.y;
	}
}


uint8_t bonus_item_width[] = {7, 4, 6};
uint8_t bonus_item_height[] = { 15, 9, 8 };

void bonus_item_picked_up(void) {
	uint8_t i;

	for (i = 0; i < num_bonus_items; i++) {
		bonus_item_t *this_bonus_item = &bonus_item[i];

		if (jumpman.x == this_bonus_item->x + bonus_item_width[this_bonus_item->type]) {
			if (jumpman.y == this_bonus_item->y + bonus_item_height[this_bonus_item->type]) {
				uint8_t score;
				gfx_Sprite_NoClip((gfx_sprite_t*)this_bonus_item->background_data, this_bonus_item->x, this_bonus_item->y);
				gfx_BlitRectangle(gfx_buffer, this_bonus_item->x, this_bonus_item->y, 16, bonus_item_height[this_bonus_item->type]);

				score = game_data.level + 1;
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
	switch (jumpman.dyingProgress) {
	case 0:
		handle_waitTimer1();
		
		// Initialize variables for rotating jumpman
		jumpman.dyingDir = jumpman.dir;
		jumpman.dir = 1;
		jumpman.sprite = 15;
		jumpman.dyingCounter = 13;
		jumpman.dyingProgress++;
		waitTimer = 8;

		// Clear all moving objects except jumpman and elevators
		disable_sprites();
		gfx_Blit(gfx_buffer);
		update_screen();
		break;
	case 1:
		handle_waitTimer1();
		waitTimer = 8;
		jumpman.dyingCounter--;

		if (jumpman.dyingCounter > 0) {
			jumpman.sprite ^= 31;
			if ((jumpman.dyingCounter & 1) != jumpman.dyingDir)
				jumpman.dir ^= 1;
			return;
		}

		jumpman.sprite = 17;
		jumpman.dir = jumpman.dyingDir;
		jumpman.dyingProgress++;
		waitTimer = 0x80;
		break;
	case 2:
		handle_waitTimer1();
		jumpman.enabled = false;
		game.stage = 0xFF;
		game_state = handle_dead;
		num_elevators = 0;
		break;
	}
}

void handle_dead(void) {
	game_data.lives--;

	game_data.score = game.score;

	if (game_data.lives == 0) {	// Game Over
		gfx_FillRectangle_NoClip(104, 144, 112, 40);
		gfx_SetTextFGColor(COLOR_LIGHT_BLUE);
		gfx_PrintStringXY("GAME%%OVER", 121, 160);
		gfx_Blit(gfx_buffer);

		waitTimer = 0xC0;
		game_state = pre_name_registration;
	}
	else {						// Has live(s) left
		game_state = pre_round_screen;
	}

	oilcan.onFire = false;
}

#if 0	//Old jump over object detection that is not used anymore
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
		numObstaclesJumped = (numObstaclesJumped - 1) << 1;
		spawn_bonus_score(numObstaclesJumped, jumpman.x - 6, jumpman.y + 9);
	}
}
#endif

#if 0	
void check_collision_jumpman(void) {
	if (game.stage == STAGE_BARRELS) {
		for (i = 0; i < num_barrels; i++) {	// check collision barrels
			barrel_t *this_barrel = &barrel[i];

			if (abs((jumpman.y_old - 3) - this_barrel->y_old) <= 2 + 6) {
				if (abs(jumpman.x_old - this_barrel->x_old) <= 2 + 4) {
					jumpman.isAlive = false;
				}
			}
		}
	}

	for (i = 0; i < num_firefoxes; i++) {	// check collision fireballs
		firefox_t *this_firefox = &firefox[i];

		if (abs(jumpman.y_old - this_firefox->y_old) <= 2 + 6) {		// 1 + 6 if firefox
			if (abs(jumpman.x_old - this_firefox->x_old) <= 3 + 4) {	// 4 + 4 if firefox
				jumpman.isAlive = false;
			}
		}
	}

	if (game.stage == STAGE_CONVEYORS) {
		for (i = 0; i < num_pies; i++) {		// check collision pies
			pie_t *this_pie = &pie[i];

			if (abs(jumpman.y_old - (this_pie->y_old + 11)) <= 3 + 6) {
				if (abs(jumpman.x_old - (this_pie->x_old + 7)) <= 8 + 4) {
					jumpman.isAlive = false;
				}
			}
		}
	}
}
#endif
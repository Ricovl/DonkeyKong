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
#include "firefoxes.h"
#include "conveyors.h"
#include "defines.h"
#include "jumpman.h"
#include "stages.h"


firefox_t firefox[MAX_FIREFOXES];
uint8_t num_firefoxes;

/* Updates the firefoxes */
void update_firefoxes(void) {
	// Check internal difficulty and timers and return based on difficulty a percentage of the time
	if (game.difficulty == 1) {
		if ((frameCounter & 1))			// return every 4/8 frames
			return;
	}
	else if (game.difficulty == 2) {
		if ((frameCounter & 7) >= 5)	// return every 3/8 frames
			return;
	}
	else if (game.difficulty <= 4) {
		if ((frameCounter & 3) == 3)	// return every 2/8 frames
			return;
	}
	else {
		if ((frameCounter & 7) == 7)	// return every 1/8 frames
			return;
	}

	if (releaseFirefox && num_firefoxes < MAX_FIREFOXES && !(game.stage == STAGE_CONVEYORS && num_firefoxes >= game.difficulty)) {
		// Initialize variables for new firefox
		firefox_t *this_firefox = &firefox[num_firefoxes];
		memset(this_firefox, 0, sizeof(firefox_t));

		this_firefox->isSpawning = true;

		this_firefox->background_data[0] = 16;
		this_firefox->background_data[1] = 16;
		num_firefoxes++;

		releaseFirefox = false;
	}

	// Process all movement for all fireballs
	move_firefoxes();
}

/* Moves the firefoxes */
void move_firefoxes(void) {
	uint8_t i;

	if (game.difficulty >= 3) {
		if ((rand() & 3) == 1 && frameCounter == 1) {
			// enable freezer mode for 2nd fire
			firefox[1].freezerMode = 2;
			// enable freezer mode for 4th fire
			firefox[4].freezerMode = 2;
		}
	}

	for (i = 0; i < num_firefoxes; i++) {
		firefox_t *this_firefox = &firefox[i];

		if (!this_firefox->isSpawning) {
			if (!this_firefox->onLadder) {
				if (this_firefox->freezerMode != 2) {
					reverse_firefox_random(this_firefox);

					if ((rand() & 3))
						goto skip_ladder_mounting;
				}
				else {
					// freezer mode is enguaged for this firefox
					handle_freezer(this_firefox);
				}

				// Jump to end if firefox is moving left
				if (this_firefox->dir == FACE_LEFT)
					goto end_firefox_movement;
			}

			// Do ladder mounting/dismouting here
			mount_dismount_ladder(this_firefox);

		skip_ladder_mounting:

			if (this_firefox->onLadder) {
				animate_firefox(this_firefox);

				// Handle all firefox on ladder stuff
				if (this_firefox->dir & 8) { // Firefox is climbing up a ladder 1/3 the normal speed
					if (this_firefox->ladderTimer == 0) {
						this_firefox->ladderTimer = 2;
						this_firefox->y--;
					}
					else {
						this_firefox->ladderTimer--;
					}
				}
				else { // Firefox is climbing down a ladder
					this_firefox->y++;
				}

			}
			else { // Firefox is moving left or right
				if (this_firefox->dir & FACE_RIGHT) {
					this_firefox->x++;
				}
				else {
					this_firefox->x--;
				}

				animate_firefox(this_firefox);

				if (gfx_GetPixel(this_firefox->x, this_firefox->y + 2) > COLOR_COLLISION) {
					if (this_firefox->dir & FACE_RIGHT) {
						this_firefox->dir = 2;
						this_firefox->x -= 2;
					}
					else {
						this_firefox->dir = FACE_RIGHT;
						this_firefox->x += 2;
					}
				}

				// Move firefox up/down if on slanted girder
				if (game.stage == STAGE_BARRELS) {
					uint8_t color = gfx_GetPixel(this_firefox->x, this_firefox->y + 1);

					if (color == COLOR_COLLISION)
						this_firefox->y--;
					else if (color > COLOR_COLLISION)
						this_firefox->y++;
				}

				// Check if fireball has reached left edge of screen
				if (this_firefox->x < 55)
					this_firefox->dir = FACE_RIGHT;
				// Check if fireball has reached right edge of screen
				if (this_firefox->x > 271)
					this_firefox->dir = 2;
			}

		end_firefox_movement:
			// Reset jumpCounter to 17 if it reached 0
			if (!this_firefox->jumpCounter)
				this_firefox->jumpCounter = 0x11;

			// Add bobbing effect to firefox movement
			this_firefox->actualY =
				this_firefox->y;
				//+ firefox_bobbingTable[this_firefox->jumpCounter];
			this_firefox->jumpCounter--;
		}
		else handle_firefox_spawning(this_firefox);
	}

}

/* This subroutine randomly reversed dir of fire every 43 fireball
 * execution frames. */
void reverse_firefox_random(firefox_t *this_firefox) {
	// Check if the firefox reverse dir timer has reached 0
	if (!this_firefox->reverseTimer) {
		this_firefox->reverseTimer = 0x2B;

		this_firefox->dir = FACE_LEFT;

		// Set firefox dir to be right with 50% probability
		if ((rand() & 1))
			this_firefox->dir = FACE_RIGHT;
	}
	this_firefox->reverseTimer--;
}

/* Handles a freezer when freezer mode is activated, including checking when to
 * freeze and when to leave freezer mode */
void handle_freezer(firefox_t *this_firefox) {
	// Check if freezeTimer is not zero; we are frozen and waiting for the timer to reach 0
	if (this_firefox->freezeTimer) {
		this_firefox->freezeTimer--;
		if (this_firefox->freezeTimer) {
			this_firefox->dir = FACE_LEFT;
			return;
		}
		// unfreeze
		this_firefox->freezerMode = 0;
		this_firefox->freezeTimer = 0;
	}
	else {
		if (this_firefox->freezeFlag) {
			this_firefox->freezeFlag = false;
			if (jumpman.y <= this_firefox->y) {
				this_firefox->freezeTimer = 0xFF;
				this_firefox->dir = FACE_LEFT;
				return;
			}
			// unfreeze
			this_firefox->freezerMode = 0;
			this_firefox->freezeTimer = 0;
		}
	}
	reverse_firefox_random(this_firefox);
}

/* Mounts a ladder if ladder nearby if already on a ladder check for dismounting*/
void mount_dismount_ladder(firefox_t *this_firefox) {
	// Check if firefox is climbing up a ladder
	if (this_firefox->onLadder) { // Firefox is climbing up or down a ladder
		if (this_firefox->y == this_firefox->dismountY) {
			if ((this_firefox->dir & 8) == FACE_RIGHT)
				if (this_firefox->freezerMode == 2)
					this_firefox->freezeFlag = true;

			this_firefox->dir = FACE_LEFT;
			this_firefox->onLadder = false;
		}
	}
	else { // Firefox is not climbing a ladder
		// Check if fireball is not on top of the top girder and the screen is not rivets
		if (!(game.stage != STAGE_RIVETS && this_firefox->y < 73)) {
			uint8_t *array = stage_data[game.stage - 1];

			while (*array < 2) {
				if (this_firefox->x == *(array + 1) + 51) {
					if (this_firefox->y + 1 == *(array + 4)) {
						this_firefox->dismountY = *(array + 2) - 1;
						this_firefox->onLadder = true;
						this_firefox->dir |= 8;		// accending ladder
						return;
					}
					else if (this_firefox->y + 1 == *(array + 2)) {
						if (this_firefox->y < jumpman.y) {
							this_firefox->dismountY = *(array + 4) - 1;
							this_firefox->onLadder = true;
							this_firefox->dir |= 4;	// descending ladder
							return;
						}
					}
				}
				array += 5;
			}
		}
	}
}

/* Toggle between firefox sprites */
void animate_firefox(firefox_t *this_firefox) {
	if (this_firefox->moveCounter == 0) {
		this_firefox->moveCounter = 2;
		this_firefox->sprite ^= 1;
	}
	else this_firefox->moveCounter--;
}

/* Handles firefox movemement when spawning */
void handle_firefox_spawning(firefox_t *this_firefox) {
	if (game.stage == STAGE_BARRELS) {							// Barrels
		// Check if this is the first time running this
		if (!this_firefox->jumpCounter) {
			this_firefox->x_old = this_firefox->x = 71;
			this_firefox->y_old = 223;
		}

		this_firefox->x++;

		if (this_firefox->jumpCounter < sizeof(firefox_girders_jumpTable)) {
			this_firefox->y = firefox_girders_jumpTable[this_firefox->jumpCounter++];
		}
		else {
			// Fire has completed its spawning
			this_firefox->jumpCounter = 0;
			this_firefox->isSpawning = false;
			this_firefox->dir = FACE_LEFT;
		}
	}
	else if (game.stage == STAGE_CONVEYORS) {					// Conveyors
		// Check if this is the first time running this
		if (!this_firefox->jumpCounter) {
			// Check if jumpman is on left side of the screen, in this case we spawn the fireball on the left
			if (jumpman.x <= 160) {
				// Set fireball dir to "special" left
				// after spawning it will check to reverse rection and receive a dir of either "right" or "left".
				this_firefox->dir = 2;
				this_firefox->x = 0xA1;
			}
			else {	// jumpman is on the right side of the screen
				this_firefox->dir = FACE_RIGHT;
				this_firefox->x = 0x9F;
			}
			this_firefox->y_old = 119;
			this_firefox->x_old = this_firefox->x;
		}

		// Check Firefox moving right
		if (this_firefox->dir == FACE_RIGHT)
			this_firefox->x++;
		else	// Firefox moving left
			this_firefox->x--;

		if (this_firefox->jumpCounter < sizeof(firefox_conveyors_jumpTable)) {
			this_firefox->y = firefox_conveyors_jumpTable[this_firefox->jumpCounter++];
		}
		else {
			// Fire has completed its spawning
			this_firefox->jumpCounter = 0;
			this_firefox->isSpawning = false;
			this_firefox->dir = FACE_LEFT;
		}
	}
	else {														// Rivets
		uint8_t R = rand() & 6;
		
		this_firefox->y_old = this_firefox->y = firefox_rivets_spawnTable[(jumpman.x > 160)][R + 1];
		this_firefox->x_old = this_firefox->x = firefox_rivets_spawnTable[(jumpman.x > 160)][R] + 33;
		
		this_firefox->isSpawning = false;
		this_firefox->dir = FACE_LEFT;
		this_firefox->sprite = 2;
	}
	this_firefox->actualY = this_firefox->y;
}


oilcan_t oilcan;
bool releaseFirefox;

/* Handle releasing of firefoxes and updates the oilcan flame */
void release_firefox(void) {
	// Update the oilcan on stage barrels and conveyors
	if (game.stage <= STAGE_CONVEYORS) {
		oilcan.updateTimer--;

		if (oilcan.updateTimer == 0) {
			oilcan.updateTimer = 4;

			if (oilcan.fireRelease == 1) {
				oilcan.sprite = (rand() & 1);
			}
			else if (oilcan.fireRelease == 3) {
				oilcan.sprite = 2 + (rand() & 1);
				
				oilcan.releaseTimer--;
				if (oilcan.releaseTimer == 0) {
					releaseFirefox = 1;
					oilcan.fireRelease = 1;
					oilcan.releaseTimer = 0x10;
				}
			}
		}
	}

	// Release firefox on stage elevators and conveyors
	if (game.stage == STAGE_RIVETS || game.stage == STAGE_CONVEYORS) {	// Stage is elevators or conveyors
		uint8_t i = (game.difficulty + 1) / 2;
		
		if (game.stage == STAGE_CONVEYORS)
			i++;

		i = 0xFF >> (i - 1);
		if ((frameCounter & i) == 0) {
			releaseFirefox = 1;
			releasePie = 1;
		}
	}
}


// Lookup tables
const uint8_t firefox_girders_jumpTable[] = {
	223, 220, 218, 217, 216, 215, 214, 213, 212, 212, 211,
	211, 211, 211, 211, 211, 212, 212, 213, 214, 215, 216,
	217, 218, 219, 220, 222, 224, 226, 228, 231 };

const uint8_t firefox_conveyors_jumpTable[] = {
	0x77, 0x72, 0x6F, 0x6D, 0x6B, 0x6A, 0x69, 0x68, 0x67, 0x67, 0x66, 0x66,
	0x66, 0x67, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F };

const uint8_t firefox_rivets_spawnTable[2][8] = {
	{	//x		y
		0xEE, 0xE7, // bottom, right
		0xDB, 0x97, // middle, right
		0xE6, 0xBF, // 2nd from bottom, right
		0xD6, 0x6F, // 2nd from top, right
	},

	{
		0x1B, 0xBF, // 2nd from bottom, left
		0x23, 0x97, // middle, left
		0x2B, 0x6F, // 2nd from top, left 76  + 33 0x21
		0x12, 0xE7, // bottom, left
	} };

const uint8_t firefox_bobbingTable[] = { 0xFF, 0x00, 0xFF, 0xFF, 0xFE, 0xFE,
										 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE,
										 0xFE, 0xFE, 0xFE, 0xFF, 0xFF, 0x00 };



#if 0
/* Replace the firefox that should get deleted with the last firefox and decreases num_firefoxes */
void del_firefox(uint8_t firefoxNum) {
	firefox[firefoxNum] = firefox[--num_firefoxes];
}
#endif
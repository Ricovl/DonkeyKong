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
#include "bonus_scores.h"
#include "overlay.h"
#include "images.h"
#include "stages.h"


elevator_t elevator[MAX_ELEVATORS];
uint8_t num_elevators;
uint8_t elevatorTimer;

void move_elevators(void) {
	if (game.stage == STAGE_ELEVATORS) {
		if (jumpman.y >= 231) {
			jumpman.isAlive = false;
		}

		if ((frameCounter & 1) == 1) {
			uint8_t i;

			// Move the elevators
			for (i = 0; i < num_elevators; i++) {
				elevator_t *this_elevator = &elevator[i];

				if (this_elevator->movingUp) {
					this_elevator->y--;
					if (this_elevator->y == 76) {
						this_elevator->x = 144;
						this_elevator->movingUp = false;
					}
				}
				else {
					this_elevator->y++;
					if (this_elevator->y == 228) {
						elevator[i] = elevator[--num_elevators];
						continue;
					}
				}
			}

			// Spawn a new elevator
			if (elevatorTimer == 0) {
				if (num_elevators < MAX_ELEVATORS) {
					elevator[num_elevators].x = 80;
					elevator[num_elevators].y = 228;
					elevator[num_elevators].movingUp = true;
					num_elevators++;

					elevatorTimer = 0x34;
				}
				else {
					elevatorTimer = 1;
				}
			}
			elevatorTimer--;

		}
		else if (jumpman.onElevator && !jumpman.isJumping) {

			if (jumpman.x < 76) {
				// falling off of elevator
				goto jumpman_falling;
			}

			if (jumpman.x <= 99) {
				// jumpman on left elevator
				if (jumpman.y < 105) {
					jumpman.isAlive =
						jumpman.onElevator = false;
				}
				jumpman.y--;
				return;
			}

			if (jumpman.x < 140) {	// 140
				// falling off of elevator
				goto jumpman_falling;
			}

			if (jumpman.x <= 163) {
				// jumpman on right elevator
				if (jumpman.y >= 221) {
					jumpman.isAlive =
						jumpman.onElevator = false;
				}
				jumpman.y++;
				return;
			}

		jumpman_falling:
			jumpman.onElevator = false;
			jumpman.startFalling = true;
		}
	}
}

const uint8_t elevator_y_table[] = {224, 172, 120, 92, 144, 196};

bool collision_elevator(void) {
	uint8_t i;

	for (i = 0; i < num_elevators; i++) {
		elevator_t *this_elevator = &elevator[i];

		if (jumpman.x > this_elevator->x - 4 &&
			jumpman.y >= this_elevator->y - 1 &&
			jumpman.x < this_elevator->x + 19 &&
			jumpman.y <= this_elevator->y + 22) {

			if (jumpman.y - 3 < this_elevator->y) {			// Jumpman landed on platform; land

				jumpman.y = this_elevator->y - 1;
				jumpman.onElevator = true;
				return true;
			}
			else if (jumpman.y - 21 >= this_elevator->y) {	// Jumpman hit head against bottom platform; die
				jumpman.isAlive = false;
			}
			else {											// Jumpman jumped against side platform; fall
				if (jumpman.jumpDir == 0) {
					jumpman.x = this_elevator->x - 4;
				}
				else {
					jumpman.x = this_elevator->x + 19;
				}
				jumpman.comingDown = true;
				jumpman.jumpDir =
					jumpman.jumpDirIndicator =
					jumpman.movingUp =
					jumpman.velocityY =
					jumpman.jumpCounter = 0;
			}
		}
	}

	return false;
}


const uint8_t bouncer_heightOffset[25] = { 0xFD, 0xFD, 0xFD, 0xFE, 0xFE, 0xFE, 0xFE, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x7F };

bouncer_t bouncer[MAX_BOUNCERS];
uint8_t num_bouncers;
bool releaseBouncer;

void move_bouncers(void) {
	if (game.stage == STAGE_ELEVATORS) {
		uint8_t i;

		// Move bouncers
		for (i = 0; i < num_bouncers; i++) {
			bouncer_t *this_bouncer = &bouncer[i];
			
			if ((frameCounter & 15) == 0)
				this_bouncer->sprite ^= 1;

			if (!this_bouncer->movingDown) {
				this_bouncer->x += 2;

				this_bouncer->y += *this_bouncer->heightOffset;

				this_bouncer->heightOffset++;
				if (*this_bouncer->heightOffset == 0x7F) {
					this_bouncer->heightOffset = &bouncer_heightOffset;
					if (this_bouncer->x >= 210)
						this_bouncer->movingDown = true;
				}
			}
			else {
				this_bouncer->y += 3;
				
				if (this_bouncer->y_old >= 240) {
					bouncer[i] = bouncer[--num_bouncers];
					continue;
				}
			}

		}

		// Spawn a new bouncer
		if (releaseBouncer && num_bouncers < MAX_BOUNCERS) {
			releaseBouncer = false;

			bouncer[num_bouncers].movingDown = false;
			bouncer[num_bouncers].sprite = 0;
			bouncer[num_bouncers].y_old = bouncer[num_bouncers].y = 57;
			bouncer[num_bouncers].x_old = bouncer[num_bouncers].x = 27 + (rand() & 15);
			
			bouncer[num_bouncers].background_data[0] = 16;
			bouncer[num_bouncers].background_data[1] = 15;

			bouncer[num_bouncers].heightOffset = &bouncer_heightOffset;

			num_bouncers++;
		}
	}
}


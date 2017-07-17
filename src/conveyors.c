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
#include "conveyors.h"
#include "images.h"


conveyor_t conveyor[3];
int8_t conveyorVector_top;
int8_t conveyorVector_middleLeft;
int8_t conveyorVector_middleRight;
int8_t conveyorVector_bottom;

/* handle conveyor dirs and adjust Mario's speed based on conveyor dirs */
void handle_conveyor_dirs(void) {
	if (game.stage == STAGE_CONVEYORS) {
		//----------------- handle top conveyor and pulleys -----------------//
		if ((frameCounter & 1) == 0) {
			conveyor[Top].reverseCounter--;
			if (conveyor[Top].reverseCounter == 0) {
				conveyor[Top].reverseCounter = 0x80;
				reverse_conveyor_dir(&conveyor[Top]);
			}
		}

		update_conveyor_vector(Top, &conveyorVector_top);

		if ((frameCounter & 31) == 1) {
			animate_pulley(&conveyor[Top]);
		}

		//---------------- handle middle conveyor and pulleys ---------------//
		if (jumpman.y >= 183) {
			if ((frameCounter & 1) == 0) {
				conveyor[Middle].reverseCounter--;
				if (conveyor[Middle].reverseCounter == 0) {
					conveyor[Middle].reverseCounter = 0xC0;
					reverse_conveyor_dir(&conveyor[Middle]);
				}
			}

		update_middle_vector:
			update_conveyor_vector(Middle, &conveyorVector_middleRight);
			conveyorVector_middleLeft = conveyorVector_middleRight * -1;

			if ((frameCounter & 31) == 0) {
				animate_pulley(&conveyor[Middle]);
				conveyor[Middle].sprite1 = pulley_right[(conveyor[Middle].sprite << 1) % 3];
			}
		}
		else {
			if (conveyor[Middle].direction > 127)
				goto update_middle_vector;
			else
				conveyor[Middle].direction = 0xFF;
		}

		//---------------- handle lower conveyor and pulleys ----------------//
		if ((frameCounter & 1) == 0) {
			conveyor[Bottom].reverseCounter--;
			if (conveyor[Bottom].reverseCounter == 0) {
				conveyor[Bottom].reverseCounter = 0xFF;
				reverse_conveyor_dir(&conveyor[Bottom]);
			}
		}

		update_conveyor_vector(Bottom, &conveyorVector_bottom);

		if ((frameCounter & 31) == 2) {
			animate_pulley(&conveyor[Bottom]);
		}

		//-------- handle mario's different speeds when on a conveyor -------//
		if (jumpman.y == 71) {			// jumpman on top conveyor
			jumpman.x += conveyorVector_top;	// This doesn't have to be here because the level ends on the top conveyor
		}
		else if (jumpman.y == 111) {	// jumpman on middle conveyor
			if (jumpman.x > 160)
				jumpman.x += conveyorVector_middleRight;
			else
				jumpman.x += conveyorVector_middleLeft;
		}
		else if (jumpman.y == 191) {	// jumpman on bottom conveyor
			jumpman.x += conveyorVector_bottom;
		}
		else return;

		if (jumpman.x >= 266)
			jumpman.x--;
		if (jumpman.x <= 54)
			jumpman.x++;
	}
}

/* Reverse the direction of the conveyor */
void reverse_conveyor_dir(conveyor_t *this_conveyor) {
	if (this_conveyor->direction < 127)
		this_conveyor->direction = 0xFE;
	else
		this_conveyor->direction = 0x02;
}

/* Update the vector value of the conveyor */
void update_conveyor_vector(uint8_t conveyor_num, int8_t *vector) {
	if ((frameCounter & 1) == 1) {
		if (conveyor[conveyor_num].direction < 127)
			*vector = 0x01;
		else
			*vector = -1;
		conveyor[conveyor_num].direction = *vector;
	}
	else *vector = 0;
}

/* Cycle through pulley sprites */
void animate_pulley(conveyor_t *this_conveyor) {
	uint8_t sprite = this_conveyor->sprite;

	if (this_conveyor->direction < 127) {
		if (sprite == 0)
			sprite = 3;
		sprite--;
	}
	else {
		sprite = (sprite + 1) % 3;
	}

	this_conveyor->sprite0 = pulley_left[sprite];
	this_conveyor->sprite1 = pulley_right[sprite];
	this_conveyor->sprite = sprite;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Pies
////////////////////////////////////////////////////////////////////////////////////////////////

pie_t pie[MAX_PIES];
uint8_t num_pies;

bool releasePie;
uint8_t pieTimer;

/* Move the pies */
void move_pies(void) {
	if (game.stage == STAGE_CONVEYORS) {
		uint8_t i;

		if (pieTimer == 0) {
			if (releasePie && num_pies < MAX_PIES) {
				pie[num_pies].y = 104;

				if ((rand() & 255) >= 0x60 && conveyor[Middle].direction > 1) {
					if ((rand() & 255) >= 0x68)
						pie[num_pies].x = 32;
					else
						pie[num_pies].x = 272;
				} 
				else {
					pie[num_pies].y = 184;

					if (conveyor[Bottom].direction <= 1)
						pie[num_pies].x = 32;
					else
						pie[num_pies].x = 272;
				}

				pie[num_pies].x_old = pie[num_pies].x;
				pie[num_pies].y_old = pie[num_pies].y;
				pie[num_pies].background_data[0] = 16;
				pie[num_pies].background_data[1] = 8;
				releasePie = false;
				pieTimer = 0x7C;
				num_pies++;
			}
		}
		else pieTimer--;

		for (i = 0; i < num_pies; i++) {
			pie_t *this_pie = &pie[i];

			if (this_pie->x < 32 || this_pie->x > 272) {
				del_pie(i);
				continue;
			}

			if (this_pie->y == 104) {		// pie on middle conveyor
				// Delete if pie is in oilcan fire
				if (this_pie->x == 152) {	// 152 is closer, but 153 is like the original
					del_pie(i);
					continue;
				}

				if (this_pie->x < 153)
					this_pie->x += conveyorVector_middleLeft;
				else
					this_pie->x += conveyorVector_middleRight;
			}
			else if (this_pie->y == 184) {	// pie on bottom conveyor
				this_pie->x += conveyorVector_bottom;
			}
		}
	}
}

void del_pie(uint8_t pieNum) {
	pie[pieNum] = pie[--num_pies];
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Retractable ladders
////////////////////////////////////////////////////////////////////////////////////////////////

retractableLadder_t retractableLadder[MAX_RETRACTABLE_LADDERS];
uint8_t num_retractable_ladders;

void move_retractable_ladders(void) {
	if (game.stage == STAGE_CONVEYORS) {
		retractableLadder_t *this_ladder = &retractableLadder[frameCounter & 1];

		if (this_ladder->state == 0) {		//// ladder is all the way up ////
			this_ladder->waitTimer--;

			if (this_ladder->waitTimer == 0) {
				this_ladder->state = 1;
				if (jumpman.y < 113 && !jumpman.isJumping && jumpman.x == this_ladder->x + 4)
					jumpman.brokenLadder = true;
				return;
			}

			if (jumpman.y < 113 && !jumpman.isJumping && jumpman.x == this_ladder->x + 4)
				jumpman.brokenLadder = false;
		}
		else if (this_ladder->state == 1) {	//// ladder is moving down ////
			this_ladder->moveTimer--;

			if (this_ladder->moveTimer == 0) {
				this_ladder->moveTimer = 4;

				this_ladder->y++;
				if (this_ladder->y == 96) {
					this_ladder->state = 2;
				}

				if (jumpman.y < 113 && !jumpman.isJumping && jumpman.x == this_ladder->x + 4) {
					if (jumpman.y < 95) {
						jumpman.y++;

						jumpman.sprite = 3;
						if ((jumpman.y & 1) == 0)
							jumpman.y++;
					}

					jumpman.ladderToggle = ((jumpman.y >> 1) & 1) ^ 1;
				}
			}
		}
		else if (this_ladder->state == 2) {	//// ladder is all the way down ////
			if ((rand() & 60) == 0) {
				this_ladder->state = 3;
			}
		}
		else {								//// ladder is moving up ////
			this_ladder->moveTimer--;

			if (this_ladder->moveTimer == 0) {
				this_ladder->moveTimer = 2;
				
				this_ladder->y--;
				if (this_ladder->y == 80) {
					this_ladder->waitTimer = 0x80;
					this_ladder->state = 0;
				}
			}
		}

	}
}


// Tables
const uint8_t pulleyTable_y[3] = { 71, 111, 191 };
const uint8_t pulleyTable_leftX[3] = { 50, 167, 50 };
const uint16_t pulleyTable_rightX[3] = { 259, 143, 259 };
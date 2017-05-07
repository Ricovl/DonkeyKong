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
#include "overlay.h"
#include "firefoxes.h"
#include "kong.h"


void handle_jumping(barrel_t *barrel);
void handle_bouncing(barrel_t *barrel);

barrel_t barrel[MAX_BARRELS];
uint8_t num_barrels;

/* Moves the barrels */
void move_barrels(void) {
	if (game.stage == STAGE_BARRELS) {
		uint8_t i;

		for (i = 0; i < num_barrels; i++) {
			barrel_t *this_barrel = &barrel[i];

			if (this_barrel->isRolling) {
				if (this_barrel->isCrazy) {			//// Barrel is a crazy barrel ////
					handle_jumping(this_barrel);

					// Check if barrel is in the girder; it bounced off previously and it collides with girder again
					if (this_barrel->y - 26 >= this_barrel->bounceY && gfx_GetPixel(this_barrel->x, this_barrel->y) <= COLOR_COLLISION) {
						for (; gfx_GetPixel(this_barrel->x, this_barrel->y) <= COLOR_COLLISION; this_barrel->y--);

						// Check if barrel is on the bottom of the screen
						if (this_barrel->y >= 220) {
							this_barrel->isCrazy = false;
							this_barrel->sprite = 0;
							this_barrel->dir = FACE_LEFT;

							this_barrel->jumpDir = 0xFF;
							this_barrel->jumpDirIndicator = 0;
							this_barrel->movingUp = false;
							this_barrel->velocityY = 0xB0;
							this_barrel->edgeIndicator = true;
							this_barrel->isJumping = true;
						}
						else {	// Barrel is not at the bottom of the screen
							handle_bouncing(this_barrel);
							bounce_crazy_barrel(this_barrel);
							this_barrel->bounceY = this_barrel->y;
						}

						this_barrel->jumpCounter =
							this_barrel->jumpCounterX =
							this_barrel->jumpCounterY = 0;
					}
					else {	// Update the barrels sprite
						if (--this_barrel->moveCounter == 0) {
							this_barrel->sprite ^= 1;	// Switch sprite between sprite 4(0100) and 5(0101)
							this_barrel->moveCounter = 4;
						}

					}

					/* Delete the barrel if it is off the screen */
					if (this_barrel->x_old < 40 || this_barrel->x_old > 278 || this_barrel->y_old > 250) {
						del_barrel(i);
					}
				}
				else if (this_barrel->onLadder) {	//// Barrel is rolling down a ladder ////
					this_barrel->y++;	// Increase barrels y-position

					// Update the barrels sprite
					if (--this_barrel->moveCounter == 0) {
						this_barrel->sprite ^= 1;	// Switch sprite between sprite 4(0100) and 5(0101)
						this_barrel->moveCounter = 4;
					}

					// Unmount ladder if the barrel reached the bottom of the ladder
					if (gfx_GetPixel(this_barrel->x, this_barrel->y + 1) == COLOR_FLOOR && gfx_GetPixel(this_barrel->x, this_barrel->y + 7) == COLOR_FLOOR) {
						this_barrel->onLadder = false;
						this_barrel->sprite = 0;
						this_barrel->dir ^= 1;
					}
				}
				else if (!this_barrel->isJumping) {	//// Barrel is moving left or right ////
					
					if (this_barrel->dir == FACE_RIGHT) {
						this_barrel->x++;
					}
					if (this_barrel->dir == FACE_LEFT) {
						this_barrel->x--;
					}

					update_sprite_rolling(this_barrel);

					// Check if there is a ladder underneath the barrel
					if ((this_barrel->x & 7) == 3 && gfx_GetPixel(this_barrel->x + 4, this_barrel->y + 9) == COLOR_LADDER) {
						// Decide if the barrel should take the ladder
						if (take_ladder(this_barrel)) {
							this_barrel->onLadder = true;
							this_barrel->sprite = 4;
						}
					}

					// Increase barrels x-position if slanted girder goes down
					if (((this_barrel->x & 15) == (this_barrel->dir ^ 1) * 14) && !(this_barrel->y == 67 && this_barrel->x <= 186) && this_barrel->y < 231) {
						this_barrel->y++;
					}

					// Barrel reached the edge of a girder
					if (this_barrel->x > 259 || this_barrel->x < 59 && gfx_GetPixel(this_barrel->x, this_barrel->y + 2) > COLOR_COLLISION) {	// Should be < 58, but doen't work because of wrong girder detection. (I can also check the heigt of the barrels instead of checking a pixel)
						this_barrel->isJumping = true;

						if (this_barrel->x > 259) {	// Right edge
							this_barrel->jumpDir = 0x00;
							this_barrel->jumpDirIndicator = 0x60;	// Indicates a roll over the right edge
						}
						else {						// Left edge
							this_barrel->jumpDir = 0xFF;
							this_barrel->jumpDirIndicator = 0xA0;	// Indicates a roll over left edge
						}

						this_barrel->movingUp = 0xFF;
						this_barrel->velocityY = 0xF0;
						this_barrel->edgeIndicator =
							this_barrel->jumpCounter =
							this_barrel->jumpCounterX =
							this_barrel->jumpCounterY = 0;
					}

					/* Delete the barrel if it is rolling off the screen(don't know if this can ever happen) */
					if (this_barrel->x_old < 42 || this_barrel->x_old > 276) {
						del_barrel(i);
					}
				}
				else {								//// Barrel is falling from edge ////
					handle_jumping(this_barrel);

					update_sprite_rolling(this_barrel);

					// Check if barrel hit the ground
					if (gfx_GetPixel(this_barrel->x, this_barrel->y) <= COLOR_COLLISION) {
						for (; gfx_GetPixel(this_barrel->x, this_barrel->y) <= COLOR_COLLISION; this_barrel->y--);

						this_barrel->edgeIndicator++;

						// Barrel hit the girder and is not done with bouncing
						if (this_barrel->edgeIndicator != 3) {
							// Check if barrel hit the ground for the first time after falling of the edge
							if (this_barrel->edgeIndicator == 1 && (this_barrel->isBlue || this_barrel->y - 14 < jumpman.y)) {
								// Reverse direction if barrel is blue or above jumpman, else bounce off screen
								if (this_barrel->jumpDir == 0)
									this_barrel->jumpDir = 0xFF;
								else
									this_barrel->jumpDir = 0x01;

								this_barrel->jumpDirIndicator = 0;
								this_barrel->dir ^= 1;
							}

							// Bounce the barrel
							handle_bouncing(&barrel[i]);
							asm("srl h");
							asm("rr l");
							asm("srl h");
							asm("rr l");
							asm("ld	iy,(ix+-3)");
							asm("ld (iy+19),h");
							asm("ld (iy+18),l");

							this_barrel->jumpCounter =
								this_barrel->jumpCounterX =
								this_barrel->jumpCounterY = 0;
						}
						else {
							// End of edge falling
							this_barrel->edgeIndicator = 0;
							this_barrel->isJumping = false;
						}

					}

					/* Delete the barrel if it is bouncing of the screen */
					if (this_barrel->x_old < 42 || this_barrel->x_old > 276) {
						del_barrel(i);
					}
				}

				/* Delete the barrel if it is touching the oil can */
				if (this_barrel->x < 72 && this_barrel->y > 225 && this_barrel->x > 70 && this_barrel->y <= 231) {
					if (this_barrel->isBlue) {
						oilcan.onFire = true;
						oilcan.fireRelease = 3;
					}
					del_barrel(i);
				}

			}
		}
	}
}

/* Decide if the barrel should take the ladder or not */
bool take_ladder(barrel_t *this_barrel) {
	uint8_t R;

	if (!oilcan.onFire)
		return true;
	if (jumpman.y <= this_barrel->y + 5)
		return false;

	R = rand() & 255; 		// random number between 0 and 255 inclusive

	if ((R % 3) >= ((game.difficulty / 2) + 1))
		return false;
	if (this_barrel->x == jumpman.x || (this_barrel->x < jumpman.x && kb_Data[kb_group_7] == kb_Left) || (this_barrel->x > jumpman.x && kb_Data[kb_group_7] == kb_Right))
		return true;

	if ((R & 0x18) != 0)
		return false;		// 75% chance of return without ladder
	return true;			// 25% chance of taking ladder
}

/* Crazy barrel hit a girder; define new bounce values here */
void bounce_crazy_barrel(barrel_t *this_barrel) {
	uint8_t R = rand();

	if (game.difficulty <= 2 || !oilcan.onFire) {	// Difficulty is 1 or 2 or fire is not lit
		uint8_t dirIndicator;

		if (oilcan.onFire)
			dirIndicator = R;
		else {
			if (game.difficulty == 1)
				dirIndicator = 0x01;
			else if (game.difficulty == 2)
				dirIndicator = 0xB1;
			else
				dirIndicator = 0xE9;
		}

		this_barrel->jumpDirIndicator = dirIndicator;
		this_barrel->jumpDir = (dirIndicator & 1) - 1;
	}
	else if (game.difficulty <= 4) {				// Difficulty is 3 or 4
		this_barrel->jumpDirIndicator = R;
		this_barrel->jumpDir = 0xFF;
		if (jumpman.x >= this_barrel->x)
			this_barrel->jumpDir = 1;
	}
	else {											// Difficulty is 5
		/*if (jumpman.x < this_barrel->x) {

		}*/
		/*
		231A  3A0362    LD      A,(#6203)		; load A with mario's X position
		231D  DD9603    SUB     (IX+#03)        ; subtract the barrel's X position
		2320  0EFF      LD      C,#FF           ; load C with #FF
		2322  DA2623    JP      C,#2326         ; if barrel is to left of mario, then jump ahead

		2325  0C        INC     C               ; else increase C to 0

		2326  07        RLCA                    ; rotate left A (doubles A) jumpman.x >> 1
		2327  CB11      RL      C               ; rotate left C				C * 2
		2329  07        RLCA                    ; rotate left A (doubles A) jumpman.x >> 1
		232A  CB11      RL      C               ; rotate left C				C * 2
		232C  DD7110    LD      (IX+#10),C      ; store C into +10			this_barrel->jumpDir
		232F  DD7711    LD      (IX+#11),A      ; store A into +11			this_barrel->jumpDirIndicator
		*/
	}
}

/* Cycles the barrels sprite variable through the rolling barrel sprites in the right direction
 * In the real donkeykong the barrel seems to turn the wrong way? */
void update_sprite_rolling(barrel_t *this_barrel) {
	if (--this_barrel->moveCounter == 0) {
		this_barrel->sprite = (this_barrel->sprite + ((this_barrel->dir - 1) | 1)) & 3;
		this_barrel->moveCounter = 4;
	}
}

/* Shifts all barrels in the array next to barrelNum to the right */
void del_barrel(uint8_t barrelNum) {
	num_barrels--;
	for (; barrelNum < num_barrels; barrelNum++)
		barrel[barrelNum] = barrel[barrelNum + 1];
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Barrel deployment
////////////////////////////////////////////////////////////////////////////////////////////////

bool deployBarrel;
bool deployingBarrel;
bool crazyIndicator;
uint8_t newBarrelType = 0;
uint8_t barrelDeploymentTimer;
uint8_t barrelDeploymentProgress;

/* Defines what type of barrel the next spawing barrel will be */
void spawn_barrel(void) {
	if (game.stage == STAGE_BARRELS && deployingBarrel == false && game.bonusTimer) {
		uint8_t i;

		// Check if we are within first 2 releases of the round
		if (game.initialBonusValue - game.bonusTimer <= 2) {
			// Next barrel is crazy if this is the first spawning barrel,
			// else this is the second spawning barrel; spawn a normal
			if (game.bonusTimer == game.initialBonusValue)
				goto spawn_crazy;
			else
				goto spawn_normal;
		}

		// Next barrel is normal barrel is the previous barrel was a crazy barrel
		if (newBarrelType & 1)
			goto spawn_normal;

		// This adds a delay to the spawning of barrels, that decreases the higher the difficulty.
		for (i = game.difficulty; i > 0; i--) {
			if (i == (frameCounter & 31)) {
				if (game.bonusTimer < game.initialBonusValue / 2) {
					if (rand() & 1) 
						return;
				}

				if ((rand() & 15) != 0)
					goto spawn_normal;
				else
					goto spawn_crazy;
			}
		} return;

	spawn_crazy:
		newBarrelType = 1;
		barrelDeploymentProgress = 3;

		goto skip_normal;
	spawn_normal:
		newBarrelType = 0;
		barrelDeploymentProgress = 4;

	skip_normal:
		deployBarrel = true;

		// Blue barrel gets spawned every 8 releases
		if (game.blueBarrelCounter == game.bonusTimer) {
			game.blueBarrelCounter -= 8;

			if (num_firefoxes < MAX_FIREFOXES) {
				newBarrelType |= 0x80;
			}
		}

	}
}

/* Handle barrel getting deployed */
void deploy_barrel(void) {
	if (game.stage == STAGE_BARRELS && deployBarrel) {
		// Check if barrel should get deployed
		if (deployBarrel && !deployingBarrel) {
			if (num_barrels < MAX_BARRELS) {

				deployingBarrel = true;

				game.bonusTimer--;
				draw_bonus_time();
				if (game.bonusTimer == 0) {
					game.timeRanOut = 1;
				}

				// If bonusTimer is equal or lower than 4, remove barrels from pile nex to kong
				if (game.bonusTimer <= 4) {
					gfx_FillRectangle_NoClip(48 + (game.bonusTimer & 1) * 10, (game.bonusTimer < 2) ? 51 : 35, 10, 16);
					gfx_BlitRectangle(gfx_buffer, 48, 35, 20, 32);
				}

				kong.sprite = 0;
			}
			else return;
		}

		barrelDeploymentTimer--;
		if (barrelDeploymentTimer)
			return;
		barrelDeploymentTimer = 0x18;

		if (barrelDeploymentProgress) {
			//update kong's sprite and barrel
			if (barrelDeploymentProgress > 1) {
				kong.sprite++;
				draw_kong();

				// Check if kong's sprite is kong holding sprite
				if (kong.sprite == 2) {
					barrel_t *this_barrel = &barrel[num_barrels];

					memset(this_barrel, 0, sizeof(barrel_t));

					this_barrel->x_old = this_barrel->x = 91;
					this_barrel->y_old = this_barrel->y = 64;

					this_barrel->dir = FACE_RIGHT;
					this_barrel->sprite = 4;
					this_barrel->moveCounter = 4;

					if (newBarrelType >> 1)
						this_barrel->isBlue = true;
					if (newBarrelType & 1)
						this_barrel->isCrazy = true;

					this_barrel->background_data[0] = 16;
					this_barrel->background_data[1] = 10;
					num_barrels++;
				}

				// Check if kong sprite is kong facing left sprite
				if (kong.sprite == 3) {
					barrel_t *this_barrel = &barrel[num_barrels - 1];
					this_barrel->x_old = this_barrel->x = 121;
					this_barrel->y_old = this_barrel->y = 66;
					this_barrel->sprite = 0;
				}

			}

			barrelDeploymentProgress--;
			if (barrelDeploymentProgress == 1) {
				barrelDeploymentTimer = 1;
			}
		}
		else {		// Deploy barrel
			kong.sprite = 0;
			draw_kong();

			barrel[num_barrels - 1].isRolling = true;
			deployBarrel = false;
			deployingBarrel = false;
		}
	}
}

/*
* This is basicly what deploy_barrel(void) does, but in text form.
* Note: there can't be more than 10 barrels!
*
*
* The barelDeploymentCounter starts at 1 when the level starts and the blue crazy barrel gets spawned
*
*
* First(this doesn't hapen in deploy_barrel function):
* barrelDeploymentProgress gets set to 4(if normal barrel)
* barrelDeploymentCounter  doesn't get set, but is mostly 0x18 except for 1st barrel
* Kong's sprite is unchanged or maybe gets set to 0?
*
* Second:
* barrelDeploymentProgress gets set to 2(if normal barrel)		4
* barrelDeploymentCounter  gets set to 0x18
* Kong's sprite changed to grabbing barrel		sprite:1
*
* Third:
* barrelDeploymentProgress gets set to 1(if normal barrel)		3
* barrelDeploymentCounter  gets set to 0x18
* Kong's sprite changed to holding barrel		sprite:2
*
* Fourth:
* barrelDeploymentProgress gets set to 0(if normal barrel)		2
* barrelDeploymentCounter  gets set to 0x01
* Kong's sprite changed to releasing barrel, it's holding a barrel		sprite:3
*
* Fifth
* barrelDeploymentProgress gets set to 0(if normal barrel)		1
* barrelDeploymentCounter  gets set to 0x018
* Kong's sprite changed to releasing barrel, it's holding a barrel			if crazy barrel kong's sprite gets changed to beating chest sprite and barrel gets released, so basicly step 6
* Barrel moves one pixel to the right			sprite:3
*
* Sixth
* barrelDeploymentProgress gets set to 0(if normal barrel)		0
* barrelDeploymentCounter  gets set to 0x018
* Kong's sprite changed to beating chest, it released the barrel	sprite:0
* Barrel Deployment thiny's get set to false
*/
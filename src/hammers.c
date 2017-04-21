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
#include "hammers.h"
#include "barrels.h"
#include "firefoxes.h"
#include "conveyors.h"
#include "drawsprites.h"
#include "images.h"


hammer_t hammer[MAX_HAMMERS];
uint8_t num_hammers;

uint8_t hammerActive = false;
uint8_t hammerTimer = 0;
uint8_t hammerLength = 0;

uint16_t firefox_hammer_palette[] = { gfx_RGBTo1555(255, 0, 0), gfx_RGBTo1555(0, 255, 255), gfx_RGBTo1555(155, 155, 255) };

/* Animate hammer while hold by jumpman */
void animate_hammer(void) {
	if (game.stage != STAGE_ELEVATORS && hammerActive) {
		hammer_t *this_hammer = &hammer[hammerActive - 1];

		if (this_hammer->active) {
			this_hammer->dir = jumpman.dir;

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
						//gfx_Sprite_NoClip((gfx_image_t*)this_hammer->background_data, this_hammer->x_old - 7, this_hammer->y_old - 9);
						gfx_BlitRectangle(gfx_buffer, this_hammer->x_old - 7, this_hammer->y_old - 9, 16, 10);
						gfx_SetDrawBuffer();

						// Delete the hammer's struct
						if (hammerActive == 1) {
							hammer[0] = hammer[1];
						}
						hammerActive = 0;
						num_hammers--;

						// Restore the firefoxes' palette
						gfx_SetPalette(sprites_gfx_pal + 6, 6, 6);
						// Restore jumpmans sprite(Could maybe just set the sprite to 0)
						jumpman.sprite = 0;
						return;
					}
				}

				if (hammerLength == 1 && ((frameCounter >> 3) & 1) == 1)
					this_hammer->sprite ^= 2;

				if (jumpman.sprite > 3)
					jumpman.sprite = ((jumpman.sprite - 7) >> 1);
			}

			if (jumpman.sprite < 3)
				jumpman.sprite = 7 + jumpman.sprite * 2 + (this_hammer->sprite & 1);
		}

		// Move the hammer sprite to an offset from jumpman
		if ((this_hammer->sprite & 1) == 0) {	// Above jumpman
			this_hammer->x = jumpman.x;
			this_hammer->y = jumpman.y - 16;
		}
		else {									// Next to jumpman
			if (jumpman.dir == FACE_LEFT)
				this_hammer->x = jumpman.x - 16;
			else
				this_hammer->x = jumpman.x + 16;
			this_hammer->y = jumpman.y;
		}

	}
}

/* Animation:
 *  big	circle
 *  wait 6 ticks
 *  middle circle
 *  wait 6 ticks
 *  big	circle
 *  wait 6 ticks
 *  middle circle
 *  wait 6 ticks
 *
 *  big circle
 *  wait 12 ticks
 *  middle circle
 *  wait 12 ticks
 *  small circle
 *  wait 12 ticks
 *  burst
 *  wait 12 ticks
 *
 *  wait 1 tick
 *  show score
*/
bool wasItemHit;
uint8_t hitItemType;
uint8_t hitItemNum;

void animate_hammer_hit(void) {
	if (wasItemHit) {
		uint8_t points, i;
		uint8_t y;
		uint24_t x;

		wasItemHit = false;
		dbg_sprintf(dbgout, "item was hit\n");

		if (hitItemType == 0) {			// barrel
			points = barrel[hitItemNum].isBlue;
			gfx_SetDrawScreen();
			gfx_BlitRectangle(gfx_buffer, barrel[hitItemNum].x - 7, barrel[hitItemNum].y - 9, 16, 10);
			gfx_SetDrawBuffer();
			del_barrel(hitItemNum);
		}
		else if (hitItemType == 1) {	// fireball/firefox
			points = firefox[hitItemNum].moveCounter;
			gfx_SetDrawScreen();
			gfx_BlitRectangle(gfx_buffer, firefox[hitItemNum].x - 7, firefox[hitItemNum].y - 15, 16, 16);
			gfx_SetDrawBuffer();
			firefox[hitItemNum] = firefox[--num_firefoxes];
		}
		else {							// pie
			// points = pie[+1].
		}

		/*if (points == 0) {
			points = 2;	// 300 pt
		}
		else {
			points = 4;	// random
		}*/

		update_screen();

		/*for (i = 0; i < 5; i++) {
			gfx_TransparentSprite_NoClip(hammer_hit[i & 1], x, y);
			waitTicks(6);
		}*/
	}
}
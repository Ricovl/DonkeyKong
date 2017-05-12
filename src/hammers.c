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
#include "hammers.h"
#include "barrels.h"
#include "firefoxes.h"
#include "conveyors.h"
#include "drawsprites.h"
#include "bonus_scores.h"
#include "images.h"


hammer_t hammer[MAX_HAMMERS];
uint8_t num_hammers;

uint8_t hammerActive;
uint8_t hammerTimer;
uint8_t hammerLength;

uint16_t firefox_hammer_palette[] = { gfx_RGBTo1555(255, 0, 0), gfx_RGBTo1555(0, 255, 255), gfx_RGBTo1555(155, 155, 255) };

/* Animate hammer while hold by jumpman */
void animate_hammer(void) {
	if (game.stage != STAGE_ELEVATORS && hammerActive) {
		hammer_t *this_hammer = &hammer[hammerActive - 1];

		if (this_hammer->active) {
			this_hammer->dir = jumpman.dir;

			if (hammerTimer == 0)
				gfx_SetPalette(firefox_hammer_palette, 6, 6);

			hammerTimer++;
			if (!(hammerTimer & 7)) {
				this_hammer->sprite ^= 1;

				if (hammerTimer == 0) {
					hammerLength++;

					if (hammerLength == 2) {
						this_hammer->active = false;

						// Remove hammer sprite from screen
						gfx_SetDrawScreen();
						gfx_BlitRectangle(gfx_buffer, this_hammer->x_old - 7, this_hammer->y_old - 9, 16, 10);
						gfx_SetDrawBuffer();

						// Delete the hammer's struct
						if (hammerActive == 1)
							hammer[0] = hammer[1];
						hammerActive = 0;
						num_hammers--;

						// Restore the firefoxes' palette
						gfx_SetPalette(sprites_gfx_pal + 6, 6, 6);
						// Restore jumpmans sprite(not the best way to do this)
						jumpman.sprite = 0;
						return;
					}
				}

				if (hammerLength == 1) {
					if (((frameCounter >> 3) & 1) == 1)
						this_hammer->sprite |= 2;	// Yellow hammer
					else
						this_hammer->sprite &= 1;	// Normal hammer
				}

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
		gfx_image_t *background_data;
		uint8_t points, i;
		uint8_t y;
		uint24_t x;

		wasItemHit = false;

		if (hitItemType == 0) {			// barrel
			points = barrel[hitItemNum].isBlue;
			gfx_SetDrawScreen();
			gfx_BlitRectangle(gfx_buffer, barrel[hitItemNum].x - 7, barrel[hitItemNum].y - 9, 16, 10);
			gfx_SetDrawBuffer();
			x = barrel[hitItemNum].x - 7;
			y = barrel[hitItemNum].y - 11;
			del_barrel(hitItemNum);
		}
		else if (hitItemType == 1) {	// fireball/firefox
			points = firefox[hitItemNum].moveCounter;
			gfx_SetDrawScreen();
			gfx_BlitRectangle(gfx_buffer, firefox[hitItemNum].x - 7, firefox[hitItemNum].y - 15, 16, 16);
			gfx_SetDrawBuffer();
			x = firefox[hitItemNum].x - 7;
			y = firefox[hitItemNum].y - 14;
			firefox[hitItemNum] = firefox[--num_firefoxes];
		}
		else {							// pie
			// points = pie[+1].
		}

		background_data = gfx_MallocSprite(16, 14);
		update_screen();
		gfx_SetDrawScreen();

		if (points == 0) {
			points = 2;			// 300 pt
		}
		else {
			points = rand() & 255;
			if ((points & 1) == 1)
				points = 3;		// 50% chance of 500 points
			else if ((points & 2) == 2)
				points = 4;		// 25% chance of 800 points
			else
				points = 2;		// else 300 points
		}

		for (i = 0; i < 8; i++) {
			uint8_t spriteNum = i & 1;
			uint8_t offset;
			if (i > 5)
				spriteNum = 2;
			offset = (spriteNum << 1) | 1;
			if (i > 6) {
				spriteNum = 3;
				offset = 0;
			}
			gfx_GetSprite(background_data, x, y);
			gfx_TransparentSprite_NoClip(hammer_hit[spriteNum], x + offset, y + offset + (spriteNum == 6));
			waitTicks(6 + (i > 3) * 6);
			gfx_Sprite_NoClip(background_data, x, y);
		}
		waitTicks(1);

		gfx_SetDrawBuffer();
		spawn_bonus_score(points, x + 1, y + 3);
		free(background_data);
	}
}
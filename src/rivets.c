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
#include "rivets.h"


bool rivet_enabled[8];
uint8_t num_rivets;

/* Checks for and handles jumpman walking/jumping over rivet */
void handle_rivets(void) {
	if (game.stage == STAGE_RIVETS) {
		if ((jumpman.x == 107 || jumpman.x == 211) && jumpman.y < 192) {
			jumpman.traversedRivet = true;
		}
		else if (jumpman.traversedRivet) {
			uint8_t y, rivetNum = 0;

			jumpman.traversedRivet = false;
			for (y = 71; y <= 191; y += 40) {
				if (jumpman.y <= y) {
					uint24_t x = 104;

					if (jumpman.x > 160) {
						x = 208;
						rivetNum += 4;
					}

					if (rivet_enabled[rivetNum]) {
						rivet_enabled[rivetNum] = false;

						gfx_FillRectangle_NoClip(x, y, 8, 9);
						gfx_BlitRectangle(gfx_buffer, x, y, 8, 9);

						spawn_bonus_score(0, jumpman.x - 5, jumpman.y + 9);
						num_rivets--;
					}
					return;
				}
				rivetNum++;
			}
		}
	}
}
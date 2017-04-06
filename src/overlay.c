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

// donkeykong stuff
#include "defines.h"
#include "overlay.h"
#include "gfx\sprites_gfx.h"


/* Draw the 1UP, HIGH SCORE and 2UP */
void draw_player_score(void) {
	gfx_SetTextFGColor(COLOR_WHITE); //Set the text color to white
	gfx_SetTextXY(11, 8);	//1UP
	gfx_PrintUInt(game.score, 6);
#if 0
	gfx_SetTextXY(263, 8);	//2UP
	gfx_PrintUInt(0, 6);
#endif
	if (game.score > game.Hscore)
		game.Hscore = game.score;
	gfx_SetTextXY(200, 8);	//HIGH  SCORE
	gfx_PrintUInt(game.Hscore, 6);
	gfx_BlitLines(gfx_buffer, 8, 7);	//1UP, HIGH  SCORE and 2UP
}

#define BONUS_BOX_X 273
#define BONUS_BOX_Y 16

/* Draw the bonus timer value */
void draw_bonus_time(void) {
	gfx_SetTextFGColor(COLOR_LADDER);
	if (game.bonusTimer < 10)
		gfx_SetTextFGColor(COLOR_RED);
	gfx_SetTextXY(BONUS_BOX_X + 6, BONUS_BOX_Y + 8);
	gfx_PrintUInt(game.bonusTimer * 100, 4);
	gfx_BlitArea(gfx_buffer, BONUS_BOX_X + 6, BONUS_BOX_Y + 8, 31, 7);
}

/* Draws the bonus box(not included in draw_overlay_full because it is not shown on the menu screens) */
void draw_bonus_box(void) {
	gfx_SetColor(COLOR_FLOOR);	// Inner square
	gfx_Rectangle_NoClip(BONUS_BOX_X + 2, BONUS_BOX_Y + 6, 39, 11);
	gfx_Sprite_NoClip(text_bonus, BONUS_BOX_X + 5, BONUS_BOX_Y);
	gfx_SetColor(COLOR_LADDER);	// Outer square
	gfx_VertLine_NoClip(BONUS_BOX_X, BONUS_BOX_Y + 1, 17);
	gfx_VertLine_NoClip(BONUS_BOX_X + 42, BONUS_BOX_Y + 1, 17);
	gfx_HorizLine_NoClip(BONUS_BOX_X + 1, BONUS_BOX_Y, 3);
	gfx_HorizLine_NoClip(BONUS_BOX_X + 39, BONUS_BOX_Y, 3);
	gfx_HorizLine_NoClip(BONUS_BOX_X + 1, BONUS_BOX_Y + 18, 41);

	draw_bonus_time();
}

/* Draw the full overlay, so text and scores */
void draw_overlay_full(void) {
	uint8_t i;

	// Draw the strings
	gfx_SetTextFGColor(COLOR_RED);	// Set the text color to red
	gfx_PrintStringXY("1UP", 28, 0);			// 1UP
	gfx_PrintStringXY("HIGH  SCORE", 185, 0);	// HIGH SCORE
	//gfx_PrintStringXY("2UP", 279, 0);			// 2UP

	gfx_SetTextFGColor(COLOR_BLUE); // Set the text color to blue
	gfx_PrintStringXY("L=", 272, 0);			// L=

	gfx_SetTextXY(287, 0);						// L= NUMBER
	gfx_PrintUInt(game.level, 2);

	// Draw the lives
	for (i = 1; i < game.lives; i++)
		gfx_Sprite_NoClip(life, 3 + i * 8, 24);

	// Draw the scores and bonus time
	draw_player_score();
}
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
#include "overlay.h"
#include "defines.h"
#include "gfx/sprites_gfx.h"


/* This flashes the 1up text every 1/4th of a second */
void flash_1up(void) {
	if ((frameCounter & 15) == 0) {
		if (((frameCounter >> 4) & 1) == 1) {
			// Set the text color to black to remove text
			gfx_SetTextFGColor(COLOR_BACKGROUND);
		}
		else {
			// Set the text color to red to draw text
			gfx_SetTextFGColor(COLOR_RED);
		}
		gfx_PrintStringXY("1UP", 27, 0);	// 1UP
		gfx_BlitRectangle(gfx_buffer, 28, 0, 22, 7);
	}
}

/* Draw the lives */
void draw_player_lives(void) {
	uint8_t i;

	for (i = 1; i < game_data.lives; i++)
		gfx_Sprite_NoClip(life, 3 + i * 8, 24);
	gfx_BlitRectangle(gfx_buffer, 11, 24, 47, 8);
}

/* Draw the 1UP, HIGH SCORE and 2UP */
void draw_player_score(void) {
	unsigned score;
	score = game_data.Hscore[0];

	gfx_SetTextFGColor(COLOR_WHITE); //Set the text color to white
	gfx_SetTextXY(11, 8);	//1UP
	gfx_PrintUInt(game_data.score, 6);
#if 0
	gfx_SetTextXY(263, 8);	//2UP
	gfx_PrintUInt(0, 6);
#endif
	if (game_data.score >= score)
		score = game_data.score;
	if (!game_data.extraLifeAwarded && game_data.score >= ExtraLifeThreshold) {
		game_data.extraLifeAwarded = true;
		game_data.lives++;
		draw_player_lives();
	}
	gfx_SetTextXY(200, 8);	//HIGH  SCORE

	gfx_PrintUInt(score, 6);
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
	gfx_FillScreen(COLOR_BACKGROUND);

	// Draw the strings
	gfx_SetTextFGColor(COLOR_RED);	// Set the text color to red
	//gfx_PrintStringXY("1UP", 27, 0);			// 1UP
	gfx_PrintStringXY("HIGH%SCORE", 184, 0);	// HIGH SCORE
	//gfx_PrintStringXY("2UP", 279, 0);			// 2UP

	gfx_SetTextFGColor(COLOR_BLUE); // Set the text color to blue
	gfx_PrintStringXY("L=", 272, 0);			// L=

	gfx_SetTextXY(288, 0);						// L= NUMBER
	gfx_PrintUInt(game_data.level, 2);

	// Draw the scores, bonus time and lives
	draw_player_score();
	draw_player_lives();
}

char *rank_num[5] = { "ST", "ND", "RD", "TH", "TH" };

/* Draws the rankings to the buffer*/
void draw_rankings(void) {
	uint8_t i = 0, y;
	char str[4];

	for (y = 144; y < 144 + 5 * 16; y += 16) {
		gfx_SetTextFGColor((i < 3) ? COLOR_RED : COLOR_YELLOW);
		sprintf(str, "%d%s", i + 1, rank_num[i]);
		gfx_PrintStringXY(str, 57, y);
		gfx_PrintStringXY(game_data.name[i], 160, y);
		gfx_SetTextXY(97, y);
		gfx_PrintUInt(game_data.Hscore[i], 6);
		i++;
	}

	gfx_SetTextFGColor(COLOR_GREEN);
	gfx_PrintStringXY("VERSION%1[0", 178, 232);

	gfx_SetTextFGColor(COLOR_LIGHT_BLUE);
}
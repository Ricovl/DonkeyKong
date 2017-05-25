// standard headers
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>
#include <intce.h>
#include <debug.h>

// shared libraries
#include <graphx.h>
#include <keypadc.h>
#include <fileioc.h>

// donkeykong stuff
#include "defines.h"
#include "screens.h"
#include "overlay.h"
#include "images.h"
#include "stages.h"
#include "kong.h"
#include "font.h"


static uint8_t keyDelay = 0;
static kb_key_t prevKey = 0;

// KeyNum = log(kb_key) / log(2) + group * 8
sk_key_t get_key_fast(void) {
	uint8_t i;
	sk_key_t key = 0;

	for (i = 1; i <= 7; i++) {
		if (kb_Data[i] != 0) {
			key = log(kb_Data[i]) / log(2);
			key += i * 8;
		}
	}

	if (keyDelay == 0 || prevKey != key) {
		keyDelay = 15;
		prevKey = key;
	}
	else {
		keyDelay--;
		key = 0;
	}

	return key;
}

void reset_game_data(void) {
	memset(&game, 0, sizeof(game_t));
	game.stage = 0xFF;
	game_data.lives = 3;
	game_data.level = 1;
}

static const char *save_name = "DKONGSV";

void load_progress(void) {
	ti_var_t variable;
	uint8_t i;

	reset_game_data();
	memset(&game_data, 0, 7);
	for (i = 0; i < 5; i++)
		strcpy(game_data.name[i], "RICO");
	memcpy(game_data.Hscore, high_score_table, 5 * sizeof(unsigned));

	ti_CloseAll();
	if ((variable = ti_Open(save_name, "r"))) {
		ti_Read(&game_data, sizeof(game_data_t), 1, variable);
	}
	ti_CloseAll();
}

void save_progress(void) {
	ti_var_t variable;

	ti_CloseAll();
	if ((variable = ti_Open(save_name, "w"))) {
		ti_Write(&game_data, sizeof(game_data_t), 1, variable);
		ti_SetArchiveStatus(true, variable);
	}
	ti_CloseAll();
	gfx_End();
	prgm_CleanUp();
}

static uint8_t option = 0;

/* Clears the screen and returns to the main screen */
void return_main(void) {
	handle_waitTimer();

	gfx_SetPalette(sprites_gfx_pal, sizeof(sprites_gfx_pal), 0);
	timer_1_ReloadValue = timer_1_Counter = (ONE_TICK);

	game.stage = 0xFF;

	draw_overlay_full();
	draw_rankings();
	gfx_PrintStringXY("RANK", 57, 128);
	gfx_PrintStringXY("SCORE", 105, 128);
	gfx_PrintStringXY("NAME", 161, 128);
	gfx_Blit(gfx_buffer);

	game_state = main_screen;
}

/* Main screen from where you can select to continue or play a new game */
void main_screen(void) {
	sk_key_t key;

	// Clear the menu text
	gfx_FillRectangle_NoClip(125, 48, 69, 48);
	gfx_SetTextFGColor(COLOR_LADDER);

	// Draw a line underneath and above the selected option
	gfx_SetColor(COLOR_LADDER);
	gfx_HorizLine_NoClip(127, 59 + 17 * option, 65);
	gfx_HorizLine_NoClip(127, 49 + 17 * option, 65);

	// Draw the options
	gfx_SetColor(COLOR_BACKGROUND);
	gfx_PrintStringXY("CONTINUE", 128, 51);
	gfx_PrintStringXY("NEW%GAME", 128, 68);
	gfx_PrintStringXY("SETTINGS", 128, 85);		

	// Handle keypresses
	key = get_key_fast();

	if (key == 56 && option < 2) {
		option++;
	}
	if (key == 59 && option > 0) {
		option--;
	}
	if ((key == 13 || key == 48) && option <= 1) {		// Continue or New Game
		game_state = pre_round_screen;
		
		if (option == 1 || game_data.lives == 0) {
			reset_game_data();
			game_state = intro_cinematic;
		}
	}

	if (game.quit) {
		/* Usual cleanup */
		save_progress();
		exit(0);
	}
}

typedef struct {
	uint8_t cursorX;
	uint8_t cursorY;
	uint8_t rankNum;
	uint8_t charNum;
	uint8_t timer;
} HighScore_t;
HighScore_t HighScore;

void pre_name_registration(void) {
	uint8_t i;

	handle_waitTimer();

	game_state = return_main;

	// Check if high score and place score in list and ask for name if high score
	for (i = 0; i < 5; i++) {
		if (game_data.score >= game_data.Hscore[i]) {
			memcpy(&game_data.Hscore[i + 1], &game_data.Hscore[i], (4 - i) * sizeof(unsigned));
			memcpy(&game_data.name[i + 1], &game_data.name[i], 6 - i);
			game_data.Hscore[i] = game_data.score;

			waitTimer = 0;
			HighScore.rankNum = i;
			game_state = name_registration_screen;
			break;
		}
	}
}

/* Name registration screen */
void name_registration_screen(void) {
	uint8_t i, yc = 72, xc = 89;
	sk_key_t key;

	// First time this is run; initialize variables
	if (waitTimer == 0) {
		uint8_t x;

		memset(&game_data.name[HighScore.rankNum], '%', 5);
		HighScore.timer = 31;
		HighScore.charNum = 0;
		HighScore.cursorX = 84;
		HighScore.cursorY = 68;

		// Draw stuff
		gfx_SetPalette(sprites_gfx_pal, sizeof(sprites_gfx_pal), 0);
		draw_overlay_full();
		draw_rankings();

		// Draw text
		gfx_PrintStringXY("NAME:", 121, 48);
		gfx_PrintStringXY("REGI%TIME%%<%%>", 97, 128);
		gfx_SetTextFGColor(COLOR_RED);
		gfx_PrintStringXY("NAME%REGISTRATION", 89, 32);

		// Draw lines next to name:
		gfx_SetColor(COLOR_LADDER);
		for (x = 161; x < 161 + 5 * 8; x += 8)
			gfx_HorizLine_NoClip(x, 57, 6);
		gfx_Blit(gfx_buffer);

		waitTimer = 1;
	}

	// Draw the screen
	gfx_SetColor(COLOR_BACKGROUND);
	gfx_FillRectangle_NoClip(84, 68, 162, 48);

	gfx_SetTextFGColor(COLOR_GREEN);
	for (i = 0; i < 28; i++) {
		gfx_SetTextXY(xc, yc);
		gfx_PrintChar(i + 65);
		xc += 16;
		if (xc > 89 + 16 * 9) {
			xc = 89;
			yc += 16;
		}
	}
	gfx_Sprite_NoClip(text_rub_end, 213, 106);

	gfx_SetTextFGColor(COLOR_LADDER);
	gfx_PrintStringXY(game_data.name[HighScore.rankNum], 160, 48);

	gfx_SetColor(COLOR_WHITE);
	gfx_Rectangle_NoClip(HighScore.cursorX, HighScore.cursorY, 16, 16);
	gfx_SetColor(COLOR_LADDER);
	gfx_Rectangle_NoClip(HighScore.cursorX + 1, HighScore.cursorY + 1, 14, 14);

	// Handle the Timer
	waitTimer++;
	if (waitTimer == 62) {

		if (HighScore.timer == 0) {
			waitTimer = 0x80;
			game_state = return_main;
			return;
		}

		gfx_SetTextFGColor(COLOR_LADDER);
		gfx_SetTextXY(192, 128);
		gfx_PrintUInt(--HighScore.timer, 2);
		gfx_BlitRectangle(gfx_buffer, 192, 128, 16, 8);

		waitTimer = 1;
	}

	// Make the score flicker
	if ((frameCounter & 15) == 0) {
		uint8_t yt = 144 + 16 * HighScore.rankNum;

		if (((frameCounter >> 4) & 1) == 1)
			gfx_SetTextFGColor(COLOR_BACKGROUND);
		else
			gfx_SetTextFGColor(COLOR_RED);

		gfx_SetTextXY(97, yt);
		gfx_PrintUInt(game_data.score, 6);
		gfx_BlitRectangle(gfx_buffer, 97, yt, 48, 7);
	}

	// Handle key presses
	key = get_key_fast();

	if (key == 13 || key == 48) {
		uint8_t character = ((HighScore.cursorX - 84) / 15) + 10 * ((HighScore.cursorY - 68) / 15);
		if (character == 28) {
			game_data.name[HighScore.rankNum][HighScore.charNum] = '%';
			if (HighScore.charNum > 0)
				HighScore.charNum--;
		}
		else if (character != 29) {
			game_data.name[HighScore.rankNum][HighScore.charNum] = character + 65;
			if (HighScore.charNum < 4)
				HighScore.charNum++;
		}
		else {
			draw_rankings();
			gfx_PrintStringXY("YOUR%NAME%WAS%REGISTERED", 89, 32);
			gfx_Blit(gfx_buffer);
			waitTimer = 0x80;
			game_state = return_main;
			return;
		}
	}

	if (key == 58) {
		if (HighScore.cursorX < 228)
			HighScore.cursorX += 16;
		else
			HighScore.cursorX = 84;
	}
	if (key == 57) {
		if (HighScore.cursorX > 84)
			HighScore.cursorX -= 16;
		else
			HighScore.cursorX = 228;
	}
	if (key == 59) {
		if (HighScore.cursorY > 68)
			HighScore.cursorY -= 16;
	}
	if (key == 56) {
		if (HighScore.cursorY < 100)
			HighScore.cursorY += 16;
	}
}

/* How high can you get? screen */
void pre_round_screen(void) {
	uint8_t i, y = 209;
	uint8_t kongs = game_data.round + 2;
	char str[6];

	draw_overlay_full();

	gfx_SetTextFGColor(COLOR_WHITE);
	gfx_PrintStringXY("HOW%HIGH%CAN%YOU%GET%?", 73, 224);
	gfx_SetFontData((&font_data) - 47 * 8);

	for (i = 0; i <= game_data.level && i <= 5; i++)
		kongs -= i;

	for (i = 1; i <= kongs; i++) {
		gfx_Sprite_NoClip(kong_goofy, 137, y - 25);
		sprintf(str, "%d/E", i * 25);
		gfx_PrintStringXY(str, 120 - gfx_GetStringWidth(str), y);
		y -= 32;
	}
	gfx_SetFontData((&font_data) - 37 * 8);

	gfx_Blit(gfx_buffer);
	
	waitTimer = 0xA0;
	game_state = initialize_stage;
}

/* Draws the rankings to the buffer */
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
	gfx_PrintStringXY("VERSION%0[7", 178, 232);

	gfx_SetTextFGColor(COLOR_LADDER);
}


char *rank_num[5] = { "ST", "ND", "RD", "TH", "TH" };
unsigned high_score_table[5] = { 7650, 6100, 5950, 5050, 4300 };
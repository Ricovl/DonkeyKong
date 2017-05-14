#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <tice.h>
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
#include "kong.h"
#include "font.h"


void reset_game_data(void) {
	memset(&game, 0, sizeof(game_t));
	memset(&game_data, 0, 7);
	game_data.lives = 3;
	game_data.level = 1;
}

static const char *save_name = "DKONGSV";

void load_progress(void) {
	ti_var_t variable;
	uint8_t i;

	reset_game_data();
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

/* Main screen from where you can select to continue or play a new game */
void main_screen(void) {
	sk_key_t key, option = 0;

	gfx_FillScreen(COLOR_BACKGROUND);
	draw_overlay_full();
	draw_rankings();

	gfx_PrintStringXY("RANK", 57, 128);
	gfx_PrintStringXY("SCORE", 105, 128);
	gfx_PrintStringXY("NAME", 161, 128);
	gfx_SetTextFGColor(COLOR_LADDER);
	gfx_Blit(gfx_buffer);

	os_GetCSC();
	while (os_GetCSC() != 0);
	key = 1;

	while (key != sk_Enter && key != sk_2nd) {
		if (key) {
			gfx_SetColor(COLOR_BACKGROUND);
			gfx_FillRectangle_NoClip(125, 48, 69, 48);

			gfx_PrintStringXY("CONTINUE", 128, 51);
			gfx_PrintStringXY("NEW%GAME", 128, 68);
			gfx_PrintStringXY("SETTINGS", 128, 85);

			gfx_SetColor(COLOR_LADDER);
			gfx_HorizLine_NoClip(127, 59 + 17 * option, 65);
			gfx_HorizLine_NoClip(127, 49 + 17 * option, 65);			

			gfx_SwapDraw();
		}

		key = os_GetCSC();

		if (key == sk_Down && option < 1) {
			option++;
		}
		if (key == sk_Up && option > 0) {
			option--;
		}
		if (key == sk_Clear) {
			/* Usual cleanup */
			save_progress();
			exit(0);
		}
	}

	gfx_SetColor(COLOR_BACKGROUND);
	if (option <= 1) {		// Continue or New Game
		if (option == 1 || game_data.lives == 0) {
			reset_game_data();
			intro_cinematic();
		}
	}
}

/* Name registration screen */
void name_registration_screen(uint8_t ranking) {
	char name[6];
	sk_key_t key = 1;
	uint8_t x, y, num = 0;
	uint8_t timer = 31 * 4;

	memset(&name, '%', 5);
	name[5] = '\0';

	timer_1_ReloadValue = QUARTER_SECOND;
	timer_1_Counter = 0;

	// Draw everything
	gfx_SetPalette(sprites_gfx_pal, sizeof(sprites_gfx_pal), 0);
	gfx_FillScreen(COLOR_BACKGROUND);
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

	x = 84, y = 68;
	do {
		if (key) {
			uint8_t i, yc = 72, xc = 89;

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
			gfx_PrintStringXY(name, 160, 48);

			gfx_SetColor(COLOR_WHITE);
			gfx_Rectangle_NoClip(x, y, 16, 16);
			gfx_SetColor(COLOR_LADDER);
			gfx_Rectangle_NoClip(x + 1, y + 1, 14, 14);
			gfx_SwapDraw();
		}

		key = os_GetCSC();

		if (timer_IntStatus & TIMER1_RELOADED) {
			uint8_t yt = 144 + 16 * ranking;

			if (timer == 0)
				break;
			gfx_SetTextFGColor(COLOR_LADDER);
			gfx_SetTextXY(192, 128);
			gfx_PrintUInt(--timer / 4, 2);
			gfx_BlitRectangle(gfx_buffer, 192, 128, 16, 8);

			// Flash the high score
			if ((timer & 1) == 1) {
				// Set the text color to black to remove text
				gfx_SetTextFGColor(COLOR_BACKGROUND);
			}
			else {
				// Set the text color to red to draw text
				gfx_SetTextFGColor((ranking < 3) ? COLOR_RED : COLOR_YELLOW);
			}
			gfx_SetTextXY(97, yt);
			gfx_PrintUInt(game_data.score, 6);
			gfx_BlitRectangle(gfx_buffer, 97, yt, 48, 7);

			/* Acknowledge the reload */
			timer_IntAcknowledge = TIMER1_RELOADED;
		}

		if (key == sk_Enter || key == sk_2nd) {
			uint8_t character = ((x - 84) / 15) + 10 * ((y - 68) / 15);
			if (character == 28) {
				name[num] = '%';
				if (num > 0)
					num--;
			}
			else if (character == 29) {
				break;
			}
			else {
				name[num] = character + 65;
				if (num < 4)
					num++;
			}
		}

		if (key == sk_Right) {
			if (x < 228)
				x += 16;
			else
				x = 84;
		}
		if (key == sk_Left) {
			if (x > 84)
				x -= 16;
			else
				x = 228;
		}
		if (key == sk_Up) {
			if (y > 68)
				y -= 16;
		}
		if (key == sk_Down) {
			if (y < 100)
				y += 16;
		}
	} while (key != sk_Clear);

	strcpy(game_data.name[ranking], name);
}

/* How high can you get? screen */
void pre_round_screen(void) {
	uint8_t i, y = 209;
	uint8_t kongs = game_data.round + 2;
	char str[6];

	gfx_FillScreen(COLOR_BACKGROUND);
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

	gfx_SwapDraw();
	waitTicks(160);
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
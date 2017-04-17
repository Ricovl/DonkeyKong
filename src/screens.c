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

// donkeykong stuff
#include "defines.h"
#include "screens.h"
#include "overlay.h"
#include "images.h"
#include "kong.h"
#include "font.h"


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

	while (key != sk_Enter) {
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

		if (key == sk_Down && option < 2) {
			option++;
		}
		if (key == sk_Up && option > 0) {
			option--;
		}
		if (key == sk_Clear) {
			/* Usual cleanup */
			gfx_End();
			prgm_CleanUp();
			exit(0);
		}
	}

	gfx_SetColor(COLOR_BACKGROUND);
	if (option == 1)
		intro_cinematic();
}

/* Name registration screen */
void name_registration(void) {
	char name[6];
	sk_key_t key = 1;
	uint8_t x, y, num = 0;
	uint8_t timer = 31;

	memset(&name, '%', 5);
	name[5] = '\0';

	timer_1_ReloadValue = ONE_SECOND;
	timer_1_Counter = 0;

	gfx_FillScreen(COLOR_BACKGROUND);
	draw_overlay_full();
	draw_rankings();

	gfx_PrintStringXY("NAME:", 121, 48);
	gfx_PrintStringXY("REGI%TIME%%<%%>", 97, 128);
	gfx_SetTextFGColor(COLOR_RED);
	gfx_PrintStringXY("NAME%REGISTRATION", 89, 32);

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
			gfx_SetTextXY(192, 128);
			gfx_PrintUInt(--timer, 2);
			gfx_BlitRectangle(gfx_buffer, 192, 128, 16, 8);

			/* Acknowledge the reload */
			timer_IntAcknowledge = TIMER1_RELOADED;
		}

		if (key == sk_Enter) {
			uint8_t character = ((x - 84) / 15) + 10 * ((y - 68) / 15);
			if (character == 28) {
				name[num] = '%';
				if (num > 0)
					num--;
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
}

/* How high can you get? screen */
void pre_round_screen(void) {
	uint8_t i, y;

	gfx_FillScreen(COLOR_BACKGROUND);
	draw_overlay_full();

	gfx_SetTextFGColor(COLOR_WHITE);
	gfx_PrintStringXY("HOW%HIGH%CAN%YOU%GET%?", 73, 224);
	gfx_SetFontData((&font_data) - 47 * 8);
	y = 209;
	for (i = 1; i < game.round + 1; i++) {
		gfx_PrintStringXY("E", 113, y); // Draw the m
		gfx_SetTextXY(88 - 8 * (i >= 4), y);
		gfx_PrintUInt(i * 25, 2 + (i >= 4));
		gfx_Sprite_NoClip(kong_goofy, 137, y - 25);
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
		gfx_SetTextXY(97, y);
		gfx_PrintUInt(0, 6);
		i++;
	}

	gfx_SetTextFGColor(COLOR_GREEN);
	gfx_PrintStringXY("VERSION%0[7", 178, 232);

	gfx_SetTextFGColor(COLOR_LADDER);
}


char *rank_num[5] = { "ST", "ND", "RD", "TH", "TH" };
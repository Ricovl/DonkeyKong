#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <tice.h>
#include <debug.h>

// shared libraries
#include <lib/ce/graphx.h>
#include <lib/ce/keypadc.h>

// donkeykong stuff
#include "defines.h"
#include "scorescreen.h"
#include "overlay.h"
#include "images.h"


const char *rank_num[] = { "ST", "ND", "RD", "TH", "TH" };


void draw_rankings() {
	uint8_t i, y;
	char str[4];

	i = 0;
	for (y = 144; y < 144 + 5 * 16; y += 16) {
		gfx_SetTextFGColor((i < 3) ? COLOR_RED : COLOR_YELLOW);
		sprintf(str, "%d%s", i + 1, rank_num[i]);
		gfx_PrintStringXY(str, 57, y);
		gfx_SetTextXY(97, y);
		gfx_PrintUInt(0, 6);
		i++;
	}
}

/* W.I.P. name registration function */
void name_registration(void) {
	uint8_t i;
	uint8_t x, y;
	sk_key_t key = 1;

	gfx_FillScreen(COLOR_BACKGROUND);
	draw_overlay_full();

	gfx_SetTextFGColor(COLOR_RED);
	gfx_PrintStringXY("NAME  REGISTRATION", 89, 32);
	gfx_SetTextFGColor(COLOR_LADDER);
	gfx_PrintStringXY("NAME :", 121, 48);
	gfx_PrintStringXY("REGI  TIME", 97, 128);


	gfx_SetTextFGColor(COLOR_GREEN);

	i = 0;
	for (y = 72; y < 72 + 16 * 3; y += 16) {
		for (x = 89; x < 89 + 16 * 10; x += 16) {
			gfx_SetTextXY(x, y);
			gfx_PrintChar(i + 65);
			i++;
		}
	}

	draw_rankings();

	i = 0, x = 84, y = 68;
	do {
		if (key) {
			gfx_SetColor(COLOR_WHITE);
			gfx_Rectangle_NoClip(x, y, 16, 16);
			gfx_SetColor(COLOR_LADDER);
			gfx_Rectangle_NoClip(x + 1, y + 1, 14, 14);

		}

		key = os_GetCSC();

		if (key) {
			gfx_SetColor(COLOR_BACKGROUND);
			gfx_Rectangle_NoClip(x, y, 16, 16);
			gfx_Rectangle_NoClip(x + 1, y + 1, 14, 14);
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


void pre_round_screen(void) {
	uint8_t i, y;

	gfx_FillScreen(COLOR_BACKGROUND);
	draw_overlay_full();

	gfx_SetTextFGColor(COLOR_WHITE);
	gfx_PrintStringXY("HOW HIGH CAN YOU GET ?", 73, 224);
	y = 208;
	for (i = 1; i < game.round + 1; i++) {
		gfx_PrintStringXY("m", 113, y);
		gfx_SetTextXY(89 - 8 * (i >= 4), y);
		gfx_PrintUInt(i * 25, 2 + (i >= 4));
		gfx_Sprite_NoClip(kong_goofy, 137, y - 24);
		y -= 32;
	}

	gfx_SwapDraw();
}
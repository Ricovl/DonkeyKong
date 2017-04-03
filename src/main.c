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
#include <lib\ce\graphx.h>
#include <lib\ce\keypadc.h>

// donkeykong stuff
#include "defines.h"
#include "jumpman.h"
#include "barrels.h"
#include "bonus_scores.h"
#include "drawsprites.h"
#include "firefoxes.h"
#include "kong.h"
#include "overlay.h"
#include "conveyors.h"
#include "scorescreen.h"
#include "stages.h"
#include "elevators.h"
#include "images.h"


game_t game;
jumpman_t jumpman;

bonus_item_t bonus_item[3];
uint8_t num_bonus_items;
bool rivet_enabled[8];
uint8_t num_rivets;

hammer_t hammer[2];
uint8_t num_hammers;
uint8_t hammerActive = false;
uint8_t hammerTimer = 0;
uint8_t hammerLength = 0;

uint8_t frameCounter;


bool check_end_stage(void);
void flash_1up(void);
void increase_difficulty(void);
void handle_bonus_timer(void);
void handle_time_ran_out(void);
void handle_rivets(void);


void main(void) {
	bool quit = false;
	uint8_t i = 0;

	malloc(0);
	srand(rtc_Time());
	gfx_Begin(gfx_8bpp);

	gfx_SetPalette(sprites_gfx_pal, sizeof(sprites_gfx_pal), 0);
	gfx_SetClipRegion(48, 16, 272, 239);
	gfx_SetTextBGColor(COLOR_BACKGROUND);
	gfx_SetTransparentColor(0x15);

	decompress_images();
	gfx_SetDrawBuffer();
	
	memset(&game, 0, sizeof(game_t));
	game.lives = 3;
	game.level = 1;
	game.round = 1;
	game.stage = STAGE_BARRELS;

	timer_Control = TIMER1_DISABLE;
	timer_1_ReloadValue = timer_1_Counter = (ONE_TICK);
	// Enable the timer, set it to the 32768 kHz clock, enable an interrupt once it reaches 0, and make it count down
	timer_Control = TIMER1_ENABLE | TIMER1_32K | TIMER1_0INT | TIMER1_DOWN;

	do {
		// how high can you get?
		pre_round_screen();
		for (i = 0; i < 90; i++) {
			while (!(timer_IntStatus & TIMER1_RELOADED));
			timer_IntStatus = TIMER1_RELOADED;
		}

		initialize_stage(game.stage);

		do {
			flash_1up();	// make this interrupt?
			increase_difficulty();

			update_bonus_scores();

			move_jumpman();

			move_barrels();
			deploy_barrel();
			spawn_barrel();

			update_firefoxes();

			move_bouncers();

			move_pies();

			release_firefox();

			hammer_stuff();	// wrong place?

			move_retractable_ladders();

			handle_rivets();

			jumpman_falling();

			move_elevators();

			handle_conveyor_dirs();

			bonus_item_picked_up();

			update_kong();

			// ..collision check

			handle_time_ran_out();

			handle_bonus_timer();

			update_screen();

			while (!(timer_IntStatus & TIMER1_RELOADED));	// Wait until the timer has reloaded
			timer_IntStatus = TIMER1_RELOADED;	// Acknowledge the reload

			frameCounter--;

			if (kb_Data[kb_group_6] & kb_Clear)
				quit = true;
		} while (!(quit) && jumpman.isAlive && !check_end_stage());

		jumpman.sprite = num_barrels = num_firefoxes = num_bonus_scores = 0;
		update_screen();

		if (!jumpman.isAlive)
			animate_jumpman_dead();
		else if(!quit) {
			end_stage_cinematic();
			next_stage();
		}

	} while (!(quit));

	/* Usual cleanup */
	free(kong_goofy);
	gfx_End();
	prgm_CleanUp();
}


/* Checks if jumpman is standing on the place where the stage ends */
bool check_end_stage(void) {

	if ((game.stage & 1) == 1) {
		if (jumpman.y > 39)
			return false;
	}
	else if (game.stage == STAGE_RIVETS) {
		if (num_rivets)
			return false;
	}
	else {	// stage conveyors
		if (jumpman.y > 71)
			return false;
	}

	// Set sprite based on wich half of the screen jumpman is
	if (jumpman.x > 160)
		jumpman.dir = FACE_LEFT;
	else
		jumpman.dir = FACE_RIGHT;

	return true;
}

void handle_time_ran_out(void) {
	if (game.timeRanOut) {
		if (game.timeRanOut == 1) {
			game.timeRanOutTimer = 0;
			game.timeRanOut = 2;
		}
		else if (game.timeRanOut == 2) {
			game.timeRanOutTimer--;
			if (game.timeRanOutTimer == 0) {
				game.timeRanOut = 3;
			}
		}
		else if (game.timeRanOut == 3) {
			if (!jumpman.isJumping) {
				game.timeRanOut = false;
				jumpman.isAlive = false;
			}
		}
	}
}

/* Counts down the bonusTimer on non-barrel stages */
void handle_bonus_timer(void) {
	if (game.stage != STAGE_BARRELS) {
		game.bonusDelay--;
		if (game.bonusDelay == 0) {
			game.bonusDelay = game.initialBonusDelay;

			// Release fire
			oilcan.fireRelease = 3;
			// Release bouncer
			releaseBouncer = true;

			game.bonusTimer--;
			draw_bonus_time();
			if (game.bonusTimer == 0)
				game.timeRanOut = true;
		}
	}
}

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
		gfx_PrintStringXY("1UP", 28, 0);	// 1UP
		gfx_BlitRectangle(gfx_buffer, 28, 0, 22, 7);
	}
}

/* Increases the difficulty about every 33 seconds */
void increase_difficulty(void) {
	if (game.difficultyTimer0 == 0) {
		if (((game.difficultyTimer1) & 7) == 0) {
			game.difficulty = game.level + (game.difficultyTimer1 >> 3);
			if (game.difficulty > 5)
				game.difficulty = 5;
		}
		game.difficultyTimer1++;
	}
	game.difficultyTimer0++;
}

void handle_rivets(void) {
	if (game.stage == STAGE_RIVETS && (jumpman.x == 107 + jumpman.dir || jumpman.x == 211 + jumpman.dir) && jumpman.y < 192) {
		uint8_t y, rivetNum = 0;

		for (y = 71; y <= 191; y += 40) {
			if (y - jumpman.y >= 0 && y - jumpman.y < 20) {
				uint8_t x;

				if (jumpman.x < 160) {
					x = 104;
				}
				else {
					x = 208;
					rivetNum += 4;
				}
				
				if (rivet_enabled[rivetNum]) {
					rivet_enabled[rivetNum] = false;

					gfx_FillRectangle_NoClip(x, y, 8, 9);
					gfx_BlitRectangle(gfx_buffer, x, y, 8, 9);

					spawn_bonus_score(0, jumpman.x - 6, jumpman.y + 9);
					num_rivets--;
				}

				return;
			}
			rivetNum++;
		}
	}
}

/*timer_Control = TIMER1_DISABLE;
timer_1_Counter = 0;
timer_Control = TIMER1_ENABLE | TIMER1_CPU | TIMER1_NOINT | TIMER1_UP;

timer_Control = TIMER1_DISABLE;
dbg_sprintf(dbgout, "timer_1_counter: %d\n", timer_1_Counter);*/

/* Stage order:
*		   0001		 0010		 0001	   0011		   0001		 0100
* lvl 1 : barrels											   - rivets
* lvl 2 : barrels						 - elevators		   - rivets
* lvl 3 : barrels - conveyors			 - elevators		   - rivets
* lvl 4 : barrels - conveyors - barrels - elevators		   - rivets
* lvl 5+: barrels - conveyors - barrels - elevators - barrels - rivets
*/

/*
; arrive here from #0701 when playing

 *197A  CALL    #1DBD         ; check for bonus items and jumping scores, rivets																			update_bonus_scores()
197D  CALL    #1E8C         ; do stuff for items hit with hammer
 *1980  CALL    #1AC3         ; mario movement																												update_jumpman()
 *1983  CALL    #1F72         ; roll barrels																												move_barrels()
 *1986  CALL    #2C8F         ; deploy barrels ?																											deploy_barrel()
 *1989  CALL    #2C03         ; do barrel deployment ? (initialize what the new barrel should become)														spawn_barrel()
 *198C  CALL    #30ED         ; update fires if needed																										update_firefoxes()
 *198F  CALL    #2E04         ; update bouncers if on elevators																								move_bouncers()
 *1992  CALL    #24EA         ; do stuff for pie factory																									move_pies()
 *1995  CALL    #2DDB         ; deploy fireball/firefoxes for conveyors and rivets																			release_firefox()
 *1998  CALL    #2ED4         ; do stuff for hammer																											hammer_stuff()		not done
 *199B  CALL    #2207         ; do stuff for conveyors(ladders)																								move_retractable_ladders()
 *199E  CALL    #1A33         ; check for and handle running over rivets																					handle_rivets()
 *19A1  CALL    #2A85         ; check for mario falling																										jumpman_falling()
 *19A4  CALL    #1F46         ; handle mario falling																										jumpman_falling()
 *19A7  CALL    #26FA         ; do stuff for elevators																										move_elevators()
 *19AA  CALL    #25F2         ; handle conveyor dirs, adjust Mario's speed based on conveyor dirs															handle_conveyor_dirs()
 *19AD  CALL    #19DA         ; check for mario picking up bonus item																						bonus_item_picked_up()
 *19B0  CALL    #03FB         ; check for kong beating chest and animate girl and her screams																update_kong()
19B3  CALL    #2808         ; check for collisions with hostile sprites [set to NOPS to make mario invincible to enemy sprites]
19B6  CALL    #281D         ; do stuff for hammers
 *19B9  CALL    #1E57         ; check for end of level																										check_end_stage()
 *19BC  CALL    #1A07         ; handle when the bonus timer has run out																						handle_time_ran_out()
 *19BF  CALL    #2FCB         ; for non-girder levels, checks for bonus timer changes. if the bonus counts down, sets a possible new fire to be released,	handle_bonus_timer()
  							  ; sets a bouncer to be deployed, updates the bonus timer onscreen, and checks for bonus time running out
*/


/* ToDo:
 * Reduce sprite data because total sprite data is 36165 bytes! 15042 bytes are from kong sprites.
 * Crazy barrels can escape out of the screen(leave artifacts)?
 * Fix offset of bonus_scores from jumpman facing left
 * Check for jumping over firefoxes, pies and flame
 * Make bonusTimer not show value below 0(or > 127) on end of not barrel stage
 * Check if firefox edge detection is exactly right
 * Fix jumpman edge of girder collision checking
 * add hammer hit animation and yellow sprite
 * end level(rivets only) and start game animations
 * start menu and end screen
 */
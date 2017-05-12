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
#include "jumpman.h"
#include "barrels.h"
#include "bonus_scores.h"
#include "drawsprites.h"
#include "firefoxes.h"
#include "kong.h"
#include "overlay.h"
#include "conveyors.h"
#include "rivets.h"
#include "screens.h"
#include "stages.h"
#include "elevators.h"
#include "images.h"
#include "font.h"
#include "hammers.h"


#define DEBUG_MODE 1

game_t game;
game_data_t game_data;

bonus_item_t bonus_item[3];
uint8_t num_bonus_items;

uint8_t frameCounter;

void flash_1up(void);
void increase_difficulty(void);
bool check_end_stage(void);
void handle_bonus_timer(void);
void handle_time_ran_out(void);

void check_collision_jumpman(void);
void check_collision_hammer(void);


void main(void) {
	bool debug = false;

	malloc(0);
	srand(rtc_Time());
	gfx_Begin(gfx_8bpp);

	gfx_SetPalette(sprites_gfx_pal, sizeof(sprites_gfx_pal), 0);
	gfx_SetClipRegion(48, 16, 272, 239);
	gfx_SetTransparentColor(0x15);

	gfx_SetTextBGColor(COLOR_BACKGROUND);
	gfx_SetFontData((&font_data) - 37 * 8);
	gfx_SetMonospaceFont(8);

	decompress_images();
	gfx_SetDrawBuffer();

	load_progress();

	// Enable the timer, set it to the 32768 kHz clock, enable an interrupt once it reaches 0, and make it count down
	timer_Control = TIMER1_ENABLE | TIMER1_32K | TIMER1_0INT | TIMER1_DOWN;

	for (;;) {
		bool quit = false;
		timer_1_ReloadValue = timer_1_Counter = (ONE_TICK);

		main_screen();

		do {
			game.stage = stage_order[game_data.round];
			
			pre_round_screen();	// how high can you get?
			initialize_stage(game.stage);

			do {
				flash_1up();	// make this interrupt?
				increase_difficulty();

				update_bonus_scores();

				animate_hammer_hit();

				move_jumpman();

				move_barrels();
				deploy_barrel();
				spawn_barrel();

				update_firefoxes();

				move_bouncers();

				move_pies();

				release_firefox();

				animate_hammer();	// wrong place?

				move_retractable_ladders();

				handle_rivets();

				check_jumpman_falling();

				handle_jumpman_falling();

				move_elevators();

				handle_conveyor_dirs();

				bonus_item_picked_up();

				update_kong();

				//check_collision_jumpman();

				check_collision_hammer();

				handle_time_ran_out();

				handle_bonus_timer();

				update_screen();

				while (!(timer_IntStatus & TIMER1_RELOADED));	// Wait until the timer has reloaded
				timer_IntStatus = TIMER1_RELOADED;	// Acknowledge the reload

#if DEBUG_MODE
				if (debug) {
					while (!(kb_ScanGroup(kb_group_6) & kb_Add) && !(kb_Data[kb_group_6] & kb_Sub));
					if (kb_Data[kb_group_6] & kb_Sub)
						debug = false;
					while ((kb_ScanGroup(kb_group_6) & kb_Add) || (kb_Data[kb_group_6] & kb_Sub));
				}
				else if (kb_Data[kb_group_6] & kb_Sub) {
					debug = true;
					while ((kb_ScanGroup(kb_group_6) & kb_Sub));
				}
#endif

				frameCounter--;

				if (kb_Data[kb_group_6] & kb_Clear)
					quit = true;
			} while (!(quit) && jumpman.isAlive && !check_end_stage());

			num_barrels = num_firefoxes = num_bonus_scores = num_hammers = 0;

			if (!jumpman.isAlive) {
				animate_jumpman_dead();
				
				game_data.lives--;
				if (game_data.lives == 0) {
					uint8_t i;

					// Draw Game Over window
					gfx_SetDrawScreen();
					gfx_Sprite_NoClip((gfx_image_t*)jumpman.buffer_data, jumpman.x_old - 7, jumpman.y_old - 15);
					gfx_FillRectangle_NoClip(104, 144, 112, 40);
					gfx_SetTextFGColor(22);
					gfx_PrintStringXY("GAME%%OVER", 121, 160);
					gfx_SetDrawBuffer();
					waitTicks(0xC0);
					quit = true;

					// Check if high score and place score in list and ask for name if high score
					for (i = 0; i < 5; i++) {
						if (game_data.score >= game_data.Hscore[i]) {
							memcpy(&game_data.Hscore[i + 1], &game_data.Hscore[i], (4 - i) * sizeof(unsigned)); 
							memcpy(&game_data.name[i + 1], &game_data.name[i], 6 - i);
							game_data.Hscore[i] = game_data.score;
							// Register name
							name_registration(i);
							break;
						}
					}
				}
			}
			else if (!quit) {
				update_screen();
				end_stage_cinematic();
			}

			//gfx_FillScreen(COLOR_BACKGROUND);
			//gfx_SwapDraw();
			gfx_SetPalette(sprites_gfx_pal, sizeof(sprites_gfx_pal), 0);
		} while (!(quit));
	}
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
	jumpman.sprite = 0;

	return true;
}

/* Handles bonus timer when reached 0 */
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
	if (game.stage != STAGE_BARRELS && game.bonusTimer) {
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
		gfx_PrintStringXY("1UP", 27, 0);	// 1UP
		gfx_BlitRectangle(gfx_buffer, 28, 0, 22, 7);
	}
}

/* Increases the difficulty about every 33 seconds */
void increase_difficulty(void) {
	if (game.difficultyTimer0 == 0) {
		if (((game.difficultyTimer1) & 7) == 0) {
			game.difficulty = game_data.level + (game.difficultyTimer1 >> 3);
			if (game.difficulty > 5)
				game.difficulty = 5;
		}
		game.difficultyTimer1++;
	}
	game.difficultyTimer0++;
}

void waitTicks(uint8_t ticks) {
	timer_1_Counter = (ONE_TICK);
	while (--ticks) {
		while (!(timer_IntStatus & TIMER1_RELOADED));	// Wait until the timer has reloaded
		timer_IntStatus = TIMER1_RELOADED;				// Acknowledge the reload
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
197D  CALL    #1E8C         ; do stuff for items hit with hammer														 give points and animate hit		animate_hammer_hit()
 *1980  CALL    #1AC3         ; mario movement																												update_jumpman()
 *1983  CALL    #1F72         ; roll barrels																												move_barrels()
 *1986  CALL    #2C8F         ; deploy barrels ?																											deploy_barrel()
 *1989  CALL    #2C03         ; do barrel deployment ? (initialize what the new barrel should become)														spawn_barrel()
 *198C  CALL    #30ED         ; update fires if needed																										update_firefoxes()
 *198F  CALL    #2E04         ; update bouncers if on elevators																								move_bouncers()
 *1992  CALL    #24EA         ; do stuff for pie factory																									move_pies()
 *1995  CALL    #2DDB         ; deploy fireball/firefoxes for conveyors and rivets																			release_firefox()
 *1998  CALL    #2ED4         ; do stuff for hammer																											animate_hammer()		not done
 *199B  CALL    #2207         ; do stuff for conveyors(ladders)																								move_retractable_ladders()
 *199E  CALL    #1A33         ; check for and handle running over rivets																					handle_rivets()
 *19A1  CALL    #2A85         ; check for mario falling																										check_jumpman_falling()
 *19A4  CALL    #1F46         ; handle mario falling																										handle_jumpman_falling()
 *19A7  CALL    #26FA         ; do stuff for elevators																										move_elevators()
 *19AA  CALL    #25F2         ; handle conveyor dirs, adjust Mario's speed based on conveyor dirs															handle_conveyor_dirs()
 *19AD  CALL    #19DA         ; check for mario picking up bonus item																						bonus_item_picked_up()
 *19B0  CALL    #03FB         ; check for kong beating chest and animate girl and her screams																update_kong()
 *19B3  CALL    #2808         ; check for collisions with hostile sprites [set to NOPS to make mario invincible to enemy sprites]
 *19B6  CALL    #281D         ; do stuff for hammers																					hammer collision		
 *19B9  CALL    #1E57         ; check for end of level																										check_end_stage()
 *19BC  CALL    #1A07         ; handle when the bonus timer has run out																						handle_time_ran_out()
 *19BF  CALL    #2FCB         ; for non-girder levels, checks for bonus timer changes. if the bonus counts down, sets a possible new fire to be released,	handle_bonus_timer()
  							  ; sets a bouncer to be deployed, updates the bonus timer onscreen, and checks for bonus time running out
*/


/* ToDo:
 * Add collision detection for oilcan fire and kong in rivets
 * Change some things in spawn_bonus_score()
 * Change the way the game loop works so the 1UP flashes everywhere and the flame keeps animated when visible
 * check ground checking for all entities
 * Check if bouncers are still spawning on the correct place and if they are drawn on the right layer.
 * Fix that there is a quick change in color when you quit a stage(Don't know if it's visible on calc)
 */

/* In progress
 * start menu and end screen and splash screen with credits
 */


/* bugs:
 * problems with hammer sprite removing(might be fixed now)
 * Crazy barrels can escape out of the screen(leave artifacts)?
 * Fix jumpman in rivets cinematic
 * You can get unlimited points by quiting and then continueing
 */
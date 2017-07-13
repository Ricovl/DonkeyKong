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
#include "main.h"
#include "barrels.h"
#include "bonus_scores.h"
#include "conveyors.h"
#include "defines.h"
#include "drawsprites.h"
#include "elevators.h"
#include "firefoxes.h"
#include "font.h"
#include "hammers.h"
#include "images.h"
#include "jumpman.h"
#include "kong.h"
#include "overlay.h"
#include "rivets.h"
#include "screens.h"
#include "stages.h"


#define DEBUG_MODE	true

game_t game;
game_data_t game_data;

uint8_t waitTimer = 1;
uint8_t frameCounter;

const void(*game_state)(void) = credits_screen;

void main(void) {
#if DEBUG_MODE
	bool debug = false;
#endif
	uint8_t quitDelay = 0;

	srand(rtc_Time());
	gfx_Begin();

	gfx_SetClipRegion(48, 16, 272, 239);
	gfx_SetDrawBuffer();

	gfx_SetTextBGColor(COLOR_BACKGROUND);
	gfx_SetFontData(&font_data - 37 * 8);
	gfx_SetMonospaceFont(8);

	decompress_images();
	load_progress();

	// Enable the timer, set it to the 32768 kHz clock, enable an interrupt once it reaches 0, and make it count down
	timer_Control = TIMER1_ENABLE | TIMER1_32K | TIMER1_0INT | TIMER1_DOWN;

	// Game loop
	for (;;) {
		kb_Scan();

		flash_1up();
		increase_difficulty();
		
		(*game_state)();

		if (cinematicProgress == 0)
				update_screen();

#if DEBUG_MODE
		if (debug) {
			while (!(kb_ScanGroup(6) & kb_Add) && !(kb_Data[6] & kb_Sub));
			if (kb_Data[6] & kb_Sub)
				debug = false;
			while ((kb_ScanGroup(6) & kb_Add) || (kb_Data[6] & kb_Sub));
		}
		else if (kb_Data[6] & kb_Sub) {
			debug = true;
			while ((kb_ScanGroup(6) & kb_Sub));
		}
#endif

		frameCounter--;
		while (!(timer_IntStatus & TIMER1_RELOADED));	// Wait until the timer has reloaded
		timer_IntStatus = TIMER1_RELOADED;				// Acknowledge the reload


		if (quitDelay) {
			quitDelay--;
		}
		else if (kb_Data[6] & kb_Clear) {
			if (game_state == main_screen) {
				// Usual cleanup
				save_progress();
				exit(0);
			}
			else if (game_state != return_main && game_state != animate_jumpman_dead) {
				disable_sprites();
				num_elevators = 0;
				oilcan.onFire = false;
				jumpman.enabled = false;

				game_data.score = game.score;

				gfx_BlitScreen();
				game.stage = 0xFF;
				cinematicProgress = 0;
				game_state = return_main;
			}
			if (game_state == intro_cinematic) {
				game_data.lives = 0;
			}

			quitDelay = 10;
		}
	}
}

void check_collision_jumpman(void);
void check_collision_hammer(void);

/* Main routine when playing a game */
void game_loop(void) {
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

	animate_hammer();

	move_retractable_ladders();

	handle_rivets();

	check_jumpman_falling();

	handle_jumpman_falling();

	move_elevators();

	handle_conveyor_dirs();

	bonus_item_picked_up();

	update_kong();


	/*if (game.stage == STAGE_BARRELS) {
		uint8_t i;

		for (i = 0; i < num_barrels; i++) {	// check collision barrels
			barrel_t *this_barrel = &barrel[i];

			if (abs((jumpman.y - 3) - this_barrel->y) < 2 + 6) {
				if (abs(jumpman.x - this_barrel->x) < 2 + 4) {
					dbg_sprinf(dbgout, "%d\n", abs((jumpman.y - 3) - this_barrel->y));
					jumpman.isAlive = false;
				}
			}
		}
	}*/
	check_collision_jumpman();	// comment this line to disable collision

	check_collision_hammer();

	handle_time_ran_out();

	handle_bonus_timer();

	check_end_stage();

	if (jumpman.isAlive == false) {
		waitTimer = 0x40;
		game_state = animate_jumpman_dead;
	}
}

/* Checks if jumpman is standing on the place where the stage ends */
void check_end_stage(void) {

	if ((game.stage & 1) == 1) {
		if (jumpman.y > 39)
			return;
	}
	else if (game.stage == STAGE_RIVETS) {
		if (num_rivets)
			return;
		goto end_of_stage;
	}
	else {	// stage conveyors
		if (jumpman.y > 71)
			return;
	}

	// Set sprite based on wich half of the screen jumpman is
	if (jumpman.x > 160)
		jumpman.dir = FACE_LEFT;
	else
		jumpman.dir = FACE_RIGHT;
	jumpman.sprite = 0;

end_of_stage:
	disable_sprites();
	game_state = end_stage_cinematic;
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

/* Handle waitTimer in more complex functions */
void handle_waitTimer(void) {
	waitTimer--;

	if (waitTimer == 0) {
		waitTimer = 1;
		return;
	}

	asm("ld	sp,ix");
	asm("pop ix");
}

/* Handle waitTimer in simple functions */
void handle_waitTimer1(void) {
	waitTimer--;

	if (waitTimer == 0) {
		waitTimer = 1;
		return;
	}

	asm("inc sp");
	asm("inc sp");
	asm("inc sp");
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

/*
 *197A  CALL    #1DBD         ; check for bonus items and jumping scores, rivets																			update_bonus_scores()
 *197D  CALL    #1E8C         ; do stuff for items hit with hammer														 give points and animate hit		animate_hammer_hit()
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
 * find and fix bugs
 * clean/optimize code
 */

/* In progress
 */


/* bugs?:
 * You can get killed by an spawning fireball what shouldn't be able to happen(won't fix this as it is a lot of work that is not worth it)
 * Crazy barrels can escape out of the screen(leave artifacts)?(I think this has been fixed)
 */
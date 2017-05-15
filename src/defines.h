#ifndef DEFINES_H
#define DEFINES_H

#include <stdint.h>
#include "stdlib.h"

#define ONE_SECOND		32768/1
#define QUARTER_SECOND  32768/4
#define ONE_TICK		32768/60

#define ExtraLifeThreshold 7000 // 7000(standard), 10000, 15000 or 20000

#define dbg_sprinf dbg_sprintf
void waitTicks(uint8_t ticks);
void game_loop(void);
extern const void(*game_state)(void);
void handle_waitTimer(void);
extern uint8_t waitTimer;

enum COLOR {
	COLOR_BACKGROUND	= 0x1A,	//  0,  0,  0
	COLOR_BLACK_FAKE	= 0x00,	//  1,  1,  1
	COLOR_LADDER		= 0x05,	//  0,254,254
	COLOR_FLOOR			= 0x03,	// 255, 33, 85
	COLOR_RED			= 0x0F, // 255,  0,  0
	COLOR_BLUE			= 0x17, //  0,  0,255
	COLOR_WHITE			= 0x0A, // 255,255,255
	COLOR_COLLISION		= 0x04, // 151,  0,  0
	COLOR_RED_DARK		= 0x0D, // 151,  1,  0

	COLOR_GREEN			= 0x14, // 0  ,255,  0
	COLOR_YELLOW		= 0x13, // 255,255,  0
	COLOR_COLLISION		= 0x04, // red_dark
};

enum dirs {
	FACE_LEFT			= 0,
	FACE_RIGHT			= 1
};

enum Stages {
	STAGE_BARRELS		= 1,
	STAGE_CONVEYORS		= 2,
	STAGE_ELEVATORS		= 3,
	STAGE_RIVETS		= 4
};

enum Items {
	ITEM_PARASOL		= 0,
	ITEM_PURSE			= 1,
	ITEM_HAT			= 2,
};

extern uint8_t frameCounter;

// Game
typedef struct {
	uint8_t stage;

	uint8_t bonusTimer, initialBonusValue;	// bonusTimer = initialinitialBonusValue = (level * 10 + 40), bonusTimer decreases when kong starts deploying a barrel in barrels or when bonusDelay reaches 0 in other stages
	uint8_t bonusDelay, initialBonusDelay;	// bonusDelay = initialBonusDelay = (220 - initialBonusValue * 2), used in non-barrel stages. When bonusDelay reaches 0 the bonusTimer gets decreased.

	uint8_t difficulty, difficultyTimer0, difficultyTimer1;

	uint8_t blueBarrelCounter;
	
	uint8_t timeRanOut, timeRanOutTimer;
	unsigned score;
	bool quit;
} game_t;

extern game_t game;

// Game data
typedef struct {
	uint8_t level, round;
	uint8_t lives;
	bool extraLifeAwarded;
	unsigned score, Hscore[5];
	char name[5][6];
} game_data_t;

extern game_data_t game_data;

// Pauline's items
extern uint8_t num_bonus_items;

typedef struct {
	uint8_t y;
	uint24_t x;

	uint8_t type;						// 0 = parasol, 1 = purse, 2 = hat

	uint8_t background_data[16 * 15 + 2];
} bonus_item_t;

extern bonus_item_t bonus_item[3];

#endif

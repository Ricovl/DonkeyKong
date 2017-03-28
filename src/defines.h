#ifndef DEFINES_H
#define DEFINES_H

#include <stdint.h>
#include "stdlib.h"

#define ONE_TICK		32768/60

#define ExtraLifeThreshold 7000 // 7000(standard), 10000, 15000 or 20000

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

	COLOR_GREEN			= 0x12, // 0  ,255,  0
	COLOR_YELLOW		= 0x14, // 255,255,  0
	COLOR_COLLISION		= 0x04, // red_dark
};

enum dirs {
	FACE_LEFT	= 0,
	FACE_RIGHT	= 1
};

enum Stages {
	STAGE_BARRELS		= 1,
	STAGE_CONVEYORS		= 2,
	STAGE_ELEVATORS		= 3,
	STAGE_RIVETS		= 4
};

enum Items {
	ITEM_PARASOL	= 0,
	ITEM_PURSE		= 1,
	ITEM_HAT		= 2,
};

extern uint8_t frameCounter;

// Game
typedef struct {
	uint8_t stage, level, round;

	uint8_t bonusTimer, initialBonusValue;	// bonusTimer = initialinitialBonusValue = (level * 10 + 40), bonusTimer decreases when kong starts deploying a barrel in barrels or when bonusDelay reaches 0 in other stages
	uint8_t bonusDelay, initialBonusDelay;	// bonusDelay = initialBonusDelay = (220 - initialBonusValue * 2), used in non-barrel stages. When bonusDelay reaches 0 the bonusTimer gets decreased.

	uint8_t difficulty, difficultyTimer0, difficultyTimer1;

	uint8_t blueBarrelCounter;
	
	uint8_t timeRanOut, timeRanOutTimer;

	unsigned score, Hscore;
	uint8_t lives;
} game_t;
extern game_t game;

// Jumpman
typedef struct {
	uint8_t y, y_old;					// 00, 01
	uint24_t x, x_old;					// 02, 05
	
	bool dir;							// 08	The dir jumpman's sprite is facing: 0 is left 1 is right
	uint8_t sprite;						// 09	

	uint8_t moveIndicator;				// 0A	
	uint8_t moveCounter;				// 0B	
	
	bool isJumping;						// 0C	1 when jumping, else 0																														12
	uint8_t jumpDir;					// 0D	0 = right, #FF = left, 0 = up																												13
	uint8_t jumpDirIndicator;			// 0E	0 when jumping straight up, #80 when jumping left or right																					14
	uint8_t jumpCounter;				// 0F	jumpCounter counts from 0 when jumpman start jupming																						15
	uint8_t jumpCounterX, jumpCounterY;	// 10, 11																																			16, 17
	uint8_t velocityY;					// 12	0x48 when jumpman starts jumping																											18
	bool movingUp;						// 13	1 when jumping 0 when falling																												19
	uint8_t comingDown;					// 14	1 when mario is at apex or on it's way down from a jump else 0. Counting down from 4 to 0 after jump										20
	uint8_t originalY;					// 15	This contains jumpmans y-position from where he started jumping
	bool fallingTooFar;					// 16	1 when jumpman is falling too far, so when he hits the ground he dies

	bool onLadder;						// 17	1 when climbing a ladder, else 0
	bool ladderToggle;					// 18	ladderToggle toggles between 1 and 0 while jumpman is climbing a ladder
	bool brokenLadder;					// 19	1 when the ladder jumpman is on is a broken or moving ladder
	uint8_t ladderTop, ladderBottom;	// 1A	ladderTop contains the y-position of the ground above ladder, and ladderBottom the y-position of the ground underneath the ladder
	
	bool onElevator;					// 1B	

	bool isAlive;						// 1D	1 when alive 0 if death
	uint8_t buffer_data[15 * 16 + 2];	// 2E	contains the data of the background behind jumpman, this is drawn over jumpman to erase him.
} jumpman_t;

extern jumpman_t jumpman;

// Hammers
extern uint8_t num_hammers;

typedef struct {
	uint8_t y, y_old;
	uint24_t x, x_old;

	bool active;
	bool dir;		// No idea why removing this is 38 bytes more!
	bool inFront;	// or this one!
	uint8_t sprite;

	uint8_t background_data[17 * 10 + 2];
} hammer_t;
extern hammer_t hammer[2];

extern uint8_t hammerActive;
extern uint8_t hammerTimer;
extern uint8_t hammerLength;

// Pauline's items
extern uint8_t num_bonus_items;

typedef struct {
	uint8_t y;
	uint24_t x;

	uint8_t type;						// 0 = parasol, 1 = purse, 2 = hat

	uint8_t background_data[16 * 15 + 2];
} bonus_item_t;
extern bonus_item_t bonus_item[3];

// Rivets
extern uint8_t num_rivets;
extern bool rivet_enabled[8];

#endif

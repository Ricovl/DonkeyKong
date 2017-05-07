#ifndef JUMPMAN_H
#define JUMPMAN_H

#include <stdint.h>

void init_jumpman(uint24_t x, uint8_t y);

void move_jumpman(void);

bool girder_collision(void);
void climb_ladder(void);
bool ladder_in_range(void);
void check_jump_over(void);
uint8_t check_collision(uint8_t loop, uint8_t *structp, uint8_t width, uint8_t height, uint8_t offsety, uint8_t size);

void check_jumpman_falling(void);
void handle_jumpman_falling(void);

void animate_jumpman_dead(void);
void bonus_item_picked_up(void);

// Jumpman
typedef struct {
	uint8_t y, y_old;					// 00, 01
	uint24_t x, x_old;					// 02, 05

	bool dir;							// 08	The dir jumpman's sprite is facing: 0 is left 1 is right
	uint8_t sprite;						// 09	

	uint8_t moveIndicator;				// 0A	
	uint8_t moveCounter;				// 0B	

	bool isJumping;						// 0C	1 when jumping, else 0
	uint8_t jumpDir;					// 0D	0 = right, #FF = left, 0 = up
	uint8_t jumpDirIndicator;			// 0E	0 when jumping straight up, #80 when jumping left or right
	uint8_t jumpCounter;				// 0F	jumpCounter counts from 0 when jumpman start jupming
	uint8_t jumpCounterX, jumpCounterY;	// 10, 11
	uint8_t velocityY;					// 12	0x48 when jumpman starts jumping
	bool movingUp;						// 13	1 when jumping 0 when falling
	uint8_t comingDown;					// 14	1 when mario is at apex or on it's way down from a jump else 0. Counting down from 4 to 0 after jump
	uint8_t originalY;					// 15	This contains jumpmans y-position from where he started jumping
	bool fallingTooFar;					// 16	1 when jumpman is falling too far, so when he hits the ground he dies

	bool onLadder;						// 17	1 when climbing a ladder, else 0
	bool ladderToggle;					// 18	ladderToggle toggles between 1 and 0 while jumpman is climbing a ladder
	bool brokenLadder;					// 19	1 when the ladder jumpman is on is a broken or moving ladder
	uint8_t ladderTop, ladderBottom;	// 1A	ladderTop contains the y-position of the ground above ladder, and ladderBottom the y-position of the ground underneath the ladder

	bool onElevator;					// 1B
	bool startFalling;					// 1C
	bool traversedRivet;				// 1D

	bool isAlive;						// 1E	1 when alive 0 if death
	uint8_t buffer_data[16 * 16 + 2];	// 1F	contains the data of the background behind jumpman, this is drawn over jumpman to erase him.
} jumpman_t;

extern jumpman_t jumpman;

// Tables
extern const uint8_t jumpman_walking_sprite_table[4];
extern uint8_t bonus_item_height[3];

#endif
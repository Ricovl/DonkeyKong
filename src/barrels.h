#ifndef BARRELS_H
#define BARRELS_H

#include <stdint.h>

void move_barrels(void);
void del_barrel(uint8_t barrelNum);

void spawn_barrel(void);
void deploy_barrel(void);

// Barrels
#define MAX_BARRELS 10

typedef struct {
	uint8_t y, y_old;					// 00, 01
	uint24_t x, x_old;					// 02, 05

	bool dir;							// 08	The direction the barrel is rolling to
	uint8_t sprite;						// 09	

	bool isRolling;						// 0A	1 if barrel is rolling 0 if barrel is in kong's hands
	uint8_t moveCounter;				// 0B	Counter/timer used for sprite changing (4, 3, 2, 1)

	bool isJumping;						// 0C	1 when jumping/falling, else 0
	uint8_t jumpDir;					// 0D	0 = right, #FF = left, 0 = up
	uint8_t jumpDirIndicator;			// 0E	0 when jumping/falling straight up, #80 when jumping/falling left or right
	uint8_t jumpCounter;				// 0F	jumpCounter counts from 0 when barrel starts jumping/falling 
	uint8_t jumpCounterX, jumpCounterY;	// 10, 11
	uint8_t velocityY;					// 12	0x48 when barrel starts jumping/falling
	bool movingUp;						// 13	1 when jumping 0 when falling
	uint8_t edgeIndicator;				// 14	increases when hitting the ground while falling from edge
	uint8_t bounceY;					// 15	Top y position of girder where crazy barrel bounced off

	bool onLadder;
	bool isBlue, isCrazy;

	uint8_t background_data[16 * 10 + 2];
} barrel_t;

extern barrel_t barrel[MAX_BARRELS];
extern uint8_t num_barrels;

extern bool deployBarrel;
extern bool deployingBarrel;
extern bool crazyIndicator;
extern uint8_t newBarrelType;
extern uint8_t barrelDeploymentTimer;
extern uint8_t barrelDeploymentProgress;

bool take_ladder(barrel_t *this_barrel);
void bounce_crazy_barrel(barrel_t *this_barrel);
void update_sprite_rolling(barrel_t *this_barrel);

#endif
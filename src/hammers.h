#ifndef HAMMERS_H
#define HAMMERS_H

#include <stdint.h>

void animate_hammer(void);
void animate_hammer_hit(void);

// Hammers
#define		MAX_HAMMERS 2

typedef struct {
	uint8_t y, y_old;
	uint24_t x, x_old;

	bool active;
	bool dir;
	uint8_t sprite;

	uint8_t background_data[16 * 10 + 2];
} hammer_t;

extern hammer_t hammer[MAX_HAMMERS];
extern uint8_t num_hammers;

extern uint8_t hammerActive;
extern uint8_t hammerTimer;
extern uint8_t hammerLength;

extern bool wasItemHit;
extern uint8_t hitItemType;
extern uint8_t hitItemNum;

#endif

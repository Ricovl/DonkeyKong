#ifndef KONG_H
#define KONG_H

#include <stdint.h>

void update_kong(void);
void draw_kong(void);
void draw_pauline(bool help);
void end_stage_cinematic(void);
void intro_cinematic(void);

// Kong
typedef struct {
	uint8_t y, y_old;
	uint24_t x, x_old;

	uint8_t sprite;

	bool timerOn;
	uint8_t timer;

	uint8_t climbCounter;
	uint8_t jumpCounter;
	uint8_t bounceCounter;

	uint8_t background_data[46 * 36 + 2];
} kong_t;

extern kong_t kong;

// Pauline
typedef struct {
	uint8_t y;
	uint24_t x;

	bool dir;
	uint8_t sprite;
} pauline_t;

extern pauline_t pauline;

extern uint8_t kong_jumpup_table[22];
extern uint8_t kong_jumpleft_table[16];

#endif
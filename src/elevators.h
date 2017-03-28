#ifndef ELEVATORS_H
#define ELEVATORS_H

#include <stdint.h>

void move_elevators(void);
bool collision_elevator(void);
void move_bouncers(void);

// Elevators
#define MAX_ELEVATORS 6

typedef struct {
	uint8_t y;
	uint24_t x;

	bool active;
	bool movingUp;
} elevator_t;

extern elevator_t elevator[MAX_ELEVATORS];
extern uint8_t num_elevators;
extern uint8_t elevatorTimer;
extern const uint8_t elevator_y_table[];

// Bouncers
#define MAX_BOUNCERS 5

typedef struct {
	uint8_t y, y_old;
	uint24_t x, x_old;

	bool movingDown;
	uint8_t sprite;
	uint8_t *heightOffset;

	uint8_t background_data[16 * 15 + 2];
} bouncer_t;

extern bouncer_t bouncer[MAX_BOUNCERS];
extern uint8_t num_bouncers;
extern bool releaseBouncer;

#endif

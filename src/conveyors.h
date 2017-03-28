#ifndef CONVEYORS_H
#define CONVEYORS_H

#include <stdint.h>

void handle_conveyor_dirs(void);
void update_conveyor_vector(uint8_t conveyor_num, int8_t *vector);
void move_pies(void);
void del_pie(uint8_t pieNum);
void move_retractable_ladders(void);

// Conveyors
enum Conveyors {
	Top = 0,
	Middle = 1,
	Bottom = 2,
};

#define NUM_CONVEYORS 3

typedef struct {
	uint8_t reverseCounter;
	uint8_t direction;
	uint8_t sprite;
	
	gfx_image_t *sprite0;
	gfx_image_t *sprite1;
} conveyor_t;

extern conveyor_t conveyor[NUM_CONVEYORS];

extern int8_t conveyorVector_top;
extern int8_t conveyorVector_middleLeft;
extern int8_t conveyorVector_middleRight;
extern int8_t conveyorVector_bottom;

void reverse_conveyor_dir(conveyor_t *this_conveyor);
void animate_pulley(conveyor_t *this_conveyor);

// Pie's
#define MAX_PIES 6

typedef struct {
	uint8_t y, y_old;
	uint24_t x, x_old;

	uint8_t background_data[16 * 8 + 2];
} pie_t;

extern pie_t pie[MAX_PIES];
extern uint8_t num_pies;

extern bool releasePie;
extern uint8_t pieTimer;

// Retractable ladders
#define MAX_RETRACTABLE_LADDERS 2

typedef struct {
	uint8_t y, y_old;
	uint24_t x;

	uint8_t state;
	uint8_t moveTimer;
	uint8_t waitTimer;

	uint8_t background_data[10 * 16 + 2];
} retractableLadder_t;

extern retractableLadder_t retractableLadder[MAX_RETRACTABLE_LADDERS];
extern uint8_t num_retractable_ladders;

// Tables
extern const uint8_t pulleyTable_y[3];
extern const uint8_t pulleyTable_leftX[3];
extern const uint16_t pulleyTable_rightX[3];

#endif
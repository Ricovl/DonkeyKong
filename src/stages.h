#ifndef STAGES_H
#define STAGES_H

#include <stdint.h>
#include "stdlib.h"

void initialize_stage(uint8_t stage);
void draw_stage(uint8_t *array_b);

typedef struct {
	uint8_t type;
	uint8_t x;
	uint8_t y;
	uint8_t x1;
	uint8_t y1;
} tile_t;

/* Lookup tables */
extern uint8_t  hammer_locations_y[];
extern uint24_t hammer_locations_x[];
extern uint8_t  item_locations_y[];
extern uint24_t item_locations_x[];
extern uint8_t	stage_order[];
extern uint16_t conveyors_palette[3];
extern uint16_t rivets_palette[3];
extern uint8_t stage_order[20];

/* Data of all the stages */
extern const uint8_t *stage_data[4];

extern const uint8_t stage_barrels_data[];

extern const uint8_t stage_conveyors_data[];

extern const uint8_t stage_elevators_data[];

extern const uint8_t stage_rivets_data[];

extern const uint8_t stage_barrels_intro_data[];

extern const uint8_t stage_barrels_slanted_top[];

extern const uint8_t stage_barrels_slanted[];

#endif